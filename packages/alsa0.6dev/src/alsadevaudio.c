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
 */

/* 
 * This file's authors: François Déchelle (dechelle@ircam.fr)
 */


/* 
 * This file include the FTS ALSA audio device.
 * Based on Ardour by Paul Barton-Davis
 */

/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/asoundlib.h>
#include <linux/asound.h>

#include "fts.h"
#include "libsndmmap.h"

static fts_symbol_t s_name;
static fts_symbol_t s_format;

#define DEFAULT_N_CHANNELS 2
#define DEF_SAMPLING_RATE ((float)44100.0f)
#define DEF_FIFO_SIZE 256

static void error( const char *fun, int err)
{
  post( "Error: snd_pcm_%s() failed: %s\n", fun, snd_strerror( err ));
}

typedef struct {
  int stream;
  snd_pcm_t *handle;
  int n_channels;
  int is_interleaved;
  int format;
} alsa_dev_data_t;

static fts_status_t alsa_dev_data_new( alsa_dev_data_t **pdata, char *pcm_name, int stream, const char *format_name, int sampling_rate, int n_channels, int fifo_size)
{
  alsa_dev_data_t *d;
  mask_t *pmask;
  snd_output_t *out;
  int frames_per_cycle, obtained_frames_per_cycle;
  snd_pcm_hw_params_t success;
  snd_pcm_hw_params_t hw_params;
  snd_pcm_sw_params_t sw_params;
  char *outbuf;
  int err;

  d = (alsa_dev_data_t *)fts_malloc( sizeof( alsa_dev_data_t));
  memset( d, 0, sizeof( *d));

  *pdata = d;

  snd_output_buffer_open (&out);

  /*
   * Open the PCM device
   */
  if ( (err = snd_pcm_open (&d->handle, pcm_name, stream, 0)) < 0)
    {
      error( "snd_pcm_open", err);
      return &fts_dev_open_error;
    }

  if ((err = snd_pcm_hw_params_any( d->handle, &hw_params)) < 0)
    {
      error( "snd_pcm_hw_params_any", err);
      return &fts_dev_open_error;
    }

  /* 
     Figure out if we are doing interleaved or non-interleaved access.
   */
  pmask = (mask_t *)fts_malloc( mask_sizeof());
	
  mask_set( pmask, SND_PCM_ACCESS_RW_INTERLEAVED);
  mask_set( pmask, SND_PCM_ACCESS_RW_NONINTERLEAVED);

  if ((err = snd_pcm_hw_param_mask( d->handle, &hw_params, SND_PCM_HW_PARAM_ACCESS, pmask)) < 0) 
    {
      error( "snd_pcm_hw_params_mask[SND_PCM_HW_PARAM_ACCESS]", err);
      return &fts_dev_open_error;
  }

  pmask = snd_pcm_hw_param_value_mask( &hw_params, SND_PCM_HW_PARAM_ACCESS);
  d->is_interleaved = mask_test( pmask, SND_PCM_ACCESS_RW_INTERLEAVED);

  /*
   * Set the sample format
   */
  d->format = snd_pcm_format_value( format_name);
  if ((err = snd_pcm_hw_param_set( d->handle, &hw_params, SND_PCM_HW_PARAM_FORMAT, d->format, 0)) < 0)
    {
      error( "snd_pcm_hw_param_set[SND_PCM_HW_PARAM_FORMAT]", err);
      return &fts_dev_open_error;
    }

  /*
   * Set the number of channels
   */
  d->n_channels = n_channels;
  if ((err = snd_pcm_hw_param_set( d->handle, &hw_params, SND_PCM_HW_PARAM_CHANNELS, n_channels, 0)) < 0) 
    {
      snd_pcm_hw_params_try_explain_failure( d->handle, &hw_params, &success, 10, out);
      snd_output_buffer_string( out, &outbuf);

      error( "snd_pcm_hw_param_set[SND_PCM_HW_PARAM_CHANNELS]", err);
      return &fts_dev_open_error;
    }

  /*
   * set periods: we ask for 2 periods
   */
  if ((err = snd_pcm_hw_param_min( d->handle, &hw_params, SND_PCM_HW_PARAM_PERIODS, 2, 0)) < 0)
    {
      snd_pcm_hw_params_try_explain_failure( d->handle, &hw_params, &success, 10, out);
      snd_output_buffer_string( out, &outbuf);

      error( "snd_pcm_hw_param_min[SND_PCM_HW_PARAM_PERIODS]", err);
      return &fts_dev_open_error;
    }

  /*
   * set period size: the period size is the fifo size divided by the number of periods
   */
  frames_per_cycle = fifo_size / 2;
  if ((err = snd_pcm_hw_param_set( d->handle, &hw_params, SND_PCM_HW_PARAM_PERIOD_SIZE, frames_per_cycle, 0)) < 0) 
    {
      snd_pcm_hw_params_try_explain_failure( d->handle, &hw_params, &success, 10, out);
      snd_output_buffer_string( out, &outbuf);

      error( "snd_pcm_hw_param_min[SND_PCM_HW_PARAM_PERIOD_SIZE]", err);
      return &fts_dev_open_error;
    }

  /*
   * set buffer size
   */
  if ((err = snd_pcm_hw_param_set( d->handle, &hw_params, SND_PCM_HW_PARAM_BUFFER_SIZE, fifo_size, 0)) < 0)
    {
      snd_pcm_hw_params_try_explain_failure( d->handle, &hw_params, &success, 10, out);
      snd_output_buffer_string( out, &outbuf);

      error( "snd_pcm_hw_param_min[SND_PCM_HW_PARAM_BUFFER_SIZE]", err);

      return &fts_dev_open_error;
    }

  /*
   * set hardware parameters
   */
  if ((err = snd_pcm_hw_params( d->handle, &hw_params)) < 0)
    {
      snd_pcm_hw_params_try_explain_failure( d->handle, &hw_params, &success, 10, out);
      snd_output_buffer_string( out, &outbuf);

      error( "snd_pcm_hw_params", err);
      return &fts_dev_open_error;
    }

  /* check the period size, since thats non-negotiable */
  obtained_frames_per_cycle = snd_pcm_hw_param_value( &hw_params, SND_PCM_HW_PARAM_PERIOD_SIZE, 0);
	
  if (obtained_frames_per_cycle != frames_per_cycle) 
    {
      return &fts_dev_open_error;
    }

  snd_pcm_sw_params_current( d->handle, &sw_params);

  if ((err = snd_pcm_sw_param_set( d->handle, &sw_params, SND_PCM_SW_PARAM_PERIOD_STEP, 1)) < 0)
    {
      return &fts_dev_open_error;
    }

  if ((err = snd_pcm_sw_param_set( d->handle, &sw_params, SND_PCM_SW_PARAM_XRUN_MODE, SND_PCM_XRUN_NONE)) < 0) 
    {
      return &fts_dev_open_error;
    }

  if ((err = snd_pcm_sw_param_set( d->handle, &sw_params, SND_PCM_SW_PARAM_START_MODE, SND_PCM_START_EXPLICIT)) < 0)
    {
      return &fts_dev_open_error;
    }

  if ((err = snd_pcm_sw_param_set( d->handle, &sw_params, SND_PCM_SW_PARAM_AVAIL_MIN, frames_per_cycle)) < 0) 
    {
      return &fts_dev_open_error;
    }

  if ((err = snd_pcm_sw_params( d->handle, &sw_params)) < 0) 
    {
      return &fts_dev_open_error;
    }

  return fts_Success;
}


