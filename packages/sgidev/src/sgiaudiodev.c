/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* This file include the device specific to the SGI architecture IRIX 6.4 with the new
   audio library.

   Initially only audio.

   These audio driver work for any number of channels channels, 

   A Device class is defined for DACs and ADCs and device class.

   A central structure keep track of the consistency of the whole business.
*/

/* Include files */


#include <unistd.h>
#ifdef IRIX6_3
#include <sys/dmcommon.h>	/* for stamp_t on IRIX 6.3 */
#endif
#include <dmedia/audio.h>
#include <dmedia/audiofile.h>

#include <fts/fts.h>

/******************************************************************************/
/*                                                                            */
/*                              DAC Devices                                   */
/*                                                                            */
/******************************************************************************/


/* Forward declarations of DAC dev and dev class static functions */

static fts_status_t sgi_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t sgi_dac_close(fts_dev_t *dev);
static void         sgi_dac_put(fts_word_t *args);
static int          sgi_dac_get_nchans(fts_dev_t *dev);
static int          sgi_dac_get_nerrors(fts_dev_t *dev);

/*
 * Channels housekeeping structure
 */

typedef struct 
{
  ALconfig   config;		/* the audio configuration structure */
  ALport     port;		/* the audio port */
  stamp_t    fts_frames;	/* out frame number, for slip detection */
  const char *name;		/* the AL2.0 device name (may be usefull) */
  int nch;			/* number of channels; used also to compute the fifo_size */
  int fifo_size;		/* the fifo size is stored in samples frames*/

  float *dac_fmtbuf;		/* buffer to format stereo/quad sample frames, allocated in the device open */
} sgi_dac_data_t;



/* Init functions */

static void sgi_dac_init(void)
{
  fts_dev_class_t *sgi_dac_class;

  /* SGI DAC class  */

  sgi_dac_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("SgiALOut"));

  /* device functions */

  fts_dev_class_set_open_fun(sgi_dac_class, sgi_dac_open);
  fts_dev_class_set_close_fun(sgi_dac_class, sgi_dac_close);

  fts_dev_class_sig_set_put_fun(sgi_dac_class, sgi_dac_put);

  fts_dev_class_sig_set_get_nchans_fun(sgi_dac_class, sgi_dac_get_nchans);

  fts_dev_class_sig_set_get_nerrors_fun(sgi_dac_class, sgi_dac_get_nerrors);
}

/* SGI DAC control/options functions */

/*
   The dev_ctrl operation for the sgi_dac
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the sgi dac.

   NOTE that for the SGI you *have* to set the sample_rate using
   the dedicated UCS command (audio-set sample-rate) , not the sgi_dac.

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in samples frame units

   ALdevice     the audio library device used        symbol  <default-device)  (the device name supported
                                                                                by the SGI AL2.0).  

*/


/* SGI DAC dev class functions */

