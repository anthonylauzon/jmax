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
 * Authors: Guenter Geiger (geiger@epy.co.at), François Déchelle (dechelle@ircam.fr)
 *
 */


/* 
 * This file include the FTS ALSA audio device.
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

typedef struct alsa_pcm_dev_data {
  snd_pcm_t *handle;
  int n_voices;
  long bytes_count;
  short *fmtbuf;
} alsa_pcm_dev_data_t;

#define DEFAULT_N_VOICES 2


/*----------------------------------------------------------------------------*/
/* Audio output device                                                        */
/*----------------------------------------------------------------------------*/

/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.
  For instance, the arguments may be:
  [0] (symbol) "voices"
  [1] (int)    2
  [2] (symbol) "card"
  [3] (int)    2
*/

static fts_status_t alsa_dac_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int n_voices;
  int err, card, device;
  alsa_pcm_dev_data_t *data;
  snd_pcm_channel_params_t params;

  /* Parameter parsing */
  n_voices = fts_get_int_by_name(nargs, args, fts_new_symbol("voices"), DEFAULT_N_VOICES);
  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), 0);
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);

  /* Allocation of device data */
  data = (alsa_pcm_dev_data_t *)fts_malloc( sizeof( alsa_pcm_dev_data_t));

  /* Device data settings */
  data->n_voices = n_voices;
  data->bytes_count = 0;
  data->fmtbuf = (short *)fts_malloc(fts_get_tick_size() * n_voices * sizeof( short));

  fts_dev_set_device_data( dev, data);

  if (( err = snd_pcm_open( &data->handle, card, device, SND_PCM_OPEN_PLAYBACK)) < 0)
    {
      post( "Error: snd_pcm_open failed (%s)\n", snd_strerror( err));
      return &fts_dev_open_error;
    }

#if 0
  if ((err = snd_pcm_channel_flush( data->handle, SND_PCM_CHANNEL_PLAYBACK)) < 0)
    {
      post( "Error: snd_pcm_flush failed (%s)\n", snd_strerror( err));
      return &fts_dev_open_error;
    }
#else
  snd_pcm_channel_flush( data->handle, SND_PCM_CHANNEL_PLAYBACK);
#endif

  memset( &params, 0, sizeof( params));

  params.channel = SND_PCM_CHANNEL_PLAYBACK;
  params.mode = SND_PCM_MODE_BLOCK;
  params.start_mode = SND_PCM_START_FULL;
  params.stop_mode = SND_PCM_STOP_STOP;

  params.format.format = SND_PCM_SFMT_S16_LE; 
  params.format.rate = 44100;
  params.format.voices = n_voices;
  params.format.interleave = 1;

  params.buf.block.frag_size = 256;
  params.buf.block.frags_max = -1;
  params.buf.block.frags_min = 1;

  if ((err = snd_pcm_channel_params( data->handle, &params)) < 0)
    { 
      post( "Error: snd_pcm_channel_params() failed: %s\n", snd_strerror( err ));
      snd_pcm_close( data->handle ); 

      return &fts_dev_open_error;
    } 

  if ( (err = snd_pcm_channel_prepare( data->handle, SND_PCM_CHANNEL_PLAYBACK)) < 0)
    {
      post( "Error: snd_pcm_channel_prepare() failed: %s\n", snd_strerror (err));
      return &fts_dev_open_error;
    }

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
  int n_voices, voice, n, i, j, size, err;

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
  size = n * n_voices * sizeof( short);
  if ((err = snd_pcm_write( data->handle, data->fmtbuf, size)) != size)
    {
      static int count = 0;

      if (count++ < 100)
	{
	  post( "error when writing %d %s\n", err, snd_strerror( err));
	}
    }
}


/* Init function */

void alsa_dac_init(void)
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

/*----------------------------------------------------------------------------*/
/* Audio input device                                                         */
/*----------------------------------------------------------------------------*/

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
  snd_pcm_channel_params_t params;

  /* Parameter parsing */
  n_voices = fts_get_int_by_name(nargs, args, fts_new_symbol("voices"), DEFAULT_N_VOICES);
  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), 0);
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);

  /* Allocation of device data */
  data = (alsa_pcm_dev_data_t *)fts_malloc( sizeof( alsa_pcm_dev_data_t));

  /* Device data settings */
  data->n_voices = n_voices;
  data->bytes_count = 0;
  data->fmtbuf = (short *)fts_malloc(fts_get_tick_size() * n_voices * sizeof( short));

  fts_dev_set_device_data( dev, data);

  if (( err = snd_pcm_open( &data->handle, card, device, SND_PCM_OPEN_CAPTURE)) < 0)
    {
      post( "Error: snd_pcm_open failed (%s)\n", snd_strerror( err));
      return &fts_dev_open_error;
    }

#if 0
  if ((err = snd_pcm_channel_flush( data->handle, SND_PCM_CHANNEL_CAPTURE)) < 0)
    {
      post( "Error: snd_pcm_flush failed (%s)\n", snd_strerror( err));
      return &fts_dev_open_error;
    }
#else
  snd_pcm_channel_flush( data->handle, SND_PCM_CHANNEL_CAPTURE);
#endif

  memset( &params, 0, sizeof( params));

  params.channel = SND_PCM_CHANNEL_CAPTURE;
  params.mode = SND_PCM_MODE_BLOCK;
  params.start_mode = SND_PCM_START_DATA;
  params.stop_mode = SND_PCM_STOP_STOP;

  params.format.format = SND_PCM_SFMT_S16_LE; 
  params.format.rate = 44100;
  params.format.voices = n_voices;
  params.format.interleave = 1;

  params.buf.block.frag_size = 256;
  params.buf.block.frags_max = 10;
  params.buf.block.frags_min = 1;

  if ((err = snd_pcm_channel_params( data->handle, &params)) < 0)
    { 
      post( "Error: snd_pcm_channel_params() failed: %s\n", snd_strerror( err ));  
      snd_pcm_close( data->handle ); 

      return &fts_dev_open_error;
    } 

  if ( (err = snd_pcm_channel_prepare( data->handle, SND_PCM_CHANNEL_CAPTURE)) < 0)
    {
      post( "Error: snd_pcm_channel_prepare() failed: %s\n", snd_strerror (err));
      return &fts_dev_open_error;
    }

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
  int n_voices, voice, n, i, j, size, err;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  n_voices = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  size = n * n_voices * sizeof( short);

  if ((err = snd_pcm_read( data->handle, data->fmtbuf, size)) != size)
    {
      static int count = 0;

      if (count++ < 100)
	{
	  post( "error when reading %d %s\n", err, snd_strerror( err));
	}
    }

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

/* Init function */

void alsa_adc_init(void)
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

