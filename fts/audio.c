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
 */

#include "ftsconfig.h"

#include <stdlib.h>
#include <stdio.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_TIME_H
#include <time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

#include <fts/fts.h>
#include <ftsprivate/connection.h>


#define AUDIOPORT_DEFAULT_IDLE ((void (*)( struct _fts_audioport_t *port))-1)

static fts_audioport_t *audioport_list;

static fts_symbol_t s_default_audio_port;
static fts_symbol_t s__superclass;
static fts_symbol_t s_audioport;
static fts_symbol_t s_audioportin;
static fts_symbol_t s_audioportout;
static fts_symbol_t s_indispatcher;
static fts_symbol_t s_outdispatcher;

static fts_audioport_t *null_audioport;

/*		   

 * The structure
       	
  -----------------
  | audioportin   |  one audioportin object per audioport
  -*-*-*-*-*-*-*-*- outlets
   | | | | | | | |	   					  
   | | | | | | | |	   					  
  -*-*-*-*-*-*-*-*- inlets                     ---------------    
  | indispatcher  | === array of channels ===> | in~         |  
  -----------------                            -*-*-*-*-*-*-*- outlets    
                                                | | | | | | |  
                                                DSP objects


                                                DSP objects
                                                | | | | | | |  
  -----------------                            -*-*-*-*-*-*-*- inlets            	       	     
  | outdispatcher |<======== pointer to ====== | out~        |
  -*-*-*-*-*-*-*-*- outlets                    ---------------    
   | | | | | | | | 	   					  
   | | | | | | | | 	   					  
  -*-*-*-*-*-*-*-*- inlets
  | audioportout  |  one audioportout object per audioport
  -----------------

 */


/* ********************************************************************** */
/*                                                                        */
/* audio port class checking                                              */
/*                                                                        */
/* ********************************************************************** */


int fts_object_is_audioport( fts_object_t *obj)
{
  fts_atom_t a[1];

  fts_object_get_prop(obj, s__superclass, a);

  return fts_is_symbol(a) && fts_get_symbol(a) == s_audioport;
}

static void fts_audioport_class_init( fts_class_t *cl)
{
  fts_atom_t a[1];

  fts_set_symbol( a, s_audioport);

  fts_class_put_prop( cl, s__superclass, a); /* set _superclass property to "audioport" */
}

/* ********************************************************************** */
/*                                                                        */
/* init and delete                                                        */
/*                                                                        */
/* ********************************************************************** */

void fts_audioport_init( fts_audioport_t *port)
{
  fts_audioport_t *current;

  if ( !fts_object_is_audioport( (fts_object_t *)port))
    fts_audioport_class_init( fts_object_get_class( (fts_object_t *)port) );

  port->idle_function = AUDIOPORT_DEFAULT_IDLE;

  for ( current = audioport_list; current; current = current->next)
    if (current == port)
      return;

  port->next = audioport_list;
  audioport_list = port;
}

void fts_audioport_delete( fts_audioport_t *port)
{
  fts_audioport_t *current, *previous = 0;

  for ( current = audioport_list; current; current = current->next)
    {
      if ( current == port)
	{
	  if (previous)
	    previous->next = current->next;
	  else
	    audioport_list = current->next;
	  break;
	}

      previous = current;
    }

  if (port->input.dsp_object)
    fts_object_delete_from_patcher( port->input.dsp_object);

  if (port->input.dispatcher)
    fts_object_delete_from_patcher( port->input.dispatcher);

  if (port->output.dsp_object)
    fts_object_delete_from_patcher( port->output.dsp_object);

  if (port->output.dispatcher)
    fts_object_delete_from_patcher( port->output.dispatcher);
}


void __fts_audioport_set_input_function( fts_audioport_t *port, fts_symbol_t name, ftl_wrapper_t fun)
{
  port->input.io_function_name = name;
  port->input.io_function = fun;
  fts_dsp_declare_function( name, fun);
}

void __fts_audioport_set_output_function( fts_audioport_t *port, fts_symbol_t name, ftl_wrapper_t fun)
{
  port->output.io_function_name = name;
  port->output.io_function = fun;
  fts_dsp_declare_function( name, fun);
}


/* ********************************************************************** */
/*                                                                        */
/* audio_idle                                                             */
/*                                                                        */
/* called when dsp is off                                                 */
/*                                                                        */
/* for each audioport that has either declared an idle function or that   */
/* has default idle function {                                            */
/*   call the input function with dummy vectors                           */
/*   call output functions with zeros                                     */
/* }                                                                      */
/*                                                                        */
/* ********************************************************************** */

