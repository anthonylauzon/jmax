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

typedef struct alsa_pcm_dev_data {
  snd_pcm_t *handle;
  int n_channels;
  long bytes_count;
  short *fmtbuf;
} alsa_pcm_dev_data_t;

#define DEFAULT_N_CHANNELS 2


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
     This function returns the number of channels of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( alsa_dac_class, alsa_dac_get_nchans);
}

/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.
  For instance, the arguments may be:
  [0] (symbol) "channels"
  [1] (int)    2
  [2] (symbol) "card"
  [3] (int)    2
*/

static fts_status_t alsa_dac_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  unsigned int n_channels;
  int err, card, device;
  alsa_pcm_dev_data_t *data;
  snd_pcm_t *handle;
  snd_pcm_playback_info_t playback_info;
  snd_pcm_format_t format;
  snd_pcm_playback_params_t pcm_playback_params;

  /* Allocation of device data */
  data = (alsa_pcm_dev_data_t *)fts_malloc( sizeof( alsa_pcm_dev_data_t));

  /* Parameter parsing */
  n_channels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEFAULT_N_CHANNELS);

  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), snd_defaults_pcm_card());
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);

  /* ALSA handle opening */
  if ( snd_pcm_open( &handle, card, device, SND_PCM_OPEN_PLAYBACK) )
    {
      post( "Error: snd_pcm_open() failed\n");
      return &fts_dev_open_error;
    }

  if ( snd_pcm_playback_info( handle, &playback_info) )
    {
      post( "Error: snd_pcm_playback_info() failed\n");
      return &fts_dev_open_error;
    }

  if ( n_channels > playback_info.max_channels || n_channels < playback_info.min_channels )
    {
      post( "Error: wrong number of channels ( %d <= %d <= %d)\n", playback_info.min_channels, n_channels, playback_info.max_channels);
      return &fts_dev_open_error;
    }

  format.format = SND_PCM_SFMT_S16_LE; 
  format.rate = 44100;
  format.channels = 2;

  if ((err = snd_pcm_playback_format( handle, &format)) < 0)
    { 
      post( "Error: format setup failed: %s\n", snd_strerror( err ));  
      snd_pcm_close( handle ); 

      return &fts_dev_open_error;
    } 

  pcm_playback_params.fragment_size = n_channels * sizeof( short) * MAXVS;
  pcm_playback_params.fragments_max = fts_param_get_int(fts_s_fifo_size, 256) / MAXVS;
  pcm_playback_params.fragments_room = 1;

  if( (err = snd_pcm_playback_params( handle, &pcm_playback_params)) < 0)
    { 
      post( "Error: pcm_playback_params failed: %s\n", snd_strerror(err));
      return &fts_dev_open_error;
    }

  /* Device data settings */
  data->handle = handle;
  data->n_channels = n_channels;
  data->bytes_count = 0;
  data->fmtbuf = (short *)fts_malloc( MAXVS * n_channels * sizeof( short));

  fts_dev_set_device_data( dev, data);

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

  return data->n_channels;
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
  int n_channels, channel, n, i, j;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  n_channels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  for ( channel = 0; channel < n_channels; channel++)
    {
      float *in;
      
      in = (float *) fts_word_get_ptr( argv + 3 + channel);

      j = channel;
      for ( i = 0; i < n; i++)
	{
	  data->fmtbuf[j] = (short) ( 32767.0f * in[i]);
	  j += n_channels;
	}
    }

  /* output the buffer */
  snd_pcm_write( data->handle, data->fmtbuf, n * n_channels * sizeof( short));
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
     This function returns the number of channels of the device
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
  unsigned int n_channels;
  int err, card, device;
  alsa_pcm_dev_data_t *data;
  snd_pcm_t *handle;
  snd_pcm_capture_info_t capture_info;
  snd_pcm_format_t format;
  snd_pcm_capture_params_t pcm_capture_params;

  /* Allocation of device data */
  data = (alsa_pcm_dev_data_t *)fts_malloc( sizeof( alsa_pcm_dev_data_t));

  /* Parameter parsing */
  n_channels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEFAULT_N_CHANNELS);

  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), snd_defaults_pcm_card());
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);

  /* ALSA handle opening */
  if ( snd_pcm_open( &handle, card, device, SND_PCM_OPEN_CAPTURE) )
    {
      post( "Error: snd_pcm_open() failed\n");
      return &fts_dev_open_error;
    }

  if ( snd_pcm_capture_info( handle, &capture_info) )
    {
      post( "Error: snd_pcm_capture_info() failed\n");
      return &fts_dev_open_error;
    }

  if ( n_channels > capture_info.max_channels || n_channels < capture_info.min_channels )
    {
      post( "Error: wrong number of channels ( %d <= %d <= %d)\n", capture_info.min_channels, n_channels, capture_info.max_channels);
      return &fts_dev_open_error;
    }

  format.format = SND_PCM_SFMT_S16_LE; 
  format.rate = 44100;
  format.channels = 2;

  if ((err = snd_pcm_capture_format( handle, &format)) < 0)
    { 
      post( "Error: format setup failed: %s\n", snd_strerror( err ));  
      snd_pcm_close( handle ); 

      return &fts_dev_open_error;
    } 

  pcm_capture_params.fragment_size = n_channels * sizeof( short) * MAXVS;
  pcm_capture_params.fragments_min = 1;

  if( (err = snd_pcm_capture_params( handle, &pcm_capture_params)) < 0)
    { 
      post( "Error: pcm_capture_params failed: %s\n", snd_strerror(err));
      return &fts_dev_open_error;
    }

  /* Device data settings */
  data->handle = handle;
  data->n_channels = n_channels;
  data->bytes_count = 0;
  data->fmtbuf = (short *)fts_malloc( MAXVS * n_channels * sizeof( short));

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

  return data->n_channels;
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
  int n_channels, channel, n, i, j;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  data = (alsa_pcm_dev_data_t *)fts_dev_get_device_data( dev);

  n_channels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  snd_pcm_read( data->handle, data->fmtbuf, n * n_channels * sizeof( short));

  for ( channel = 0; channel < n_channels; channel++)
    {
      float *out;
      
      out = (float *) fts_word_get_ptr( argv + 3 + channel);

      j = channel;
      for ( i = 0; i < n; i++)
	{
	  out[i] = (float)data->fmtbuf[j] / 32767.0f;
	  j += n_channels;
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

