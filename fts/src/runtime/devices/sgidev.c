/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* This file include the device specific to the SGI architecture.
   Initially only audio and MIDI.

   These audio driver work for any number of channels channels.

   A Device class is defined for DACs and ADCs and device class for 
   MIDI i/o.

   A central structure keep track of the consistency of the whole business.
*/

/* Include files */

#include <unistd.h>
#include <audio.h>
#include <audiofile.h>
#include <dmedia/midi.h>
#include <sys/types.h>
#include <bstring.h>
#include <sys/time.h>

#include "sys.h"
#include "lang.h"
#include "runtime/devices.h"

/* forward declarations */

static void sgi_dac_init(void);
static void sgi_adc_init(void);
static void sgi_midi_init(void); 
static void sgi_soundfile_init(void);

/******************************************************************************/
/*                                                                            */
/*                              Error Definition                              */
/*                                                                            */
/******************************************************************************/

static fts_status_description_t sgi_no_midi =
{
  "No Midi port configured"
};

/******************************************************************************/
/*                                                                            */
/*                              Module Definition                             */
/*                                                                            */
/******************************************************************************/

/* Module definition and init function */

void
sgidev_init(void)
{
  sgi_dac_init();
  sgi_adc_init();
  sgi_midi_init(); 
  sgi_soundfile_init();
}



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

/*
 * Channels housekeeping structure
 */

static struct sgi_dac_data_struct
{
  ALconfig   config;		/* the audio configuration structure */
  ALport     port;		/* the audio port */

  int nch;			/* number of channels; used also to compute the fifo_size */
  int fifo_size;		/* the fifo size is stored in samples frames*/

  fts_dev_t *dac_dev;		/* the device itself, stored here gainst double instantiation */

  float *dac_fmtbuf;		/* buffer to format stereo/quad sample frames, allocated in the device open */
} sgi_dac_data;



/* Init functions */

static void sgi_dac_init(void)
{
  fts_dev_class_t *sgi_dac_class;

  /* defaults and init for the shared data */

  sgi_dac_data.dac_dev = 0;
  sgi_dac_data.nch = 2;
    
  /* SGI DAC class  */

  sgi_dac_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("sgi_dac"));

  fts_dev_class_set_open_fun(sgi_dac_class, sgi_dac_open);
  fts_dev_class_set_close_fun(sgi_dac_class, sgi_dac_close);
  fts_dev_class_sig_set_put_fun(sgi_dac_class, sgi_dac_put);
  fts_dev_class_sig_set_get_nchans_fun(sgi_dac_class, sgi_dac_get_nchans);
}

/* SGI DAC control/options functions */

/*
   The dev_ctrl operation for the sgi_dac
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the sgi dac.

   NOTE that for the SGI you *have* to set the sample_rate using
   the dedicated UCS command (audio-set sample-rate), or with the Audiopanel,
   not the sgi_dac.

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in samples frame units

*/


/* SGI DAC dev class functions */

static fts_status_t
sgi_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_atom_t local_args[10];
  int local_nargs;			/* X/Motif style arg counter */

  if (sgi_dac_data.dac_dev)
    return &fts_dev_open_error; /*Error: a device was already opened for the ch */

  sgi_dac_data.dac_dev = dev;

  /* Create the configuration object, and initialize some fields */

  sgi_dac_data.config = ALnewconfig();

  /* Set float representation */

  ALsetsampfmt(sgi_dac_data.config, AL_SAMPFMT_FLOAT);
  ALsetfloatmax(sgi_dac_data.config, 1.1f); /*
					      10 % of tollerance beacause the conversion
					      is not saturating
					    */
  ALsetwidth(sgi_dac_data.config, AL_SAMPLE_16); /* just to possibly quit purify :-> */

  /* Parse the paramenters */

  ALsetqueuesize(sgi_dac_data.config, 2048); /* default value */

  sgi_dac_data.nch = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);
  ALsetchannels(sgi_dac_data.config, sgi_dac_data.nch);

  sgi_dac_data.fifo_size = fts_param_get_int(fts_s_fifo_size, 256);
  ALsetqueuesize(sgi_dac_data.config, sgi_dac_data.nch * sgi_dac_data.fifo_size);

  /* Assume the config structure is ok, after all the configuration parameters,
     and open the audio port
   */

  sgi_dac_data.port = ALopenport("sgi_dac", "w", sgi_dac_data.config); 

  /* Allocate the DAC  formatting buffer */

  sgi_dac_data.dac_fmtbuf = (float *) fts_malloc(MAXVS * sgi_dac_data.nch * sizeof(float));

  return fts_Success;
}


static fts_status_t
sgi_dac_close(fts_dev_t *dev)
{
  /* Check that the sgi dac class is open */

  if (! sgi_dac_data.dac_dev)
    return &fts_dev_close_error; /* error: the class was not open */

  sgi_dac_data.dac_dev = 0;

  /* free the config data */

  ALfreeconfig(sgi_dac_data.config);

  /* free the audio formatting buffer */

  fts_free(sgi_dac_data.dac_fmtbuf);

  /* close the audio output port */

  ALcloseport(sgi_dac_data.port);

  return fts_Success;
}

