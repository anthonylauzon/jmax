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
 * Author: Guenter Geiger (geiger@epy.co.at), François Déchelle (dechelle@ircam.fr)
 *
 */


/* This file include the FTS ALSA device.

   For now, this device is only:
    - an audio input device
    - an audio output device
*/

/* Include files */

#include <stdio.h>
#include <stdlib.h>

#include <sys/asoundlib.h>
#include <linux/asound.h>

#include "fts.h"

extern void fts_dsp_set_dac_slip_dev(fts_dev_t *dev);


/******************************************************************************/
/*                                                                            */
/* Audio devices                                                              */
/*                                                                            */
/******************************************************************************/

/*
  This structure will be stored in the "device_data" and will be
  available in any device function using fts_dev_get_device_data.

  It contains ALSA specific datas.
*/

#define CHANNEL_CNT 20


typedef struct alsa_pcm_dev_data {
  snd_pcm_t *handle;
  snd_pcm_t *pcm_handle[CHANNEL_CNT];
  int n_voices;
  long bytes_count;
  short *fmtbuf;
} alsa_pcm_dev_data_t;

#define DEFAULT_N_VOICES 1


/*----------------------------------------------------------------------------*/
/* Audio output device                                                        */
/*----------------------------------------------------------------------------*/

/* Forward declarations of device and device class functions */

static fts_status_t alsa_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t alsa_dac_close(fts_dev_t *dev);
static void         alsa_dac_put(fts_word_t *args);

static int          alsa_dac_get_nchans(fts_dev_t *dev);
static int          alsa_dac_get_nerrors(fts_dev_t *dev);


/* Init function */

static void alsa_dac_init(void)
{
  fts_dev_class_t *alsa_dac_class;
    
  /* Declaration of the device class */
  alsa_dac_class = fts_dev_class_new( fts_sig_dev, fts_new_symbol("alsa_dac"));

  /* Definition of the device class `open' function */
  fts_dev_class_set_open_fun( alsa_dac_class, alsa_dac_open);
  /* Definition of the device class `close' function */
  fts_dev_class_set_close_fun( alsa_dac_class, alsa_dac_close);
  /* Definition of the device class `put' function */
  fts_dev_class_sig_set_put_fun( alsa_dac_class, alsa_dac_put);
  /* Definition of the device class `get_nerrors' function
     This function is used to get the number of I/O errors (for instance,
     synchronization errors 
  */
  fts_dev_class_sig_set_get_nerrors_fun( alsa_dac_class, alsa_dac_get_nerrors);
  /* Definition of the device class `get_nchans' function
     This function returns the number of voices of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( alsa_dac_class, alsa_dac_get_nchans);
}

/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.
  For instance, the arguments may be:
  [0] (symbol) "voices"
  [1] (int)    2
  [2] (symbol) "card"
  [3] (int)    2
*/

int
set_parameters (int card, int rate, int latency_samples)

{
        snd_ctl_t *ctl_handle;
        int i, err, dev, idx;
        snd_switch_t sw;

        if ((err = snd_ctl_open (&ctl_handle, card)) < 0) {
                fprintf(stderr, 
                        "error: control open (%i): %s\n", 
                        card, snd_strerror(err));
                return 1;
        }

        /* SAMPLE RATE */

        strcpy (sw.name, "sample rate");

        if ((err = snd_ctl_switch_read (ctl_handle, &sw)) < 0) {
                fprintf (stderr, "can't read sample rate (%s)\n",
                         snd_strerror (err));
                return 1;
        }

        post ("Old sample rate: %ukHz\n", sw.value.data32[0]);

        sw.value.data32[0] = rate;

        if ((err = snd_ctl_switch_write (ctl_handle, &sw)) < 0) {
                fprintf (stderr, "can't write sample rate (%s)\n",
                         snd_strerror (err));
                return 1;
        }

        post ("New sample rate: %ukHz\n", sw.value.data32[0]);

        /* LATENCY */

        strcpy (sw.name, "latency");

        if ((err = snd_ctl_switch_read (ctl_handle, &sw)) < 0) {
                fprintf (stderr, "can't read latency (%s)\n",
                         snd_strerror (err));
                return 1;
        }

        post ("Old latency: %d samples\n", sw.value.data16[0]);

        sw.value.data16[0] = latency_samples;

        if ((err = snd_ctl_switch_write (ctl_handle, &sw)) < 0) {
                fprintf (stderr, "can't write latency (%s)\n",
                         snd_strerror (err));
                return 1;
        }

        post ("New latency: %d samples\n", sw.value.data16[0]);

        return 0;
}

static fts_status_t open_subdev( snd_pcm_t **handle, int subdev)
{
  snd_pcm_channel_params_t params;
  int err;

  if ( (err = snd_pcm_open_subdevice( handle, 0, 0, subdev, SND_PCM_OPEN_PLAYBACK)) < 0 )
    {
      post( "Error: snd_pcm_open_subdevice() failed: %s\n", snd_strerror (err));
      return &fts_dev_open_error;
    }


  memset( &params, 0, sizeof( params));

  params.channel = SND_PCM_CHANNEL_PLAYBACK;
  params.mode = SND_PCM_MODE_BLOCK;
  params.start_mode = SND_PCM_START_DATA;
  params.stop_mode = SND_PCM_STOP_ROLLOVER;

  params.format.format = SND_PCM_SFMT_U32_LE; 
  params.format.rate = 44100;
  params.format.voices = 1;
  params.format.interleave = 0;

  params.buf.block.frag_size = 4 * 1024;
  params.buf.block.frags_min = 2;
  params.buf.block.frags_max = 2;

  /*    params.buf.stream.queue_size = n_voices * sizeof( short) * MAXVS; */
  /*    params.buf.stream.fill = SND_PCM_FILL_SILENCE; */
  /*    params.buf.stream.max_fill = 1024; */

#if 0
  if ((err = snd_pcm_channel_flush (*handle, SND_PCM_CHANNEL_PLAYBACK)) < 0) 
    {
      post( "Error: snd_pcm_channel_flush() failed: %s\n", snd_strerror (err));

      return &fts_dev_open_error;
    }
#endif

  if ((err = snd_pcm_channel_params( *handle, &params)) < 0)
    { 
      post( "Error: snd_pcm_channel_params() failed: %s\n", snd_strerror( err ));
      snd_pcm_close( *handle ); 

      return &fts_dev_open_error;
    } 

  if ( (err = snd_pcm_channel_prepare (*handle, SND_PCM_CHANNEL_PLAYBACK)) < 0)
    {
      post( "Error: snd_pcm_channel_prepare() failed: %s\n", snd_strerror (err));
      return &fts_dev_open_error;
    }

  return fts_Success;
}


static fts_status_t alsa_dac_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int n_voices, i;
  int err, card, device;
  alsa_pcm_dev_data_t *data;
  snd_pcm_t **handle;
  snd_pcm_channel_info_t channel_info;
  snd_pcm_channel_params_t params;

  n_voices = 8;

  /* Allocation of device data */
  data = (alsa_pcm_dev_data_t *)fts_malloc( sizeof( alsa_pcm_dev_data_t));
  /* Device data settings */
  data->n_voices = n_voices;
  data->bytes_count = 0;
  data->fmtbuf = (short *)fts_malloc( MAXVS * n_voices * sizeof( short));

  fts_dev_set_device_data( dev, data);

  /* Parameter parsing */
  n_voices = fts_get_int_by_name(nargs, args, fts_new_symbol("voices"), DEFAULT_N_VOICES);

  post( "n_voices %d\n", n_voices);

  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), snd_defaults_pcm_card());
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);

  /* ALSA handle opening */