#define GET_DEV_DATA(DEV) ((alsa_dev_data_t *)fts_dev_get_device_data( DEV))

/*----------------------------------------------------------------------------*/
/* Audio output device                                                        */
/*----------------------------------------------------------------------------*/

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
  int n_channels, sampling_rate, fifo_size;
  const char *pcm_name;
  const char *format_name;
  int err;

  sampling_rate = (int) fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);
  fifo_size = fts_param_get_int(fts_s_fifo_size, DEF_FIFO_SIZE);

  /* Parameter parsing */
  n_channels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEFAULT_N_CHANNELS);
  pcm_name = fts_symbol_name( fts_get_symbol_by_name( nargs, args, s_name, 0));
  format_name = fts_symbol_name( fts_get_symbol_by_name( nargs, args, s_format, 0));

  /* Allocation of device data */
  data = alsa_dev_data_new( pcm_name, SND_PCM_STREAM_PLAYBACK);
  fts_dev_set_device_data( dev, data);

  return &fts_dev_open_error;

  fts_dsp_set_dac_slip_dev( dev);

  return fts_Success;
}

/*
  Device `close' function
*/
static fts_status_t alsa_dac_close(fts_dev_t *dev)
{

  return fts_Success;
}

static int alsa_dac_get_nchans( fts_dev_t *dev)
{
  alsa_pcm_dev_data_t *data;

  data = GET_DEV_DATA( dev);

  return data->n_channels;
}

