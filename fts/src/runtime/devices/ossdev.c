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


/* This file include the device specific to the OSS/LINUX architecture.

   These audio driver work for any number of channels, allow access to all
   the OSS/LINUX audio parameters from the FTS UCS system.

   A Device class is defined for DACs and ADCs.

   In the Future, a io_ctrl operations at the UCS level will allow access
   to the oss audio paramters from UCS programs, accessing to the linux device
   directly.
*/

#define DEF_DEVICE_NAME "/dev/audio"
#define DEF_SAMPLING_RATE ((float)44100.0f)
#define DEF_FIFO_SIZE 256
#define DEF_CHANNELS 2

#undef OSSDEV_DEBUG

/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "sys.h"
#include "lang.h"
#include "runtime/devices/devices.h"
#include "runtime/devices/unixdev.h"
#include "runtime/files.h"

extern void fts_dsp_set_dac_slip_dev(fts_dev_t *dev);

/* forward declarations */

static void oss_dac_init(void);
static void oss_adc_init(void);
static void oss_midi_init(void);

/******************************************************************************/
/*                                                                            */
/* Audio descriptor: keeps file descriptor and parameters                     */
/*                                                                            */
/******************************************************************************/

typedef struct _audio_desc
{
  fts_symbol_t device_name;
  /* status */
  int dac_open;
  int adc_open;
  /* file descriptor */
  int fd;
  /* parameters */
  int sampling_rate;
  int fifo_size;
  int nchannels;
  /* formating buffers */
  short *dac_fmtbuf;
  short *adc_fmtbuf;
  /* output bytes count, for dac slip detection */
  long bytes_count;
} audio_desc_t;

static fts_hash_table_t *audio_desc_table;

static void audio_desc_init( audio_desc_t *aud, fts_symbol_t device_name)
{
  aud->device_name = device_name;

  aud->dac_open = 0;
  aud->adc_open = 0;

  aud->fd = -1;

  aud->sampling_rate = 0;
  aud->fifo_size = 0;
  aud->nchannels = 0;

  aud->dac_fmtbuf = 0;
  aud->adc_fmtbuf = 0;

  aud->bytes_count = 0;
}

static audio_desc_t *audio_desc_new( fts_symbol_t device_name)
{
  audio_desc_t *p;

  p = (audio_desc_t *)fts_malloc( sizeof( audio_desc_t));

  assert( p != 0);

  audio_desc_init( p, device_name);

  return p;
}

static void audio_desc_destroy( audio_desc_t *aud)
{
  if ( aud->fd >= 0)
    {
      close( aud->fd);
      aud->fd = -1;
    }

  if (aud->dac_fmtbuf)
    {
      fts_free( aud->dac_fmtbuf);
      aud->dac_fmtbuf = 0;
    }

  if (aud->adc_fmtbuf)
    {
      fts_free( aud->adc_fmtbuf);
      aud->adc_fmtbuf = 0;
    }
}

static void audio_desc_free( audio_desc_t *aud)
{
  audio_desc_destroy( aud);
  fts_free( aud);
}

