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
 * This file's authors: Francois Dechelle.
 */


#include <math.h>

#include <fts/fts.h>
#include "dtddefs.h"
#include "dtdfifo.h"
#include "dtdserver.h"

static fts_symbol_t s_open;
static fts_symbol_t s_close;
static fts_symbol_t s_play;
static fts_symbol_t s_pause;
static fts_symbol_t s_record;

static int number_of_dtdobjs = 0;
static int dtdserver_started = 0;

/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* readsf~                                                                */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

typedef enum { 
  readsf_closed, 
  readsf_opened, 
  readsf_pending, 
  readsf_playing, 
  readsf_paused 
} readsf_state_t;

typedef struct {
  fts_object_t _o;
  int n_channels;
  readsf_state_t state;
  int id;
  dtdfifo_t *fifo;
  fts_alarm_t eof_alarm;
  int can_post_data_late;
  fts_alarm_t post_data_late_alarm;
} readsf_t;

static fts_symbol_t readsf_dsp_function;

static void readsf_open_realize( readsf_t *this, const char *filename)
{
  int id;

  id = dtdfifo_allocate( FTS_SIDE);

  if (id < 0)
    {
      post( "readsf~: error: cannot allocate fifo for DTD server\n");
      return;
    }

  this->id = id;
  this->fifo = dtdfifo_get( this->id);

  dtdserver_open( id, filename, fts_symbol_name(fts_get_search_path()), this->n_channels);

  this->state = readsf_opened;
}

static void readsf_close_realize( readsf_t *this)
{
  if (this->id >= 0)
    {
      dtdfifo_set_used( this->fifo, FTS_SIDE, 0);

      dtdserver_close( this->id);
    }

  this->id = -1;
  this->fifo = 0;
  this->state = readsf_closed;
}

static void readsf_state_machine( readsf_t *this, fts_symbol_t message, int ac, const fts_atom_t *at)
{
  switch( this->state) {
  case readsf_closed:
    if (message == s_open)
      readsf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
    else if (message == s_close)
      {
      }
    else if (message == s_play)
      post( "readsf~: error: no file opened\n");
    else if (message == s_pause)
      post( "readsf~: error: not playing\n");
    break;

  case readsf_opened:
    if (message == s_open)
      {
	readsf_close_realize( this);
	readsf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
      }
    else if (message == s_close)
      readsf_close_realize( this);
    else if (message == s_play)
      this->state = readsf_pending;
    else if (message == s_pause)
      post( "readsf~: error: not playing\n");
    break;

  case readsf_pending:
    if (message == s_open)
      {
	readsf_close_realize( this);
	readsf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
      }
    else if (message == s_close)
      readsf_close_realize( this);
    else if (message == s_play)
      {
      }
    else if (message == s_pause)
      {
      }
    break;

  case readsf_playing:
    if (message == s_open)
      {
	readsf_close_realize( this);
	readsf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
      }
    else if (message == s_close)
      readsf_close_realize( this);
    else if (message == s_play)
      {
      }
    else if (message == s_pause)
      {
	this->state = readsf_paused;
      }
    break;

  case readsf_paused:
    if (message == s_open)
      {
	readsf_close_realize( this);
	readsf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
      }
    else if (message == s_close)
      readsf_close_realize( this);
    else if (message == s_play)
      this->state = readsf_playing;
    else if (message == s_pause)
      {
      }
    break;

  }
}

static void readsf_eof_alarm( fts_alarm_t *alarm, void *p)
{
  fts_object_t *o = (fts_object_t *)p;

  fts_alarm_unarm( alarm);

  fts_outlet_bang( o, fts_object_get_outlets_number(o) - 1);
}

static void readsf_post_data_late_alarm( fts_alarm_t *alarm, void *p)
{
  readsf_t *this = (readsf_t *)p;

  fts_alarm_unarm( alarm);

  this->can_post_data_late = 1;
}

static void readsf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int n_channels;

  number_of_dtdobjs++;

  n_channels = fts_get_long_arg(ac, at, 1, 1);
  this->n_channels = (n_channels < 1) ? 1 : n_channels;

  /* 
   * Create enough fifos so that there is at least X*number_of_objects fifos
   * so that, when you open a file, you don't need to create the fifo, i.e.
   * create the file and mmap it, which would block FTS
   */
#define X 2.0

  while ( dtdfifo_get_number_of_fifos() < (int) (ceil( X * number_of_dtdobjs)) )
    {
      dtdserver_new_fifo();
    }

  this->state = readsf_closed;
  this->can_post_data_late = 1;

  fts_alarm_init( &(this->eof_alarm), 0, readsf_eof_alarm, this);	
  fts_alarm_init( &(this->post_data_late_alarm), 0, readsf_post_data_late_alarm, this);	

  dsp_list_insert(o);
}