static fts_status_t 
sgi_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int deviceId;
  int ret;
  fts_symbol_t sym;
  sgi_dac_data_t *dev_data;

  fts_atom_t local_args[10];
  int local_nargs;			/* X/Motif style arg counter */

  /* it should check for double opening ???*/

  dev_data = (sgi_dac_data_t *)fts_malloc(sizeof(sgi_dac_data_t));

  fts_dev_set_device_data(dev, dev_data);

  /* Create the configuration object, and initialize some fields */

  dev_data->config = alNewConfig();

  /* Set float representation */

  ret = alSetSampFmt(dev_data->config, AL_SAMPFMT_FLOAT);

  if (ret != 0)
    {
      if (ret == AL_BAD_CONFIG)
	post("alSetSampFmt bad config\n");
      else if (ret == AL_BAD_SAMPFMT)
	post("alSetSampFmt bad sampgmt\n");
    }
  
 /* We set a 10 % of tollerance beacause the conversion
    is not saturating; should probabily use the limiter
    */

  ret = alSetFloatMax(dev_data->config, 1.1f); /*
					      10 % of tollerance beacause the conversion
					      is not saturating; should probabily use the limiter
					    */

  if (ret != 0)
    {
      if (ret == AL_BAD_CONFIG)
	post("alSetFloatMax: bad config\n");
      else if (ret == AL_BAD_FLOATMAX)
	post("alSetFloatMax: bad FloatMax\n");
    }


  /* sample width */

  ret = alSetWidth(dev_data->config, AL_SAMPLE_24);

  if (ret != 0)
    {
      if (ret == AL_BAD_CONFIG)
	post("alSetWidth: bad config\n");
      else if (ret == AL_BAD_WIDTH)
	post("alSetWidth: bad width\n");
    }

  /* Parameter parsing  and setting: channels */

  dev_data->nch = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);
  ret = alSetChannels(dev_data->config, dev_data->nch); 

  if (ret != 0)
    {
      if (ret == AL_BAD_CONFIG)
	post("alSetChannels: bad config\n");
      else if (ret == AL_BAD_CHANNELS)
	post("alSetChannels: bad channels\n");
    }


  /* Parameter parsing  and setting: fifo size */

  dev_data->fifo_size = fts_param_get_int(fts_s_fifo_size, 256);

  if (dev_data->fifo_size)
    {
      int ret;

      ret = alSetQueueSize(dev_data->config, dev_data->fifo_size);

      if (ret != 0)
	{
	  if (ret == AL_BAD_CONFIG)
	    post("alSetQueueSize: bad config\n");
	  else if (ret == AL_BAD_QSIZE)
	    post("alSetQueueSize: bad qsize\n");
	}
    }

  /* Parameter parsing  and setting: device name */

  sym = fts_get_symbol_by_name(nargs, args, fts_new_symbol("ALdevice"), 0);
  
  if (sym)
    {
      dev_data->name = fts_symbol_name(sym);

      deviceId = alGetResourceByName(AL_SYSTEM, (char *)dev_data->name, AL_DEVICE_TYPE);

      if (! deviceId)
	{
	  return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */
	}

      alSetDevice(dev_data->config, deviceId);
    }

  /* Assume the config structure is ok, after all the configuration parameters,
     and open the audio port
   */

  dev_data->port = alOpenPort("fts_sgi_dac", "w", dev_data->config); 

  if (! dev_data->port)
    {
      /* find out what the error is and post a warning, then return an error */

      post("Error opening device SgiALOut: ");

      switch (oserror())
	{
	case AL_BAD_CONFIG:
	  post(" config is invalid.\n");
	  break;

	case AL_BAD_DIRECTION:
	  post("direction is neither r nor w.\n");
	  break;

	case AL_BAD_OUT_OF_MEM:
	  post("insufficient memory is available to allocate the ALport.\n");
	  break;

	case AL_BAD_DEVICE_ACCESS:
	  post("audio hardware is not available, or is improperly configured.\n");
	  break;

	case AL_BAD_DEVICE:
	  post("the device given in the config is bad, either\n");
	  post("because it does not exist, or because it has the\n");
	  post("wrong direction (input vs. output).\n");
	  break;

	case AL_BAD_NO_PORTS:
	  post("no audio ports are currently available.\n");
	  break;

	case AL_BAD_QSIZE:
	  post("Bad fifo-size\n");
	  break;

	default:
	  post("Unknown error %d\n", oserror());
	  break;
	}

      return &fts_dev_open_error;
    }

  /* record the dac out frame number */
  {
    stamp_t al_time;		/* ignore */

    alGetFrameTime(dev_data->port, &(dev_data->fts_frames), &al_time);
  }

  /* Allocate the DAC  formatting buffer */

  dev_data->dac_fmtbuf = (float *) fts_malloc(fts_dsp_get_tick_size() * dev_data->nch * sizeof(float));

  /* Install this device as dac slip check device;
     this means that if multiple sgidac device are opened, the
     last opened is checked; anyway, since all the device must
     be syncronized, this make no difference */

  fts_dsp_set_dac_slip_dev(dev);

  return fts_Success;
}


