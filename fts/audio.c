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
#include <string.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <ftsprivate/audio.h>
#include <ftsprivate/dsp.h>
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

#define fts_audioport_is_direction( port, direction) ((port)->inout[(direction)].valid)
#define fts_audioport_is_open( port, direction) ((port)->inout[(direction)].open)

#define fts_audioport_is_input( port) fts_audioport_is_direction( (port), FTS_AUDIO_INPUT)
#define fts_audioport_is_output( port) fts_audioport_is_direction( (port), FTS_AUDIO_OUTPUT)


void
fts_audioport_init( fts_audioport_t *port)
{
  fts_audioport_t *current;
  int i;

  for ( current = audioport_list; current; current = current->next)
    if (current == port)
      return;

  port->next = audioport_list;
  audioport_list = port;

  fts_audioport_unset_valid(port, FTS_AUDIO_INPUT);
  fts_audioport_unset_valid(port, FTS_AUDIO_OUTPUT);

  fts_audioport_unset_open(port, FTS_AUDIO_INPUT);  
  fts_audioport_unset_open(port, FTS_AUDIO_OUTPUT);
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

  /* FIXME */
  /* unset all the input/output ports for all the labels that refers to this port */
}

void fts_audioport_set_channels( fts_audioport_t *port, int direction, int channels)
{
  int i;

  port->inout[direction].channels = channels;

  if (channels == 0)
  {
    port->inout[direction].channel_used = 0;
    port->inout[direction].buffers = 0;
    return;
  }
  
  port->inout[direction].channel_used = (int *)fts_malloc( channels * sizeof( int));
  port->inout[direction].buffers = (float **)fts_malloc( channels * sizeof( float *));

  for ( i = 0; i < channels; i++)
    {
      int j;
      float *buff;

      buff = (float *)fts_malloc( sizeof(float) * fts_dsp_get_tick_size());

      for ( j = 0; j < fts_dsp_get_tick_size(); j++)
	buff[j] = 0.0;

      port->inout[direction].buffers[i] = buff;
      port->inout[direction].channel_used[i] = 0;
    }
}

static void
fts_audioport_set_channel_used( fts_audioport_t *port, int direction, int channel)
{
  if (port->inout[direction].channel_used)
    port->inout[direction].channel_used[channel]++;
}

static void
fts_audioport_set_channel_unused( fts_audioport_t *port, int direction, int channel)
{
  if (port->inout[direction].channel_used)
    port->inout[direction].channel_used[channel]--;
}

int
fts_audioport_is_channel_used( fts_audioport_t *port, int direction, int channel)
{
  return port->inout[direction].channel_used && port->inout[direction].channel_used[channel] > 0;
}

static void
fts_audioport_add_label( fts_audioport_t *port, int direction, fts_audiolabel_t *label)
{
  fts_symbol_t selector = (direction == FTS_AUDIO_INPUT) ? fts_s_open_input : fts_s_open_output;

  /* Call "open" method when adding first label */
  if ( !port->inout[direction].used && !fts_audioport_is_open( port, direction))
    {
      fts_atom_t a[1];

      fts_set_symbol( a, fts_audiolabel_get_name( label));
      fts_send_message( (fts_object_t *)port, selector, 1, a);
    }

  port->inout[direction].used++;

  fts_audioport_set_channel_used( port, direction, fts_audiolabel_get_channel( label, direction));
}

static void
fts_audioport_remove_label( fts_audioport_t *port, int direction, fts_audiolabel_t *label)
{
  fts_symbol_t selector = (direction == FTS_AUDIO_INPUT) ? fts_s_close_input : fts_s_close_output;

  port->inout[direction].used--;

  /* Call "close" method when removing last label */
  if ( !port->inout[direction].used)
    fts_send_message( (fts_object_t *)port, selector, 0, 0);

  fts_audioport_set_channel_unused( port, direction, fts_audiolabel_get_channel( label, direction));
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

static fts_hashtable_t audiolabel_table;

static fts_symbol_t s_input_channel;
static fts_symbol_t s_output_channel;

fts_class_t *fts_audiolabel_class;

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

fts_audiolabel_t *
fts_audiolabel_get( fts_symbol_t name)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  if (fts_hashtable_get( &audiolabel_table, &k, &v))
    return (fts_audiolabel_t *)fts_get_object( &v);

  return NULL;
}