static int
sgi_dac_get_nchans(fts_dev_t *dev)
{
  return sgi_dac_data.nch;
}

/* fts_dev_t *dev, int n, float *buf1 ... bufm 
   the device is ignored (only one device for the moment allowed)

*/

static void
sgi_dac_put(fts_word_t *argv)
{
  long n = fts_word_get_long(argv + 2);
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  nchans = fts_word_get_long(argv + 1);
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

	  sgi_dac_data.dac_fmtbuf[i] = f1;
	  sgi_dac_data.dac_fmtbuf[i + off2] = f2;
	  sgi_dac_data.dac_fmtbuf[i + off3] = f3;
	  sgi_dac_data.dac_fmtbuf[i + off4] = f4;
	}
    }

  ALwritesamps(sgi_dac_data.port, sgi_dac_data.dac_fmtbuf, n * nchans);
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


static struct sgi_adc_data_struct
{
  ALconfig   config;		/* the audio configuration structure */
  ALport     port;		/* the audio port */

  int nch;			/* number of channels; used only to compute the fifo_size */
  int fifo_size;		/* the fifo size is stored in sample frames */

  fts_dev_t *adc_dev;		/* the device itself, stored here gainst double instantiation */

  float *adc_fmtbuf;		/* buffer to format stereo/quad sample frames, allocated in class_open */
} sgi_adc_data;


/* Init  function */

static void 
sgi_adc_init(void)
{
  fts_dev_class_t *sgi_adc_class;

  /* defaults and init for the shared data */

  sgi_adc_data.adc_dev = 0;
  sgi_adc_data.nch = 2;
    
  /* SGI ADC class  */

  sgi_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("sgi_adc"));

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

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in sample frame units
*/


/* SGI ADC dev class functions */

static fts_status_t
sgi_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_atom_t local_args[10];
  int local_nargs;			/* X/Motif style arg counter */

  if (sgi_adc_data.adc_dev)
    return &fts_dev_open_error; /*Error: a device was already opened for the ch */

  /* Create the configuration object, and initialize some fields */

  sgi_adc_data.config = ALnewconfig();

  /* Set float representation */

  ALsetsampfmt(sgi_adc_data.config, AL_SAMPFMT_FLOAT);
  ALsetfloatmax(sgi_adc_data.config, 1.1f); 

  /* Set defaults for the other values */

  ALsetwidth(sgi_dac_data.config, AL_SAMPLE_16); /* just to possibly quit purify :-> */

  sgi_adc_data.nch = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);
  ALsetchannels(sgi_dac_data.config, sgi_dac_data.nch);

  sgi_adc_data.fifo_size = fts_param_get_int(fts_s_fifo_size, 256);
  ALsetqueuesize(sgi_dac_data.config, sgi_dac_data.nch * sgi_dac_data.fifo_size);

  /* Assume the config structure is ok, after all the configuration parameters,
     and open the audio port
   */

  sgi_adc_data.port = ALopenport("sgi_adc", "r", sgi_adc_data.config); 

  /* Allocate the ADC  formatting buffer */

  sgi_adc_data.adc_fmtbuf = (float *) fts_malloc(MAXVS * sgi_adc_data.nch * sizeof(float));

  return fts_Success;
}

static fts_status_t
sgi_adc_close(fts_dev_t *dev)
{
  /* Check that the sgi adc class is open */

  if (! sgi_adc_data.adc_dev)
    return &fts_dev_close_error; /* error: the class was not open */

  sgi_adc_data.adc_dev = 0;

  /* close the audio output port */

  ALcloseport(sgi_adc_data.port);

  /* free the config data */

  ALfreeconfig(sgi_adc_data.config);

  /* free the audio formatting buffer */

  fts_free(sgi_adc_data.adc_fmtbuf);


  return fts_Success;
}


static int
sgi_adc_get_nchans(fts_dev_t *dev)
{
  return sgi_adc_data.nch;
}


/* fts_dev_t *dev, int n, float *buf1 ... bufm 
   the device is ignored (only one device for the moment allowed)

*/

