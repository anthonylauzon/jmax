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

#include <fts/fts.h>
#include <ftsconfig.h>

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

#include <ftsprivate/connection.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/audio.h>

#define AUDIOPORT_DEFAULT_IDLE ((void (*)(fts_audioport_t *port))-1)

static fts_audioport_t *audioport_list = 0;

static fts_symbol_t s_default_audio_port;
static fts_symbol_t s__superclass;
static fts_symbol_t s_audioport;

static fts_symbol_t s_audioport_guard;

static fts_class_t *indispatcher_class;
static fts_class_t *outdispatcher_class;
static fts_class_t *audioportin_class;
static fts_class_t *audioportout_class;

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
  {
    fts_object_unpatch( port->input.dsp_object);
    fts_object_destroy( port->input.dsp_object);
  }

  if (port->input.dispatcher)
    fts_object_destroy( port->input.dispatcher);

  if (port->output.dsp_object)
  {
    fts_object_unpatch( port->output.dsp_object);
    fts_object_destroy( port->output.dsp_object);
  }

  if (port->output.dispatcher)
    fts_object_destroy( port->output.dispatcher);
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

  fts_word_set_pointer( at+0, port);
  fts_word_set_int( at+1, len);
  for ( i = 0; i < channels; i++)
    fts_word_set_pointer( at+2+i, sig);

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

  if ( !at_least_one_io_fun_called)
    fts_sleep();
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
  fts_dsp_object_t o;
  int at_least_one_io_fun_called;
} audioport_guard_t;

static audioport_guard_t *audioport_guard;

static void
audio_guard_dsp_function(fts_word_t *argv)
{
  fts_sleep();
}

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
  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void audioport_guard_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void audioport_guard_put_prologue( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  audioport_guard_arm();
}

static void audioport_guard_put_epilogue( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (audioport_guard_is_armed())
    fts_dsp_add_function(s_audioport_guard, 0, 0);
}

static void audioport_guard_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( audioport_guard_t), audioport_guard_init, audioport_guard_delete);

  fts_class_message_varargs(cl, fts_s_put_prologue, audioport_guard_put_prologue);
  fts_class_message_varargs(cl, fts_s_put_epilogue, audioport_guard_put_epilogue);

  fts_dsp_declare_function( s_audioport_guard, audio_guard_dsp_function);
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

typedef struct 
{
  fts_dsp_object_t o;
  fts_audioport_t *port;
} fts_audioportin_t;

static void audioportin_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioportin_t *this = (fts_audioportin_t *)o;

  if ( ac > 0 && fts_is_object( at))
  {
    fts_audioport_t *port = (fts_audioport_t *)fts_get_object(at);
    int outlets = fts_audioport_get_input_channels(port);
      
    this->port = port;
    fts_object_set_outlets_number(o, outlets);

    fts_dsp_object_init((fts_dsp_object_t *)o);
  }
  else
    fts_object_set_error(o, "audioport required");
}

static void audioportin_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
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
  dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  fts_set_pointer( args+0, this->port);
  fts_set_int( args+1, fts_dsp_get_output_size(dsp, 0));
  for ( i = 0; i < channels; i++)
    fts_set_symbol( args+2+i, fts_dsp_get_output_name( dsp, i));

  fts_dsp_add_function( fts_audioport_get_input_function_name( this->port), channels+2, args);

  audioport_guard_unarm();
}

static void audioportin_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( fts_audioportin_t), audioportin_init, audioportin_delete);

  fts_class_message_varargs(cl, fts_s_put, audioportin_put);

  fts_dsp_declare_outlet( cl, 0);
}

typedef struct {
  fts_object_t head;
  fts_channel_t *channel_objects;
} indispatcher_t;

static void indispatcher_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  indispatcher_t *this  = (indispatcher_t *)o;
  int inlets, i;

  if (ac > 0 && fts_is_int( at))
    inlets = fts_get_int( at);
  else
    inlets = 2;

  fts_object_set_inlets_number( o, inlets);

  this->channel_objects = (fts_channel_t *)fts_malloc( inlets * sizeof( fts_channel_t));

  for ( i = 0; i < inlets; i++)
    fts_channel_init( &this->channel_objects[i]);
}

static void indispatcher_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  indispatcher_t *this  = (indispatcher_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  int inlet = fts_get_int( at+2);

  fts_channel_propagate_input( &this->channel_objects[inlet], propagate_fun, propagate_context, inlet);
}

static void indispatcher_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( indispatcher_t), indispatcher_init, 0);

  fts_class_message_varargs(cl, fts_s_propagate_input, indispatcher_propagate_input);

  fts_dsp_declare_inlet( cl, 0);
}

static void fts_audioport_create_in_objects( fts_audioport_t *port)
{
  fts_atom_t a;
  int channels, i;

  channels = fts_audioport_get_input_channels( port);

  fts_set_object( &a, (fts_object_t *)port);
  port->input.dsp_object = fts_object_create( audioportin_class, NULL, 1, &a);

  if ( !port->input.dsp_object)
  {
    fprintf( stderr, "[FTS] audioport internal error (cannot create input dsp object)\n");
    return;
  }

  fts_set_int( &a, channels);
  port->input.dispatcher = fts_object_create( indispatcher_class, NULL, 1, &a);

  if ( !port->input.dispatcher)
  {
    fprintf( stderr, "[FTS] audioport internal error (cannot create input dispatcher)\n");
    return;
  }

  for ( i = 0; i < channels; i++)
    fts_connection_new(port->input.dsp_object, i, port->input.dispatcher, i, fts_c_anything);
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

typedef struct 
{
  fts_dsp_object_t o;
  fts_audioport_t *port;
} fts_audioportout_t;

static void audioportout_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioportout_t *this = (fts_audioportout_t *)o;

  if (ac > 0 && fts_is_object( at))
  {
    fts_audioport_t *port = (fts_audioport_t *)fts_get_object(at);
    int inlets = fts_audioport_get_output_channels(port);

    this->port = port;
    fts_object_set_inlets_number(o, inlets);

    fts_dsp_object_init((fts_dsp_object_t *)o);
  }
}