static void audioport_call_io_fun( fts_audioport_t *port, ftl_wrapper_t fun, int len, int channels, float *sig)
{
  fts_word_t *at;
  int i;

  at = (fts_word_t *)alloca( (channels+2) * sizeof( fts_word_t));

  fts_word_set_ptr( at+0, port);
  fts_word_set_int( at+1, len);
  for ( i = 0; i < channels; i++)
    fts_word_set_ptr( at+2+i, sig);

  (*fun)( at);
}

void fts_audio_idle( void)
{
  fts_audioport_t *port;
  float *sig_dummy, *sig_zero;
  int i, tick_size, at_least_one_io_fun_called;

  tick_size = fts_dsp_get_tick_size();
  sig_dummy = (float *)alloca( tick_size * sizeof( float));
  sig_zero = (float *)alloca( tick_size * sizeof( float));

  for ( i = 0; i < tick_size; i++)
    sig_zero[i] = 0.0;

  at_least_one_io_fun_called = 0;
  for ( port = audioport_list; port; port = port->next)
    {
      if ( port->idle_function == AUDIOPORT_DEFAULT_IDLE)
	{
	  ftl_wrapper_t fun;
	  int channels;

	  fun = fts_audioport_get_input_function( port);
	  channels = fts_audioport_get_input_channels( port);

	  if (fun && channels != 0)
	    {
	      audioport_call_io_fun( port, fun, tick_size, channels, sig_dummy);
	      at_least_one_io_fun_called = 1;
	    }

	  fun = fts_audioport_get_output_function( port);
	  channels = fts_audioport_get_output_channels( port);
	  if (fun && channels != 0)
	    {
	      audioport_call_io_fun( port, fun, tick_size, channels, sig_zero);
	      at_least_one_io_fun_called = 1;
	    }
	}
      else if ( port->idle_function)
	{
	  (*port->idle_function)( port);
	  at_least_one_io_fun_called = 1;
	}
    }

  if ( !at_least_one_io_fun_called && null_audioport)
    {
      ftl_wrapper_t fun;
      fts_word_t at[1];

      fun = fts_audioport_get_output_function( null_audioport);
      fts_word_set_ptr( at+0, null_audioport);

      (*fun)( at);
    }
}

/* ********************************************************************** */
/*                                                                        */
/* audioport_guard object                                                 */
/* an object used to guard FTS from running without at least one          */
/* synchronization function called, and thus locking the machine when     */
/* running with real-time scheduling                                      */
/*                                                                        */
/* ********************************************************************** */

typedef struct _audioport_guard_t {
  fts_object_t head;
  int at_least_one_io_fun_called;
} audioport_guard_t;

static audioport_guard_t *audioport_guard;

static void audioport_guard_arm( void)
{
  audioport_guard->at_least_one_io_fun_called = 0;
}

static void audioport_guard_unarm( void)
{
  audioport_guard->at_least_one_io_fun_called = 1;
}

static int audioport_guard_is_armed( void)
{
  return !audioport_guard->at_least_one_io_fun_called;
}

static void audioport_guard_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_add_object(o);
}

static void audioport_guard_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}

static void audioport_guard_put_prologue( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  audioport_guard_arm();
}

static void audioport_guard_put_epilogue( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ( audioport_guard_is_armed() && null_audioport)
    {
      fts_atom_t args[1];

      /* schedule the null audioport */
      fts_set_ptr( args+0, null_audioport);
      fts_dsp_add_function( fts_audioport_get_output_function_name( null_audioport), 1, args);
    }
}

static fts_status_t audioport_guard_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( audioport_guard_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, audioport_guard_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, audioport_guard_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put_prologue, audioport_guard_put_prologue);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put_epilogue, audioport_guard_put_epilogue);

  return fts_Success;
}



/* ********************************************************************** */
/*                                                                        */
/* audioportin and indispatcher objects                                   */
/* audioportin is a DSP object that schedules in the DSP chain the input  */
/* function of the audioport. Its outlets are connected to the            */
/* indispatcher object                                                    */
/* The indispatcher object is a thru object that maintains a list of      */
/* channels. The propagation operation propagates to the in~ objects      */
/*                                                                        */
/* ********************************************************************** */

typedef struct {
  fts_object_t head;
  fts_audioport_t *port;
} fts_audioportin_t;


static void audioportin_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioportin_t *this = (fts_audioportin_t *)o;

  this->port = (fts_audioport_t *)fts_get_object( at+1);

  fts_dsp_add_object(o);
}