static void
sgi_adc_get(fts_word_t *argv)
{
  long n = fts_word_get_long(argv + 2);
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  nchans = fts_word_get_long(argv + 1);
  off2 = nchans;
  off3 = 2 * nchans;
  off4 = 3 * nchans;
  inc  = 4 * nchans;

  /* if it is the first device of the tick, read the frame in the 
     buffer */

  ALreadsamps(sgi_adc_data.port, sgi_adc_data.adc_fmtbuf, n * nchans);

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < nchans; ch++)
    {
      float *out;
      
      out = (float *) fts_word_get_ptr(argv + 3 + ch);

      for (i = ch, j = 0; j < n; i = i + inc, j += 4)
	{
	  float f1, f2, f3, f4;

	  f1 = sgi_adc_data.adc_fmtbuf[i];
	  f2 = sgi_adc_data.adc_fmtbuf[i + off2];
	  f3 = sgi_adc_data.adc_fmtbuf[i + off3];
	  f4 = sgi_adc_data.adc_fmtbuf[i + off4];

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

/* The MIDI device, called "sgi_midi" support input and output on the same device */

/* Forward declaration of midi device/class functions */

static fts_status_t sgi_midi_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t sgi_midi_close(fts_dev_t *dev);

static fts_status_t sgi_midi_get(fts_dev_t *dev, unsigned char *cp);
static fts_status_t sgi_midi_put(fts_dev_t *dev, unsigned char c);

static fts_status_t 
sgi_midi_init(void)
{
  fts_dev_class_t *sgi_midi_dev_class;

  if (mdInit() <= 0)
    return & sgi_no_midi;

  /* adding device functions: the device support only basic 
   character i/o; no callback functions, no sync functions */

  sgi_midi_dev_class = fts_dev_class_new(fts_char_dev, fts_new_symbol("sgi_midi"));

  fts_dev_class_set_open_fun(sgi_midi_dev_class, sgi_midi_open);
  fts_dev_class_set_close_fun(sgi_midi_dev_class, sgi_midi_close);
  fts_dev_class_char_set_get_fun(sgi_midi_dev_class, sgi_midi_get);
  fts_dev_class_char_set_put_fun(sgi_midi_dev_class, sgi_midi_put);
}


/* The instantiation function has one keyword argument "port", the
   name of the MIDI port; it is an optional argument; we don't do any
   locking on the MIDI port 
   */

#define MAX_OUT_SYSEX 1024
#define MAX_MIDI_EV 32

typedef struct
{
  MDport in_port;
  MDport out_port;

  /* input unparsing support */

  enum {input_waiting, reading_message, reading_sysex} input_status;
  unsigned char in_buf[3];	
  unsigned char *in_sysex_buf;

  MDevent in_midi_event[MAX_MIDI_EV];
  int in_current_char_count;
  int in_current_event_length;
  int in_current_event;
  int in_event_count;

  /* output parsing support */

  MDevent out_midi_event;

  enum {output_waiting, message_need_first, message_need_first_and_second,
	  message_need_second, getting_sysex } output_status;

  unsigned char out_sysex_buf[MAX_OUT_SYSEX];
  unsigned int out_sysex_count;
} sgimidi_dev_data;


static fts_status_t
sgi_midi_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  sgimidi_dev_data *data;
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
	fts_symbol_t s;

	s = fts_get_symbol(&args[i]);

	if (s == fts_new_symbol("port"))
	  port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
	else if (s == fts_new_symbol("in_port"))
	  in_port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
	else if (s == fts_new_symbol("out_port"))
	  out_port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
      }



  /* allocate the device data and open the input and output port */

  data = fts_malloc(sizeof(sgimidi_dev_data));
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
}

static fts_status_t
sgi_midi_close(fts_dev_t *dev)
{
  sgimidi_dev_data *data = (sgimidi_dev_data *)dev->device_data;

  /* Send a all not off, and close the port */

  mdPanic(data->out_port);
  mdClosePort(data->out_port);
  mdClosePort(data->in_port);

  fts_free(data);

  return fts_Success;
}

/* I/O functions */

static void
sgi_midi_prepare_next_event(sgimidi_dev_data *data)
{
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
}

static fts_status_t
sgi_midi_get(fts_dev_t *dev, unsigned char *cp)
{
  sgimidi_dev_data *data = (sgimidi_dev_data *)dev->device_data;

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
	    sgi_midi_prepare_next_event(data);
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
	    sgi_midi_prepare_next_event(data);
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

		sgi_midi_prepare_next_event(data);

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

  return fts_Success;
}


static fts_status_t
sgi_midi_put(fts_dev_t *dev, unsigned char c)
{
  int ret;
  sgimidi_dev_data *data = (sgimidi_dev_data *)dev->device_data;

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
	  data->out_midi_event.msg[0] = MD_SYSEX; 
	  mdSend(data->out_port, &(data->out_midi_event), 1);

	  /* keep status to sysex, setting count to 0 and msg[0] to 0xf0 */

	  data->out_sysex_count = 0;

	  data->output_status = getting_sysex;
	}
      else if (c == MD_EOX) 
	{
	  /* store char */

	  data->out_sysex_buf[data->out_sysex_count++] = c;

	  /* send */

	  data->out_midi_event.msg[0] = MD_SYSEX; 
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

  return fts_Success;
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



static fts_status_t
sgi_soundfile_init(void)
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
  long n = fts_word_get_long(argv + 2);
  struct soundfile_data *dev_data;
  short *out;
  int i,j;
  int off2, off3, off4;
  int nchans, ch, inc;

  dev_data = fts_dev_get_device_data(dev);

  /* exit if not active or if read only: No error is signalled in any case */

  if ((! dev_data->active) || (dev_data->active != soundfile_write_only))
    return;

  nchans = fts_word_get_long(argv + 1);
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
  long n = fts_word_get_long(argv + 2);
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

      nchans = fts_word_get_long(argv + 1);
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
	{
	  float *out;

	  out = (float *) fts_word_get_ptr(argv + 3 + ch);	  

	  for(i = 0; i < n; i++)
	    out[i] = 0.0f;
	}
    }
}
