/* This file include the device specific to the SOLARIS architecture.
   Initially only audio and MIDI.

   These audio driver work for any number of channels channels, allow access to all
   the SOLARIS audio parameters from the FTS UCS system.

   A Device class is defined for DACs and ADCs and device class for 
   MIDI i/o.

   A central structure keep track of the consistency of the whole business.

   In the Future, a io_ctrl operations at the UCS level will allow access
   to the solaris audio paramters from UCS programs, accessing to the solaris device
   directly.
*/

/* Include files */

#include <unistd.h>
#include <stdio.h>		
#include <sys/types.h>
#include <sys/time.h>
#include <sys/audioio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
/*#include <multimedia/libaudio.h>
#include <multimedia/audio_device.h>
#include <multimedia/audio_encode.h>
*/
/*
#include "evdefs.h"
#include "ftstypes.h"
#include "ftsdefs.h"
#include "modules.h"
#include "error.h"
#include "dev.h"
#include "mem.h"
#include "ucs.h"
*/
#include "sys.h"
#include "lang.h"
#include "runtime/devices.h"

/* forward declarations */

/*patch by gigi
static fts_status_t solaris_ucs_init(void);*/
static fts_status_t solaris_dac_init(void);
static fts_status_t solaris_adc_init(void);
static fts_status_t solaris_midi_init(void); 
static fts_status_t solaris_soundfile_init(void);

/* global variables definition */
static int sample_rate = 44100;

static const char audio_dev[] = "/dev/audio";
static const char audio_ctl_dev[] = "/dev/audioctl";

/*#define SOLARIS_AUDIO_DEBUG*/

#define AUDIO_BUFFERIZE
/*#undef SOLARIS2_PORTAGE*/
#define SOLARIS2_PORTAGE
/******************************************************************************/
/*                                                                            */
/*                              Error Definition                              */
/*                                                                            */
/******************************************************************************/

static fts_status_description_t solaris_no_midi =
{
  "No Midi port configured"
};

static fts_status_description_t solaris_audio_opening_error =
{
  "Cannot open Solaris audio device"
};

static fts_status_description_t solaris_set_config_error =
{
  "Solaris DAC : set configuration error"
};

/******************************************************************************/
/*                                                                            */
/*                              Module Definition                             */
/*                                                                            */
/******************************************************************************/

/* Module definition and init function */

static void solarisdev_shutdown(void);
static void solarisdev_init(void);

fts_module_t fts_solarisdev_module = {"Solarisdev", "Device for the SOLARIS platform", solarisdev_init, solarisdev_shutdown};

static void
solarisdev_init(void)
{
  /*solaris_ucs_init();patch by gigi*/
  solaris_dac_init();
  solaris_adc_init();
#ifndef SOLARIS2_PORTAGE
  solaris_midi_init(); 
  solaris_soundfile_init();
#endif
}

static void
solarisdev_shutdown(void)
{
}

/******************************************************************************/
/*                                                                            */
/*                              SOLARIS UCS COntrol                               */
/*                                                                            */
/******************************************************************************/


/* This section of the file define a number of UCS commands to control 
   the solaris audio library parameter; the old control device structure has
   been dropped. 
*/


/*
 * This ucs commands  allow to configure the
 * solaris audio system; there is (will be) one UCS command for each ALsetparam so
 * that the entire SOLARIS audio configuration can be accessed from the
 * configuration file.  
 *
 *
 * PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES
 *
 * sample_rate   the sample_rate source  int      44100
 *
 * SOON we will add SOLARIS all the audio parameters.
 *
 * Commands:
 *
 * audio-set sample_rate <int>
 *
 */

/*Patch by gigi
static fts_status_t 
solaris_ucs_set_audio_command(int argc, const fts_atom_t *argv)
{
  if (argc >= 2)
    if (fts_is_symbol(argv) && (fts_get_symbol(argv) == fts_new_symbol("sample_rate")) &&
	fts_is_long(argv + 1))
      {
	sample_rate = fts_get_long(argv + 1);
	return fts_Success;
      }

  return &fts_dev_invalid_value_error;
}*/

/*Patch by gigi
static fts_status_t
solaris_ucs_init(void)
{
  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("audio"), solaris_ucs_set_audio_command,
			 "set audio [sample_rate <int> | ... ]",
			 "Set SOLARIS audio system parameters\nIn this release, only sample_rate is supported");
  return fts_Success;
}*/

/******************************************************************************/
/*                                                                            */
/*                              DAC Devices                                   */
/*                                                                            */
/******************************************************************************/


/* Forward declarations of DAC dev and dev class static functions */

static fts_status_t solaris_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t solaris_dac_close(fts_dev_t *dev);
static void         solaris_dac_put(fts_word_t *args);

static int          solaris_dac_get_nchans(fts_dev_t *dev);

/*
 * Channels housekeeping structure
 */

static struct solaris_dac_data_struct
{
  int fd;                       /* the audio file descriptor */
  audio_info_t config;          /* the audio configuration structure */
  int nch;			/* number of channels; used also to compute the fifo_size */
  int fifo_size;		/* the fifo size is stored in samples frames*/

  fts_dev_t *dac_dev;		/* the device itself, stored here gainst double instantiation */

  short *dac_fmtbuf;		/* buffer to format stereo/quad sample frames, allocated in the device open */
#ifdef AUDIO_BUFFERIZE
#define AUDIO_NVECTS 8
  short *dac_buffer;
  int nvects;
#endif
} solaris_dac_data;



/* Init and shutdown functions */

static fts_status_t
solaris_dac_init(void)
{
  fts_dev_class_t *solaris_dac_class;

  /* defaults and init for the shared data */

  solaris_dac_data.dac_dev = 0;
  solaris_dac_data.nch = 2;
    
  /* SOLARIS DAC class  */

  solaris_dac_class = fts_dev_class_new(fts_sig_dev);

  /* device functions */

  set_open_fun(solaris_dac_class, solaris_dac_open);
  set_close_fun(solaris_dac_class, solaris_dac_close);

  set_sig_dev_put_fun(solaris_dac_class, solaris_dac_put);

  set_sig_dev_get_nchans_fun(solaris_dac_class, solaris_dac_get_nchans);

  return fts_dev_class_register(fts_new_symbol("solaris_dac"), solaris_dac_class);
}