static void audioportin_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}

static void audioportin_put( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioportin_t *this = (fts_audioportin_t *)o;
  fts_atom_t *args;
  fts_dsp_descr_t *dsp;
  int channels, i;

  if ( !this->port)
    return;

  channels = fts_audioport_get_input_channels( this->port);
  args = (fts_atom_t *)alloca( (channels+2) * sizeof( fts_atom_t));
  dsp = (fts_dsp_descr_t *)fts_get_ptr(at);

  fts_set_ptr( args+0, this->port);
  fts_set_int( args+1, fts_dsp_get_output_size(dsp, 0));
  for ( i = 0; i < channels; i++)
    fts_set_symbol( args+2+i, fts_dsp_get_output_name( dsp, i));

  fts_dsp_add_function( fts_audioport_get_input_function_name( this->port), channels+2, args);

  audioport_guard_unarm();
}

static fts_status_t audioportin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int outlets, i;

  ac--;
  at++;

  if ( ac == 1 && fts_is_object( at))
    outlets = fts_audioport_get_input_channels( (fts_audioport_t *)fts_get_object( at) );
  else
    return &fts_CannotInstantiate;
    
  fts_class_init( cl, sizeof( fts_audioportin_t), 0, outlets, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, audioportin_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, audioportin_delete, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, audioportin_put);

  for ( i = 0; i < outlets; i++)
    fts_dsp_declare_outlet( cl, i);

  return fts_Success;
}

typedef struct {
  fts_object_t head;
  fts_channel_t *channel_objects;
} indispatcher_t;

static void indispatcher_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  indispatcher_t *this  = (indispatcher_t *)o;
  int inlets, i;

  inlets = fts_object_get_inlets_number( o);

  this->channel_objects = (fts_channel_t *)fts_malloc( inlets * sizeof( fts_channel_t));

  for ( i = 0; i < inlets; i++)
    fts_channel_init( &this->channel_objects[i]);
}

static void indispatcher_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  indispatcher_t *this  = (indispatcher_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_fun(at + 0);
  void *propagate_context = fts_get_ptr(at + 1);
  int inlet = fts_get_int( at+2);

  fts_channel_propagate_input( &this->channel_objects[inlet], propagate_fun, propagate_context, inlet);
}

static fts_status_t indispatcher_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int inlets, i;

  ac--;
  at++;

  if ( ac == 1 && fts_is_int( at))
    inlets = fts_get_int( at);
  else
    return &fts_CannotInstantiate;

  fts_class_init( cl, sizeof( indispatcher_t), inlets, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, indispatcher_init);

  for ( i = 0; i < inlets; i++)
    fts_dsp_declare_inlet( cl, i);

  fts_class_define_thru( cl, indispatcher_propagate_input);

  return fts_Success;
}

static void fts_audioport_create_in_objects( fts_audioport_t *port)
{
  fts_atom_t a[2];
  int channels, i;

  channels = fts_audioport_get_input_channels( port);

  fts_set_symbol( a+0, s_audioportin);
  fts_set_object( a+1, (fts_object_t *)port);
  fts_object_new_to_patcher( fts_get_root_patcher(), 2, a, &port->input.dsp_object);
  if ( !port->input.dsp_object)
    {
      fprintf( stderr, "[FTS] audioport internal error (cannot create input dsp object)\n");
      return;
    }

  fts_set_symbol( a+0, s_indispatcher);
  fts_set_int( a+1, channels);
  fts_object_new_to_patcher( fts_get_root_patcher(), 2, a, &port->input.dispatcher);
  if ( !port->input.dispatcher)
    {
      fprintf( stderr, "[FTS] audioport internal error (cannot create input dispatcher)\n");
      return;
    }

  for ( i = 0; i < channels; i++)
    fts_connection_new( FTS_NO_ID, port->input.dsp_object, i, port->input.dispatcher, i);
}


void fts_audioport_set_input_channels( fts_audioport_t *port, int channels)
{
  port->input.channels = channels;

  if ( !port->input.dsp_object && channels != 0)
    fts_audioport_create_in_objects( port);
}

/* ********************************************************************** */
/*                                                                        */
/* audioportout object                                                    */
/* This object is a DSP object and schedules in the DSP chain the output  */
/* function of the audioport. Its inlets are connected to the             */
/* outdispatcher object                                                   */
/*                                                                        */
/* ********************************************************************** */

typedef struct {
  fts_object_t head;
  fts_audioport_t *port;
} fts_audioportout_t;


