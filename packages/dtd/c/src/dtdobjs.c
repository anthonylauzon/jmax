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
 * this file's authors: francois dechelle.
 */


#include <math.h>
#include <string.h>
#include <fts/fts.h>

#include "dtddefs.h"
#include "dtdfifo.h"
#include "dtdserver.h"

static fts_symbol_t s_open;
static fts_symbol_t s_close;
static fts_symbol_t s_play;
static fts_symbol_t s_record;
static fts_symbol_t s_pause;

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
  dtdserver_t *server;
  dtdfifo_t *fifo;
  int can_post_data_late;
  fts_symbol_t filename;
  fts_timebase_t *timebase;
} readsf_t;

static fts_symbol_t readsf_symbol;

static int readsf_do_open( readsf_t *this, fts_symbol_t filename)
{
  if (filename)
    this->filename = filename;
  
  if (this->filename)
    {
      this->fifo = dtdserver_open_read( this->server, this->filename, this->n_channels);
      
      if (this->fifo)
	return 1;
      else
	post( "readsf~: error: cannot allocate fifo for dtd server\n");
    }
  else
    post( "readsf~: error: no file name specified\n");

  return 0;
}

static void readsf_do_close( readsf_t *this)
{
  if (this->fifo)
    {
      dtdserver_close( this->server, this->fifo);
    }

  this->fifo = 0;
}

static void readsf_state_machine( readsf_t *this, fts_symbol_t message, int ac, const fts_atom_t *at)
{
  switch( this->state) {
  case readsf_closed:
    if (message == s_open)
      {
	if(readsf_do_open( this, fts_get_symbol( at)))
	  this->state = readsf_opened;
      }
    else if (message == fts_s_stop)
      {
      }
    else if (message == fts_s_start)
      {
	if(readsf_do_open( this, 0))
	  this->state = readsf_pending;
      }
    else if (message == s_pause)
      {
      }
    break;

  case readsf_opened:
    if (message == s_open)
      {
	readsf_do_close( this);

	if(!readsf_do_open( this, fts_get_symbol( at)))
	  this->state = readsf_closed;	  
      }
    else if (message == fts_s_stop)
      {
	readsf_do_close( this);
	this->state = readsf_closed;
      }
    else if (message == fts_s_start)
      this->state = readsf_pending;
    else if (message == s_pause)
      {
      }
    break;

  case readsf_pending:
    if (message == s_open)
      {
	readsf_do_close( this);

	if(readsf_do_open( this, fts_get_symbol( at)))
	  this->state = readsf_opened;
	else
	  this->state = readsf_closed;	  
      }
    else if (message == fts_s_stop)
      {
	readsf_do_close( this);
	this->state = readsf_closed;
      }
    else if (message == fts_s_start) 
      {
      }
    else if (message == s_pause)
      {
      }
    break;

  case readsf_playing:
    if (message == s_open)
      {
	readsf_do_close( this);

	if(readsf_do_open( this, fts_get_symbol( at)))
	  this->state = readsf_opened;
	else
	  this->state = readsf_closed;	  
      }
    else if (message == fts_s_stop)
      {
	readsf_do_close( this);
	this->state = readsf_closed;
      }
    else if (message == fts_s_start)
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
	readsf_do_close( this);

	if(readsf_do_open( this, fts_get_symbol( at)))
	  this->state = readsf_opened;
	else
	  this->state = readsf_closed;	  
      }
    else if (message == fts_s_stop)
      {
	readsf_do_close( this);
	this->state = readsf_closed;
      }
    else if (message == fts_s_start)
      this->state = readsf_playing;
    else if (message == s_pause)
      {
      }
    break;

  }
}

static void 
readsf_can_post_data_late_alarm(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  this->can_post_data_late = 1;
}

static void 
readsf_eof_alarm(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang( o, fts_object_get_outlets_number(o) - 1);
}

static void readsf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int n_channels;
  fts_symbol_t filename;

  this->server = 0;
  this->filename = 0;

  n_channels = fts_get_int_arg(ac, at, 0, 1);
  this->n_channels = (n_channels < 1) ? 1 : n_channels;

  if(ac == 2 && fts_is_symbol( at + 1))
    this->filename = fts_get_symbol( at + 1);

  if(!this->server)
    this->server = dtdserver_get_default_instance();

  if(!this->server)
    {
      fts_object_set_error( o, "Error starting direct-to-disk server");
      return;
    }

  dtdserver_add_object( this->server, this);

  this->state = readsf_closed;
  this->can_post_data_late = 1;

  fts_dsp_add_object(o);

  this->timebase = fts_get_timebase();
  fts_object_set_outlets_number(o, n_channels);
}