static void audioportout_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
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
  dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  fts_set_pointer( args+0, this->port);
  fts_set_int( args+1, fts_dsp_get_input_size(dsp, 0));
  for ( i = 0; i < channels; i++)
    fts_set_symbol( args+2+i, fts_dsp_get_input_name( dsp, i));

  fts_dsp_add_function( fts_audioport_get_output_function_name( this->port), channels+2, args);

  audioport_guard_unarm();
}

static void audioportout_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( fts_audioportout_t), audioportout_init, audioportout_delete);

  fts_class_message_varargs(cl, fts_s_put, audioportout_put);

  fts_dsp_declare_inlet(cl, 0);
}

static void outdispatcher_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int outlets;

  if (ac > 0 && fts_is_int( at))
    outlets = fts_get_int( at);
  else
    outlets = 2;

  fts_object_set_outlets_number( o, outlets);
}

static void outdispatcher_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( fts_object_t), outdispatcher_init, 0);

  fts_class_outlet_anything(cl, 0);
}

static void fts_audioport_create_out_objects( fts_audioport_t *port)
{
  fts_atom_t a;
  int channels, i;

  channels = fts_audioport_get_output_channels( port);

  fts_set_int( &a, channels);
  port->output.dispatcher = fts_object_create( outdispatcher_class, NULL, 1, &a);

  if ( !port->output.dispatcher)
  {
    fprintf( stderr, "[FTS] audioport internal error (cannot create output dispatcher)\n");
    return;
  }

  fts_set_object( &a, (fts_object_t *)port);
  port->output.dsp_object = fts_object_create( audioportout_class, NULL, 1, &a);

  if ( !port->output.dsp_object)
  {
    fprintf( stderr, "[FTS] audioport internal error (cannot create output dsp object)\n");
    return;
  }

  for ( i = 0; i < channels; i++)
    fts_connection_new(port->output.dispatcher, i, port->output.dsp_object, i, fts_c_anything);
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

static fts_audioport_t *default_audioport = 0;
static fts_symbol_t default_audioport_class = 0;

fts_symbol_t fts_audioport_get_default_class( void)
{
  return default_audioport_class;
}

void fts_audioport_set_default_class( fts_symbol_t name)
{
  fts_object_t *obj;
  fts_atom_t a[1];

  default_audioport_class = name;

  fts_log("[audioport]: No default audioport was installed, instanciating the default class %s\n", default_audioport_class);

  fts_set_symbol(a, default_audioport_class);
  obj = fts_eval_object_description( fts_get_root_patcher(), 1, a);

  if (fts_object_is_error(obj) || !fts_object_is_audioport(obj) )
  {
    fts_patcher_remove_object(fts_get_root_patcher(), obj);
    return;
  }

  if (default_audioport)
    fts_patcher_remove_object(fts_get_root_patcher(), (fts_object_t *)default_audioport);

  default_audioport = (fts_audioport_t *)obj;
}

fts_audioport_t *fts_audioport_get_default( fts_object_t *obj)
{
  return default_audioport;  
}

/****************************************************
 *
 *  AUDIO configuration class
 *
 */
fts_class_t* audioconfig_type = NULL;
static fts_symbol_t audioconfig_s_name;

/* DUMMY CONSTRUCTOR */
static void
audioconfig_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  audioconfig_t* self = (audioconfig_t*)o;
}

/* DUMMY DESTRUCTOR */
static void
audioconfig_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  audioconfig_t* self = (audioconfig_t*)o;
}

/* DUMMY CLASS INSTANTIATION */
static void
audioconfig_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(audioconfig_t), audioconfig_init, audioconfig_delete);
}
/***********************************************************************
 *
 * Initialization
 *
 */

void fts_audio_config( void)
{
  fts_class_t *audioport_guard_class;

  s_default_audio_port = fts_new_symbol( "DefaultAudioPort");
  s__superclass = fts_new_symbol( "_superclass");
  s_audioport = fts_new_symbol( "audioport");

  s_audioport_guard = fts_new_symbol( "audioport_guard");

  indispatcher_class = fts_class_install( NULL, indispatcher_instantiate);
  outdispatcher_class = fts_class_install( NULL, outdispatcher_instantiate);
  audioportin_class = fts_class_install( NULL, audioportin_instantiate);
  audioportout_class = fts_class_install( NULL, audioportout_instantiate);

  audioport_guard_class = fts_class_install( NULL, audioport_guard_instantiate);
  audioport_guard = (audioport_guard_t *)fts_object_create( audioport_guard_class, NULL, 0, 0);

  /* AUDIO configuration class */
  audioconfig_s_name = fts_new_symbol("__audioconfig");
  audioconfig_type = fts_class_install(audioconfig_s_name, audioconfig_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