static void readsf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  fts_alarm_unarm( &this->eof_alarm);	
  fts_alarm_unarm( &this->post_data_late_alarm);	

  dsp_list_remove(o);

  number_of_dtdobjs--;
}

static void clear_outputs( int n, int n_channels, fts_word_t *outputs)
{
  int channel, i;
  float *out;

  for ( channel = 0; channel < n_channels; channel++)
    {
      out = (float *)fts_word_get_ptr( outputs + channel);

      for ( i = 0; i < n; i++)
	out[i] = 0.0;
    }
}

static void read_fifo( int n, int n_channels, dtdfifo_t *fifo, fts_word_t *outputs)
{
  volatile float *src;
  int channel;

  src = (volatile float *)dtdfifo_get_read_pointer( fifo);

  for ( channel = 0; channel < n_channels; channel++)
    {
      float *out;
      int i, j;

      out = (float *)fts_word_get_ptr( outputs + channel);
      j = channel;

      for ( i = 0; i < n; i++)
	{
	  out[i] = src[j];
	  j += n_channels;
	}
    }

  dtdfifo_incr_read_index( fifo, n_channels * n * sizeof(float));
}

static void readsf_dsp( fts_word_t *argv)
{
  readsf_t *this;
  int n, n_channels, read_size;
  fts_word_t *outputs;

  this = (readsf_t *)fts_word_get_ptr( argv + 0);
  n = fts_word_get_long( argv + 1);
  outputs = argv+2;

  n_channels = this->n_channels;

  read_size = n_channels * n * sizeof( float);

  switch (this->state) {
  case readsf_closed:
  case readsf_opened:
  case readsf_paused:
    clear_outputs( n, n_channels, outputs);
    break;

  case readsf_pending:
    if ( dtdfifo_get_read_level( this->fifo) < read_size )
      {
	clear_outputs( n, n_channels, outputs);
      }
    else
      {
	read_fifo( n, n_channels, this->fifo, outputs);

	this->state = readsf_playing;
      }
    break;

  case readsf_playing:
    if ( dtdfifo_get_read_level( this->fifo) >= read_size )
      {
	read_fifo( n, n_channels, this->fifo, outputs);
      }
    else
      {
	if ( !dtdfifo_is_used( this->fifo, DTD_SIDE))
	  {
	    /* end of file */
	    fts_alarm_set_delay( &this->eof_alarm, 0.01f);
	    fts_alarm_arm( &this->eof_alarm);

	    dtdfifo_set_used( this->fifo, FTS_SIDE, 0);

	    this->id = -1;
	    this->fifo = 0;
	    this->state = readsf_closed;
	  }
	else if ( this->can_post_data_late)
	  {
	    post( "Warning: readsf~ data late\n");

	    this->can_post_data_late = 0;

	    fts_alarm_set_delay( &this->post_data_late_alarm, 200.0f);
	    fts_alarm_arm( &this->post_data_late_alarm);
	  }

	clear_outputs( n, n_channels, outputs);
      }
  }
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
  readsf_state_machine( (readsf_t *)o, s_open, ac, at);
}

static void readsf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_state_machine( (readsf_t *)o, s_close, ac, at);
}

static void readsf_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_state_machine( (readsf_t *)o, s_play, ac, at);
}

static void readsf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_state_machine( (readsf_t *)o, s_pause, ac, at);
}

static void readsf_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int n;

  n = fts_get_int_arg( ac, at, 0, 0);

  if ( n == 1)
    readsf_state_machine( (readsf_t *)o, s_play, ac, at);
  else if ( n == 0)
    readsf_state_machine( (readsf_t *)o, s_close, ac, at);
}

static fts_status_t readsf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n_channels, i;
  fts_type_t a[4];

  n_channels = fts_get_long_arg(ac, at, 1, 1);

  if (n_channels < 1)
    n_channels = 1;

  fts_class_init(cl, sizeof(readsf_t), 1, n_channels + 1, 0);

  a[0] = fts_t_symbol;
  a[1] = fts_t_int;
  a[2] = fts_t_int;
  a[3] = fts_t_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, readsf_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, readsf_delete, 0, 0);

  a[0] = fts_t_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, readsf_put, 1, a);

  a[0] = fts_t_symbol;
  a[1] = fts_t_int;
  a[2] = fts_t_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("open"),  readsf_open, 3, a, 0);

  fts_method_define( cl, 0, fts_new_symbol("play"), readsf_play, 0, 0);
  fts_method_define( cl, 0, fts_new_symbol("start"), readsf_play, 0, 0);

  fts_method_define( cl, 0, fts_new_symbol("pause"), readsf_pause, 0, 0);

  fts_method_define( cl, 0, fts_new_symbol("close"), readsf_close, 0, 0);
  fts_method_define( cl, 0, fts_new_symbol("stop"), readsf_close, 0, 0);

  a[0] = fts_t_int;
  fts_method_define( cl, 0, fts_s_int, readsf_number, 1, a);

  for (i = 0; i < n_channels; i++)
    dsp_sig_outlet(cl, i);

  readsf_dsp_function = fts_new_symbol( "readsf~");
  dsp_declare_function( readsf_dsp_function, readsf_dsp);

  if ( !dtdserver_started)
    {
      dtdserver_start();
      dtdserver_started = 1;
    }

  return fts_Success;
}