static void readsf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  dtdserver_remove_object( this->server, this);

  fts_timebase_remove_object( this->timebase, (fts_object_t *)this);	

  fts_dsp_remove_object(o);
}

static void clear_outputs( int n, int n_channels, fts_word_t *outputs)
{
  int channel, i;
  float *out;

  for ( channel = 0; channel < n_channels; channel++)
    {
      out = (float *)fts_word_get_pointer( outputs + channel);

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

      out = (float *)fts_word_get_pointer( outputs + channel);
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

  this = (readsf_t *)fts_word_get_pointer( argv + 0);
  n = fts_word_get_int( argv + 1);
  outputs = argv + 2;

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
	    fts_timebase_add_call( this->timebase, (fts_object_t *)this, readsf_eof_alarm, 0, 0.0);

	    dtdfifo_set_used( this->fifo, FTS_SIDE, 0);

	    this->fifo = 0;
	    this->state = readsf_closed;
	  }
	else if ( this->can_post_data_late)
	  {
	    post( "warning: readsf~ data late\n");

	    this->can_post_data_late = 0;

	    fts_timebase_add_call( this->timebase, (fts_object_t *)this, readsf_can_post_data_late_alarm, 0, 200.0);
	  }

	clear_outputs( n, n_channels, outputs);
      }
  }
}

static void readsf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[32];
  int i;

  fts_set_pointer( argv + 0, this);
  fts_set_int( argv + 1, fts_dsp_get_output_size( dsp, 0));

  for ( i = 0; i < this->n_channels; i++)
    fts_set_symbol( argv + 2 + i, fts_dsp_get_output_name( dsp, i));

  fts_dsp_add_function( readsf_symbol, 2 + this->n_channels, argv);
}

static void readsf_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_state_machine( (readsf_t *)o, s_open, ac, at);
}

static void readsf_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_state_machine( (readsf_t *)o, fts_s_start, ac, at);
}

static void readsf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_state_machine( (readsf_t *)o, fts_s_stop, ac, at);
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
    readsf_state_machine( (readsf_t *)o, fts_s_start, ac, at);
  else if ( n == 0)
    readsf_state_machine( (readsf_t *)o, s_pause, ac, at);
}

static void readsf_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(readsf_t), readsf_init, readsf_delete);

  fts_class_method_varargs(cl, fts_s_put, readsf_put);

  fts_class_method_varargs(cl, s_open,  readsf_open);

  fts_class_method_varargs(cl, fts_s_bang, readsf_start);
  fts_class_inlet_int(cl, 0, readsf_number);

  fts_class_method_varargs(cl, s_play, readsf_start);
  fts_class_method_varargs(cl, fts_s_start, readsf_start);

  fts_class_method_varargs(cl, s_pause, readsf_pause);

  fts_class_method_varargs(cl, fts_s_stop, readsf_stop);
  fts_class_method_varargs(cl, s_close, readsf_stop);

  fts_dsp_declare_outlet(cl, 0);

  fts_dsp_declare_function( readsf_symbol, readsf_dsp);
}



/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* writesf~                                                               */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

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
  dtdserver_t *server;
  dtdfifo_t *fifo;
  int can_post_fifo_overflow;
  fts_symbol_t filename;
  fts_timebase_t *timebase;
} writesf_t;

static fts_symbol_t writesf_symbol;


static void writesf_do_open( writesf_t *this, fts_symbol_t filename)
{
  if(filename)
    this->filename = filename;
  
  if(this->filename)
    {
      this->fifo = dtdserver_open_write( this->server, this->filename, this->n_channels);
      
      if (this->fifo)
	this->state = writesf_opened;
      else
	post( "writesf~: error: cannot allocate fifo for DTD server\n");
    }
  else
    post( "writesf~: error: no file name specified\n");
}

static void writesf_do_close( writesf_t *this)
{
  if (this->fifo)
    {
      dtdserver_close( this->server, this->fifo);
    }

  this->fifo = 0;
  this->state = writesf_closed;
}

