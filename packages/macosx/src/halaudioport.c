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
 * This file include the jMax HAL audio port for Mac OS X
 */

#include <stdio.h>

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 256
#define DEFAULT_CHANNELS 2

typedef struct {
  fts_audioport_t head;
} halaudioport_t;

static void halaudioport_input( fts_word_t *argv)
{
  halaudioport_t *port;
  int n, channels, ch, i, j;

  port = (halaudioport_t *)fts_word_get_ptr( argv+0);
  n = fts_word_get_long(argv + 1);
  channels = fts_audioport_get_input_channels( port);

  for ( ch = 0; ch < channels; ch++)
    {
      float *out = (float *) fts_word_get_ptr( argv + 2 + ch);

      j = ch;
      for ( i = 0; i < n; i++)
	{
	  short s0 = port->adc_fmtbuf[j];

#ifdef FTS_HAS_BIG_ENDIAN
	  SWAP_SHORT( s0);
#endif

	  out[i] = (float)s0 / 32767.0f;
	  j += channels;
	}
    }
}

static void halaudioport_output( fts_word_t *argv)
{
  halaudioport_t *port;
  int n, channels, ch, i, j;

  port = (halaudioport_t *)fts_word_get_ptr( argv+0);
  n = fts_word_get_long(argv + 1);
  channels = fts_audioport_get_output_channels( port);

  port->bytes_count += (channels * n * sizeof(short));

  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_ptr( argv + 2 + ch);

      j = ch;
      for ( i = 0; i < n; i++)
	{
	  short s0 = (short) ( 32767.0f * in[i]);

#ifdef FTS_HAS_BIG_ENDIAN
	  SWAP_SHORT( s0);
#endif

	  port->dac_fmtbuf[j] = s0;
	  j += channels;
	}
    }

  write( port->fd, port->dac_fmtbuf, channels * n * sizeof(short));
}


static int halaudioport_xrun( fts_audioport_t *port)
{
  halaudioport_t *halport = (halaudioport_t *)port;

  return 0;
}

static void halaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int sample_rate, p_sample_rate, fragparam, fragment_size, channels, p_channels, flags, format, i;
  float sr;
  halaudioport_t *this = (halaudioport_t *)o;
  char device_name[256];

  fts_audioport_init( &this->head);

  ac--;
  at++;
  sr = fts_param_get_float( fts_s_sampling_rate, DEFAULT_SAMPLING_RATE);
  sample_rate = (int)sr ;
  this->fifo_size = fts_param_get_int(fts_s_fifo_size, DEFAULT_FIFO_SIZE);

  strcpy( device_name, fts_symbol_name( fts_get_symbol_arg( ac, at, 0, s_slash_dev_slash_audio)));

  channels = fts_get_int_arg( ac, at, 1, DEFAULT_CHANNELS);

  if (flags != O_WRONLY)
    {
      fts_audioport_set_input_channels( (fts_audioport_t *)this, channels);
      fts_audioport_set_input_function( (fts_audioport_t *)this, halaudioport_input);
    }
  if (flags != O_RDONLY)
    {
      fts_audioport_set_output_channels( (fts_audioport_t *)this, channels);
      fts_audioport_set_output_function( (fts_audioport_t *)this, halaudioport_output);
    }

  fts_audioport_set_xrun_function( (fts_audioport_t *)this, halaudioport_xrun);

  this->adc_fmtbuf = (short *)fts_malloc( fts_get_tick_size() * channels * sizeof(short));
  this->dac_fmtbuf = (short *)fts_malloc( fts_get_tick_size() * channels * sizeof(short));

  this->no_xrun_message_already_posted = 0;
}

static void halaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;

  fts_audioport_delete( &this->head);

  fts_free( this->adc_fmtbuf);
  fts_free( this->dac_fmtbuf);
}

static void halaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static fts_status_t halaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( halaudioport_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, halaudioport_init);
  fts_method_define( cl, fts_SystemInlet, fts_s_delete, halaudioport_delete, 0, 0);
  /* define variable */
  fts_class_add_daemon( cl, obj_property_get, fts_s_state, halaudioport_get_state);

  return fts_Success;
}

void halaudioport_config( void)
{
  fts_class_install( fts_new_symbol("halaudioport"), halaudioport_instantiate);
}
