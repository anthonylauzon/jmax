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
 *
 */


/* This file include a template for writing FTS devices.
   The devices are:
    - an audio input device
    - an audio output device
    - a MIDI input/output device
*/

/* Include files */

#include <stdio.h>
#include <stdlib.h>

#include "fts.h"


/******************************************************************************/
/*                                                                            */
/* Audio devices                                                              */
/*                                                                            */
/******************************************************************************/

/*
  This structure will be stored in the "device_data" and will be
  available in any device function using fts_dev_get_device_data.

  It can be used to store device specific datas as well as
  handle to the platform specific audio datas.
*/

typedef short platform_sample_t;

typedef struct {
  int nothing;
} platform_specific_audio_info_t;

typedef struct template_audio_dev_data {
  int n_channels;
  fts_symbol_t port;
  int buffer_size;
  platform_sample_t *samples_buffer;
  platform_specific_audio_info_t *info;
} template_audio_dev_data_t;

#define DEFAULT_N_CHANNELS 2
#define DEFAULT_PORT       "default"


/*----------------------------------------------------------------------------*/
/* Audio output device                                                        */
/*----------------------------------------------------------------------------*/

/* Forward declarations of device and device class functions */

static fts_status_t template_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t template_dac_close(fts_dev_t *dev);
static void         template_dac_put(fts_word_t *args);

static int          template_dac_get_nchans(fts_dev_t *dev);
static int          template_dac_get_nerrors(fts_dev_t *dev);


/* Init function */

static void template_dac_init(void)
{
  fts_dev_class_t *template_dac_class;
    
  /* Declaration of the device class */
  template_dac_class = fts_dev_class_new( fts_sig_dev, fts_new_symbol("template_dac"));

  /* Definition of the device class `open' function */
  fts_dev_class_set_open_fun( template_dac_class, template_dac_open);
  /* Definition of the device class `close' function */
  fts_dev_class_set_close_fun( template_dac_class, template_dac_close);
  /* Definition of the device class `put' function */
  fts_dev_class_sig_set_put_fun( template_dac_class, template_dac_put);
  /* Definition of the device class `get_nerrors' function
     This function is used to get the number of I/O errors (for instance,
     synchronization errors 
  */
  fts_dev_class_sig_set_get_nerrors_fun( template_dac_class, template_dac_get_nerrors);
  /* Definition of the device class `get_nchans' function
     This function returns the number of channels of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( template_dac_class, template_dac_get_nchans);
}

/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.
  For instance, the arguments may be:
  [0] (symbol) "channels"
  [1] (int)    2
  [2] (symbol) "port"
  [3] (symbol) "SoundBlasterOutPort1"
  The functions fts_get_<TYPE>_by_name can be used to retrieve the values.
  
  Usually, a device `open' function:
   - parses its arguments
   - allocate a structure to store the values
   - store the pointer to this structure as the "dev_data" using fts_dev_set_device_data
   - calls the platform specific function to open the audio port
*/

static fts_status_t template_dac_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int n_channels;
  fts_symbol_t port;
  template_audio_dev_data_t *data;

  /* Allocation of device data */
  data = (template_audio_dev_data_t *)fts_malloc( sizeof( template_audio_dev_data_t));
  fts_dev_set_device_data( dev, data);

  /* Parameter parsing */
  data->n_channels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEFAULT_N_CHANNELS);
  data->port = fts_get_symbol_by_name( nargs, args, fts_new_symbol("port"), fts_new_symbol( DEFAULT_PORT));
  data->buffer_size = 0;
  data->samples_buffer = 0;

  /* This is to inform the scheduler that it should use this device to
     check for I/O errors */
  fts_dsp_set_dac_slip_dev( dev);

  /* Platform specific code should go here */
  data->info = 0;

  return fts_Success;
}


/*
  Device `close' function

  It can use the platform specific datas stored in the `device_data'
*/
static fts_status_t template_dac_close(fts_dev_t *dev)
{
  template_audio_dev_data_t *data;
  platform_specific_audio_info_t *info;

  data = (template_audio_dev_data_t *)fts_dev_get_device_data( dev);
  info = data->info;

  /* Platform specific code should go here */

  return fts_Success;
}

static int template_dac_get_nchans( fts_dev_t *dev)
{
  template_audio_dev_data_t *data;

  data = (template_audio_dev_data_t *)fts_dev_get_device_data( dev);

  return data->n_channels;
}