#if 0
  if ( snd_pcm_open( &handle, card, device, SND_PCM_OPEN_PLAYBACK) )
    {
      post( "Error: snd_pcm_open() failed\n");
      return &fts_dev_open_error;
    }
#else

  if (set_parameters ( 0, 44100, 1024))
    {
      return &fts_dev_open_error;
    }
      

  for ( i = 0; i < CHANNEL_CNT; i++)
    {
      fts_status_t err;

      if ( (err = open_subdev( &(data->pcm_handle[i]), i)) != fts_Success)
	return err;
    }
#endif
  /* This is to inform the scheduler that it should use this device to check for I/O errors */
  fts_dsp_set_dac_slip_dev( dev);

  return fts_Success;
}


/*
  Device `close' function
*/
static fts_status_t alsa_dac_close(fts_dev_t *dev)
{
  alsa_pcm_dev_data_t *data;

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  if ( snd_pcm_close( data->handle) )
    {
      return &fts_dev_open_error;
    }

  return fts_Success;
}

static int alsa_dac_get_nchans( fts_dev_t *dev)
{
  alsa_pcm_dev_data_t *data;

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  return data->n_voices;
}

static int alsa_dac_get_nerrors( fts_dev_t *dev)
{
  alsa_pcm_dev_data_t *data;

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  return 0;
}


/*
  Device `put' function

  It gets its arguments in an array of fts_word_t, the arguments being
  organized as:
  [0] fts_dev_t **pdev
  [1] int nchans
  [2] int n
  [3] float *buf_1
  [4] float *buf_2
  etc

*/

static void alsa_dac_put( fts_word_t *argv)
{
  fts_dev_t *dev;
  alsa_pcm_dev_data_t *data;
  int n_voices, voice, n, i, j;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  n_voices = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  for ( voice = 0; voice < n_voices; voice++)
    {
      float *in;
      
      in = (float *) fts_word_get_ptr( argv + 3 + voice);

      j = voice;
      for ( i = 0; i < n; i++)
	{
	  data->fmtbuf[j] = (short) ( 32767.0f * in[i]);
	  j += n_voices;
	}
    }

  /* output the buffer */
  snd_pcm_write( data->handle, data->fmtbuf, n * n_voices * sizeof( short));
}


