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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Riccardo Borghesi.
 *
 */

/*
TODO:
remove function fts_audioport_add_input_output_objects( void). See NOTE below.

NOTE:
the fts_audioport_add_input_output_objects( void) is called by the dsp compiler
to make sure that all audioport will schedule their corresponding audioport_in
and audioport_out objects. This is to be sure that once an audioport is opened
and has a valid input or ouput, it will schedule the input or output object
and therefore synchronize fts, avoiding free run and 100% CPU eating.

But the result was that the null_audioport was scheduled... So I added the test
      if (port == null_audioport)
	continue;
in fts_audioport_add_input_output_objects. But I don't like this solution.

The problem is that you can't create the input or output objects in 
fts_audioport_init because you don't know yet the number of input
channels or output channels.

May be we could create the objects in fts_audioport_set_input_channels
or fts_audioport_set_output_function (resp. output). In these functions,
we are sure that the port is valid in the corresponding direction.

*/




#include <stdlib.h>
#ifndef MACOSX
#include <alloca.h>
#endif
#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime.h>
#include <fts/runtime/sched/sched.h>
#include <fts/runtime/audio/audioport.h>

static fts_class_t *audioportout_class;

static fts_audioport_t *audioport_list = 0;

static fts_symbol_t s_default_audio_port;

static fts_symbol_t s__superclass;
static fts_symbol_t s_audioport;
static fts_symbol_t s_audioportin;
static fts_symbol_t s_audioportout;
static fts_symbol_t s_indispatch;
static fts_symbol_t s_outdispatch;

static fts_audioport_t *null_audioport;

/*		   

 * The structure (see also in_tilda.c and out_tilda.c)
       	
  ---------------
  | audioportin |  one audioportin object per audioport
  -*-*-*-*-*-*-*- outlets
   | | | | | | | 	   					  
   | | | | | | | 	   					  
  -*-*-*-*-*-*-*- inlets                     ---------------    
  | indispatch  |========= pointer to =====> | in~         |  (one indispatch object per in~ object)
  ---------------                            -*-*-*-*-*-*-*- outlets    
                                              | | | | | | |  
                                              DSP objects


                                              DSP objects
                                              | | | | | | |  
  ---------------                            -*-*-*-*-*-*-*- inlets            	       	     
  | outdispatch |<======== pointer to ====== | out~        |  (one outdispatch object per out~ object)
  -*-*-*-*-*-*-*- outlets                    ---------------    
   | | | | | | | 	   					  
  ///////////////    	
   | | | | | | | 	   					  
  -*-*-*-*-*-*-*- inlets
  | audioportout|  one audioportout object per audioport
  ---------------

 */

#define AUDIOPORT_DEFAULT_IDLE ((void (*)( struct _fts_audioport_t *port))-1)

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

  if (port->audioportin)
    fts_object_delete_from_patcher( port->audioportin);

  if (port->audioportout)
    fts_object_delete_from_patcher( port->audioportout);
}

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

void fts_audioport_idle( fts_word_t *args)
{
  fts_audioport_t *port;
  float *sig_dummy, *sig_zero;
  int i, tick_size, at_least_one_io_fun_called;

  tick_size = fts_get_tick_size();
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
    {
      ftl_wrapper_t fun;
      fts_word_t at[1];

      fun = fts_audioport_get_output_function( null_audioport);
      fts_word_set_ptr( at+0, null_audioport);

      (*fun)( at);
    }
}

void __fts_audioport_set_input_function( fts_audioport_t *port, fts_symbol_t name, ftl_wrapper_t fun)
{
  port->input_fun_name = name;
  port->input_fun = fun;
  fts_dsp_declare_function( name, fun);
}

void __fts_audioport_set_output_function( fts_audioport_t *port, fts_symbol_t name, ftl_wrapper_t fun)
{
  port->output_fun_name = name;
  port->output_fun = fun;
  fts_dsp_declare_function( name, fun);
}



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
  if ( audioport_guard_is_armed() )
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



typedef struct {
  fts_object_t head;
  fts_audioport_t *port;
} fts_audioportin_t;


static void audioportin_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioportin_t *this = (fts_audioportin_t *)o;

  ac--;
  at++;

  if ( ac == 1 && fts_is_ptr( at))
    this->port = (fts_audioport_t *)fts_get_ptr( at);

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
  fts_audioport_t *port;

  ac--;
  at++;

  if (ac == 0)
    outlets = 2;
  else if ( ac == 1 && fts_is_ptr( at))
    {
      port = (fts_audioport_t *)fts_get_ptr( at);

      outlets = fts_audioport_get_input_channels( port);
    }
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
  fts_audioport_t *port;
} fts_audioportout_t;


static void audioportout_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioportout_t *this = (fts_audioportout_t *)o;

  ac--;
  at++;
  
  if ( ac == 1 && fts_is_ptr( at))
    this->port = (fts_audioport_t *)fts_get_ptr( at);

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
  fts_audioport_t *port;

  ac--;
  at++;

  if (ac == 0)
    inlets = 2;
  else if ( ac == 1 && fts_is_ptr( at))
    {
      port = (fts_audioport_t *)fts_get_ptr( at);

      inlets = fts_audioport_get_output_channels( port);
    }
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


