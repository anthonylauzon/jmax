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

#include <ftsprivate/audio.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/audioconfig.h>
#include <ftsprivate/midi.h>
#include <ftsprivate/config.h>

/* **********************************************************************
 * 
 * Audio ports
 *
 */
#define AUDIOPORT_DEFAULT_IDLE ((void (*)(fts_audioport_t *port))-1)

static fts_audioport_t *audioport_list = 0;

void
fts_audioport_init( fts_audioport_t *port)
{
  fts_audioport_t *current;

  port->idle_function = AUDIOPORT_DEFAULT_IDLE;

  for ( current = audioport_list; current; current = current->next)
    if (current == port)
      return;

  port->next = audioport_list;
  audioport_list = port;
}

void
fts_audioport_delete( fts_audioport_t *port)
{
  fts_audioport_t **p;

  for ( p = &audioport_list; *p; p = &(*p)->next)
    {
      if ( *p == port)
	{
	  *p = (*p)->next;
	  break;
	}
    }
}

void
fts_audioport_set_channel_used( fts_audioport_t *port, int channel, int direction, int used)
{
  if (used)
    port->inout[direction].used_channels |= 1 << channel;
  else
    port->inout[direction].used_channels &= ~(1 << channel);
}

int
fts_audioport_is_channel_used( fts_audioport_t *port, int channel, int direction)
{
  return port->inout[direction].used_channels & ~(1 << channel);
}

static fts_audiolabel_t **lookup_label( fts_audioport_t *port, int direction, fts_audiolabel_t *label)
{
  fts_audiolabel_t **p = &port->inout[direction].labels;

  while ( *p && (*p) != label)
    p = &(*p)->inout[direction].next_same_port;

  return p;
}

void
fts_audioport_add_label( fts_audioport_t *port, int direction, fts_audiolabel_t *label)
{
  fts_audiolabel_t **p = lookup_label( port, direction, label);

  if (!*p)
    {
      *p = label;
      (*p)->inout[direction].next_same_port = 0;
    }
}

void
fts_audioport_remove_label( fts_audioport_t *port, int direction, fts_audiolabel_t *label)
{
  fts_audiolabel_t **p = lookup_label( port, direction, label);

  if (*p)
    *p = (*p)->inout[direction].next_same_port;
}


/* **********************************************************************
 * 
 * Audio labels
 *
 */

typedef struct audiolabel_listener {
  fts_object_t *listener;
  fts_method_t label_added;
  fts_method_t label_removed;
  struct audiolabel_listener *next;
} audiolabel_listener_t;

static audiolabel_listener_t *audiolabel_listeners;
static fts_heap_t *audiolabel_listeners_heap;

static fts_symbol_t s_input_channel;
static fts_symbol_t s_output_channel;

static audiolabel_listener_t **lookup_listener( fts_object_t *listener)
{
  audiolabel_listener_t **p = &audiolabel_listeners;

  while ( *p && (*p)->listener != listener)
    p = &(*p)->next;

  return p;
}

void
fts_audiolabel_add_listener( fts_object_t *listener, fts_method_t label_added, fts_method_t label_removed)
{
  audiolabel_listener_t **p = lookup_listener( listener);

  if (!*p)
    {
      *p = (audiolabel_listener_t *)fts_heap_alloc( audiolabel_listeners_heap);
      (*p)->listener = listener;
      (*p)->label_added = label_added;
      (*p)->label_removed = label_removed;
      (*p)->next = 0;
    }
}

void
fts_audiolabel_remove_listener( fts_object_t *listener)
{
  audiolabel_listener_t **p = lookup_listener( listener);

  if (*p)
    {
      audiolabel_listener_t *to_delete = *p;

      *p = (*p)->next;
      fts_heap_free( to_delete, audiolabel_listeners_heap);
    }
}

static void
audiolabel_fire_added( fts_symbol_t label_name)
{
  audiolabel_listener_t *p;
  fts_atom_t a[1];

  fts_set_symbol( a, label_name);
  for ( p = audiolabel_listeners; p; p = p->next)
    (*p->label_added)( p->listener, -1, NULL, 1, a);
}