static fts_status_t
sgi_dac_close(fts_dev_t *dev)
{
  sgi_dac_data_t *dev_data = (sgi_dac_data_t *) fts_dev_get_device_data(dev);

  /* free the config data */

  alFreeConfig(dev_data->config);

  /* free the audio formatting buffer */

  fts_free(dev_data->dac_fmtbuf);

  /* close the audio output port */

  alClosePort(dev_data->port);

  /* free the device data */

  fts_free(dev_data);

  return fts_Success;
}

static int
sgi_dac_get_nchans(fts_dev_t *dev)
{
  sgi_dac_data_t *dev_data = (sgi_dac_data_t *) fts_dev_get_device_data(dev);

  return dev_data->nch;
}


static int
sgi_dac_get_nerrors(fts_dev_t *dev)
{
  sgi_dac_data_t *dev_data = (sgi_dac_data_t *) fts_dev_get_device_data(dev);
  stamp_t al_frames;
  stamp_t al_time;

  alGetFrameTime(dev_data->port, &al_frames, &al_time);

  if (al_frames > (dev_data->fts_frames + dev_data->fifo_size))
    {
      int ret; 

      ret = al_frames - (dev_data->fts_frames + dev_data->fifo_size);
      dev_data->fts_frames = al_frames;
      return ret;
    }
  else
    return 0;
}


/*
  fts_dev_t *dev, int n, float *buf1 ... bufm 
*/


static void
sgi_dac_put(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  sgi_dac_data_t *dev_data;

  long n = fts_word_get_int(argv + 2);
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  dev_data = fts_dev_get_device_data(dev);

  dev_data->fts_frames += n;	/* count for the slip detection */

  nchans = fts_word_get_int(argv + 1);
  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *in;
      
      in = (float *) fts_word_get_ptr(argv + 3 + ch);

      for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	{
	  float f1, f2, f3, f4;

	  f1 = in[j];
	  f2 = in[j + 1];
	  f3 = in[j + 2];
	  f4 = in[j + 3];

	  dev_data->dac_fmtbuf[i] = f1;
	  dev_data->dac_fmtbuf[i + off2] = f2;
	  dev_data->dac_fmtbuf[i + off3] = f3;
	  dev_data->dac_fmtbuf[i + off4] = f4;
	}
    }

  alWriteFrames(dev_data->port, dev_data->dac_fmtbuf, n);
}


/******************************************************************************/
/*                                                                            */
/*                              ADC Devices                                   */
/*                                                                            */
/******************************************************************************/

/* Forward declarations of ADC dev and dev class static functions */

static fts_status_t sgi_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t sgi_adc_close(fts_dev_t *dev);
static void         sgi_adc_get(fts_word_t *args);

static int          sgi_adc_get_nchans(fts_dev_t *dev);


typedef struct
{
  ALconfig   config;		/* the audio configuration structure */
  ALport     port;		/* the audio port */

  const char *name;		/* the AL2.0 device name (may be usefull) */

  int nch;			/* number of channels; used only to compute the fifo_size */
  int fifo_size;		/* the fifo size is stored in sample frames */

  float *adc_fmtbuf;		/* buffer to format stereo/quad sample frames, allocated in class_open */
} sgi_adc_data_t;


/* Init  functions */

static void sgi_adc_init(void)
{
  fts_dev_class_t *sgi_adc_class;

  /* SGI ADC class  */

  sgi_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("SgiALIn"));

  /* device functions */

  fts_dev_class_set_open_fun(sgi_adc_class, sgi_adc_open);
  fts_dev_class_set_close_fun(sgi_adc_class, sgi_adc_close);
  fts_dev_class_sig_set_get_fun(sgi_adc_class, sgi_adc_get);

  fts_dev_class_sig_set_get_nchans_fun(sgi_adc_class, sgi_adc_get_nchans);
}

/* SGI ADC control/options functions */

/*
   The dev_ctrl operation for the sgi_adc
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the sgi adc.

   NOTE that for the SGI you *have* to set the sample_rate using
   the sgi device, not the sgi_adc.

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in sample frame units

   ALdevice       the device used        symbol  <default-device)  (the values supported by SGI AL2.0).  
*/