static void
audiolabel_rename(fts_symbol_t new_name, fts_audiolabel_t* label)
{
  fts_atom_t k, v;
  fts_iterator_t keys, values;
  fts_audiolabel_t* cur_label;
  
  fts_hashtable_get_keys(&audiolabel_table, &keys);
  fts_hashtable_get_values(&audiolabel_table, &values);
  
  while (fts_iterator_has_more(&keys))
  {
    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &v);
    cur_label = (fts_audiolabel_t*)fts_get_object(&v);
    if (cur_label == label)
    {
      fts_hashtable_remove(&audiolabel_table, &k);
      fts_set_symbol(&k, new_name);
      fts_hashtable_put(&audiolabel_table, &k, &v);
      break;
    }
  }
}

static void
audiolabel_put( fts_symbol_t name, fts_audiolabel_t *label)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_set_object (&v, (fts_object_t *)label);
  fts_hashtable_put( &audiolabel_table, &k, &v);
}

static void
audiolabel_remove( fts_symbol_t name)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_hashtable_remove( &audiolabel_table, &k);
}

static void
audiolabel_set_port( fts_audiolabel_t *label, int direction, fts_symbol_t port_name)
{
  /* Make a query on audiomanager to retrieve corresponding fts_audioport_t* */
  fts_audioport_t *port = fts_audiomanager_get_port( port_name);

  if ( label->inout[direction].port != NULL)
    {
      fts_audioport_remove_label( label->inout[direction].port, direction, label);
      fts_object_release( label->inout[direction].port);
    }

  label->inout[direction].port_name = port_name;
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
    fts_audioport_set_channel_unused( label->inout[direction].port, direction, label->inout[direction].channel);

  label->inout[direction].channel = channel;

  if ( label->inout[direction].port != NULL && channel >= 0)
    fts_audioport_set_channel_used( label->inout[direction].port, direction, channel);
}

static void
audiolabel_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  fts_symbol_t port_name = fts_get_symbol(at);

  audiolabel_set_port( self, FTS_AUDIO_INPUT, port_name);
  /* @@@@@@ set channel to 0 @@@@@@ */
  /* Java do it in graphical interface */
  audiolabel_set_channel(self, FTS_AUDIO_INPUT, 0);

  fts_client_send_message(o, fts_s_input, 1, at);  

  fts_config_set_dirty((fts_config_t *)fts_config_get(), 1);
}

static void
audiolabel_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  fts_symbol_t port_name = fts_get_symbol(at);

  audiolabel_set_port( self, FTS_AUDIO_OUTPUT, port_name);
  /* @@@@@@ set channel to 0 @@@@@@ */
  /* Java do it in graphical interface */
  audiolabel_set_channel(self, FTS_AUDIO_OUTPUT, 0);

  fts_client_send_message(o, fts_s_output, 1, at);  

  fts_config_set_dirty((fts_config_t *)fts_config_get(), 1);
}

static void
audiolabel_input_channel(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  int channel = fts_get_int(at);

  audiolabel_set_channel( self, FTS_AUDIO_INPUT, channel);

  fts_client_send_message( o, s_input_channel, 1, at);  

  fts_config_set_dirty((fts_config_t *)fts_config_get(), 1);
}