static int alsa_dac_get_nerrors( fts_dev_t *dev)
{
  int xrun;

  xrun = snd_get_xrun( &single_instance->md);
  snd_clear_xrun( &single_instance->md);

  return xrun;
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
  alsa_dev_data_t *data;
  int n_channels, channel, n, i;

/*    dev = *((fts_dev_t **)fts_word_get_ptr( argv)); */
/*    data = GET_DEV_DATA( dev); */
  data = single_instance;

  n_channels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  for ( channel = 0; channel < n_channels; channel++)
    {
      long *buffer = (long *)snd_get_playback_fragment( &data->md, channel) + data->count;
      float *in;
      
      in = (float *) fts_word_get_ptr( argv + 3 + channel);

      for ( i = 0; i < n; i++)
	{
	  buffer[i] = (long) (((1<<23) - 1) * in[i]) << 8;
	}
    }

  data->count += n;
  if ( data->count >= snd_get_frag_size( &data->md))
    {
      snd_done_fragment( &data->md);
      data->count = 0;
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
     This function returns the number of channels of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( alsa_dac_class, alsa_dac_get_nchans);

  if (!s_name)
    s_name = fts_new_symbol( "name");
  if (!s_format)
    s_format = fts_new_symbol( "format");
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
  /* 
     There is only one instance now, because of the way the FTS "devices"
     are opened (input and output separated, which does not work well).
  */
  if (single_instance == 0)
    {
      single_instance = create_instance( nargs, args);

      if ( !single_instance)
	return &fts_dev_open_error;
    }

  return fts_Success;
}


static fts_status_t alsa_adc_close(fts_dev_t *dev)
{
  if (single_instance != 0 && snd_close( &single_instance->md) < 0)
    {
      single_instance = 0;

      return &fts_dev_open_error;
    }

  return fts_Success;
}

static int alsa_adc_get_nchans(fts_dev_t *dev)
{
  return single_instance->n_channels;
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
  alsa_dev_data_t *data;
  int n_channels, channel, n, i;

/*    dev = *((fts_dev_t **)fts_word_get_ptr( argv)); */
/*    data = GET_DEV_DATA( dev); */
  data = single_instance;

  n_channels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  if ( data->count == 0)
    snd_do_poll( &data->md);

  for ( channel = 0; channel < n_channels; channel++)
    {
      long *buffer = (long *)snd_get_capture_fragment( &data->md, channel) + data->count;
      float *out;
      
      out = (float *) fts_word_get_ptr( argv + 3 + channel);

#define DIV ((float)((1<<23) - 1))

      for ( i = 0; i < n; i++)
	{
	  out[i] = (float)(buffer[i] >> 8) / DIV;
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
     This function returns the number of channels of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( alsa_adc_class, alsa_adc_get_nchans);
}