/* SOLARIS DAC control/options functions */

/*
   The dev_ctrl operation for the solaris_dac
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the solaris dac.

   NOTE that for the SOLARIS you *have* to set the sample_rate using
   the dedicated UCS command (audio-set sample-rate) , not the solaris_dac.

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in samples frame units

*/


/* The command table is defined after the action functions, together with the actual
   control function */


static fts_status_t
solaris_dac_set_config()
{
  if (ioctl(solaris_dac_data.fd, AUDIO_SETINFO,
	    &solaris_dac_data.config) < 0)
    {
      perror("set config");
      return &solaris_set_config_error;
    }

#ifdef SOLARIS_AUDIO_DEBUG

  sys_log("sample_rate : %d\n", solaris_dac_data.config.play.sample_rate);
  sys_log("channels    : %d\n", solaris_dac_data.config.play.channels);
  sys_log("precision   : %d\n", solaris_dac_data.config.play.precision);
  sys_log("encoding    : %d\n", solaris_dac_data.config.play.encoding);
  sys_log("gain        : %d\n", solaris_dac_data.config.play.gain);
  sys_log("port        : %d\n", solaris_dac_data.config.play.port);
  sys_log("buffer_size : %d\n", solaris_dac_data.config.play.buffer_size);

  if (ioctl(solaris_dac_data.fd, AUDIO_GETINFO,
	    &solaris_dac_data.config) < 0)
    {
      perror("set config");
      return &solaris_set_config_error;
    }

  sys_log("sample_rate : %d\n", solaris_dac_data.config.play.sample_rate);
  sys_log("channels    : %d\n", solaris_dac_data.config.play.channels);
  sys_log("precision   : %d\n", solaris_dac_data.config.play.precision);
  sys_log("encoding    : %d\n", solaris_dac_data.config.play.encoding);
  sys_log("gain        : %d\n", solaris_dac_data.config.play.gain);
  sys_log("port        : %d\n", solaris_dac_data.config.play.port);
  sys_log("buffer_size : %d\n", solaris_dac_data.config.play.buffer_size);

#endif

  return fts_Success;
}

static fts_status_t 
solaris_dac_set_channels_action(fts_dev_t *dev, fts_atom_t *value)
{
  if (! fts_is_long(value))
    return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */

  solaris_dac_data.nch  = fts_get_long(value);
  solaris_dac_data.config.play.channels   = solaris_dac_data.nch;

  return solaris_dac_set_config();
}


static fts_status_t 
solaris_dac_set_fifo_size_action(fts_dev_t *dev, fts_atom_t *value)
{
  if (! fts_is_long(value))
    return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */

  solaris_dac_data.fifo_size = fts_get_long(value);

  return fts_Success;
}

static fts_status_t 
solaris_dac_set_end_action(fts_dev_t *dev, fts_atom_t *value)
{
  return fts_Success;
}


/*struct command_entry  solaris_dac_command_table[] =
{
  {"set", "channels",    solaris_dac_set_channels_action},
  {"set", "fifo_size",   solaris_dac_set_fifo_size_action},
  {"set", "end",         solaris_dac_set_end_action},
  {0,0,0}
};*/


/* SOLARIS DAC dev class functions */

static fts_status_t
solaris_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_atom_t local_args[10];
  int local_nargs;			/* X/Motif style arg counter */
  /*struct parser_status solaris_dac_parser_status; */

  if (solaris_dac_data.dac_dev)
    return &fts_dev_open_error; /*Error: a device was already opened for the ch */

  solaris_dac_data.dac_dev = dev;

  /*solaris_dac_data.fd = open(audio_dev, O_WRONLY | O_NDELAY); */
  solaris_dac_data.fd = open(audio_dev, O_WRONLY);

  if (solaris_dac_data.fd < 0)
    return &solaris_audio_opening_error;

/*
  if (fcntl(solaris_dac_data.fd, F_SETFL,
	    fcntl(solaris_dac_data.fd, F_GETFL, 0) | FNDELAY) < 0)
    return &solaris_audio_opening_error;
*/

  /* Create the configuration object, and initialize some fields */

  if (ioctl(solaris_dac_data.fd, AUDIO_GETINFO,
	    &solaris_dac_data.config) < 0)
    return &solaris_audio_opening_error;

  /* Set PCM 16 bits linear encoding */

  solaris_dac_data.config.play.precision   = 16;

  solaris_dac_data.config.play.encoding   = AUDIO_ENCODING_LINEAR;

  /* Set defaults for the other values */

  solaris_dac_data.config.play.sample_rate   = sample_rate;

  solaris_dac_data.config.play.channels   = 2;


  solaris_dac_data.nch  = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);
  solaris_dac_data.config.play.channels   = solaris_dac_data.nch;
  solaris_dac_set_config();

  solaris_dac_data.fifo_size = fts_get_int_by_name(nargs, args, fts_new_symbol("fifo_size"), 256);


  /* Allocate the DAC  formatting buffer */

  solaris_dac_data.dac_fmtbuf = (short *) fts_malloc(MAXVS * solaris_dac_data.nch * sizeof(short));

#ifdef AUDIO_BUFFERIZE
  solaris_dac_data.dac_buffer = (short *) fts_malloc(MAXVS * AUDIO_NVECTS * solaris_dac_data.nch * sizeof(short));
  solaris_dac_data.nvects = 0;
#endif

  return solaris_dac_set_config();
}


static fts_status_t
solaris_dac_close(fts_dev_t *dev)
{
  /* Check that the solaris dac class is open */

  if (! solaris_dac_data.dac_dev)
    return &fts_dev_close_error; /* error: the class was not open */

  solaris_dac_data.dac_dev = 0;

  /* free the audio formatting buffer */

  fts_free(solaris_dac_data.dac_fmtbuf);

  /* close the audio output port */

  close(solaris_dac_data.fd);

  return fts_Success;
}

static int
solaris_dac_get_nchans(fts_dev_t *dev)
{
  return solaris_dac_data.nch;
}

