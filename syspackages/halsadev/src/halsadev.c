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
 * Author: François Déchelle (dechelle@ircam.fr)
 * Based on code by: Paul Barton-Davis (pbd@op.net)
 *
 */

/* The default values */
#define DEF_SAMPLING_RATE ((float)44100.0f)
#define DEF_FIFO_SIZE 256

/* 
   NOTE: (François Déchelle, dechelle@ircam.fr)
   This file include the FTS ALSA Hammerfall device.
   It is actually implemented as a separate device because
   of Hammerfall specificities.
   *BUT* it will be merged with ALSA standard code as soon
   as possible, following the evolution of the Hammerfall
   ALSA driver and the ALSA code itself.
*/

/* Include files */

#include <stdio.h>
#include <stdlib.h>

#include <sys/asoundlib.h>
#include <linux/asound.h>

#include "fts.h"
#include "hm.h"

#undef ZOB

extern void fts_dsp_set_dac_slip_dev(fts_dev_t *dev);

/*
  NOTE: (François Déchelle, dechelle@ircam.fr)
  There is a problem with the FTS devices API: to open a device for input 
  *and* output always leads to twisted code, though it is what is done in 
  almost all cases. So, what to do ?

  To overcome this limitation, we have here only one device !!!
*/

static hm_t *s_hm = 0;
static int underrun_count = 0;
static int samples_count = 0;
static int fragment_samples = 0;
static long **input_buffers = 0;
static long **output_buffers = 0;

static int init( int card, int device)
{
  int err;
  int sample_rate, fifo_size;
  int n_channels, channel;

  if ( s_hm )
    return 0;

  s_hm = (hm_t *)fts_malloc( sizeof( hm_t));

  sample_rate = (int) fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);

  fifo_size = fts_param_get_int( fts_s_fifo_size, DEF_FIFO_SIZE);
  if ( fifo_size % 2 != 0 && fifo_size % MAXVS != 0)
    {
      post( "Error: fifo size (%d) is not a multiple of 2 and a multiple of %d\n", fifo_size, MAXVS);
      return -1;
    }

  fragment_samples = fifo_size;

  if ( (err = hm_open( s_hm, card, device, sample_rate, fragment_samples)) < 0)
    {
      post( "Error: open failed: %s\n", snd_strerror( err));
      return -1;
    }

  n_channels = hm_get_n_channels( s_hm);

  input_buffers = (long **)fts_malloc( n_channels * sizeof( long *));
  output_buffers = (long **)fts_malloc( n_channels * sizeof( long *));

  for ( channel = 0; channel < n_channels; channel++)
    {
      input_buffers[channel] = (long *)fts_zalloc( fragment_samples * sizeof( long));
      output_buffers[channel] = (long *)fts_zalloc( fragment_samples * sizeof( long));
    }

  return 0;
}

static void finish( void)
{
  if (s_hm)
    {
      hm_close( s_hm);

      fts_free( s_hm);

      s_hm = 0;
    }
}


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
  alsa_dac_class = fts_dev_class_new( fts_sig_dev, fts_new_symbol("halsa_dac"));

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

static fts_status_t alsa_dac_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int card, device;

  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), 0);
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);

  if ( init( card, device) < 0)
    return &fts_dev_open_error;

  /* This is to inform the scheduler that it should use this device to check for I/O errors */
  fts_dsp_set_dac_slip_dev( dev);

  return fts_Success;
}


/*
  Device `close' function
*/
static fts_status_t alsa_dac_close(fts_dev_t *dev)
{
  finish();

  return fts_Success;
}

static int alsa_dac_get_nchans( fts_dev_t *dev)
{
  return hm_get_n_channels( s_hm);
}

static int alsa_dac_get_nerrors( fts_dev_t *dev)
{
  if ( underrun_count != hm_get_underrun_count( s_hm))
    {
      underrun_count = hm_get_underrun_count( s_hm);
      return 1;
    }

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
  int channel, n_channels, n, i;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  n = fts_word_get_long(argv + 2);

  n_channels = hm_get_n_channels( s_hm);

  for ( channel = 0; channel < n_channels; channel++)
    {
      float *in;
      long *dst;
      
      in = (float *) fts_word_get_ptr( argv + 3 + channel);
      dst = output_buffers[ channel] + samples_count;

#define MULT ((1<<23) - 1)

      for ( i = 0; i < n; i++)
	dst[i] = ((long)(MULT * in[i])) << 8;
    }
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
  alsa_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("halsa_adc"));

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
  int card, device;

  card = fts_get_int_by_name( nargs, args, fts_new_symbol( "card"), 0);
  device = fts_get_int_by_name( nargs, args, fts_new_symbol( "device"), 0);

  if ( init( card, device) < 0)
    return &fts_dev_open_error;

  return fts_Success;
}


static fts_status_t alsa_adc_close(fts_dev_t *dev)
{
  finish();

  return fts_Success;
}

static int alsa_adc_get_nchans(fts_dev_t *dev)
{
  return hm_get_n_channels( s_hm);
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
  int channel, n_channels, n, i;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  if (samples_count == 0)
    {
      if ( hm_select( s_hm) == 0)
	return;

      for ( channel = 0; channel < hm_get_n_channels( s_hm); channel++)
	{
	  memcpy( input_buffers[ channel], hm_get_input_buffer( s_hm, channel), fragment_samples * sizeof( long));
	  memcpy( hm_get_output_buffer( s_hm, channel), output_buffers[ channel], fragment_samples * sizeof( long));

	  hm_done_capture_fragment( s_hm, channel);
	  hm_done_playback_fragment( s_hm, channel);
	}
    }

  n = fts_word_get_long(argv + 2);

  n_channels = hm_get_n_channels( s_hm);

  for ( channel = 0; channel < n_channels; channel++)
    {
      float *out;
      long *src;
      
      out = (float *) fts_word_get_ptr( argv + 3 + channel);
      src = input_buffers[ channel ] + samples_count;

#define DIV ((float)((1<<23) - 1))

      for ( i = 0; i < n; i++)
	out[i] = (float)(src[i] >> 8) / DIV;
    }

  samples_count += n;

  if (samples_count >= fragment_samples)
    samples_count = 0;
}

/******************************************************************************/
/*                                                                            */
/* Module declaration                                                         */
/*                                                                            */
/******************************************************************************/

static void halsadev_init(void);

fts_module_t halsadev_module = { "halsadev", "Hammerfall ALSA devices", halsadev_init, 0, 0, 0};

static void halsadev_init(void)
{
  alsa_dac_init();
  alsa_adc_init();
}

