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

/*
 * This file's authors:
 * François Déchelle (dechelle@ircam.fr)
 */

/* 
 * This file include the jMax OSS audio port.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <linux/soundcard.h>

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 256
#define DEFAULT_CHANNELS 2

typedef struct {
  fts_audioport_t head;
  int fd;
  int fifo_size;
  short *dac_fmtbuf;
  short *adc_fmtbuf;
  /* output bytes count, for dac slip detection */
  long bytes_count;
  int no_xrun_message_already_posted;
} ossaudioport_t;

static fts_symbol_t s_slash_dev_slash_audio;
static fts_symbol_t s_read_only;
static fts_symbol_t s_write_only;

static void ossaudioport_input( fts_word_t *argv)
{
  ossaudioport_t *port;
  int n, channels, ch, i, j;

  port = (ossaudioport_t *)fts_word_get_pointer( argv+0);
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_input_channels( port);

  read( port->fd, port->adc_fmtbuf, channels * n * sizeof( short));

  for ( ch = 0; ch < channels; ch++)
    {
      float *out = (float *) fts_word_get_pointer( argv + 2 + ch);

      j = ch;
      for ( i = 0; i < n; i++)
	{
	  short s0 = port->adc_fmtbuf[j];

	  out[i] = (float)s0 / 32767.0f;
	  j += channels;
	}
    }
}

static void ossaudioport_output( fts_word_t *argv)
{
  ossaudioport_t *port;
  int n, channels, ch, i, j;

  port = (ossaudioport_t *)fts_word_get_pointer( argv+0);
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_output_channels( port);

  port->bytes_count += (channels * n * sizeof(short));

  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_pointer( argv + 2 + ch);

      j = ch;
      for ( i = 0; i < n; i++)
	{
	  short s0 = (short) ( 32767.0f * in[i]);

	  port->dac_fmtbuf[j] = s0;
	  j += channels;
	}
    }

  write( port->fd, port->dac_fmtbuf, channels * n * sizeof(short));
}


static int ossaudioport_xrun( fts_audioport_t *port)
{
  ossaudioport_t *ossport = (ossaudioport_t *)port;
  count_info count;
  int size;
  
  if (ioctl( ossport->fd, SNDCTL_DSP_GETOPTR, &count) < 0)
    {
      if ( !ossport->no_xrun_message_already_posted)
	{
	  post( "Warning: this device does not support SNDCTL_DSP_GETOPTR\n");
	  post( "         Synchronisation errors (\"dac slip\") will not be reported\n");

	  ossport->no_xrun_message_already_posted = 1;
	}

      return 0;
    }

  size = fts_audioport_get_output_channels( port) * sizeof( short) * ossport->fifo_size;
  if (count.bytes > (ossport->bytes_count + size))
    {
      ossport->bytes_count = count.bytes;
      return 1;
    }

  return 0;
}

#ifdef DEBUG
static void ossaudioport_debug( int fd)
{
  audio_buf_info info;

  if ( ioctl( fd, SNDCTL_DSP_GETOSPACE, &info) == -1)
    post( "SNDCTL_DSP_GETOSPACE\n");
  post( "fragments: %d\n", info.fragments);
  post( "total number of fragments: %d\n", info.fragstotal);
  post( "fragment size: %d bytes\n", info.fragsize);
  post( "bytes: %d\n", info.bytes);
}
#endif

static void ossaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int sample_rate, p_sample_rate, fragparam, fragment_size, channels, p_channels, flags, format, wanted_format, i;
  float sr;
  ossaudioport_t *this = (ossaudioport_t *)o;
  char device_name[256];

  fts_audioport_init( &this->head);

  sr = fts_dsp_get_sample_rate();
  sample_rate = (int)sr ;
  this->fifo_size = DEFAULT_FIFO_SIZE;

  strcpy( device_name, fts_get_symbol_arg( ac, at, 0, s_slash_dev_slash_audio));

  channels = fts_get_int_arg( ac, at, 1, DEFAULT_CHANNELS);

  if ( fts_get_symbol_arg( ac, at, 2, 0) == s_read_only)
    flags = O_RDONLY;
  else if ( fts_get_symbol_arg( ac, at, 2, 0) == s_write_only)
    flags = O_WRONLY;
  else
    flags = O_RDWR;

  if ( (this->fd = open( device_name, flags, 0)) < 0 )
    {
      fts_object_set_error( o, "cannot open device \"%s\" (%s)", device_name, strerror( errno));
      post("ossaudioport: cannot open device \"%s\" (%s)", device_name, strerror( errno));
      return;
    }

  /* Set fragment size */
  fragment_size = 2 * sizeof( short) * fts_dsp_get_tick_size();

  for( i = 0; i < 16; i++)
    if (fragment_size & (1<<i))
      break;

  fragparam = ((this->fifo_size / fts_dsp_get_tick_size()) <<16) | (i);

  if (ioctl( this->fd, SNDCTL_DSP_SETFRAGMENT, &fragparam))
    {
      fts_object_set_error( o, "cannot set fragment size or number of fragment (%s)", strerror( errno));
      post("ossaudioport: cannot set fragment size or number of fragment (%s)\n", strerror( errno));
      return;
    }