static void
solaris_write_samples(int nchans, int n, int out)
{
  char *p = (char *)solaris_dac_data.dac_fmtbuf;
  int len = n * nchans * sizeof(short);
  int fd = solaris_dac_data.fd;

  if (!out)
    return;

#ifdef AUDIO_BUFFERIZE
  if (out == 1)
    {
      memcpy((char *)(solaris_dac_data.dac_buffer) +
	 (solaris_dac_data.nvects * len),
	     (char *)solaris_dac_data.dac_fmtbuf, len);	     
      
      if (++solaris_dac_data.nvects != AUDIO_NVECTS)
	return;

      solaris_dac_data.nvects = 0;
    }

  p = (char *)solaris_dac_data.dac_buffer;
  len = n * nchans * sizeof(short) * AUDIO_NVECTS;

#endif
  for (;;)
    {
      int n = write(fd, p, len);

      if (n == len || n <= 0)
	break;

      p   += n;
      len -= n;
    }
}

/* conversion macros */

#define CONV_COEF 32768.
#define CONVERT(X)  ((short)(CONV_COEF*(X)))
#define RCONVERT(X) (((float)(X))/CONV_COEF)


#if 0
static fts_status_t
solaris_dac_put(fts_dev_t *dev, float **buf, int n)
{
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;
  int out;

  nchans = solaris_dac_data.nch;

  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *in;
      
      in = buf[ch];

      for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	{
	  float f1, f2, f3, f4;

	  f1 = in[j];
	  f2 = in[j + 1];
	  f3 = in[j + 2];
	  f4 = in[j + 3];

	  solaris_dac_data.dac_fmtbuf[i] = CONVERT(f1);
	  solaris_dac_data.dac_fmtbuf[i + off2] = CONVERT(f2);
	  solaris_dac_data.dac_fmtbuf[i + off3] = CONVERT(f3);
	  solaris_dac_data.dac_fmtbuf[i + off4] = CONVERT(f4);
	}
    }

#if defined(AUDIO_BUFFERIZE)
  if (solaris_dac_data.nvects)
    {
      memset(solaris_dac_data.dac_buffer, 0,
	     MAXVS * AUDIO_NVECTS * 2 * sizeof(short));
      solaris_dac_data.nvects = AUDIO_NVECTS - 1;
      out = -1;
    }
  else
    out = 0;
  solaris_write_samples(nchans, n, out);
  solaris_dac_data.nvects = 0;
#else
  solaris_write_samples(nchans, n, 0);
#endif
}
#endif

/* FTL version of the above Args in the word vect: fts_dev_t *dev, int n, float *buf1 ... bufm 
   the device is ignored (only one device for the moment allowed)

*/

static void
solaris_dac_put(fts_word_t *argv)
{
  long n = fts_word_get_long(argv + 1);
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  nchans = solaris_dac_data.nch;
  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *in;
      
      in = (float *) fts_word_get_obj(argv + 2 + ch);

      for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	{
	  float f1, f2, f3, f4;

	  f1 = in[j];
	  f2 = in[j + 1];
	  f3 = in[j + 2];
	  f4 = in[j + 3];

	  solaris_dac_data.dac_fmtbuf[i] = CONVERT(f1);
	  solaris_dac_data.dac_fmtbuf[i + off2] = CONVERT(f2);
	  solaris_dac_data.dac_fmtbuf[i + off3] = CONVERT(f3);
	  solaris_dac_data.dac_fmtbuf[i + off4] = CONVERT(f4);
	}
    }

  solaris_write_samples(nchans, n, 1);
}


/******************************************************************************/
/*                                                                            */
/*                              ADC Devices                                   */
/*                                                                            */
/******************************************************************************/

/* Forward declarations of ADC dev and dev class static functions */

static fts_status_t solaris_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t solaris_adc_close(fts_dev_t *dev);
static void         solaris_adc_get(fts_word_t *args);

static int          solaris_adc_get_nchans(fts_dev_t *dev);


static struct solaris_adc_data_struct
{
  int fd;                       /* the audio file descriptor */
  audio_info_t config;          /* the audio configuration structure */
  int nch;			/* number of channels; used only to compute the fifo_size */
  int fifo_size;		/* the fifo size is stored in sample frames */

  fts_dev_t *adc_dev;		/* the device itself, stored here gainst double instantiation */

  short *adc_fmtbuf;		/* buffer to format stereo/quad sample frames, allocated in class_open */
} solaris_adc_data;


/* Init and shutdown functions */

static fts_status_t
solaris_adc_init(void)
{
  fts_dev_class_t *solaris_adc_class;

  /* defaults and init for the shared data */

  solaris_adc_data.adc_dev = 0;
  solaris_adc_data.nch = 2;
    
  /* SOLARIS ADC class  */

  solaris_adc_class = fts_dev_class_new(fts_sig_dev);

  /* device functions */

  set_open_fun(solaris_adc_class, solaris_adc_open);
  set_close_fun(solaris_adc_class, solaris_adc_close);
  set_sig_dev_get_fun(solaris_adc_class, solaris_adc_get);

  set_sig_dev_get_nchans_fun(solaris_adc_class, solaris_adc_get_nchans);

  return fts_dev_class_register(fts_new_symbol("solaris_adc"), solaris_adc_class);
}

/* SOLARIS ADC control/options functions */

/*
   The dev_ctrl operation for the solaris_adc
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the solaris adc.

   NOTE that for the SOLARIS you *have* to set the sample_rate using
   the solaris device, not the solaris_adc.

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in sample frame units
*/