/* SGI ADC dev class functions */

static fts_status_t
sgi_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int ret;
  int deviceId;
  fts_atom_t local_args[10];
  int local_nargs;			/* X/Motif style arg counter */

  sgi_adc_data_t *dev_data;

  dev_data = (sgi_adc_data_t *)fts_malloc(sizeof(sgi_adc_data_t));
  fts_dev_set_device_data(dev, dev_data);

  /* Create the configuration object, and initialize some fields */

  dev_data->config = alNewConfig();

  /* Set float representation */

  alSetSampFmt(dev_data->config, AL_SAMPFMT_FLOAT);
  alSetFloatMax(dev_data->config, 1.1f); 

  /* Set defaults for the other values */

  alSetWidth(dev_data->config, AL_SAMPLE_24); 

  /* Parameter parsing and setting: channels */

  dev_data->nch = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);

  ret = alSetChannels(dev_data->config, dev_data->nch); 

  if (ret != 0)
    {
      if (ret == AL_BAD_CONFIG)
	post("alSetChannels: bad config\n");
      else if (ret == AL_BAD_CHANNELS)
	post("alSetChannels: bad channels\n");
    }

  /* Parameter parsing and setting: fifo size */


  dev_data->fifo_size = fts_param_get_int(fts_s_fifo_size, 256);

  if (dev_data->fifo_size)
    alSetQueueSize(dev_data->config, dev_data->fifo_size);

  /* Parameter parsing and setting: device */

  dev_data->name = fts_symbol_name(fts_get_symbol_by_name(nargs, args, fts_new_symbol("ALdevice"), fts_new_symbol(" ")));

  deviceId = alGetResourceByName(AL_SYSTEM, (char *)dev_data->name, AL_DEVICE_TYPE);

  if (! deviceId)
    return &fts_dev_invalid_value_error;/* error: invalid argument for parameter */

  alSetDevice(dev_data->config, deviceId);

  /* Assume the config structure is ok, after all the configuration parameters,
     and open the audio port
   */

  dev_data->port = alOpenPort("fts_sgi_adc", "r", dev_data->config); 

  if (! dev_data->port)
    {
      /* find out what the error is and post a warning, then return an error */

      post("Error opening device SgiALIn: ");

      switch (oserror())
	{
	case AL_BAD_CONFIG:
	  post(" config is invalid.\n");
	  break;

	case AL_BAD_DIRECTION:
	  post("direction is neither r nor w.\n");
	  break;

	case AL_BAD_OUT_OF_MEM:
	  post("insufficient memory is available to allocate the ALport.\n");
	  break;

	case AL_BAD_DEVICE_ACCESS:
	  post("audio hardware is not available, or is improperly configured.\n");
	  break;

	case AL_BAD_DEVICE:
	  post("the device given in the config is bad, either\n");
	  post("because it does not exist, or because it has the\n");
	  post("wrong direction (input vs. output).\n");
	  break;

	case AL_BAD_NO_PORTS:
	  post("no audio ports are currently available.\n");
	  break;

	default:
	  post("Unknown error %d\n", oserror());
	  break;
	}

      return &fts_dev_open_error;
    }


  /* Verify if there are samples coming on the adc; in practice, this means
     to test for the digital input carrier.
     The technique is simple: we verify if the input fifo is empty, if it is 
     empty we wait a few millisecond and we check again; if it is still empty,
     we have no carrier.
     */

  if (alGetFilled(dev_data->port) == 0)
    {
      struct timespec pause_time;

      pause_time.tv_sec = 0;
      pause_time.tv_nsec = 1000000;
      nanosleep( &pause_time, 0);

      if (alGetFilled(dev_data->port) == 0)
	{
	  post("No Carrier On Audio Input %s\n", dev_data->name);

	  return &fts_dev_open_error;
	}
    }


  /* Allocate the ADC  formatting buffer */

  dev_data->adc_fmtbuf = (float *) fts_malloc(fts_dsp_get_tick_size() * dev_data->nch * sizeof(float));

  return fts_Success;
}