static int audio_desc_set_parameters( audio_desc_t *aud)
{
  int format, sampling_rate, fragparam, fragment_size, max_fragments, nchannels, i;

  /* Set fragment size */
  /* HACK !!! Vector size is not known when this function is called */
  fragment_size = 2 * sizeof( short) * 64;

  for( i = 0; i < 16; i++)
    if (fragment_size & (1<<i))
      break;

  max_fragments = aud->fifo_size / 64;

  fragparam = (max_fragments<<16) | (i);

  if (ioctl( aud->fd, SNDCTL_DSP_SETFRAGMENT, &fragparam))
    {
      post( "Error in ioctl(SNDCTL_DSP_SETFRAGMENT): %s\n", strerror( errno));
      return -1;
    }

#ifdef OSSDEV_DEBUG
  {
    audio_buf_info info;

    if ( ioctl( aud->fd, SNDCTL_DSP_GETOSPACE, &info) == -1)
      post( "SNDCTL_DSP_GETOSPACE\n");
    post( "fragments: %d\n", info.fragments);
    post( "total number of fragments: %d\n", info.fragstotal);
    post( "fragment size: %d bytes\n", info.fragsize);
    post( "bytes: %d\n", info.bytes);
  }
#endif

  /* Set 16 bit format */
  format = AFMT_S16_LE;
  if (ioctl( aud->fd, SNDCTL_DSP_SETFMT, &format) == -1)
    {
      post("Error in ioctl(SNDCTL_DSP_SETFMT)\n");
      return -1;
    }

  if (format != AFMT_S16_LE)
    {
      post("Audio device doesn't support 16 bit mode\n");
      return -1;
    }

  /* Set number of channels */
  nchannels = aud->nchannels;
  if (ioctl( aud->fd, SNDCTL_DSP_CHANNELS, &nchannels) == -1)
    {
      post( "Error in ioctl(SNDCTL_DSP_CHANNELS)\n");
      return -1;
    }

  if ( nchannels != aud->nchannels)
    {
      post("Audio device doesn't support %d channels\n", aud->nchannels);
      return -1;
    }

  /* Set sampling rate */
  sampling_rate = aud->sampling_rate;
  if (ioctl( aud->fd, SNDCTL_DSP_SPEED, &sampling_rate) == -1)
    {
      post("error in ioctl(SNDCTL_DSP_SPEED)\n");
      return -1;
    }

  if (aud->sampling_rate != sampling_rate)
    {
      post("Audio device doesn't support requested sampling rate (wanted %d, got %d)\n", aud->sampling_rate, sampling_rate);
    }

  return 0;
}

/* 
   To put the audio device in the right configuration,
   set the dac_open/adc_open fields in the right
   status and call the following function; work also
   to close the unix device.

   Return 0 in case of success, -1 in case of errors in opening.
  */

static int audio_desc_check_full_duplex_caps( audio_desc_t *aud)
{
  int caps;

  if ( ioctl( aud->fd, SNDCTL_DSP_GETCAPS, &caps) < 0)
    {
      post( "Error in ioctl(SNDCTL_DSP_GETCAPS): %s\n", strerror( errno));
      return -1;
    }

  if ( (caps & DSP_CAP_DUPLEX) == 0)
    {
      post( "Audio device does not support full duplex\n");
      return -1;
    }

  return 0;
}

#define CMD_OPEN_DAC    1
#define CMD_OPEN_ADC    2
#define CMD_CLOSE_DAC   3
#define CMD_CLOSE_ADC   4

static int audio_desc_update( audio_desc_t *aud, int cmd)
{
  int fd, flags;

  flags = -1;

  switch (cmd) {
  case CMD_OPEN_DAC:
    flags = ( aud->adc_open ) ? O_RDWR : O_WRONLY;
    aud->dac_open = 1;
    break;
  case CMD_OPEN_ADC:
    flags = ( aud->dac_open ) ? O_RDWR : O_RDONLY;
    aud->adc_open = 1;
    break;
  case CMD_CLOSE_DAC:
    if ( aud->adc_open )
      flags = O_RDONLY;
    aud->dac_open = 0;
    break;
  case CMD_CLOSE_ADC:
    if ( aud->dac_open )
      flags = O_WRONLY;
    aud->adc_open = 0;
    break;
  }

  if (flags < 0)
    {
      fts_symbol_t device_name;

      device_name = aud->device_name;

      audio_desc_free( aud);

      fts_hash_table_remove( audio_desc_table, device_name);

      return 0;
    }

  audio_desc_destroy( aud);

  fd = open( fts_symbol_name( aud->device_name), flags, 0);

  if ( fd < 0 )
    {
      post( "Error opening OSS device: %s\n", strerror(errno));

      audio_desc_free( aud);

      fts_hash_table_remove( audio_desc_table, aud->device_name);

      return -1;
    }

  aud->fd = fd;

  /* 
     If the device is opened read and write, check 
     if the device supports full duplex.
     If it does not support full duplex, then return error 
  */
  if ( aud->dac_open && aud->adc_open)
    {
      audio_desc_check_full_duplex_caps( aud);
    }

  if ( audio_desc_set_parameters( aud) )
    {
      close( aud->fd);

      aud->fd = -1;

      return -1;
    }

  /* Allocate the formatting buffers */
  if (aud->dac_open)
    aud->dac_fmtbuf = (short *) fts_malloc( MAXVS * aud->nchannels * sizeof(short));
  if (aud->adc_open)
    aud->adc_fmtbuf = (short *) fts_malloc( MAXVS * aud->nchannels * sizeof(short));

  return 0;
}