static fts_status_t
solaris_adc_set_config()
{
  if (ioctl(solaris_adc_data.fd, AUDIO_SETINFO,
	    &solaris_adc_data.config) < 0)
    {
      perror("set config");
      return &solaris_set_config_error;
    }

#ifdef SOLARIS_AUDIO_DEBUG

  sys_log("sample_rate : %d\n", solaris_adc_data.config.record.sample_rate);
  sys_log("channels    : %d\n", solaris_adc_data.config.record.channels);
  sys_log("precision   : %d\n", solaris_adc_data.config.record.precision);
  sys_log("encoding    : %d\n", solaris_adc_data.config.record.encoding);
  sys_log("gain        : %d\n", solaris_adc_data.config.record.gain);
  sys_log("port        : %d\n", solaris_adc_data.config.record.port);
  sys_log("buffer_size : %d\n", solaris_adc_data.config.record.buffer_size);

  if (ioctl(solaris_adc_data.fd, AUDIO_GETINFO,
	    &solaris_adc_data.config) < 0)
    {
      perror("set config");
      return &solaris_set_config_error;
    }

  sys_log("sample_rate : %d\n", solaris_adc_data.config.record.sample_rate);
  sys_log("channels    : %d\n", solaris_adc_data.config.record.channels);
  sys_log("precision   : %d\n", solaris_adc_data.config.record.precision);
  sys_log("encoding    : %d\n", solaris_adc_data.config.record.encoding);
  sys_log("gain        : %d\n", solaris_adc_data.config.record.gain);
  sys_log("port        : %d\n", solaris_adc_data.config.record.port);
  sys_log("buffer_size : %d\n", solaris_adc_data.config.record.buffer_size);

#endif

  return fts_Success;
}

/* The command table is defined after the action functions, together with the actual
   control function */


static fts_status_t 
solaris_adc_set_channels_action(fts_dev_t *dev, fts_atom_t *value)
{
  if (! fts_is_long(value))
    return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */

  solaris_adc_data.nch  = fts_get_long(value);
  solaris_adc_data.config.record.channels = solaris_adc_data.nch;

  return solaris_adc_set_config();
}

static fts_status_t 
solaris_adc_set_fifo_size_action(fts_dev_t *dev, fts_atom_t *value)
{
  if (! fts_is_long(value))
    return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */

  solaris_adc_data.fifo_size = fts_get_long(value);

  return fts_Success;
}


static fts_status_t 
solaris_adc_set_end_action(fts_dev_t *dev, fts_atom_t *value)
{
  return fts_Success;
}


/*struct command_entry  solaris_adc_command_table[] =
{
  {"set", "channels",    solaris_adc_set_channels_action},
  {"set", "fifo_size",   solaris_adc_set_fifo_size_action},
  {"set", "end",         solaris_adc_set_end_action},
  {0,0,0}
};*/


/* SOLARIS ADC dev class functions */

static fts_status_t
solaris_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_atom_t local_args[10];
  int local_nargs;			/* X/Motif style arg counter */
  /*struct parser_status solaris_adc_parser_status;*/ 

  if (solaris_adc_data.adc_dev)
    return &fts_dev_open_error; /*Error: a device was already opened for the ch */

  /* Initialize the parser status for open adc class arguemnts */

  /* Create the configuration object, and initialize some fields */

  solaris_adc_data.fd = open(audio_dev, O_RDONLY);

  if (solaris_adc_data.fd < 0)
    return &solaris_audio_opening_error;

  if (ioctl(solaris_adc_data.fd, AUDIO_GETINFO,
	    &solaris_adc_data.config) < 0)
    return &solaris_audio_opening_error;

  /* Set PCM 16 bits linear encoding */

  solaris_adc_data.config.record.precision = 16;
  solaris_adc_data.config.record.encoding = AUDIO_ENCODING_LINEAR;

  /* Set defaults for the other values */

  solaris_adc_data.config.record.sample_rate = sample_rate;
  solaris_adc_data.config.record.channels = 2;

  solaris_adc_data.nch  = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);
  solaris_adc_data.config.record.channels = solaris_adc_data.nch;
  solaris_adc_set_config();
  solaris_adc_data.fifo_size = fts_get_int_by_name(nargs, args, fts_new_symbol("fifo_size"), 256);;


  /* Allocate the ADC  formatting buffer */

  solaris_adc_data.adc_fmtbuf = (short *) fts_malloc(MAXVS * solaris_adc_data.nch * sizeof(short));
  return fts_Success;
}

static fts_status_t
solaris_adc_close(fts_dev_t *dev)
{
  /* Check that the solaris adc class is open */

  if (! solaris_adc_data.adc_dev)
    return &fts_dev_close_error; /* error: the class was not open */

  solaris_adc_data.adc_dev = 0;

  /* close the audio output port */

  close(solaris_adc_data.fd);

  /* free the audio formatting buffer */

  fts_free(solaris_adc_data.adc_fmtbuf);

  return fts_Success;
}


static int
solaris_adc_get_nchans(fts_dev_t *dev)
{
  return solaris_adc_data.nch;
}

static void
solaris_read_samples(int nchans, int n)
{
  char *p = (char *)solaris_adc_data.adc_fmtbuf;
  int len = n * nchans * sizeof(short);
  int fd = solaris_adc_data.fd;

  for (;;)
    {
      int n = read(fd, p, len);

      if (n == len || n <= 0)
	break;

      p   += n;
      len -= n;
    }
}

#if 0
static fts_status_t
solaris_adc_get(fts_dev_t *dev, float **buf, int n)
{
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  nchans = solaris_adc_data.nch;
  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;

  /* if it is the first device of the tick, read the frame in the 
     buffer */

  solaris_read_samples(n, nchans);

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *out;
      
      out = buf[ch];

      for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	{
	  float f1, f2, f3, f4;

	  f1 = RCONVERT(solaris_adc_data.adc_fmtbuf[i]);
	  f2 = RCONVERT(solaris_adc_data.adc_fmtbuf[i + off2]);
	  f3 = RCONVERT(solaris_adc_data.adc_fmtbuf[i + off3]);
	  f4 = RCONVERT(solaris_adc_data.adc_fmtbuf[i + off4]);

	  out[j] = f1;
	  out[j + 1] = f2;
	  out[j + 2] = f3;
	  out[j + 3] = f4;
	}
    }

  return fts_Success;
}
#endif

/* FTL version of the above Args in the word vect: fts_dev_t *dev, int n, float *buf1 ... bufm 
   the device is ignored (only one device for the moment allowed)

*/