static fts_status_t
sgi_adc_close(fts_dev_t *dev)
{
  sgi_adc_data_t *dev_data = (sgi_adc_data_t *) fts_dev_get_device_data(dev);

  /* close the audio output port */

  alClosePort(dev_data->port);

  /* free the config data */

  alFreeConfig(dev_data->config);

  /* free the audio formatting buffer */

  fts_free(dev_data->adc_fmtbuf);

  /* free the device data */

  fts_free(dev_data);

  return fts_Success;
}


static int
sgi_adc_get_nchans(fts_dev_t *dev)
{
  sgi_adc_data_t *dev_data = (sgi_adc_data_t *) fts_dev_get_device_data(dev);

  return dev_data->nch;
}

/* fts_dev_t *dev, int n, float *buf1 ... bufm 
   the device is ignored (only one device for the moment allowed)

*/

static void
sgi_adc_get(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  sgi_adc_data_t *dev_data;

  long n = fts_word_get_int(argv + 2);
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  dev_data = fts_dev_get_device_data(dev);

  nchans = fts_word_get_int(argv + 1);
  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;

  /* if it is the first device of the tick, read the frame in the 
     buffer */

  alReadFrames(dev_data->port, dev_data->adc_fmtbuf, n);

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *out;
      
      out = (float *) fts_word_get_ptr(argv + 3 + ch);

      for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	{
	  float f1, f2, f3, f4;

	  f1 = dev_data->adc_fmtbuf[i];
	  f2 = dev_data->adc_fmtbuf[i + off2];
	  f3 = dev_data->adc_fmtbuf[i + off3];
	  f4 = dev_data->adc_fmtbuf[i + off4];

	  out[j] = f1;
	  out[j + 1] = f2;
	  out[j + 2] = f3;
	  out[j + 3] = f4;
	}
    }
}

/******************************************************************************/
/*                                                                            */
/*                               FILE DAC Device                              */
/*                                                                            */
/******************************************************************************/

/* Test implementation for the SGI

   Support only AIFF files (AIFF-C are dropped by SGI), any number of
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

static fts_status_t sgi_soundfile_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t sgi_soundfile_close(fts_dev_t *dev);

static void sgi_soundfile_put(fts_word_t *argv);
static void sgi_soundfile_get(fts_word_t *argv);

static fts_status_t sgi_soundfile_activate(fts_dev_t *dev);
static fts_status_t sgi_soundfile_deactivate(fts_dev_t *dev);
static int          sgi_soundfile_get_nchans(fts_dev_t *dev);



static void sgi_soundfile_init(void)
{
  fts_dev_class_t *sgi_soundfile_class;

  /* dac file */

  sgi_soundfile_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("soundfile"));

  /* Installation of all the device class functions */

  fts_dev_class_set_open_fun(sgi_soundfile_class, sgi_soundfile_open);
  fts_dev_class_set_close_fun(sgi_soundfile_class, sgi_soundfile_close);

  fts_dev_class_sig_set_put_fun(sgi_soundfile_class, sgi_soundfile_put);
  fts_dev_class_sig_set_get_fun(sgi_soundfile_class, sgi_soundfile_get);

  fts_dev_class_sig_set_activate_fun(sgi_soundfile_class, sgi_soundfile_activate);
  fts_dev_class_sig_set_deactivate_fun(sgi_soundfile_class, sgi_soundfile_deactivate);
  fts_dev_class_sig_set_get_nchans_fun(sgi_soundfile_class, sgi_soundfile_get_nchans);
}




/* SOUNDFILE files have a data structure that is set as the device local
   data.

   We support mono and stero and quad files.

   Parameters are channels/sample_rate/vectorsize ???

*/

struct soundfile_data
{
  /* audio file */

  AFfilesetup   filesetup;	/* audio file setup                  */
  AFfilehandle  file;		/* audio file handle                 */
  fts_symbol_t  file_name;	/* the file name */

  /* configuration */