static void
audiolabel_fire_removed( fts_symbol_t label_name)
{
  audiolabel_listener_t *p;
  fts_atom_t a[1];

  fts_set_symbol( a, label_name);
  for ( p = audiolabel_listeners; p; p = p->next)
    (*p->label_removed)( p->listener, -1, NULL, 1, a);
}

static void
audiolabel_set_port( fts_audiolabel_t *label, int direction, fts_audioport_t *port)
{
  if ( label->inout[direction].port != NULL)
    {
      fts_audioport_remove_label( label->inout[direction].port, direction, label);
      fts_object_release( label->inout[direction].port);
    }

  label->inout[direction].port = port;

  if (port != NULL)
    {
      fts_object_refer( port);
      fts_audioport_add_label( port, direction, label);
    }
}

static void
audiolabel_set_channel( fts_audiolabel_t *label, int direction, int channel)
{
  if ( label->inout[direction].port != NULL && label->inout[direction].channel >= 0)
    fts_audioport_set_channel_used( label->inout[direction].port, direction, label->inout[direction].channel, 0);

  label->inout[direction].channel = channel;

  if ( label->inout[direction].port != NULL && channel >= 0)
    fts_audioport_set_channel_used( label->inout[direction].port, channel, direction, 1);
}

static void
audiolabel_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  fts_symbol_t name = fts_get_symbol(at);

  post("[audiolabel:] audiolabel_input, label name: %s, output device: %s\n", self->name, name);

  /* Make a query on audiomanager to retreive corresponding fts_audioport_t* */
  audiolabel_set_port( self, FTS_AUDIO_INPUT, fts_audiomanager_get_port( name));

  fts_client_send_message(o, fts_s_input, 1, at);  

  fts_config_set_dirty((fts_config_t *)fts_config_get(), 1);
}

static void
audiolabel_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  fts_symbol_t name = fts_get_symbol(at);

  post("[audiolabel:] audiolabel_output, label name: %s, output device: %s\n", self->name, name);
  
  /* Make a query on audiomanager to retreive corresponding fts_audioport_t* */
  audiolabel_set_port( self, FTS_AUDIO_OUTPUT, fts_audiomanager_get_port( name));

  fts_client_send_message(o,  fts_s_output, 1, at);  

  fts_config_set_dirty((fts_config_t *)fts_config_get(), 1);
}

static void
audiolabel_input_channel(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  int channel = fts_get_int(at);

  post("[audiolabel] label name: %s, input channel: %d\n", self->name, channel);

  audiolabel_set_channel( self, FTS_AUDIO_INPUT, channel);

  fts_client_send_message( o, s_input_channel, 1, at);  

  fts_config_set_dirty((fts_config_t *)fts_config_get(), 1);
}


static void
audiolabel_output_channel(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  int channel = fts_get_int(at);

  post("[audiolabel] label name: %s, output channel: %d\n", self->name, channel);

  audiolabel_set_channel( self, FTS_AUDIO_OUTPUT, channel);

  fts_client_send_message( o, s_output_channel, 1, at);  

  fts_config_set_dirty((fts_config_t *)fts_config_get(), 1);
}

static void
audiolabel_change_label(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  fts_audioconfig_t *aconfig = (fts_audioconfig_t *)fts_audioconfig_get();
  fts_symbol_t new_label = fts_get_symbol(at);
  fts_atom_t a;

  if (fts_audioconfig_label_get_by_name( aconfig, new_label) != NULL)
    self->name = fts_audioconfig_get_fresh_label_name( aconfig, new_label);
  else
    self->name = new_label;
  
  fts_set_symbol(&a, self->name);
  fts_client_send_message(o, fts_s_label, 1, &a);  

  fts_config_set_dirty((fts_config_t *)fts_config_get(), 1);
}

static void
audiolabel_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  fts_symbol_t name;

  if (ac >= 1)
    self->name = fts_get_symbol( at);
  else
    self->name = fts_new_symbol( "unnamed");

  self->inout[FTS_AUDIO_INPUT].port = NULL;
  self->inout[FTS_AUDIO_INPUT].channel = -1;
  self->inout[FTS_AUDIO_OUTPUT].port = NULL;
  self->inout[FTS_AUDIO_OUTPUT].channel = -1;
}

