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
 * Authors: Francois Dechelle.
 *
 */
#include "fts.h"
#include "dtdfifo.h"
#include "dtdserver.h"

typedef struct {
  fts_object_t _o;
  int n_channels;
  enum { dtd_playing, dtd_pause} state;
  dtdfifo_t *fifo;
  fts_alarm_t alarm;
} readsf_t;

static fts_symbol_t readsf_dsp_function;

static void readsf_alarm( fts_alarm_t *alarm, void *p)
{
  fts_object_t *o = (fts_object_t *)p;

  fts_alarm_unarm( alarm);

  fts_outlet_bang( o, fts_object_get_outlets_number(o) - 1);
}

static void readsf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int n_channels;

  n_channels = fts_get_long_arg(ac, at, 1, 1);
  this->n_channels = (n_channels < 1) ? 1 : n_channels;

  this->fifo = dtdserver_new( this->n_channels);

  this->state = dtd_pause;

  fts_alarm_init( &(this->alarm), 0, readsf_alarm, this);	

  dsp_list_insert(o);
}

static void readsf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  dtdserver_free( this->fifo);

  fts_alarm_unarm( &this->alarm);	

  dsp_list_remove(o);
}

static void clear_outputs( fts_word_t *argv)
{
  readsf_t *this;
  int n, channel, i;
  float *out;

  this = (readsf_t *)fts_word_get_ptr( argv + 0);
  n = fts_word_get_long( argv + 1);

  for ( channel = 0; channel < this->n_channels; channel++)
    {
      out = (float *)fts_word_get_ptr( argv + 2 + channel);

      for ( i = 0; i < n; i++)
	out[i] = 0.0;
    }
}

static void readsf_dsp( fts_word_t *argv)
{
  readsf_t *this;
  dtdfifo_t *fifo;
  int n, n_channels, read_size, channel;
  volatile float *src;

  this = (readsf_t *)fts_word_get_ptr( argv + 0);

  if ( this->state == dtd_pause)
    {
      clear_outputs( argv);
      return;
    }

  n = fts_word_get_long( argv + 1);

  fifo = this->fifo;

  if ( dtdfifo_get_state( fifo) == FIFO_INACTIVE)
    {
      clear_outputs( argv);
      return;
    }

  n_channels = this->n_channels;

  read_size = n_channels * n * sizeof( float);

  if ( dtdfifo_get_read_level( fifo) < read_size )
    {
      clear_outputs( argv);

      if ( dtdfifo_get_state( fifo) == FIFO_EOF)
	{
	  fts_alarm_set_delay( &this->alarm, 0.01f);
	  fts_alarm_arm( &this->alarm);

	  dtdfifo_set_state( fifo, FIFO_INACTIVE);
	}
      else if (dtdfifo_get_state( fifo) == FIFO_ACTIVE)
	post( "Warning: readsf~ data late\n");

      return;
    }

  src = (volatile float *)dtdfifo_get_read_pointer( fifo);

  for ( channel = 0; channel < n_channels; channel++)
    {
      float *out;
      int i, j;

      out = (float *)fts_word_get_ptr( argv + 2 + channel);
      j = channel;

      for ( i = 0; i < n; i++)
	{
	  out[i] = src[j];
	  j += n_channels;
	}
    }

  dtdfifo_incr_read_index( fifo, read_size);
}

static void readsf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[32];
  int i;

  fts_set_ptr( argv + 0, this);
  fts_set_int( argv + 1, fts_dsp_get_output_size( dsp, 0));

  for ( i = 0; i < this->n_channels; i++)
    fts_set_symbol( argv + 2 + i, fts_dsp_get_output_name( dsp, i));

  dsp_add_funcall( readsf_dsp_function, 2 + this->n_channels, argv);
}

static void readsf_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  const char *filename;

  filename = fts_symbol_name( fts_get_symbol_arg( ac, at, 0, 0));

/*    if ( !fts_file_get_read_path( filename, filepath)) */
/*      { */
/*        post("readsf~: cannot open file '%s' for reading\n", filename); */
/*        return; */
/*      } */

  this->state = dtd_pause;

  dtdserver_open( this->fifo, filename, fts_symbol_name(fts_get_search_path()) );
}

static void readsf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  dtdserver_close( this->fifo);

  this->state = dtd_pause;
}

static void readsf_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  this->state = dtd_playing;
}

static void readsf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  this->state = dtd_pause;
}

static void readsf_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int n;

  n = fts_get_int_arg( ac, at, 0, 0);

  if ( n == 1 && this->state == dtd_pause)
    readsf_play( o, 0, 0, 0, 0);
  else if ( n == 0 && this->state == dtd_playing)
    readsf_pause( o, 0, 0, 0, 0);
}

static fts_status_t readsf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n_channels, i;
  fts_symbol_t a[4];

  n_channels = fts_get_long_arg(ac, at, 1, 1);

  if (n_channels < 1)
    n_channels = 1;

  fts_class_init(cl, sizeof(readsf_t), 1, n_channels + 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, readsf_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, readsf_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, readsf_put, 1, a);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("open"),  readsf_open, 3, a, 0);

  fts_method_define( cl, 0, fts_new_symbol("play"), readsf_play, 0, 0);
  fts_method_define( cl, 0, fts_new_symbol("start"), readsf_play, 0, 0);

  fts_method_define( cl, 0, fts_new_symbol("pause"), readsf_pause, 0, 0);

  fts_method_define( cl, 0, fts_new_symbol("close"), readsf_close, 0, 0);

  a[0] = fts_s_int;
  fts_method_define( cl, 0, fts_s_int, readsf_number, 1, a);

  for (i = 0; i < n_channels; i++)
    dsp_sig_outlet(cl, i);

  readsf_dsp_function = fts_new_symbol( "readsf~");
  dsp_declare_function( readsf_dsp_function, readsf_dsp);

  return fts_Success;
}

void dtdtest_config(void)
{
  fts_metaclass_install(fts_new_symbol("readsf~"), readsf_instantiate, fts_first_arg_equiv);
}