/*----------------------------------------------------------------------------*/
/* Audio input device                                                         */
/*----------------------------------------------------------------------------*/

/* Forward declarations of device and device class functions */

static fts_status_t alsa_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t alsa_adc_close(fts_dev_t *dev);
static void         alsa_adc_get(fts_word_t *args);

static int          alsa_adc_get_nchans(fts_dev_t *dev);


/* Init function */

static void alsa_adc_init(void)
{
  fts_dev_class_t *alsa_adc_class;

  /* Declaration of the device class */
  alsa_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("alsa_adc"));

  /* Definition of the device class `open' function */
  fts_dev_class_set_open_fun( alsa_adc_class, alsa_adc_open);
  /* Definition of the device class `close' function */
  fts_dev_class_set_close_fun( alsa_adc_class, alsa_adc_close);
  /* Definition of the device class `get' function */
  fts_dev_class_sig_set_get_fun( alsa_adc_class, alsa_adc_get);

  /* Definition of the device class `get_nchans' function
     This function returns the number of voices of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( alsa_adc_class, alsa_adc_get_nchans);
}

/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.

  See similar function for `dac' device
*/

static fts_status_t alsa_adc_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int n_voices;
  int err, card, device;
  alsa_pcm_dev_data_t *data;
  snd_pcm_t *handle;
  snd_pcm_channel_info_t channel_info;
  snd_pcm_channel_params_t params;

  /* Allocation of device data */
  data = (alsa_pcm_dev_data_t *)fts_malloc( sizeof( alsa_pcm_dev_data_t));

  /* Parameter parsing */
  n_voices = fts_get_int_by_name(nargs, args, fts_new_symbol("voices"), DEFAULT_N_VOICES);

  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), snd_defaults_pcm_card());
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);

  /* ALSA handle opening */
  if ( snd_pcm_open( &handle, card, device, SND_PCM_OPEN_CAPTURE) )
    {
      post( "Error: snd_pcm_open() failed\n");
      return &fts_dev_open_error;
    }

  if ( snd_pcm_channel_info( handle, &channel_info) )
    {
      post( "Error: snd_pcm_channel_info() failed\n");
      return &fts_dev_open_error;
    }

  if ( n_voices > channel_info.max_voices || n_voices < channel_info.min_voices )
    {
      post( "Error: wrong number of voices ( %d <= %d <= %d)\n", channel_info.min_voices, n_voices, channel_info.max_voices);
      return &fts_dev_open_error;
    }

  params.format.interleave = 1;
  params.format.format = SND_PCM_SFMT_S16_LE; 
  params.format.rate = 44100;
  params.format.voices = 2;
  params.buf.block.frag_size = n_voices * sizeof( short) * MAXVS;
  params.buf.block.frags_max = fts_param_get_int(fts_s_fifo_size, 256) / MAXVS;

  if ((err = snd_pcm_channel_params( handle, &params)) < 0)
    { 
      post( "Error: snd_pcm_channel_params() failed: %s\n", snd_strerror( err ));  
      snd_pcm_close( handle ); 

      return &fts_dev_open_error;
    } 

  /* Device data settings */
  data->handle = handle;
  data->n_voices = n_voices;
  data->bytes_count = 0;
  data->fmtbuf = (short *)fts_malloc( MAXVS * n_voices * sizeof( short));

  fts_dev_set_device_data( dev, data);

  return fts_Success;
}


static fts_status_t alsa_adc_close(fts_dev_t *dev)
{
  alsa_pcm_dev_data_t *data;

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  if ( snd_pcm_close( data->handle) )
    {
      return &fts_dev_open_error;
    }

  return fts_Success;
}

static int alsa_adc_get_nchans(fts_dev_t *dev)
{
  alsa_pcm_dev_data_t *data;

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  return data->n_voices;
}

/*
  Device `get' function

  It gets its arguments in an array of fts_word_t, the arguments being
  organized as:
  [0] fts_dev_t **pdev
  [1] int nchans
  [2] int n
  [3] float *buf_1
  [4] float *buf_2
  etc
*/

static void alsa_adc_get( fts_word_t *argv)
{
  fts_dev_t *dev;
  alsa_pcm_dev_data_t *data;
  int n_voices, voice, n, i, j;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  n_voices = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  snd_pcm_read( data->handle, data->fmtbuf, n * n_voices * sizeof( short));

  for ( voice = 0; voice < n_voices; voice++)
    {
      float *out;
      
      out = (float *) fts_word_get_ptr( argv + 3 + voice);

      j = voice;
      for ( i = 0; i < n; i++)
	{
	  out[i] = (float)data->fmtbuf[j] / 32767.0f;
	  j += n_voices;
	}
    }
}

/******************************************************************************/
/*                                                                            */
/* Module declaration                                                         */
/*                                                                            */
/******************************************************************************/

static void alsadev_init(void);

fts_module_t alsadev_module = { "alsadev", "ALSA devices", alsadev_init, 0, 0, 0};

static void alsadev_init(void)
{
  alsa_dac_init();
  alsa_adc_init();
}