static void audioportout_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioportout_t *this = (fts_audioportout_t *)o;

  this->port = (fts_audioport_t *)fts_get_object( at+1);

  fts_dsp_add_object(o);
}

static void audioportout_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}

static void audioportout_put( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioportout_t *this = (fts_audioportout_t *)o;
  fts_atom_t *args;
  fts_dsp_descr_t *dsp;
  int channels, i;

  if ( !this->port)
    return;

  channels = fts_audioport_get_output_channels( this->port);
  args = (fts_atom_t *)alloca( (channels+2) * sizeof( fts_atom_t));
  dsp = (fts_dsp_descr_t *)fts_get_ptr(at);

  fts_set_ptr( args+0, this->port);
  fts_set_int( args+1, fts_dsp_get_input_size(dsp, 0));
  for ( i = 0; i < channels; i++)
    fts_set_symbol( args+2+i, fts_dsp_get_input_name( dsp, i));

  fts_dsp_add_function( fts_audioport_get_output_function_name( this->port), channels+2, args);

  audioport_guard_unarm();
}

static fts_status_t audioportout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int inlets, i;

  ac--;
  at++;

  if ( ac == 1 && fts_is_object( at))
    inlets = fts_audioport_get_output_channels( (fts_audioport_t *)fts_get_object( at) );
  else
    return &fts_CannotInstantiate;
    
  fts_class_init( cl, sizeof( fts_audioportout_t), inlets, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, audioportout_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, audioportout_delete, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, audioportout_put);

  for ( i = 0; i < inlets; i++)
    fts_dsp_declare_inlet( cl, i);

  return fts_Success;
}

static fts_status_t outdispatcher_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int outlets;

  ac--;
  at++;

  if ( ac == 1 && fts_is_int( at))
    outlets = fts_get_int( at);
  else
    return &fts_CannotInstantiate;

  fts_class_init( cl, sizeof( fts_object_t), 0, outlets, 0);

  return fts_Success;
}

static void fts_audioport_create_out_objects( fts_audioport_t *port)
{
  fts_atom_t a[2];
  int channels, i;

  channels = fts_audioport_get_output_channels( port);

  fts_set_symbol( a+0, s_outdispatcher);
  fts_set_int( a+1, channels);
  fts_object_new_to_patcher( fts_get_root_patcher(), 2, a, &port->output.dispatcher);
  if ( !port->output.dispatcher)
    {
      fprintf( stderr, "[FTS] audioport internal error (cannot create output dispatcher)\n");
      return;
    }

  fts_set_symbol( a+0, s_audioportout);
  fts_set_object( a+1, (fts_object_t *)port);
  fts_object_new_to_patcher( fts_get_root_patcher(), 2, a, &port->output.dsp_object);
  if ( !port->output.dsp_object)
    {
      fprintf( stderr, "[FTS] audioport internal error (cannot create output dsp object)\n");
      return;
    }

  for ( i = 0; i < channels; i++)
    fts_connection_new( FTS_NO_ID, port->output.dispatcher, i, port->output.dsp_object, i);
}

void fts_audioport_set_output_channels( fts_audioport_t *port, int channels)
{
  port->output.channels = channels;

  if ( !port->output.dsp_object && channels != 0)
    fts_audioport_create_out_objects( port);
}

/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */

void fts_audioport_add_input_object( fts_audioport_t *port, int channel, fts_object_t *object)
{
  indispatcher_t *indispatcher = (indispatcher_t *)port->input.dispatcher;

  if ( indispatcher)
    fts_channel_add_target( &indispatcher->channel_objects[channel], object);
}

void fts_audioport_remove_input_object( fts_audioport_t *port, int channel, fts_object_t *object)
{
  indispatcher_t *indispatcher = (indispatcher_t *)port->input.dispatcher;

  if ( indispatcher)
    fts_channel_remove_target( &indispatcher->channel_objects[channel], object);
}


/* ********************************************************************** */
/*                                                                        */
/* xrun ("dac slip") report                                               */
/*                                                                        */
/* ********************************************************************** */

int fts_audioport_report_xrun( void)
{
  fts_audioport_t *port;
  int xrun;

  xrun = 0;
  for ( port = audioport_list; port; port = port->next)
    {
      if (port->xrun_function)
	{
	  xrun |= (*port->xrun_function)( port);
	}
    }

  return xrun;
}



/* ********************************************************************** */
/*                                                                        */
/* default audio port handling                                            */
/*                                                                        */
/* ********************************************************************** */