/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* writesf~                                                               */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

#if 0

typedef enum { 
  writesf_closed, 
  writesf_opened, 
  writesf_recording, 
  writesf_paused 
} writesf_state_t;

typedef struct {
  fts_object_t _o;
  int n_channels;
  writesf_state_t state;
  int id;
  dtdfifo_t *fifo;
  int can_post_fifo_overflow;
  fts_alarm_t post_fifo_overflow_alarm;
} writesf_t;

static fts_symbol_t writesf_dsp_function;


static void writesf_open_realize( writesf_t *this, const char *filename)
{
  this->id = dtdserver_open( filename, fts_symbol_name(fts_get_search_path()), this->n_channels);

  if ( this->id < 0)
    {
      post( "writesf~: error: cannot allocate fifo for DTD server\n");
      return;
    }

  this->fifo = dtdfifo_get( this->id);

  this->state = writesf_opened;
}

static void writesf_close_realize( writesf_t *this)
{
  if (this->id >= 0)
    dtdserver_close( this->id);

  this->id = -1;
  this->fifo = 0;
  this->state = writesf_closed;
}

static void writesf_state_machine( writesf_t *this, fts_symbol_t message, int ac, const fts_atom_t *at)
{
  switch( this->state) {
  case writesf_closed:
    if (message == s_open)
      writesf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
    else if (message == s_close)
      writesf_close_realize( this);
    else if (message == s_record)
      post( "writesf~: error: no file opened\n");
    else if (message == s_pause)
      post( "writesf~: error: not recording\n");
    break;

  case writesf_opened:
    if (message == s_open)
      {
	writesf_close_realize( this);
	writesf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
      }
    else if (message == s_close)
      writesf_close_realize( this);
    else if (message == s_record)
      this->state = writesf_recording;
    else if (message == s_pause)
      post( "writesf~: error: not recording\n");
    break;

  case writesf_recording:
    if (message == s_open)
      {
	writesf_close_realize( this);
	writesf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
      }
    else if (message == s_close)
      writesf_close_realize( this);
    else if (message == s_record)
      {
      }
    else if (message == s_pause)
      {
	this->state = writesf_paused;
      }
    break;

  case writesf_paused:
    if (message == s_open)
      {
	writesf_close_realize( this);
	writesf_open_realize( this, fts_symbol_name( fts_get_symbol( at)));
      }
    else if (message == s_close)
      writesf_close_realize( this);
    else if (message == s_record)
      this->state = writesf_recording;
    else if (message == s_pause)
      {
      }
    break;

  }
}

static void writesf_post_fifo_overflow_alarm( fts_alarm_t *alarm, void *p)
{
  writesf_t *this = (writesf_t *)p;

  fts_alarm_unarm( alarm);

  this->can_post_fifo_overflow = 1;
}

static void writesf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  int n_channels;

  number_of_dtdobjs++;

  n_channels = fts_get_long_arg(ac, at, 1, 1);
  this->n_channels = (n_channels < 1) ? 1 : n_channels;

  this->state = writesf_closed;
  this->can_post_fifo_overflow = 1;

  fts_alarm_init( &(this->post_fifo_overflow_alarm), 0, writesf_post_fifo_overflow_alarm, this);	

  dsp_list_insert(o);
}

static void writesf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  fts_alarm_unarm( &this->post_fifo_overflow_alarm);	

  dsp_list_remove(o);

  number_of_dtdobjs--;
}

static void write_fifo( int n, int n_channels, dtdfifo_t *fifo, fts_word_t *inputs)
{
  volatile float *dst;
  int channel;

  dst = (volatile float *)dtdfifo_get_write_pointer( fifo);

  for ( channel = 0; channel < n_channels; channel++)
    {
      float *in;
      int i, j;

      in = (float *)fts_word_get_ptr( inputs + channel);
      j = channel;

      for ( i = 0; i < n; i++)
	{
	  dst[j] = in[i];
	  j += n_channels;
	}
    }

  dtdfifo_incr_write_index( fifo, n_channels * n * sizeof(float));
}