static audio_desc_t *audio_desc_get( fts_symbol_t s)
{
  fts_atom_t a;
  audio_desc_t *aud;

  if ( fts_hash_table_lookup( audio_desc_table, s, &a))
    {
      aud = (audio_desc_t *)fts_get_ptr( &a);
    }
  else
    {
      aud = audio_desc_new( s);

      assert( aud != 0);

      fts_set_ptr( &a, aud);
      assert( fts_hash_table_insert( audio_desc_table, s, &a) != 0);
    }

  return aud;
}


/******************************************************************************/
/*                                                                            */
/*                              DAC Devices                                   */
/*                                                                            */
/******************************************************************************/


/* Forward declarations of DAC dev and dev class static functions */

static fts_status_t oss_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t oss_dac_close(fts_dev_t *dev);
static void         oss_dac_put(fts_word_t *args);

static int          oss_dac_get_nchans(fts_dev_t *dev);
static int oss_dac_get_nerrors(fts_dev_t *dev);

/* Init and shutdown functions */

static void oss_dac_init(void)
{
  fts_dev_class_t *oss_dac_class;
    
  /* OSS DAC class  */

  oss_dac_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("oss_dac"));

  fts_dev_class_set_open_fun(oss_dac_class, oss_dac_open);
  fts_dev_class_set_close_fun(oss_dac_class, oss_dac_close);
  fts_dev_class_sig_set_put_fun(oss_dac_class, oss_dac_put);
  fts_dev_class_sig_set_get_nerrors_fun( oss_dac_class, oss_dac_get_nerrors);
  fts_dev_class_sig_set_get_nchans_fun(oss_dac_class, oss_dac_get_nchans);
}

/* OSS DAC control/options functions */

/*
   The dev_ctrl operation for the oss_dac
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the oss dac.

   NOTE that for the OSS you *have* to set the sample_rate using
   the dedicated UCS command (audio-set sample-rate) , not the oss_dac.

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in samples frame units

   sample_rate  sampling frequency     int      44100     ANY (as long as it is supported by the machine)
 
*/

/* OSS DAC dev class functions */
static fts_status_t oss_dac_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_symbol_t s;
  audio_desc_t *aud;

  s = fts_get_symbol_by_name( nargs, args, fts_new_symbol("device"), fts_new_symbol(DEF_DEVICE_NAME));

  aud = audio_desc_get( s);

  /* Parameter parsing  */
  aud->sampling_rate = (int) fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);
  aud->fifo_size = fts_param_get_int(fts_s_fifo_size, DEF_FIFO_SIZE);
  aud->nchannels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEF_CHANNELS);

  /* Open the device */
  if ( audio_desc_update( aud, CMD_OPEN_DAC) < 0)
    return &fts_dev_open_error;

  fts_dev_set_device_data(dev, aud);

  fts_dsp_set_dac_slip_dev( dev);

  return fts_Success;
}


static fts_status_t oss_dac_close(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);

  audio_desc_update( aud, CMD_CLOSE_DAC);

  return fts_Success;
}