static void writesf_state_machine( writesf_t *this, fts_symbol_t message, int ac, const fts_atom_t *at)
{
  switch( this->state) {
  case writesf_closed:
    if (message == s_open)
      writesf_do_open( this, fts_get_symbol( at));
    else if (message == s_close)
      {
      }
    else if (message == s_record)
      writesf_do_open( this, 0);
    else if (message == s_pause)
      {
      }
    break;

  case writesf_opened:
    if (message == s_open)
      {
	writesf_do_close( this);
	writesf_do_open( this, fts_get_symbol( at));
      }
    else if (message == s_close)
      writesf_do_close( this);
    else if (message == s_record)
      this->state = writesf_recording;
    else if (message == s_pause)
      {
      }
    break;

  case writesf_recording:
    if (message == s_open)
      {
	writesf_do_close( this);
	writesf_do_open( this, fts_get_symbol( at));
      }
    else if (message == s_close)
      writesf_do_close( this);
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
	writesf_do_close( this);
	writesf_do_open( this, fts_get_symbol( at));
      }
    else if (message == s_close)
      writesf_do_close( this);
    else if (message == s_record)
      this->state = writesf_recording;
    else if (message == s_pause)
      {
      }
    break;

  }
}

static void 
writesf_post_fifo_overflow_alarm(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  this->can_post_fifo_overflow = 1;
}

static void 
writesf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  int n_channels;

  this->server = 0;
  this->filename = 0;

  n_channels = fts_get_int_arg(ac, at, 0, 1);
  this->n_channels = (n_channels < 1) ? 1 : n_channels;

  if(ac == 2 && fts_is_symbol( at + 1))
    this->filename = fts_get_symbol( at + 1);

  this->server = (dtdserver_t *)fts_get_pointer_arg(ac, at, 1, dtdserver_get_default_instance());

  if (!this->server)
    {
      fts_object_set_error( o, "Error starting direct-to-disk server");
      return;
    }

  dtdserver_add_object( this->server, this);

  this->state = writesf_closed;
  this->can_post_fifo_overflow = 1;

  fts_dsp_add_object(o);

  this->timebase = fts_get_timebase();
  fts_object_set_inlets_number(o, n_channels);
}

static void writesf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  dtdserver_remove_object( this->server, this);

  fts_timebase_remove_object( this->timebase, (fts_object_t *)this);	

  fts_dsp_remove_object(o);
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

      in = (float *)fts_word_get_pointer( inputs + channel);
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

  this = (writesf_t *)fts_word_get_pointer( argv + 0);
  n = fts_word_get_int( argv + 1);
  inputs = argv + 2;

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

	    fts_timebase_add_call( this->timebase, (fts_object_t *)this, writesf_post_fifo_overflow_alarm, 0, 200.0);
	  }
      }
  }
}

static void writesf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[32];
  int i;

  fts_set_pointer( argv + 0, this);
  fts_set_int( argv + 1, fts_dsp_get_input_size( dsp, 0));

  for ( i = 0; i < this->n_channels; i++)
    fts_set_symbol( argv + 2 + i, fts_dsp_get_input_name( dsp, i));

  fts_dsp_add_function( writesf_symbol, 2 + this->n_channels, argv);
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

static void 
writesf_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(writesf_t), writesf_init, writesf_delete);

  fts_class_method_varargs(cl, fts_s_put, writesf_put);

  fts_class_method_varargs(cl, s_open, writesf_open);

  fts_class_method_varargs(cl, fts_s_bang, writesf_record);
  fts_class_inlet_int(cl, 0, writesf_number);

  fts_class_method_varargs(cl, s_record, writesf_record);
  fts_class_method_varargs(cl, fts_s_start, writesf_record);

  fts_class_method_varargs(cl, s_pause, writesf_pause);

  fts_class_method_varargs(cl, fts_s_stop, writesf_close);
  fts_class_method_varargs(cl, s_close, writesf_close);

  fts_dsp_declare_inlet(cl, 0);

  fts_dsp_declare_function( writesf_symbol, writesf_dsp);
}


/* ********************************************************************** */
/* ********************************************************************** */
/*                                                                        */
/* init                                                                   */
/*                                                                        */
/* ********************************************************************** */
/* ********************************************************************** */

void dtdobjs_config( void)
{
  readsf_symbol = fts_new_symbol( "readsf~");
  writesf_symbol = fts_new_symbol( "writesf~");

  fts_class_install(readsf_symbol, readsf_instantiate);
  fts_class_install(writesf_symbol, writesf_instantiate);

  s_open = fts_new_symbol( "open");
  s_close = fts_new_symbol( "close");
  s_play = fts_new_symbol( "play");
  s_record = fts_new_symbol( "record");
  s_pause = fts_new_symbol( "pause");
}