  int nch;			/* number of channels*/
  int sample_rate;		/* needed only for the file format/header  */
  enum {soundfile_read_only, soundfile_write_only} mode; /* the file mode */

  /* housekeeping */

  short *soundfile_fmtbuf;	/* buffer to format stereo/quad sample frames, allocated in device_open */
  int active;			/* 1 if the data must be saved, 0 if the data can be ignored */
  int eof;
};


static fts_status_t
sgi_soundfile_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  if ((nargs >= 1) && fts_is_symbol(&args[0]))
    {
      struct soundfile_data *dev_data;
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


      /* parse the other file parameters : channels */

      dev_data->nch = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);

      if ((dev_data->nch != 1) && 
	  (dev_data->nch != 2) &&
	  (dev_data->nch != 4)) 
	dev_data->nch = 2;

      AFinitchannels(dev_data->filesetup, AF_DEFAULT_TRACK, dev_data->nch); 

      /* parse the other file parameters : sample_rate  */

      dev_data->sample_rate = fts_get_int_by_name(nargs, args, fts_new_symbol("sample_rate"), 2);
      AFinitrate(dev_data->filesetup, AF_DEFAULT_TRACK, (double) dev_data->sample_rate); 

      /* parse the other file parameters : mode */

      if (fts_get_boolean_by_name(nargs, args, fts_new_symbol("write"), 0))
	dev_data->mode = soundfile_write_only;
      else
	dev_data->mode = soundfile_read_only;

      /* allocate the formatting buffer size and zero it */

      dev_data->soundfile_fmtbuf = (short *)fts_malloc(fts_dsp_get_tick_size() * dev_data->nch *sizeof(short));

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
}

static fts_status_t
sgi_soundfile_close(fts_dev_t *dev)
{
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);

  /* close the audio file */

  AFclosefile(dev_data->file);   /* this is important: it updates the file header !!! */

  return fts_Success;
}


static fts_status_t 
sgi_soundfile_activate(fts_dev_t *dev)
{
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 1;

  return fts_Success;
}


static fts_status_t 
sgi_soundfile_deactivate(fts_dev_t *dev)
{
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  dev_data->active = 0;

  return fts_Success;
}


static int
sgi_soundfile_get_nchans(fts_dev_t *dev)
{
  struct soundfile_data *dev_data;

  dev_data = fts_dev_get_device_data(dev);
  return dev_data->nch;
}


#define MAX_VALUE (32767)				/* 2^15 - 1*/

static void
sgi_soundfile_put(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  long n = fts_word_get_int(argv + 2);
  struct soundfile_data *dev_data;
  short *out;
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  dev_data = fts_dev_get_device_data(dev);

  /* exit if not active or if read only: No error is signalled in any case */

  if ((! dev_data->active) || (dev_data->active != soundfile_write_only))
    return;

  nchans = fts_word_get_int(argv + 1);
  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;
  out = dev_data->soundfile_fmtbuf;

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *in;
      
      in = (float *) fts_word_get_ptr(argv + 3 + ch);

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
}

/* If the file is finished, we just return 0s, no error are signalled */

static void
sgi_soundfile_get(fts_word_t *argv)
{
  fts_dev_t *dev = *((fts_dev_t **) fts_word_get_ptr(argv));
  long n = fts_word_get_int(argv + 2);
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

      nchans = fts_word_get_int(argv + 1);
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

	  out = (float *) fts_word_get_ptr(argv + 3 + ch);

	  for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	    {
	      float f1, f2, f3, f4;

	      f1 = in[i] / ((float) MAX_VALUE);
	      f2 = in[i + off2]/ ((float) MAX_VALUE);
	      f3 = in[i + off3]/ ((float) MAX_VALUE);
	      f4 = in[i + off4]/ ((float) MAX_VALUE);

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
	{
	  float *out;

	  out = (float *) fts_word_get_ptr(argv + 3 + ch);	  

	  for(i = 0; i < n; i++)
	    out[i] = 0.0f;
	}
    }
}


void sgi_audio_init()
{
  sgi_dac_init();
  sgi_adc_init();
  sgi_soundfile_init();
}