fts_audioport_t *fts_audioport_get_default( fts_object_t *obj)
{
  fts_atom_t *value;
  fts_audioport_t *default_audioport = 0;

  value = fts_variable_get_value( fts_object_get_patcher( obj), s_default_audio_port);

  if (value && fts_is_object(value))
    {
      default_audioport = (fts_audioport_t *)fts_get_object( value);
    }
  
  if (obj)
    fts_variable_add_user( fts_object_get_patcher(obj), s_default_audio_port, obj);

  return default_audioport;
}

/***********************************************************************
 *
 * Null audio port: used to prevent from 100% CPU locking
 *
 */

typedef struct {
  fts_audioport_t head;
  fts_timer_t timer;
#ifdef WIN32
  HANDLE event;
  UINT mmtimer;
#endif
} nullaudioport_t;

static void nullaudioport_output( fts_word_t *argv)
{
  nullaudioport_t *port = (nullaudioport_t *)fts_word_get_ptr( argv+0);

#ifdef WIN32
  if ( fts_timer_get_time( &port->timer) >= (double)10.0)
    {
      DWORD result;
      result = WaitForSingleObject(port->event, 100);
      if (result != WAIT_OBJECT_0) {
	/* FIXME: then what? */
      }
    }
#else
  if ( fts_timer_get_time( &port->timer) >= (double)100.0)
    {
      struct timespec pause_time;

      pause_time.tv_sec = 0;
      pause_time.tv_nsec = 100000000L;

      nanosleep( &pause_time, 0);

      fts_timer_reset( &port->timer);
    }
#endif
}

static void nullaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nullaudioport_t *this = (nullaudioport_t *)o;

  fts_audioport_init( &this->head);

  fts_audioport_set_idle_function( (fts_audioport_t *)this, 0);
  fts_audioport_set_output_function( (fts_audioport_t *)this, nullaudioport_output);
  /*
   * The nullaudioport does not defines channels, in order to avoid creating
   * its DSP objects which would be scheduled...
   */

  fts_timer_init( &this->timer, 0);
  fts_timer_start( &this->timer);

#ifdef WIN32
  this->event = CreateEvent(NULL, FALSE, TRUE, "FtsNullAudioDevice");
  
  this->mmtimer = timeSetEvent(10, 0, (LPTIMECALLBACK) this->event, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
#endif
}

static void nullaudioport_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nullaudioport_t *this = (nullaudioport_t *)o;

  fts_audioport_delete( (fts_audioport_t *) this);

#ifdef WIN32
  if (this->mmtimer) {
    timeKillEvent(this->mmtimer);
  }
  if (this->event) {
    CloseHandle(this->event);
  }
#endif
}

static void nullaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static fts_status_t nullaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( nullaudioport_t), 0, 0, 0);
  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, nullaudioport_init);
  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_delete, nullaudioport_delete);

  fts_class_add_daemon( cl, obj_property_get, fts_s_state, nullaudioport_get_state);

  return fts_Success;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_audio_config( void)
{
  fts_atom_t argv[1];

  s_default_audio_port = fts_new_symbol( "DefaultAudioPort");
  s__superclass = fts_new_symbol( "_superclass");
  s_audioport = fts_new_symbol( "audioport");
  s_audioportin = fts_new_symbol( "audioportin");
  s_audioportout = fts_new_symbol( "audioportout");
  s_indispatcher = fts_new_symbol( "indispatcher");
  s_outdispatcher = fts_new_symbol( "outdispatcher");

  fts_class_install( s_indispatcher, indispatcher_instantiate);
  fts_class_install( s_outdispatcher, outdispatcher_instantiate);
  fts_metaclass_install( s_audioportin, audioportin_instantiate, fts_never_equiv);
  fts_metaclass_install( s_audioportout, audioportout_instantiate, fts_never_equiv);
  fts_class_install( fts_new_symbol( "audioport_guard"), audioport_guard_instantiate);
  fts_class_install( fts_new_symbol("nullaudioport"), nullaudioport_instantiate);

  fts_set_symbol( &argv[0], fts_new_symbol("nullaudioport"));
  fts_object_new_to_patcher( fts_get_root_patcher(), 1, argv, (fts_object_t **)&null_audioport);
  if (!null_audioport)
    fprintf( stderr, "[FTS] audioport internal error (cannot create null audio port)\n");

  fts_set_symbol( &argv[0], fts_new_symbol("audioport_guard"));
  fts_object_new_to_patcher( fts_get_root_patcher(), 1, argv, (fts_object_t **)&audioport_guard);
}