static void writesf_dsp( fts_word_t *argv)
{
  writesf_t *this;
  int n, n_channels, write_size;
  fts_word_t *inputs;

  this = (writesf_t *)fts_word_get_ptr( argv + 0);
  n = fts_word_get_long( argv + 1);
  inputs = argv+2;

  n_channels = this->n_channels;

  write_size = n_channels * n * sizeof( float);

  switch (this->state) {
  case writesf_closed:
  case writesf_opened:
  case writesf_paused:
    break;

  case writesf_recording:
    if ( dtdfifo_get_write_level( this->fifo) >= write_size )
      {
	write_fifo( n, n_channels, this->fifo, inputs);
      }
    else
      {
	if ( this->can_post_fifo_overflow)
	  {
	    post( "Warning: writesf~ fifo overflow\n");

	    this->can_post_fifo_overflow = 0;

	    fts_alarm_set_delay( &this->post_fifo_overflow_alarm, 200.0f);
	    fts_alarm_arm( &this->post_fifo_overflow_alarm);
	  }
      }
  }
}

static void writesf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[32];
  int i;

  fts_set_ptr( argv + 0, this);
  fts_set_int( argv + 1, fts_dsp_get_input_size( dsp, 0));

  for ( i = 0; i < this->n_channels; i++)
    fts_set_symbol( argv + 2 + i, fts_dsp_get_input_name( dsp, i));

  dsp_add_funcall( writesf_dsp_function, 2 + this->n_channels, argv);
}

static void writesf_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_state_machine( (writesf_t *)o, s_open, ac, at);
}

static void writesf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_state_machine( (writesf_t *)o, s_close, ac, at);
}

static void writesf_record(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_state_machine( (writesf_t *)o, s_record, ac, at);
}

static void writesf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_state_machine( (writesf_t *)o, s_pause, ac, at);
}

static void writesf_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int n;

  n = fts_get_int_arg( ac, at, 0, 0);

  if ( n == 1)
    writesf_state_machine( (writesf_t *)o, s_record, ac, at);
  else if ( n == 0)
    writesf_state_machine( (writesf_t *)o, s_close, ac, at);
}

static fts_status_t writesf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n_channels, i;
  fts_type_t a[4];

  n_channels = fts_get_long_arg(ac, at, 1, 1);

  if (n_channels < 1)
    n_channels = 1;

  fts_class_init(cl, sizeof(writesf_t), 1, n_channels + 1, 0);

  a[0] = fts_t_symbol;
  a[1] = fts_t_int;
  a[2] = fts_t_int;
  a[3] = fts_t_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, writesf_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, writesf_delete, 0, 0);

  a[0] = fts_t_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, writesf_put, 1, a);

  a[0] = fts_t_symbol;
  a[1] = fts_t_int;
  a[2] = fts_t_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("open"),  writesf_open, 3, a, 0);

  fts_method_define( cl, 0, fts_new_symbol("record"), writesf_record, 0, 0);
  fts_method_define( cl, 0, fts_new_symbol("start"), writesf_record, 0, 0);

  fts_method_define( cl, 0, fts_new_symbol("pause"), writesf_pause, 0, 0);

  fts_method_define( cl, 0, fts_new_symbol("close"), writesf_close, 0, 0);
  fts_method_define( cl, 0, fts_new_symbol("stop"), writesf_close, 0, 0);

  a[0] = fts_t_int;
  fts_method_define( cl, 0, fts_s_int, writesf_number, 1, a);

  for (i = 0; i < n_channels; i++)
    dsp_sig_inlet(cl, i);

  writesf_dsp_function = fts_new_symbol( "writesf~");
  dsp_declare_function( writesf_dsp_function, writesf_dsp);

  if ( !dtdserver_started)
    {
      dtdserver_start();
      dtdserver_started = 1;
    }

  return fts_Success;
}

#endif
/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* init                                                                   */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

void dtdobjs_init( void)
{
  fts_metaclass_install(fts_new_symbol("readsf~"), readsf_instantiate, fts_first_arg_equiv);
#if 0
  fts_metaclass_install(fts_new_symbol("writesf~"), writesf_instantiate, fts_first_arg_equiv);
#endif

  s_open = fts_new_symbol( "open");
  s_close = fts_new_symbol( "close");
  s_play = fts_new_symbol( "play");
  s_record = fts_new_symbol( "record");
  s_pause = fts_new_symbol( "pause");
}