static int template_dac_get_nerrors( fts_dev_t *dev)
{
  template_audio_dev_data_t *data;
  platform_specific_audio_info_t *info;

  data = (template_audio_dev_data_t *)fts_dev_get_device_data( dev);
  info = data->info;

  /* Platform specific code should go here */


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

  It can use the platform specific datas stored in the `device_data'
*/

static void template_dac_put( fts_word_t *argv)
{
  fts_dev_t *dev;
  template_audio_dev_data_t *data;
  platform_specific_audio_info_t *info;
  int n_channels, channel, n, i, j;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  data = (template_audio_dev_data_t *)fts_dev_get_device_data( dev);
  info = data->info;

  n_channels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  if ( data->buffer_size < n || !data->samples_buffer)
    {
      int new_size;

      data->buffer_size = n;
      new_size = data->buffer_size * data->n_channels * sizeof( platform_sample_t);

      data->samples_buffer = (platform_sample_t *)fts_realloc( data->samples_buffer, new_size);
    }

  /* Platform specific code should go here */

  /* This is an example how to format a buffer of interleaved short samples
     from the passed arguments.
     This buffer will then be send to the audio port
  */
  for ( channel = 0; channel < n_channels; channel++)
    {
      float *in;
      
      in = (float *) fts_word_get_ptr( argv + 3 + channel);

      j = channel;
      for ( i = 0; i < n; i++)
	{
	  data->samples_buffer[j] = (short) ( 32767.0f * in[i]);
	  j += n_channels;
	}
    }

  /* output the buffer */
  /* Use the platform specific functions */
}


/*----------------------------------------------------------------------------*/
/* Audio output device                                                        */
/*----------------------------------------------------------------------------*/

/* Forward declarations of device and device class functions */

static fts_status_t template_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t template_adc_close(fts_dev_t *dev);
static void         template_adc_get(fts_word_t *args);

static int          template_adc_get_nchans(fts_dev_t *dev);


/* Init function */

static void template_adc_init(void)
{
  fts_dev_class_t *template_adc_class;

  /* Declaration of the device class */
  template_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("template_adc"));

  /* Definition of the device class `open' function */
  fts_dev_class_set_open_fun( template_adc_class, template_adc_open);
  /* Definition of the device class `close' function */
  fts_dev_class_set_close_fun( template_adc_class, template_adc_close);
  /* Definition of the device class `get' function */
  fts_dev_class_sig_set_get_fun( template_adc_class, template_adc_get);

  /* Definition of the device class `get_nchans' function
     This function returns the number of channels of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( template_adc_class, template_adc_get_nchans);
}

/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.

  See similar function for `dac' device
*/

static fts_status_t template_adc_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int n_channels;
  fts_symbol_t port;
  template_audio_dev_data_t *data;

  /* Allocation of device data */
  data = (template_audio_dev_data_t *)fts_malloc( sizeof( template_audio_dev_data_t));
  fts_dev_set_device_data( dev, data);

  /* Parameter parsing */
  data->n_channels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEFAULT_N_CHANNELS);
  data->port = fts_get_symbol_by_name( nargs, args, fts_new_symbol("port"), fts_new_symbol( DEFAULT_PORT));
  data->buffer_size = 0;
  data->samples_buffer = 0;

  /* Platform specific code should go here */
  data->info = 0;

  return fts_Success;
}


static fts_status_t template_adc_close(fts_dev_t *dev)
{
  template_audio_dev_data_t *data;
  platform_specific_audio_info_t *info;

  data = (template_audio_dev_data_t *)fts_dev_get_device_data( dev);
  info = data->info;

  /* Platform specific code should go here */

  return fts_Success;
}