static void
audiolabel_output_channel(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;
  int channel = fts_get_int(at);

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
  
  audiolabel_rename(self->name, self);

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

  audiolabel_put( self->name, self);

  self->inout[FTS_AUDIO_INPUT].port_name = fts_s_unconnected;
  self->inout[FTS_AUDIO_INPUT].port = NULL;
  self->inout[FTS_AUDIO_INPUT].channel = 0;

  self->inout[FTS_AUDIO_OUTPUT].port_name = fts_s_unconnected;
  self->inout[FTS_AUDIO_OUTPUT].port = NULL;
  self->inout[FTS_AUDIO_OUTPUT].channel = 0;

  audiolabel_fire_added( self->name);
}

static void
audiolabel_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audiolabel_t *self = (fts_audiolabel_t *)o;

  audiolabel_set_port( self, FTS_AUDIO_INPUT, NULL);
  audiolabel_set_port( self, FTS_AUDIO_OUTPUT, NULL);

  audiolabel_remove( self->name);

  audiolabel_fire_removed( self->name);
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

void
fts_audiolabel_input( fts_audiolabel_t *label, float *buff, int buffsize)
{
  fts_audioport_t *port = fts_audiolabel_get_port( label, FTS_AUDIO_INPUT);
  int channel = fts_audiolabel_get_channel( label, FTS_AUDIO_INPUT);
  int i;

  if (port)
    {
      float *input_buffer = port->inout[FTS_AUDIO_INPUT].buffers[channel];

      /* copy the buffer */
      for ( i = 0; i < buffsize; i += 4)
	{
	  buff[i] = input_buffer[i];
	  buff[i+1] = input_buffer[i+1];
	  buff[i+2] = input_buffer[i+2];
	  buff[i+3] = input_buffer[i+3];
	}
    }
  else
    {
      /* zero the buffer */
      for ( i = 0; i < buffsize; i += 4)
	{
	  buff[i] = 0.0;
	  buff[i+1] = 0.0;
	  buff[i+2] = 0.0;
	  buff[i+3] = 0.0;
	}
    }
}

void
fts_audiolabel_output( fts_audiolabel_t *label, float *buff, int buffsize)
{
  fts_audioport_t *port = fts_audiolabel_get_port( label, FTS_AUDIO_OUTPUT);
  int channel = fts_audiolabel_get_channel( label, FTS_AUDIO_OUTPUT);
  float *output_buff;
  int i;

  if (!port)
    return;

  output_buff = port->inout[FTS_AUDIO_OUTPUT].buffers[channel];
  for ( i = 0; i < buffsize; i++)
    output_buff[i] += buff[i];
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

static int sort_symbol(const void* a, const void* b)
{
  fts_symbol_t* pa = (fts_symbol_t*)a;
  fts_symbol_t* pb = (fts_symbol_t*)b;

  return strcmp(*pa, *pb);
}

static fts_symbol_t *audiomanager_get_names( int direction)
{
  static int count = -1;
  static fts_symbol_t *names;
  fts_symbol_t *p;
  int n;
  fts_iterator_t keys, values;
  int n_direction = 0;

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
      {
	*p++ = fts_get_symbol( &k);
	++n_direction;
      }
    }

  *p = NULL;
  
  qsort((void*)names, n_direction, sizeof(fts_symbol_t), sort_symbol);

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

static void 
audio_sched_run( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioport_t *port;
  int channel, tick_size, at_least_one_io_fun_called;

  tick_size = fts_dsp_get_tick_size();

  at_least_one_io_fun_called = 0;

  /* Call the audio port input functions */
  for ( port = audioport_list; port; port = port->next)
    {
      if ( !fts_audioport_is_input( port) || !fts_audioport_is_open( port, FTS_AUDIO_INPUT))
	continue;

      (*fts_audioport_get_io_fun( port, FTS_AUDIO_INPUT))( port, port->inout[FTS_AUDIO_INPUT].buffers, tick_size);
      at_least_one_io_fun_called = 1;
    }

  /* Zero the output buffers */
  for ( port = audioport_list; port; port = port->next)
    {
      if ( !fts_audioport_is_output( port) || !fts_audioport_is_open( port, FTS_AUDIO_OUTPUT))
	continue;

      for ( channel = 0; channel < fts_audioport_get_channels( port, FTS_AUDIO_OUTPUT); channel++)
	{
	  float *output_buff;
	  int i;

	  if (!fts_audioport_is_channel_used( port, FTS_AUDIO_OUTPUT, channel))
	    continue;

	  output_buff = port->inout[FTS_AUDIO_OUTPUT].buffers[channel];

	  for ( i = 0; i < tick_size; i++)
	    output_buff[i] = 0.0;
	}
    }

  /* Call the DSP */
  fts_dsp_run_tick();

  /* Call the audio port output functions */
  for ( port = audioport_list; port; port = port->next)
    {
      if ( !fts_audioport_is_output( port) || !fts_audioport_is_open( port, FTS_AUDIO_OUTPUT))
	continue;

      (*fts_audioport_get_io_fun( port, FTS_AUDIO_OUTPUT))( port, port->inout[FTS_AUDIO_OUTPUT].buffers, tick_size);

      at_least_one_io_fun_called = 1;
    }

  if ( !at_least_one_io_fun_called)
    fts_sleep();
}

static void
audio_sched_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fts_sched_add( o, FTS_SCHED_ALWAYS);
}