#ifdef DEBUG
  ossaudioport_debug( this->fd);
#endif

  /* Set 16 bit format */
#if WORDS_BIGENDIAN
  format = AFMT_S16_BE;
#else
  format = AFMT_S16_LE;
#endif

  wanted_format = format;
  if ( (ioctl( this->fd, SNDCTL_DSP_SETFMT, &format) == -1) || (format != wanted_format))
    {
      fts_object_set_error( o, "cannot set sample format to signed 16 bits little endian (%s)", strerror( errno));
      post("ossaudioport: cannot set sample format to signed 16 bits little endian (%s)\n", strerror( errno));
      return;
    }

  /* Set number of channels */
  p_channels = channels;
  if ( (ioctl( this->fd, SNDCTL_DSP_CHANNELS, &p_channels) == -1) || (channels != p_channels) )
    {
      fts_object_set_error( o, "cannot set number of channels (%s)", strerror( errno));
      post("ossaudioport: cannot set number of channels (%s)\n", strerror( errno));
      return;
    }

  /* Set sample rate */
  p_sample_rate = sample_rate;
  if (ioctl( this->fd, SNDCTL_DSP_SPEED, &p_sample_rate) == -1)
    {
      fts_object_set_error( o, "cannot set sample rate (%s)", strerror( errno));
      post("ossaudioport: cannot set sample rate (%s)\n", strerror( errno));
      return;
    }

 if (sample_rate != p_sample_rate)
 {
     post("[ossaudioport]: cannot set to wanted sample rate (%d), get (%d)\n",
	  sample_rate, p_sample_rate);
     fts_log("[ossaudioport]: cannot set to wanted sample rate (%d), get (%d)\n",
	     sample_rate, p_sample_rate);
 }

 /*
   TODO:
   Add a conditionnal to know if difference of sampling rate is too important:
   e.g. a threshold on difference between given and wanted
 */
  if (flags != O_WRONLY)
    {
      fts_audioport_set_input_channels( (fts_audioport_t *)this, channels);
      fts_audioport_set_input_function( (fts_audioport_t *)this, ossaudioport_input);
    }
  if (flags != O_RDONLY)
    {
      fts_audioport_set_output_channels( (fts_audioport_t *)this, channels);
      fts_audioport_set_output_function( (fts_audioport_t *)this, ossaudioport_output);
    }

  fts_audioport_set_xrun_function( (fts_audioport_t *)this, ossaudioport_xrun);

  this->adc_fmtbuf = (short *)fts_malloc( fts_dsp_get_tick_size() * channels * sizeof(short));
  this->dac_fmtbuf = (short *)fts_malloc( fts_dsp_get_tick_size() * channels * sizeof(short));

  this->no_xrun_message_already_posted = 0;
}

static void ossaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ossaudioport_t *this = (ossaudioport_t *)o;

  fts_audioport_delete( &this->head);

  if (this->fd >= 0)
    close( this->fd);

  fts_free( this->adc_fmtbuf);
  fts_free( this->dac_fmtbuf);
}

static void ossaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static void ossaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( ossaudioport_t), ossaudioport_init, ossaudioport_delete);

  fts_class_add_daemon( cl, obj_property_get, fts_s_state, ossaudioport_get_state);
}

void ossaudioport_config( void)
{
  fts_symbol_t s = fts_new_symbol("ossaudioport");

  fts_class_install( s, ossaudioport_instantiate);
  /*
   * If a default class is not installed, install it.
   * If ALSA package has already installed a default,
   * we don't overwrite it
   */
  if (!fts_audioport_get_default_class())
    fts_audioport_set_default_class(s);

  s_slash_dev_slash_audio = fts_new_symbol( "/dev/audio");
  s_read_only = fts_new_symbol( "read_only");
  s_write_only = fts_new_symbol( "write_only");
}