static void
solaris_adc_get(fts_word_t *argv)
{
  long n = fts_word_get_long(argv + 1);
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  nchans = solaris_adc_data.nch;
  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;

  /* if it is the first device of the tick, read the frame in the 
     buffer */

  solaris_read_samples(n, nchans);

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *out;
      
      out = (float *) fts_word_get_obj(argv + 2 + ch);

      for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	{
	  float f1, f2, f3, f4;

	  f1 = RCONVERT(solaris_adc_data.adc_fmtbuf[i]);
	  f2 = RCONVERT(solaris_adc_data.adc_fmtbuf[i + off2]);
	  f3 = RCONVERT(solaris_adc_data.adc_fmtbuf[i + off3]);
	  f4 = RCONVERT(solaris_adc_data.adc_fmtbuf[i + off4]);

	  out[j] = f1;
	  out[j + 1] = f2;
	  out[j + 2] = f3;
	  out[j + 3] = f4;
	}
    }
}


/******************************************************************************/
/*                                                                            */
/*                              MIDI Device                                   */
/*                                                                            */
/******************************************************************************/

/* The MIDI device, called "solaris_midi" support input and output on the same device */

/* Forward declaration of midi device/class functions */

static fts_status_t solaris_midi_open(fts_dev_t *dev, int nargs, fts_atom_t *args);
static fts_status_t solaris_midi_close(fts_dev_t *dev);

static fts_status_t solaris_midi_get(fts_dev_t *dev, unsigned char *cp);
static fts_status_t solaris_midi_put(fts_dev_t *dev, unsigned char c);

static fts_status_t 
solaris_midi_init(void)
{
#ifndef SOLARIS2_PORTAGE
  fts_dev_class_t *solaris_midi_dev_class;

  if (mdInit() <= 0)
    return & solaris_no_midi;

  /* adding device functions: the device support only basic 
   character i/o; no callback functions, no sync functions */

  solaris_midi_dev_class = fts_dev_class_new(fts_char_dev);

  set_open_fun(solaris_midi_dev_class, solaris_midi_open);
  set_close_fun(solaris_midi_dev_class, solaris_midi_close);
  set_char_dev_get_fun(solaris_midi_dev_class, solaris_midi_get);
  set_char_dev_put_fun(solaris_midi_dev_class, solaris_midi_put);

  /* Installing the class */

  return fts_dev_class_register(fts_new_symbol("solaris_midi"), solaris_midi_dev_class);
#else
  return fts_Success;
#endif
}


/* The instantiation function has one keyword argument "port", the
   name of the MIDI port; it is an optional argument; we don't do any
   locking on the MIDI port 
   */

#define MAX_OUT_SYSEX 1024
#define MAX_MIDI_EV 32

typedef struct
{
#ifndef SOLARIS2_PORTAGE
  MDport in_port;
  MDport out_port;
#endif
  /*input unparsing support */

  enum {input_waiting, reading_message, reading_sysex} input_status;
  unsigned char in_buf[3];	
  unsigned char *in_sysex_buf;

#ifndef SOLARIS2_PORTAGE
  MDevent in_midi_event[MAX_MIDI_EV];
#endif
  int in_current_char_count;
  int in_current_event_length;
  int in_current_event;
  int in_event_count;

  /* output parsing support */

#ifndef SOLARIS2_PORTAGE
  MDevent out_midi_event;
#endif
  enum {output_waiting, message_need_first, message_need_first_and_second,
	  message_need_second, getting_sysex } output_status;

  unsigned char out_sysex_buf[MAX_OUT_SYSEX];
  unsigned int out_sysex_count;
} solarismidi_dev_data;


static fts_status_t
solaris_midi_open(fts_dev_t *dev, int nargs, fts_atom_t *args)
{
#ifndef SOLARIS2_PORTAGE
  solarismidi_dev_data *data;
  char *port_name = 0;
  char *in_port_name = 0;
  char *out_port_name = 0;
  int i;

  /* Look for the port name; if not specified, just use NULL,
     that will open the first MIDI port found (according to the manual)
   */

  for (i = 0; i < nargs - 1; i++)
    if (fts_is_symbol(&args[i]) && fts_is_symbol(&args[i+1]))
      {
	fts_symbol_t *s;

	s = fts_get_symbol(&args[i]);

	if (s == fts_new_symbol("port"))
	  port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
	else if (s == fts_new_symbol("in_port"))
	  in_port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
	else if (s == fts_new_symbol("out_port"))
	  out_port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
      }



  /* allocate the device data and open the input and output port */

  data = fts_malloc(sizeof(solarismidi_dev_data));
  data->input_status = input_waiting;
  data->output_status = output_waiting;

  data->in_port = mdOpenInPort(in_port_name ? in_port_name : port_name);
  data->out_port = mdOpenOutPort(out_port_name ? out_port_name : port_name);

  if (data->in_port && data->out_port)
    {
      /* no error, end configuring the port and return */
      
      dev->device_data = (void *) data;	
      mdSetStampMode(data->in_port, MD_NOSTAMP);
      mdSetStampMode(data->out_port, MD_NOSTAMP);

      return fts_Success;
    }
  else
    return &fts_dev_open_error;
#else
  return fts_Success;
#endif
}

static fts_status_t
solaris_midi_close(fts_dev_t *dev)
{
#ifndef SOLARIS2_PORTAGE
  solarismidi_dev_data *data = (solarismidi_dev_data *)dev->device_data;

  /* Send a all not off, and close the port */

  mdPanic(data->out_port);
  mdClosePort(data->out_port);
  mdClosePort(data->in_port);

  fts_free(data);
#endif
  return fts_Success;
}

/* I/O functions */

static void
solaris_midi_prepare_next_event(solarismidi_dev_data *data)
{
#ifndef SOLARIS2_PORTAGE
  MDevent *midi_event;

  midi_event = &(data->in_midi_event[data->in_current_event]);

  if ((midi_event->msg[0] == MD_SYSEX) &&  midi_event->sysexmsg)
    {
      /* received sysex */
      data->input_status = reading_sysex;
      data->in_current_char_count = 0;

      data->in_sysex_buf = (unsigned char *) midi_event->sysexmsg;
      data->in_current_event_length = midi_event->msglen;
    }
  else
    {
      data->input_status = reading_message;
      data->in_current_char_count = 0;
      
      data->in_buf[0] = midi_event->msg[0];

      data->in_current_event_length = midi_event->msglen;

      if (data->in_current_event_length > 1)
	data->in_buf[1] = mdGetByte1(midi_event->msg);

      if (data->in_current_event_length > 2)
	data->in_buf[2] = mdGetByte2(midi_event->msg);
    }
#endif
}