static int template_adc_get_nchans(fts_dev_t *dev)
{
  template_audio_dev_data_t *data;

  data = (template_audio_dev_data_t *)fts_dev_get_device_data( dev);

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

  It can use the platform specific datas stored in the `device_data'
*/

static void template_adc_get( fts_word_t *argv)
{
  fts_dev_t *dev;
  template_audio_dev_data_t *data;
  platform_specific_audio_info_t *info;
  int n_channels, channel, n, i, j;

  dev = *((fts_dev_t **)fts_word_get_ptr( argv));

  data = (template_audio_dev_data_t *)fts_dev_get_device_data( dev);
  info = data->info;

  n_channels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  if ( data->buffer_size < n || !data->samples_buffer)
    {
      int new_size;

      data->buffer_size = n;
      new_size = data->buffer_size * data->n_channels * sizeof( platform_sample_t);

      data->samples_buffer = (platform_sample_t *)fts_realloc( data->samples_buffer, new_size);
    }

  /* Platform specific code should go here */

  /* input the buffer */
  /* Use the platform specific functions */

  /* This is an example how to read samples from a buffer of interleaved short samples.
     This supposes that the buffer read is formated as interleaved shorts...
  */
  for ( channel = 0; channel < n_channels; channel++)
    {
      float *out;
      
      out = (float *) fts_word_get_ptr( argv + 3 + channel);

      j = channel;
      for ( i = 0; i < n; i++)
	{
	  out[i] = (float)data->samples_buffer[j] / 32767.0f;
	  j += n_channels;
	}
    }
}


/******************************************************************************/
/*                                                                            */
/* MIDI device                                                                */
/*                                                                            */
/******************************************************************************/

/*
  This structure will be stored in the "device_data" and will be
  available in any device function using fts_dev_get_device_data.

  It can be used to store device specific datas as well as
  handle to the platform specific audio datas.
*/

typedef struct template_midi_dev_data {
  /* Here you can store the platform specific handles */
} template_midi_dev_data_t;


/* Forward declarations of device and device class functions */

static fts_status_t template_midi_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t template_midi_close(fts_dev_t *dev);
static fts_status_t template_midi_put( fts_dev_t *dev, unsigned char c);
static fts_status_t template_midi_get( fts_dev_t *dev, unsigned char *cp);

static void template_midi_init( void)
{
  fts_dev_class_t *template_midi_class;

  template_midi_class = fts_dev_class_new( fts_char_dev, fts_new_symbol( "template_midi"));

  fts_dev_class_set_open_fun( template_midi_class, template_midi_open);
  fts_dev_class_set_close_fun( template_midi_class, template_midi_close);
  fts_dev_class_char_set_get_fun( template_midi_class, template_midi_get);
  fts_dev_class_char_set_put_fun( template_midi_class, template_midi_put);
}


/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.
  For instance, the arguments may be:
  [0] (symbol) "port"
  [1] (symbol) "SoundBlasterMidiPort"
  The functions fts_get_<TYPE>_by_name can be used to retrieve the values.
  
  Usually, a device `open' function:
   - parses its arguments
   - allocate a structure to store the values
   - store the pointer to this structure as the "dev_data" using fts_dev_set_device_data
   - calls the platform specific function to open the audio port
*/

static fts_status_t template_midi_open( fts_dev_t *dev, int ac, const fts_atom_t *av)
{
  template_midi_dev_data_t *data;
  fts_symbol_t port;

  /* Allocation of device data */
  data = (template_midi_dev_data_t *)fts_malloc( sizeof( template_midi_dev_data_t));
  fts_dev_set_device_data( dev, data);

  /* Arguments parsing */
  port = fts_get_symbol_by_name( ac, av, fts_new_symbol("port"), fts_new_symbol( DEFAULT_PORT));

  /* Platform specific code should go here */

  return fts_Success;
}

/*
  Device `close' function

  It can use the platform specific datas stored in the `device_data'
*/
static fts_status_t template_midi_close( fts_dev_t *dev)
{
  template_midi_dev_data_t *data;

  data = (template_midi_dev_data_t *)fts_dev_get_device_data( dev);

  /* Platform specific code should go here */

  return fts_Success;
}

/*
  Device `put' function

  It can use the platform specific datas stored in the `device_data'

  This function returns a `fts_status_t' which value is:
   . `fts_success' in case of success (!)
   . `fts_dev_io_error' or something more specific in case of error
*/
static fts_status_t template_midi_put( fts_dev_t *dev, unsigned char c)
{
  template_midi_dev_data_t *data;

  data = (template_midi_dev_data_t *)fts_dev_get_device_data( dev);

  /* Platform specific code should go here */

  /* Return code */
  if (something_went_wrong())
    return &fts_dev_io_error;

  return fts_Success;
}

/*
  Device `get' function

  It can use the platform specific datas stored in the `device_data'

  NOTE: the `get' function must be NON-BLOCKING !
  On unix systems, this is usually implemented using select().
  This is very inefficient, and must be changed using either a select()
  with multiple file descriptors, or several threads.

  This function returns a `fts_status_t' which value is:
   . `fts_success' in case of success (!)
   . `fts_data_not_ready' if no data
   . `fts_dev_io_error' or something more specific in case of error
*/
static fts_status_t template_midi_get( fts_dev_t *dev, unsigned char *cp)
{
  template_midi_dev_data_t *data;

  data = (template_midi_dev_data_t *)fts_dev_get_device_data( dev);

  /* Platform specific code should go here */

  /* Return code */
  if (something_went_wrong())
    return &fts_dev_io_error;
  else if (there_is_nothing_to_read())
    return &fts_data_not_ready;

  /* Fill *cp with read data */
  *cp = 'A';

  return fts_Success;
}

/******************************************************************************/
/*                                                                            */
/* Module declaration                                                         */
/*                                                                            */
/******************************************************************************/

static void templatedev_init(void);

fts_module_t templatedev_module = { "templatedev", "TEMPLATE devices", templatedev_init, 0, 0, 0};

static void templatedev_init(void)
{
  template_dac_init();
  template_adc_init();

  template_midi_init();
}