static void
audiolabel_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;

  audiolabel_set_port( self, FTS_AUDIO_INPUT, NULL);
  audiolabel_set_port( self, FTS_AUDIO_OUTPUT, NULL);
}

static void
audiolabel_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(fts_audiolabel_t), audiolabel_init, audiolabel_delete);

  fts_class_message_varargs(cl, fts_s_input, audiolabel_input);
  fts_class_message_varargs(cl, fts_s_output, audiolabel_output);

  fts_class_message_varargs(cl, fts_s_label, audiolabel_change_label);

  fts_class_message_varargs(cl, s_input_channel, audiolabel_input_channel);
  fts_class_message_varargs(cl, s_output_channel, audiolabel_output_channel);
}


/* **********************************************************************
 * 
 * Audio manager
 *
 */

static fts_hashtable_t audiomanager_table;

fts_audioport_t *
fts_audiomanager_get_port( fts_symbol_t name)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  if (fts_hashtable_get( &audiomanager_table, &k, &v))
    return (fts_audioport_t *)fts_get_object( &v);
  
  /* This should not happen ??? */
  fts_log( "[audioport] no mapped audioport for name \"%s\"\n", name);

  return NULL;
}

void
fts_audiomanager_put_port( fts_symbol_t name, fts_audioport_t *port)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_set_object (&v, (fts_object_t *)port);
  fts_hashtable_put( &audiomanager_table, &k, &v);
}

void
fts_audiomanager_remove_port( fts_symbol_t name)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_hashtable_remove( &audiomanager_table, &k);
}

static fts_symbol_t *audiomanager_get_names( int direction)
{
  static int count;
  static fts_symbol_t *names;
  fts_symbol_t *p;
  int n;
  fts_iterator_t keys, values;

  n = fts_hashtable_get_size( &audiomanager_table);

  if ( n > count)
    {
      count = n;
      names = fts_realloc( names, (n+1) * sizeof( fts_symbol_t));
    }

  fts_hashtable_get_keys( &audiomanager_table, &keys);
  fts_hashtable_get_values( &audiomanager_table, &values);

  p = names;
  while ( fts_iterator_has_more( &keys))
    {
      fts_atom_t k, v;
  
      fts_iterator_next( &keys, &k);
      fts_iterator_next( &values, &v);

      if ( fts_audioport_is_direction( (fts_audioport_t *)(fts_get_object( &v)), direction))
	*p++ = fts_get_symbol( &k);
    }

  *p = NULL;

  return names;
}

fts_symbol_t *fts_audiomanager_get_input_names(void)
{
  return audiomanager_get_names( FTS_AUDIO_INPUT);
}

fts_symbol_t *fts_audiomanager_get_output_names(void)
{
  return audiomanager_get_names( FTS_AUDIO_OUTPUT);
}


/* **********************************************************************
 *
 * audio idle
 *
 */
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
/*       ftl_wrapper_t fun; */
/*       int channels; */

/*       fun = fts_audioport_get_input_function( port); */
/*       channels = fts_audioport_get_input_channels( port); */

/*       if (fun && channels != 0) */
/*       { */
/* 	audioport_call_io_fun( port, fun, tick_size, channels, sig_dummy); */
/* 	at_least_one_io_fun_called = 1; */
/*       } */

/*       fun = fts_audioport_get_output_function( port); */
/*       channels = fts_audioport_get_output_channels( port); */
/*       if (fun && channels != 0) */
/*       { */
/* 	audioport_call_io_fun( port, fun, tick_size, channels, sig_zero); */
/* 	at_least_one_io_fun_called = 1; */
/*       } */
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

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_audio_config( void)
{
  audiolabel_listeners_heap = fts_heap_new( sizeof( audiolabel_listener_t));

  s_input_channel = fts_new_symbol("input_channel");  
  s_output_channel = fts_new_symbol("output_channel");

  fts_class_install( fts_new_symbol("__audiolabel"), audiolabel_instantiate);

  fts_hashtable_init( &audiomanager_table, FTS_HASHTABLE_SMALL);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