static fts_status_t
solaris_midi_get(fts_dev_t *dev, unsigned char *cp)
{
#ifndef SOLARIS2_PORTAGE
  solarismidi_dev_data *data = (solarismidi_dev_data *)dev->device_data;

  switch(data->input_status)
    {
    case reading_message:
      *cp = (char) (data->in_buf[data->in_current_char_count ++]);
      if (data->in_current_char_count >= data->in_current_event_length)
	{
	  data->in_current_event++;
	  if (data->in_current_event == data->in_event_count)
	    data->input_status = input_waiting;
	  else
	    solaris_midi_prepare_next_event(data);
	}
      return fts_Success;

    case reading_sysex:
      *cp = (char) ((data->in_sysex_buf)[data->in_current_char_count ++]);
      if (data->in_current_char_count == data->in_current_event_length)
	{
	  data->in_current_event++;
	  if (data->in_current_event == data->in_event_count)
	    {
	      data->input_status = input_waiting;
	      mdFree(data->in_sysex_buf);
	    }
	  else
	    solaris_midi_prepare_next_event(data);
	}
      return fts_Success;

    case input_waiting:
      {
	fd_set check;
	struct timeval timeout;
	int fd;
	int ret;

	/* First, test the fd using the select, otherwise MIreceive
	   is blocking !!! */

	fd = mdGetFd(data->in_port);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&check);
	FD_SET(fd, &check);
	ret = select(64, &check, (fd_set *)0, (fd_set *)0, &timeout);

	if (FD_ISSET(fd, &check))
	  {
	    data->in_event_count = mdReceive(data->in_port,  data->in_midi_event,  MAX_MIDI_EV);

	    if (data->in_event_count <= 0)
	      return &fts_data_not_ready;
	    else
	      {
		data->in_current_event = 0;

		solaris_midi_prepare_next_event(data);

		if (data->input_status == reading_message)
		  {
		    *cp = (char) (data->in_buf[data->in_current_char_count++]);
		    return fts_Success;
		  }
		else if (data->input_status == reading_sysex)
		  {
		    *cp = (char) (data->in_sysex_buf[data->in_current_char_count++]);
		    return fts_Success;
		  }
	      }
	  }
	else
	  return &fts_data_not_ready;
      }
      break;

    default :
      return &fts_data_not_ready;
    }
#endif
  return fts_Success;
}


static fts_status_t
solaris_midi_put(fts_dev_t *dev, unsigned char c)
{
#ifndef SOLARIS2_PORTAGE
  int ret;
  solarismidi_dev_data *data = (solarismidi_dev_data *)dev->device_data;

  switch (data->output_status)
    {
    case output_waiting:
      switch (c & 0xf0)
	{
	case MD_NOTEOFF:
	case MD_NOTEON:
	case MD_POLYKEYPRESSURE:
	case MD_CONTROLCHANGE:
	case MD_PITCHBENDCHANGE:
	  data->output_status = message_need_first_and_second;
	  data->out_midi_event.msglen = 3;
	  break;
	case MD_PROGRAMCHANGE:
	case MD_CHANNELPRESSURE:
	  data->output_status = message_need_first;
	  data->out_midi_event.msglen = 2;
	  break;
	case MD_SYSEX:
	  data->output_status = getting_sysex;
	  data->out_sysex_count = 0;
	  return fts_Success;
	default:
	  data->output_status = output_waiting;
	  mdSend(data->out_port, &(data->out_midi_event), 1);
	  data->out_midi_event.msglen = 1;
	  break;
	}
      data->out_midi_event.msg[0] = c;
      return fts_Success;

    case message_need_first_and_second:
      data->out_midi_event.msg[1] = c;
      data->output_status = message_need_second;
      break;

    case message_need_first:
      data->out_midi_event.msg[1] = c;
      data->output_status = output_waiting;
      mdSend(data->out_port, &(data->out_midi_event), 1);
      break;

    case message_need_second:
      data->out_midi_event.msg[2] = c;
      data->output_status = output_waiting;
      ret = mdSend(data->out_port, &(data->out_midi_event), 1);
      break;

    case getting_sysex:

      if (data->out_sysex_count >= MAX_OUT_SYSEX)
	{
	  /* store char */

	  data->out_sysex_buf[data->out_sysex_count] = c;

	  /* send */

	  data->out_midi_event.sysexmsg = (char *) data->out_sysex_buf;
	  data->out_midi_event.msglen   = data->out_sysex_count;
	  mdSend(data->out_port, &(data->out_midi_event), 1);

	  /* keep status to sysex, setting count to 0 and msg[0] to 0xf0 */

	  data->out_sysex_count = 0;
	  data->out_midi_event.msg[0] = MD_SYSEX; 
	  data->output_status = getting_sysex;
	}
      else if (c == MD_EOX) 
	{
	  /* store char */

	  data->out_sysex_buf[data->out_sysex_count++] = c;

	  /* send */

	  data->out_midi_event.sysexmsg = (char *)data->out_sysex_buf;
	  data->out_midi_event.msglen   = data->out_sysex_count;
	  mdSend(data->out_port, &(data->out_midi_event), 1);

	  data->output_status = output_waiting;
	}
      else
	{
	  /* store char */

	  data->out_sysex_buf[data->out_sysex_count++] = c;
	}
    }

#endif
  return fts_Success;
}


/******************************************************************************/
/*                                                                            */
/*                               FILE DAC Device                              */
/*                                                                            */
/******************************************************************************/