static void
audio_sched_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fts_sched_remove( o);
}

static void
audio_sched_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof(fts_object_t), audio_sched_init, audio_sched_delete);

  fts_class_message_varargs( cl, fts_s_sched_ready, audio_sched_run);
}


/***********************************************************************
 *
 * null audio port object
 *
 */

static void nullaudioport_open(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
}

static void nullaudioport_close(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
}

static void nullaudioport_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioport_t* self = (fts_audioport_t*)o;
  
  fts_audioport_init(self);
  fts_audioport_set_valid(self, FTS_AUDIO_INPUT);
  fts_audioport_set_valid(self, FTS_AUDIO_OUTPUT);

  fts_audioport_set_channels(self, FTS_AUDIO_INPUT, 2);
  fts_audioport_set_channels(self, FTS_AUDIO_OUTPUT, 2);
  
}

static void nullaudioport_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioport_t* self = (fts_audioport_t*)o;
  
  fts_audioport_delete(self);
}

static void nullaudioport_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(fts_audioport_t), nullaudioport_init, nullaudioport_delete);
  fts_class_message_varargs(cl, fts_s_open_input, nullaudioport_open);
  fts_class_message_varargs(cl, fts_s_open_output, nullaudioport_open);
  
  fts_class_message_varargs(cl, fts_s_close_input, nullaudioport_close);
  fts_class_message_varargs(cl, fts_s_close_output, nullaudioport_close);
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_audio_config( void)
{
  fts_class_t *audio_sched_class;
  fts_class_t* nullaudioport_class;
  fts_object_t *audio_sched;
  fts_object_t* nullaudioport;

  audiolabel_listeners_heap = fts_heap_new( sizeof( audiolabel_listener_t));

  s_input_channel = fts_new_symbol("input_channel");  
  s_output_channel = fts_new_symbol("output_channel");

  fts_audiolabel_class = fts_class_install( fts_new_symbol("__audiolabel"), audiolabel_instantiate);

  fts_hashtable_init( &audiolabel_table, FTS_HASHTABLE_SMALL);

  fts_hashtable_init( &audiomanager_table, FTS_HASHTABLE_SMALL);

  audio_sched_class = fts_class_install( NULL, audio_sched_instantiate);
  audio_sched = fts_object_create( audio_sched_class, 0, 0);

  nullaudioport_class = fts_class_install(NULL, nullaudioport_instantiate);
  nullaudioport = fts_object_create(nullaudioport_class, 0, NULL);
  fts_object_refer(nullaudioport);
  fts_audiomanager_put_port(fts_s_unconnected, (fts_audioport_t*)(nullaudioport));
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