static int oss_dac_get_nchans(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);

  return aud->nchannels;
}

static int oss_dac_get_nerrors(fts_dev_t *dev)
{
  count_info count;
  int size;
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);
  
  if (ioctl( aud->fd, SNDCTL_DSP_GETOPTR, &count) < 0)
    {
      post("Error in ioctl(SNDCTL_DSP_GETOPTR)\n");
      return 0;
    }

  size = aud->nchannels * sizeof( short) * aud->fifo_size;
  if (count.bytes > (aud->bytes_count + size))
    {
      aud->bytes_count = count.bytes;
      return 1;
    }
  else
    return 0;
}


/* 
   Arguments: fts_dev_t *dev, int nchans, int n, float *buf1 ... bufm 
*/
static void oss_dac_put(fts_word_t *argv)
{
  int n, i, nchannels, channel, j;
  audio_desc_t *aud;

  aud = (audio_desc_t *)fts_dev_get_device_data(*((fts_dev_t **)fts_word_get_ptr( argv) ));

  if (aud->fd < 0)
    return;

  nchannels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  aud->bytes_count += (nchannels * n * sizeof(short));

  if ( nchannels == 2)
    {
      float *in0, *in1;

      in0 = (float *) fts_word_get_ptr(argv + 3);
      in1 = (float *) fts_word_get_ptr(argv + 4);

      j = 0;
      for ( i = 0; i < n; i++)
	{
	  aud->dac_fmtbuf[j++] = (short) ( 32767.0f * in0[i]);
	  aud->dac_fmtbuf[j++] = (short) ( 32767.0f * in1[i]);
	}
    }
  else
    {
      for ( channel = 0; channel < nchannels; channel++)
	{
	  float *in;

	  in = (float *) fts_word_get_ptr( argv + 3 + channel);

	  j = channel;
	  for ( i = 0; i < n; i++)
	    {
	      aud->dac_fmtbuf[j] = (short) ( 32767.0f * in[i]);
	      j += nchannels;
	    }
	}
    }

  write( aud->fd, aud->dac_fmtbuf, nchannels * n * sizeof(short));
}


/******************************************************************************/
/*                                                                            */
/*                              ADC Devices                                   */
/*                                                                            */
/******************************************************************************/

/* Forward declarations of ADC dev and dev class static functions */

static fts_status_t oss_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t oss_adc_close(fts_dev_t *dev);
static void         oss_adc_get(fts_word_t *args);

static int          oss_adc_get_nchans(fts_dev_t *dev);


/* Init and functions */

static void oss_adc_init(void)
{
  fts_dev_class_t *oss_adc_class;

  /* OSS ADC class  */

  oss_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("oss_adc"));

  /* device functions */

  fts_dev_class_set_open_fun(oss_adc_class, oss_adc_open);
  fts_dev_class_set_close_fun(oss_adc_class, oss_adc_close);
  fts_dev_class_sig_set_get_fun(oss_adc_class, oss_adc_get);

  fts_dev_class_sig_set_get_nchans_fun(oss_adc_class, oss_adc_get_nchans);
}

/* OSS ADC control/options functions: use the same parser as the dac */

/* OSS ADC dev class functions */

static fts_status_t oss_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_symbol_t s;
  audio_desc_t *aud;

  s = fts_get_symbol_by_name( nargs, args, fts_new_symbol("device"), fts_new_symbol(DEF_DEVICE_NAME));

  aud = audio_desc_get( s);

  /* Parameter parsing  */
  aud->sampling_rate = (int) fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);
  aud->fifo_size = fts_param_get_int(fts_s_fifo_size, DEF_FIFO_SIZE);
  aud->nchannels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEF_CHANNELS);

  /* Open the device */
  aud->adc_open = 1;
  if ( audio_desc_update( aud, CMD_OPEN_ADC) < 0)
    return &fts_dev_open_error;

  fts_dev_set_device_data(dev, aud);

  return fts_Success;
}