/* Test implementation for the SOLARIS

   Support only AIFF files (AIFF-C are dropped by SOLARIS), any number of
   them in input or output.

   The open class operation do actually nothing.

   Devices use a device local structure, with all the information
   about the file; they should have all the same sample rate, that
   have to be the same of FTS, and of the ADC/DAC used if any.

   It is possible to mix file and dac output, realtime depend on the
   complexity of the patch and on the machine load.

   Different device on different channels share the same file descriptor,
   and matched by file name; so, for example, the following is a good
   configuration description:

   open device audio-in as soundfile /tmp/sound_in.aiff mode read sample_rate 44100 channels 2 
   open device audio-out as soundfile /tmp/sound_out.aiff mode write sample_rate 44100 channels 2 

   Note that a sound file can be used either
   as input device or output device, but not together.

   Note that if you give paramenter that do not correspond to the file header, 
   you can get unexpected results.
*/



/* forward declarations */

static fts_status_t solaris_soundfile_open(fts_dev_t *dev, int nargs, fts_atom_t *args);
static fts_status_t solaris_soundfile_close(fts_dev_t *dev);

static fts_status_t solaris_soundfile_put(fts_dev_t *dev, float **buf, int n);
static fts_status_t solaris_soundfile_get(fts_dev_t *dev, float **buf, int n);

static fts_status_t solaris_soundfile_activate(fts_dev_t *dev);
static fts_status_t solaris_soundfile_deactivate(fts_dev_t *dev);
static int          solaris_soundfile_get_nchans(fts_dev_t *dev);



static fts_status_t
solaris_soundfile_init(void)
{
#ifndef SOLARIS2_PORTAGE
  fts_dev_class_t *solaris_soundfile_class;

  /* dac file */

  solaris_soundfile_class = fts_dev_class_new(fts_sig_dev);

  /* Installation of all the device class functions */

  set_open_fun(solaris_soundfile_class, solaris_soundfile_open);
  set_close_fun(solaris_soundfile_class, solaris_soundfile_close);

  set_sig_dev_put_fun(solaris_soundfile_class, solaris_soundfile_put);
  set_sig_dev_get_fun(solaris_soundfile_class, solaris_soundfile_get);

  set_sig_dev_activate_fun(solaris_soundfile_class, solaris_soundfile_activate);
  set_sig_dev_deactivate_fun(solaris_soundfile_class, solaris_soundfile_deactivate);
  set_sig_dev_get_nchans_fun(solaris_soundfile_class, solaris_soundfile_get_nchans);

  /* Install the device class */

  return fts_dev_class_register(fts_new_symbol("soundfile"), solaris_soundfile_class);
#endif
}




/* SOUNDFILE files have a data structure that is set as the device local
   data.

   We support mono and stero and quad files.

   Parameters are channels/sample_rate/vectorsize

*/

struct soundfile_data
{
  /* audio file */
#ifndef SOLARIS2_PORTAGE

  AFfilesetup   filesetup;	/* audio file setup                  */
  AFfilehandle  file;		/* audio file handle                 */
  fts_symbol_t     *file_name;	/* the file name */

  /* configuration */

  int nch;			/* number of channels*/
  int vector_size;		/* needed to compute the formatting buffer */
  int sample_rate;		/* needed only for the file format/header  */
  enum {soundfile_read_only, soundfile_write_only} mode; /* the file mode */

  /* housekeeping */

  short *soundfile_fmtbuf;	/* buffer to format stereo/quad sample frames, allocated in device_open */
  int active;			/* 1 if the data must be saved, 0 if the data can be ignored */
#endif
  int eof;
};

/* Parameter setting and parsing functions */

static fts_status_t 
soundfile_set_channels_action(fts_dev_t *dev, fts_atom_t *value)
{
#ifndef SOLARIS2_PORTAGE
  struct soundfile_data *dev_data;

  if (! fts_is_long(value))
    return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */

  dev_data = fts_dev_get_device_data(dev);
  dev_data->nch  = fts_get_long(value);
  if ((dev_data->nch != 1) && 
      (dev_data->nch != 2) &&
      (dev_data->nch != 4)) 
      dev_data->nch = 2;

  AFinitchannels(dev_data->filesetup, AF_DEFAULT_TRACK, dev_data->nch); 
#endif
  return fts_Success;
}

static fts_status_t 
soundfile_set_vector_size_action(fts_dev_t *dev, fts_atom_t *value)
{
#ifndef SOLARIS2_PORTAGE
  if (fts_is_long(value))
    {
      struct soundfile_data *dev_data;

      dev_data = fts_dev_get_device_data(dev);
      dev_data->vector_size = fts_get_long(value);

      return fts_Success;
    }
    else
      return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */
#else
  return fts_Success;
#endif
}

static fts_status_t 
soundfile_set_sample_rate_action(fts_dev_t *dev, fts_atom_t *value)
{
#ifndef SOLARIS2_PORTAGE
  if (fts_is_long(value))
    {
      struct soundfile_data *dev_data;

      dev_data = fts_dev_get_device_data(dev);
      dev_data->sample_rate = fts_get_long(value);
      AFinitrate(dev_data->filesetup, AF_DEFAULT_TRACK, (double) dev_data->sample_rate); 

      return fts_Success;
    }
    else
      return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */
#endif
}

static fts_status_t 
soundfile_set_mode_action(fts_dev_t *dev, fts_atom_t *value)
{
#ifndef SOLARIS2_PORTAGE
  if (fts_is_symbol(value))
    {
      struct soundfile_data *dev_data;

      dev_data = fts_dev_get_device_data(dev);

      if (fts_get_symbol(value) == fts_new_symbol("write"))
	dev_data->mode = soundfile_write_only;
      else
	dev_data->mode = soundfile_read_only;
      return fts_Success;
    }
    else
      return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */
#endif
}


static fts_status_t 
soundfile_set_end_action(fts_dev_t *dev, fts_atom_t *value)
{
  return fts_Success;
}

/*struct command_entry  solaris_soundfile_command_table[] =
{
  {"set", "channels",    soundfile_set_channels_action},
  {"set", "vector_size", soundfile_set_vector_size_action},
  {"set", "sample_rate", soundfile_set_sample_rate_action},
  {"set", "mode",        soundfile_set_mode_action},
  {"set", "end",         soundfile_set_end_action},
  {0,0,0}
};*/