typedef struct {
  fts_object_t head;
  fts_object_t *owner;
  int outlet;
} indispatch_t;

static void indispatch_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  indispatch_t *this  = (indispatch_t *)o;

  ac--;
  at++;

  this->owner = fts_get_object( at);
  this->outlet = fts_get_int( at+1);
}

static void indispatch_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  indispatch_t *this  = (indispatch_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_fun(at + 0);
  void *propagate_context = fts_get_ptr(at + 1);

  (*propagate_fun)( propagate_context, this->owner, this->outlet);
}

static fts_status_t indispatch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( indispatch_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, indispatch_init);

  fts_dsp_declare_inlet( cl, 0);

  fts_class_define_thru( cl, indispatch_propagate_input);

  return fts_Success;
}

static void fts_audioport_create_audioportin( fts_audioport_t *port)
{
  fts_atom_t a[2];

  if ( !port->audioportin && port->input_channels)
    {
      fts_set_symbol( a+0, s_audioportin);
      fts_set_ptr( a+1, port);
      fts_object_new_to_patcher( fts_get_root_patcher(), 2, a, &port->audioportin);
    }
}

fts_object_t *fts_audioport_get_in_object( fts_audioport_t *port, fts_object_t *owner, int outlet)
{
  fts_object_t *in;
  fts_atom_t a[3];

  fts_audioport_create_audioportin( port);

  fts_set_symbol( a+0, s_indispatch);
  fts_set_object( a+1, owner);
  fts_set_int( a+2, outlet);
  fts_object_new_to_patcher( fts_get_root_patcher(), 3, a, &in);

  fts_connection_new( FTS_NO_ID, port->audioportin, outlet, in, 0);

  return in;
}

void fts_audioport_remove_in_object( fts_object_t *in_object)
{
  fts_connection_t *p;

  while ((p = in_object->in_conn[0]))
    fts_connection_delete(p);
}


static fts_status_t outdispatch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( fts_object_t), 0, 1, 0);

  fts_dsp_declare_outlet( cl, 0);

  return fts_Success;
}

static void fts_audioport_create_audioportout( fts_audioport_t *port)
{
  fts_atom_t a[2];

  if ( !port->audioportout && port->output_channels)
    {
      fts_set_symbol( a+0, s_audioportout);
      fts_set_ptr( a+1, port);
      fts_object_new_to_patcher( fts_get_root_patcher(), 2, a, &port->audioportout);
    }
}

fts_object_t *fts_audioport_get_out_object( fts_audioport_t *port, int inlet)
{
  fts_object_t *out;
  fts_atom_t a[2];

  fts_audioport_create_audioportout( port);

  fts_set_symbol( a+0, s_outdispatch);
  fts_object_new_to_patcher( fts_get_root_patcher(), 1, a, &out);

  fts_connection_new( FTS_NO_ID, out, 0, port->audioportout, inlet);

  return out;
}

void fts_audioport_remove_out_object( fts_object_t *out_object)
{
  fts_connection_t *p;

  while ((p = out_object->out_conn[0]))
    fts_connection_delete(p);
}

void fts_audioport_add_input_output_objects( void)
{
  fts_audioport_t *port;

  for ( port = audioport_list; port; port = port->next)
    {
      if (port == null_audioport)
	continue;

      fts_audioport_create_audioportin( port);
      fts_audioport_create_audioportout( port);
    }
}




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


static void create_null_audioport( void)
{
  fts_object_t *obj;
  fts_atom_t argv[1];

  fts_set_symbol( &argv[0], fts_new_symbol("nullaudioport"));

  fts_object_new_to_patcher( fts_get_root_patcher(), 1, argv, &obj);
  null_audioport = (fts_audioport_t *)obj;
}

static void create_audioport_guard( void)
{
  fts_atom_t argv[1];

  fts_set_symbol( &argv[0], fts_new_symbol("audioport_guard"));

  fts_object_new_to_patcher( fts_get_root_patcher(), 1, argv, (fts_object_t **)&audioport_guard);
}

void audioport_config( void)
{
  s_default_audio_port = fts_new_symbol( "DefaultAudioPort");

  s__superclass = fts_new_symbol( "_superclass");
  s_audioport = fts_new_symbol( "audioport");
  s_audioportin = fts_new_symbol( "audioportin");
  s_audioportout = fts_new_symbol( "audioportout");
  s_indispatch = fts_new_symbol( "indispatch");
  s_outdispatch = fts_new_symbol( "outdispatch");

  fts_class_install( s_indispatch, indispatch_instantiate);
  fts_class_install( s_outdispatch, outdispatch_instantiate);

  fts_metaclass_install( s_audioportin, audioportin_instantiate, fts_never_equiv);
  fts_metaclass_install( s_audioportout, audioportout_instantiate, fts_never_equiv);

  fts_class_install( fts_new_symbol( "audioport_guard"), audioport_guard_instantiate);

  create_null_audioport();
  create_audioport_guard();
}

