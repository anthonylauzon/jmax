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

extern void fts_dsp_set_dac_slip_dev(fts_dev_t *dev);

#define DEFAULT_N_CHANNELS 2
#define DEF_SAMPLING_RATE ((float)44100.0f)
#define DEF_FIFO_SIZE 256

typedef struct {
  mmapdev_t md;
  int count;
  int n_channels;
} alsa_dev_data_t;

#define GET_DEV_DATA(DEV) ((alsa_dev_data_t *)fts_dev_get_device_data( DEV))

static alsa_dev_data_t *single_instance = 0;

static alsa_dev_data_t *create_instance( int nargs, const fts_atom_t *args)
{
  int n_channels, sampling_rate, fifo_size, frag_size, card, device, subdevice;
  alsa_dev_data_t *data;
#if 1
  int init_frag;
#endif

  sampling_rate = (int) fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);
  fifo_size = fts_param_get_int(fts_s_fifo_size, DEF_FIFO_SIZE);

  /* Compute fragment size */
  /* For now, we assume that there is only 2 fragments in the buffer */
  frag_size = fifo_size / 2;
  if (frag_size % FTS_DEF_TICK_SIZE != 0)
    {
      frag_size = FTS_DEF_TICK_SIZE * (frag_size / FTS_DEF_TICK_SIZE + 1);
    }

  /* Parameter parsing */
  n_channels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEFAULT_N_CHANNELS);
  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), 0);
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);
  subdevice = fts_get_int_by_name( nargs, args, fts_new_symbol( "subdevice"), 0);
#if 1
  init_frag = fts_get_int_by_name( nargs, args, fts_new_symbol( "init_frag"), 0);
#endif

  subdevice = fts_get_int_by_name( nargs, args, fts_new_symbol( "subdevice"), 0);

  /* Allocation of device data */
  data = (alsa_dev_data_t *)fts_malloc( sizeof( alsa_dev_data_t));

#if 1
  if (snd_open( &data->md, card, device, subdevice, SND_PCM_SFMT_S32_LE, sampling_rate, frag_size, init_frag) < 0)
    return 0;
#else
  if (snd_open( &data->md, card, device, subdevice, SND_PCM_SFMT_S32_LE, sampling_rate, frag_size) < 0)
    return 0;
#endif

  if (snd_start( &data->md) < 0)
    return 0;

  data->count = 0;
  data->n_channels = n_channels;

  return data;
}

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

  /* This is to inform the scheduler that it should use this device to check for I/O errors */
  fts_dsp_set_dac_slip_dev( dev);

  return fts_Success;
}

/*
  Device `close' function
*/
static fts_status_t alsa_dac_close(fts_dev_t *dev)
{
  if (single_instance != 0 && snd_close( &single_instance->md) < 0)
    {
      single_instance = 0;

      return &fts_dev_open_error;
    }

  return fts_Success;
}

static int alsa_dac_get_nchans( fts_dev_t *dev)
{
  return single_instance->n_channels;
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