static fts_status_t oss_adc_close(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);

  aud->adc_open = 0;
  audio_desc_update( aud, CMD_CLOSE_ADC);

  return fts_Success;
}

static int oss_adc_get_nchans(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);

  return aud->nchannels;
}

/* 
   Arguments: fts_dev_t *dev, int n, float *buf1 ... bufm 
*/
static void oss_adc_get( fts_word_t *argv)
{
  int n, i, nchannels, channel, j;
  audio_desc_t *aud;

  aud = (audio_desc_t *)fts_dev_get_device_data(*((fts_dev_t **)fts_word_get_ptr( argv)));

  if (aud->fd < 0)
    return;

  nchannels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

   read( aud->fd, aud->adc_fmtbuf, nchannels * n * sizeof( short));

  if ( nchannels == 2)
    {
      float *out0, *out1;

      out0 = (float *) fts_word_get_ptr(argv + 3);
      out1 = (float *) fts_word_get_ptr(argv + 4);

      j = 0;
      for ( i = 0; i < n; i++)
	{
	  out0[i] = (float)aud->adc_fmtbuf[j++] / 32767.0f;
	  out1[i] = (float)aud->adc_fmtbuf[j++] / 32767.0f;
	}
    }
  else
    {
      for ( channel = 0; channel < nchannels; channel++)
	{
	  float *out;

	  out = (float *) fts_word_get_ptr( argv + 3 + channel);

	  j = channel;
	  for ( i = 0; i < n; i++)
	    {
	      out[i] = (float)aud->adc_fmtbuf[j] / 32767.0f;
	      j += nchannels;
	    }
	}
    }
}


/******************************************************************************/
/*                                                                            */
/* MIDI device                                                                */
/*                                                                            */
/******************************************************************************/

#define MIDI_BUFSIZE 256

static fts_status_t oss_midi_open( fts_dev_t *dev, int ac, const fts_atom_t *av)
{
  fts_symbol_t name;
  fd_dev_data_t *p;
  int fd;

  p = fd_data_new( MIDI_BUFSIZE, MIDI_BUFSIZE);
  fts_dev_set_device_data( dev, (void *) p);

  name = fts_get_symbol_by_name( ac, av, fts_new_symbol("device"), fts_new_symbol( "/dev/midi00"));
  fd = open( fts_symbol_name( name), O_RDWR);

  if (fd < 0)
    {
      return &fts_dev_open_error;
    }

  fd_data_set_input_fd( p, fd);
  fd_data_set_output_fd( p, fd);

  return fts_Success;
}

static fts_status_t oss_midi_close( fts_dev_t *dev)
{
  fd_dev_data_t *p = (fd_dev_data_t *)fts_dev_get_device_data( dev);

  close( fd_data_get_input_fd( p));

  fd_data_delete( p);

  return fts_Success;
}

static fts_status_t oss_midi_put( fts_dev_t *dev, unsigned char c)
{
  fts_status_t status;

  status = fd_dev_put( dev, c);
  if (status != fts_Success)
    return status;

  return fd_dev_flush( dev);
}


static void oss_midi_init( void)
{
  fts_dev_class_t *oss_midi_class;

  oss_midi_class = fts_dev_class_new( fts_char_dev, fts_new_symbol( "oss_midi"));

  fts_dev_class_set_open_fun( oss_midi_class, oss_midi_open);
  fts_dev_class_set_close_fun( oss_midi_class, oss_midi_close);
  fts_dev_class_char_set_get_fun( oss_midi_class, fd_dev_get);
  fts_dev_class_char_set_put_fun( oss_midi_class, oss_midi_put);
}

/******************************************************************************/
/*                                                                            */
/*                              Init function                                 */
/*                                                                            */
/******************************************************************************/

void ossdev_init(void)
{
  audio_desc_table = fts_hash_table_new();

  oss_dac_init();
  oss_adc_init();

  oss_midi_init();
}