static fts_status_t
solaris_soundfile_open(fts_dev_t *dev, int nargs, fts_atom_t *args)
{
#ifndef SOLARIS2_PORTAGE
  if ((nargs >= 1) && fts_is_symbol(&args[0]))
    {
      struct soundfile_data *dev_data;
      struct parser_status solaris_soundfile_parser_status; 
      fts_atom_t local_args[10];
      int local_nargs;			/* X/Motif style arg counter */

      /* make the structure */

      dev_data = (struct soundfile_data *) fts_malloc(sizeof(struct soundfile_data));
      fts_dev_set_device_data(dev, dev_data);

      /* create the setup, initialize the right fields and so on */

      dev_data->filesetup    = AFnewfilesetup();
      AFinitfilefmt(dev_data->filesetup, AF_FILE_AIFFC); 
      AFinitsampfmt(dev_data->filesetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, (long) 16); /*in bits */

      /* get the file name */

      dev_data->file_name = fts_get_symbol(&args[0]);
      dev_data->active = 0;
      dev_data->eof = 0;

			/* parse the other parameters */
			dev_data->nch = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);
			if ((dev_data->nch != 1) && (dev_data->nch != 2) &&	(dev_data->nch != 4)) 
				dev_data->nch = 2;


      /* allocate the formatting buffer size and zero it */

      dev_data->soundfile_fmtbuf = (short *)fts_malloc(MAXVS * dev_data->nch *sizeof(short));

      /* Open the audio file  */
      /* should add check on the result !!! */

      if (dev_data->mode == soundfile_write_only)
	dev_data->file = AFopenfile(fts_symbol_name(dev_data->file_name), "w", dev_data->filesetup);
      else
	dev_data->file = AFopenfile(fts_symbol_name(dev_data->file_name), "r", 0);

      AFfreefilesetup(dev_data->filesetup); 

      return fts_Success;
    }
  else
    return &fts_dev_open_error;	/* Error: cannot open device */
#else
  return fts_Success;
#endif
}

static fts_status_t
solaris_soundfile_close(fts_dev_t *dev)
{
#ifndef SOLARIS2_PORTAGE
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);

  /* close the audio file */

  AFclosefile(dev_data->file);   /* this is important: it updates the file header !!! */

#endif
  return fts_Success;
}


static fts_status_t 
solaris_soundfile_activate(fts_dev_t *dev)
{
#ifndef SOLARIS2_PORTAGE
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 1;
#endif
  return fts_Success;
}


static fts_status_t 
solaris_soundfile_deactivate(fts_dev_t *dev)
{
#ifndef SOLARIS2_PORTAGE
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 0;
#endif
  return fts_Success;
}


static int
solaris_soundfile_get_nchans(fts_dev_t *dev)
{
#ifndef SOLARIS2_PORTAGE
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  return dev_data->nch;
#else
  return 0;
#endif
}


#define MAX_VALUE (32767)				/* 2^15 - 1*/

static fts_status_t
solaris_soundfile_put(fts_dev_t *dev, float **buf, int n)
{
#ifndef SOLARIS2_PORTAGE
  struct soundfile_data *dev_data;
  short *out;
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  dev_data = fts_dev_get_device_data(dev);

  /* exit if not active or if read only: No error is signalled in any case */

  if ((! dev_data->active) || (dev_data->active != soundfile_write_only))
    return fts_Success;

  nchans = dev_data->nch;
  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;
  out = dev_data->soundfile_fmtbuf;

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *in;
      
      in = buf[ch];

      for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	{
	  float f1, f2, f3, f4;

	  f1 = in[j];
	  f2 = in[j + 1];
	  f3 = in[j + 2];
	  f4 = in[j + 3];

	  out[i] = f1 * MAX_VALUE;
	  out[i + off2] = f2 * MAX_VALUE;
	  out[i + off3] = f3 * MAX_VALUE;
	  out[i + off4] = f4 * MAX_VALUE;
	}
    }
      
  AFwriteframes(dev_data->file, AF_DEFAULT_TRACK, out, (long) n);
#endif
  return fts_Success;
}

/* If the file is finished, we just return 0s, no error are signalled */

static fts_status_t
solaris_soundfile_get(fts_dev_t *dev, float **buf, int n)
{
#ifndef SOLARIS2_PORTAGE
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);

  /* return all zeros if not active or if write only or in the eof case */

  if (dev_data->active && (dev_data->active == soundfile_read_only) && (! dev_data->eof))
    {
      short *in;
      int i,j;
      int off2, off3, off4;
      int nchans, ch, inc;
      int ret;

      nchans = dev_data->nch;
      off2 = nchans;
      off3 = 2 * nchans;
      off4 = 3 * nchans;
      inc  = 4 * nchans;
      in   = dev_data->soundfile_fmtbuf;

      /* read the buffers */

      ret = AFreadframes(dev_data->file, AF_DEFAULT_TRACK, in, (long) n);

      /* if we read less frames than expected, just add zeros at the end;
	 this should normally happen at the end of the file */

      if (ret < n * nchans)
	{
	  int i;

	  for (i = ret * nchans; i < n * nchans; i++)
	    in[i] = (short)0;

	  dev_data->eof = 1;
	}


      /* do the data transfer: unrolled pipelined loop */

      for (ch = 0; ch < nchans; ch++)
	{
	  float *out;
      
	  out = buf[ch];

	  for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	    {
	      float f1, f2, f3, f4;

	      f1 = in[i] / ((double) MAX_VALUE);
	      f2 = in[i + off2]/ ((double) MAX_VALUE);
	      f3 = in[i + off3]/ ((double) MAX_VALUE);
	      f4 = in[i + off4]/ ((double) MAX_VALUE);

	      out[j] = f1;
	      out[j + 1] = f2;
	      out[j + 2] = f3;
	      out[j + 3] = f4;
	    }
	}
    }
  else
    {
      int i;
      int nchans, ch;

      nchans = dev_data->nch;

      /* Set the whole buffer at zeros */
      for (ch = 0; ch < nchans; ch++)
	for(i = 0; i < n; i++)
	  (buf[ch])[i] = 0.0f;
    }
#endif
  return fts_Success;
}
