/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include <stdlib.h>
#include <string.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/client.h>
#include <ftsprivate/patcherobject.h>
#include <ftsprivate/object.h>
#include <ftsprivate/variable.h>
#include <ftsprivate/midi.h>
#include <ftsprivate/audioconfig.h> /* requires audiolabel.h */
#include <ftsprivate/config.h> /* requires audioconfig.h */

/*
 * This file contains the following structures related to MIDI:
 *  - MIDI fifo
 *  - MIDI port (incl. midibus and midinull class)
 *  - MIDI label
 *  - MIDI manager
 *
 * MIDI files are handled in midifile.c
 */

/***************************************************
 *
 *  midi fifo
 *
 */
void
fts_midififo_init(fts_midififo_t *fifo, int size)
{
  int bytes = sizeof(fts_midififo_entry_t) * size;
  fts_midififo_entry_t *entries;
  int i;

  fts_fifo_init(&fifo->data, fts_malloc(bytes), bytes);
  entries = (fts_midififo_entry_t *)fts_fifo_get_buffer(&fifo->data);

  for(i=0; i<size; i++) 
  {
    fts_object_t *obj = fts_object_create(fts_midievent_type, 0, 0);
    fts_object_refer(obj);
    entries[i].event = (fts_midievent_t *)obj;
  }

  fifo->delta = 0.0;
  fifo->size = size;
}

void
fts_midififo_reset(fts_midififo_t *fifo)
{
  fts_midififo_entry_t *entries = (fts_midififo_entry_t *)fts_fifo_get_buffer(&fifo->data);
  int i;

  for(i=0; i<fifo->size; i++)
    fts_object_release((fts_object_t *)entries[i].event);

  fts_free((void *)fifo->data.buffer);
}

void
fts_midififo_resync(fts_midififo_t *fifo)
{
  fifo->delta = 0.0;
}

/* read next fifo entry into time base (returns pointer to atom of newly allocated entry) */
void
fts_midififo_poll(fts_midififo_t *fifo)
{
  if(fts_fifo_read_level(&fifo->data) >= sizeof(fts_midififo_entry_t)) 
  {
    fts_midififo_entry_t *entry = (fts_midififo_entry_t *)fts_fifo_read_pointer(&fifo->data);
    double time = entry->time - fts_get_time();
    double delay;
    fts_atom_t a;
      
    /* set midievent argument */
    fts_set_object(&a, entry->event);
      
    /* time == 0.0 means: send now */
    if(time > 0.0) 
    {        
      /* adjust delta time on very first fifo entry */
      if(fifo->delta == 0.0)
	fifo->delta = time;
	  
      /* translate event time to delay */
      delay = time - fifo->delta;
          	  
      /* adjust delta time */
      if(delay < 0.0) 
      {
	delay = 0.0;
	fifo->delta = time;
      }
	  
      /* schedule midiport input call */
      fts_timebase_add_call(fts_get_timebase(), entry->port, fts_midiport_input, &a, delay);
    } 
    else
      fts_midiport_input(entry->port, 0, 0, 1, &a);
      
    fts_object_release(entry->event);
      
    /* insert a new midievent into fifo and claim it */
    entry->event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
    fts_object_refer(entry->event);
      
    fts_fifo_incr_read(&fifo->data, sizeof(fts_midififo_entry_t));
  }
}

fts_midievent_t *
fts_midififo_get_event(fts_midififo_t *fifo)
{
  if(fts_fifo_write_level(&fifo->data) >= sizeof(fts_midififo_entry_t)) 
  {
    fts_midififo_entry_t *entry = (fts_midififo_entry_t *)fts_fifo_write_pointer(&fifo->data);
    return entry->event;
  } 
  else
    return NULL;
}

void
fts_midififo_write(fts_midififo_t *fifo, fts_object_t *port, double time)
{
  fts_midififo_entry_t *entry = (fts_midififo_entry_t *)fts_fifo_write_pointer(&fifo->data);
  
  /* set midport */
  entry->port = port;
  fts_object_refer(port);

  /* set time */
  entry->time = time;

  /* send entry */
  fts_fifo_incr_write(&fifo->data, sizeof(fts_midififo_entry_t));
}

/****************************************************
 *
 *  MIDI port
 *
 */

/* MIDI port listeners */
static void 
add_listener(fts_midiport_listener_t **list, fts_object_t *object, fts_method_t fun)
{
  fts_midiport_listener_t *l = (fts_midiport_listener_t *)fts_malloc(sizeof(fts_midiport_listener_t));

  l->callback = fun;
  l->listener = object;
  l->next = *list;
  
  *list = l;
}

static void 
remove_listener(fts_midiport_listener_t **list, fts_object_t *o)
{
  fts_midiport_listener_t *l = *list;

  if(l)
  {
    fts_midiport_listener_t *freeme = 0;
      
    if(l->listener == o)
    {
      freeme = l;
      *list = l->next;
    }
    else
    {
      while(l->next)
      {
	if(l->next->listener == o)
	{
	  freeme = l->next;
	  l->next = l->next->next;
		  
	  break;
	}
	      
	l = l->next;
      }
    }
      
    /* free removed listener */
    if(freeme)
      fts_free(freeme);
  }
}

void 
fts_midiport_add_listener(fts_midiport_t *port, enum midi_type type, int chan, int num, fts_object_t *obj, fts_method_t fun)
{
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t **list = type_list + (num + 1) * (n_midi_channels + 1) + chan;

  add_listener(list, obj, fun);
}

void
fts_midiport_remove_listener(fts_midiport_t *port, enum midi_type type, int chan, int num, fts_object_t *obj)
{
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t **list = type_list + (num + 1) * (n_midi_channels + 1) + chan;

  if (NULL != list)
  {
    remove_listener(list, obj);
  }
}

void
fts_midiport_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port = (fts_midiport_t *)o;
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);
  int type = fts_midievent_get_type(event);
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t *l;
  int channel = fts_midievent_channel_message_get_channel(event);

  if(type <= midi_control_change)
  {
    /* fire number listeners (note, poly pressure, control_change only) */
    int number = fts_midievent_channel_message_get_first(event); /* note or controller number */
    int onset = (number + 1) * (n_midi_channels + 1);

    /* fire callbacks for given channel and number */
    l = type_list[onset + channel];
    while(l)
    {
      l->callback(l->listener, 0, 0, 1, at);
      l = l->next;
    }

    /* fire callbacks for any channel and given number*/
    l = type_list[onset];
    while(l)
    {
      l->callback(l->listener, 0, 0, 1, at);
      l = l->next;
    }
  }

  if(type <= midi_pitch_bend)
  {
    /* fire callbacks for given channel and any number */
    l = type_list[channel];
    while(l)
    {
      l->callback(l->listener, 0, 0, 1, at);
      l = l->next;
    }
  }
  
  /* fire callbacks for given event type */
  l = type_list[0];
  while(l)
  {
    l->callback(l->listener, 0, 0, 1, at);
    l = l->next;
  }

  /* fire callbacks for any event */
  l = *(port->listeners[0]);
  while(l)
  {
    l->callback(l->listener, 0, 0, 1, at);
    l = l->next;
  }
}

void 
fts_midiport_output(fts_midiport_t *port, fts_midievent_t *event, double time)
{
  if(port->output != NULL)
  {
    fts_object_refer((fts_object_t *)event);

    port->output((fts_object_t *)port, event, time);

    fts_object_release((fts_object_t *)event);
  }
}

void
fts_midiport_init(fts_midiport_t *port)
{
  int type;

  for(type=midi_type_any; type<n_midi_types; type++)
    port->listeners[type + 1] = NULL;

  port->output = NULL;
}

void
fts_midiport_reset(fts_midiport_t *port)
{
  /* free listeners */
  if(port->listeners[midi_type_any])
  {
    int type;

    for(type=midi_type_any; type<n_midi_types; type++)
      fts_free(port->listeners[type + 1]);
  }
}

void
fts_midiport_set_input(fts_midiport_t *port)
{
  enum midi_type type;

  for(type=midi_type_any; type<n_midi_types; type++)
  {
    switch(type)
    {
    case midi_type_any:
    {
      fts_midiport_listener_t **any_list;

      any_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *));
      *any_list = 0;

      port->listeners[type + 1] = any_list;
    }
    break;

    case midi_note:
    case midi_poly_pressure:
    case midi_control_change:
    {
      /* channel and poly listeners */
      fts_midiport_listener_t **poly_list;
      int size = (n_midi_channels + 1) * (n_midi_controllers + 1);
      int i;

      poly_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *) * size);

      for(i=0; i<size; i++)
	poly_list[i] = 0;

      port->listeners[type + 1] = poly_list;
    }
    break;

    case midi_program_change:
    case midi_channel_pressure:
    case midi_pitch_bend:
    {
      /* channel listeners */
      fts_midiport_listener_t **chan_list;
      int i;

      chan_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *) * (n_midi_channels + 1));

      for(i=0; i<n_midi_channels; i++)
	chan_list[i] = 0;

      port->listeners[type + 1] = chan_list;
    }
    break;

    default:
    {
      /* system message listeners */
      fts_midiport_listener_t **sys_list;

      sys_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *));
      *sys_list = 0;

      port->listeners[type + 1] = sys_list;
    }
    break;
    }
  }
}

void
fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_t function)
{
  port->output = function;
}

void
fts_midiport_class_init(fts_class_t *cl)
{
  fts_class_message_varargs(cl, fts_s_midievent, fts_midiport_input);
}

int
fts_object_is_midiport(fts_object_t *obj)
{
  return (fts_class_get_method_varargs(fts_object_get_class(obj), fts_s_midievent) == fts_midiport_input);
}

/****************************************************
 *
 *  internal MIDI port class
 *
 */
static fts_class_t *midibus_type = NULL;

static void
midibus_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  fts_atom_t a;

  fts_set_object(&a, event);
  fts_midiport_input(o, 0, 0, 1, &a);
}

static void
midibus_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_init(this);
  fts_midiport_set_input(this);
  fts_midiport_set_output(this, midibus_output);
}

static void
midibus_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_reset(this);
}

static void
midibus_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_midiport_t), midibus_init, midibus_delete);
}

/****************************************************
 *
 *  null MIDI port class
 *
 */
static fts_class_t *midinull_type = NULL;

static void
midinull_output(fts_object_t *o, fts_midievent_t *event, double time)
{
}

static void
midinull_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_init(this);
  fts_midiport_set_input(this);
  fts_midiport_set_output(this, midinull_output);
}

static void
midinull_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_reset(this);
}

static void
midinull_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_midiport_t), midinull_init, midinull_delete);
}

/************************************************************
 *
 *  MIDI label
 *
 */

static fts_midilabel_t *
midilabel_new(fts_symbol_t name)
{
  fts_midilabel_t *label = (fts_midilabel_t *)fts_malloc(sizeof(fts_midilabel_t));

  label->name = name;
  label->input = NULL;
  label->output = NULL;
  label->input_name = NULL;
  label->output_name = NULL;
  label->next = NULL;

  return label;
}

static void
midilabel_delete(fts_midilabel_t *label)
{
  if(label->input != NULL)
    fts_object_release(label->input);

  if(label->output != NULL)
    fts_object_release(label->output);

  fts_free(label);
}

static void
midilabel_set_input(fts_midilabel_t *label, fts_midiport_t *port, fts_symbol_t name)
{
  if(port != label->input) 
  {
    if(label->input != NULL)
      fts_object_release(label->input);
      
    label->input = port;
    label->input_name = name;
      
    if(port != NULL)
      fts_object_refer(port);
  }
}

static void
midilabel_set_output(fts_midilabel_t *label, fts_midiport_t *port, fts_symbol_t name)
{
  if(port != label->output) 
  {
    if(label->output != NULL)
      fts_object_release(label->output);
      
    label->output = port;
    label->output_name = name;
      
    if(port != NULL)
      fts_object_refer(port);
  }
}

/************************************************************
 *
 *  MIDI configuration
 *
 */

/* current MIDI configuration */
/* static midiconfig_t *midiconfig = NULL; */

/* array of device names */
static fts_objectlist_t midiconfig_listeners;

/* array of device names */
static fts_array_t midiconfig_inputs;
static fts_array_t midiconfig_outputs;

/* list of MIDI managers */
static fts_midimanager_t *midimanagers = NULL;

/* MIDI port used for invalid references */
static fts_midiport_t *midinull = NULL;

static fts_symbol_t midiconfig_s_name;
static fts_symbol_t midiconfig_s_internal_bus;
static fts_symbol_t midiconfig_s_inputs;
static fts_symbol_t midiconfig_s_outputs;

fts_symbol_t fts_midimanager_s_get_default_input;
fts_symbol_t fts_midimanager_s_get_default_output;
fts_symbol_t fts_midimanager_s_append_input_names;
fts_symbol_t fts_midimanager_s_append_output_names;
fts_symbol_t fts_midimanager_s_get_input;
fts_symbol_t fts_midimanager_s_get_output;

/* handle midimanagers */
static fts_midiport_t *
midimanagers_get_input(fts_symbol_t device_name, fts_symbol_t label_name)
{
  fts_midiport_t *port = NULL;

  if(device_name != NULL)
  {
    fts_midimanager_t *mm;
    fts_atom_t args[3];
      
    fts_set_pointer(args + 0, &port);
    fts_set_symbol(args + 1, device_name);
    fts_set_symbol(args + 2, label_name);
      
    for(mm = midimanagers; mm != NULL && port == NULL; mm = mm->next)
      fts_send_message((fts_object_t *)mm, fts_midimanager_s_get_input, 3, args);
  }

  return port;      
}

static fts_midiport_t *
midimanagers_get_output(fts_symbol_t device_name, fts_symbol_t label_name)
{
  fts_midiport_t *port = NULL;

  if(device_name != NULL)
  {
    fts_midimanager_t *mm;
    fts_atom_t args[3];
      
    fts_set_pointer(args + 0, &port);
    fts_set_symbol(args + 1, device_name);
    fts_set_symbol(args + 2, label_name);
      
    for(mm = midimanagers; mm != NULL && port == NULL; mm = mm->next)
      fts_send_message((fts_object_t *)mm, fts_midimanager_s_get_output, 3, args);
  }
      
  return port;
}

static void
midimanagers_get_device_names(void)
{
  fts_midimanager_t *mm;
  fts_atom_t arg;
  
  fts_set_pointer(&arg, &midiconfig_inputs);
  
  for(mm = midimanagers; mm != NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_midimanager_s_append_input_names, 1, &arg);

  fts_set_pointer(&arg, &midiconfig_outputs);
  
  for(mm = midimanagers; mm != NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_midimanager_s_append_output_names, 1, &arg);
}

static fts_symbol_t
midimanagers_get_default_input(void)
{
  fts_symbol_t name = NULL;
  fts_midimanager_t *mm;
  fts_atom_t arg;
  
  fts_set_pointer(&arg, &name);

  for(mm = midimanagers; mm != NULL && name == NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_midimanager_s_get_default_input, 1, &arg);

  return name;
}

static fts_symbol_t
midimanagers_get_default_output(void)
{
  fts_symbol_t name = NULL;
  fts_midimanager_t *mm;
  fts_atom_t arg;
  
  fts_set_pointer(&arg, &name);

  for(mm = midimanagers; mm != NULL && name == NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_midimanager_s_get_default_output, 1, &arg); 

  return name;
}

static fts_midilabel_t *
midiconfig_label_get_by_index(fts_midiconfig_t *config, int index)
{
  fts_midilabel_t *label =  config->labels;

  while(label && index--)
    label = label->next;

  return label;
}

static fts_midilabel_t *
midiconfig_label_get_by_name(fts_midiconfig_t *config, fts_symbol_t name)
{
  fts_midilabel_t *label =  config->labels;

  while(label && label->name != name)
    label = label->next;

  return label;
}

static fts_midilabel_t *
midiconfig_label_insert(fts_midiconfig_t *config, int index, fts_symbol_t name)
{
  fts_midilabel_t **p = &config->labels;
  fts_midilabel_t *label = midilabel_new(name);
  int n = index;

  label->input_name = fts_s_unconnected;
  label->output_name = fts_s_unconnected;

  /* inset label to list */
  while((*p) && n--)
    p = &(*p)->next;

  label->next = (*p);
  *p = label;

  config->n_labels++;

  /* send new label to client */
  if(fts_object_has_id((fts_object_t *)config)) 
  {
    fts_atom_t args[4];
      
    fts_set_int(args, index);
    fts_set_symbol(args + 1, name);
    fts_set_symbol(args + 2, fts_s_unconnected);
    fts_set_symbol(args + 3, fts_s_unconnected);
    fts_client_send_message((fts_object_t *)config, fts_s_set, 4, args);
  }
  
  fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);

  return label;
}

static void
midiconfig_label_remove(fts_midiconfig_t *config, int index)
{
  fts_midilabel_t **p = &config->labels;
  int n = index;

  /* remove label and send to client */
  while((*p) && n--)
    p = &(*p)->next;

  if(*p) 
  {
    fts_midilabel_t *label = *p;
      
    *p = (*p)->next;
    config->n_labels--;
      
    midilabel_delete(label);
  }

  /* send remove to client */
  if(fts_object_has_id((fts_object_t *)config)) 
  {
    fts_atom_t arg;
      
    fts_set_int(&arg, index);
    fts_client_send_message((fts_object_t *)config, fts_s_remove, 1, &arg);
  }

  fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);
}

static void
midiconfig_label_set_input(fts_midiconfig_t *config, fts_midilabel_t *label, int index, fts_midiport_t *midiport, fts_symbol_t name)
{
  if(midiport == NULL)
    name = fts_s_unconnected;

  if(midiport != label->input || name != label->input_name) 
  {
    midilabel_set_input(label, midiport, name);
      
    if(fts_object_has_id((fts_object_t *)config)) 
    {
      fts_atom_t args[2];
	  
      fts_set_int(args + 0, index);
      fts_set_symbol(args + 1, name);
      fts_client_send_message((fts_object_t *)config, fts_s_input, 2, args);
    }

    fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);
  }
}

static void
midiconfig_label_set_output(fts_midiconfig_t *config, fts_midilabel_t *label, int index, fts_midiport_t *midiport, fts_symbol_t name)
{
  if(midiport == NULL)
    name = fts_s_unconnected;
  
  if(midiport != label->output || name != label->output_name) 
  {
    midilabel_set_output(label, midiport, name);
      
    if(fts_object_has_id((fts_object_t *)config)) 
    {
      fts_atom_t args[2];
	  
      fts_set_int(args + 0, index);
      fts_set_symbol(args + 1, name);
      fts_client_send_message((fts_object_t *)config, fts_s_output, 2, args);
    }
    
    fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);
  }
}

static void
midiconfig_label_set_internal(fts_midiconfig_t *config, fts_midilabel_t *label, int index)
{
  /* create internal MIDI port */
  fts_midiport_t *port = (fts_midiport_t *)fts_object_create(midibus_type, 0, 0);

  /* set input and output to internal MIDI port */
  midiconfig_label_set_input(config, label, index, port, midiconfig_s_internal_bus);
  midiconfig_label_set_output(config, label, index, port, midiconfig_s_internal_bus);
}

static void
midiconfig_set_input(fts_midiconfig_t *config, int index, fts_symbol_t name)
{
  fts_midilabel_t *label = midiconfig_label_get_by_index(config, index);  

  /* reset output to none if it is set to internal */
  if(name != midiconfig_s_internal_bus && label->output && label->output_name == midiconfig_s_internal_bus)
    midiconfig_label_set_output(config, label, index, NULL, NULL);

  if(name == fts_s_unconnected)
    midiconfig_label_set_input(config, label, index, NULL, NULL);
  else if(name == midiconfig_s_internal_bus)
    midiconfig_label_set_internal(config, label, index);
  else
    midiconfig_label_set_input(config, label, index, midimanagers_get_input(name, label->name), name);
}

static void
midiconfig_set_output(fts_midiconfig_t *config, int index, fts_symbol_t name)
{
  fts_midilabel_t *label = midiconfig_label_get_by_index(config, index);

  /* reset output to none if it is set to internal */
  if(name != midiconfig_s_internal_bus && label->input && label->input_name == midiconfig_s_internal_bus)
    midiconfig_label_set_input(config, label, index, NULL, NULL);

  if(name == fts_s_unconnected)
    midiconfig_label_set_output(config, label, index, NULL, NULL);
  else if(name == midiconfig_s_internal_bus)
    midiconfig_label_set_internal(config, label, index);
  else
    midiconfig_label_set_output(config, label, index, midimanagers_get_output(name, label->name), name);
}

/* name utility */
static fts_symbol_t
midiconfig_get_fresh_label_name(fts_midiconfig_t *config, fts_symbol_t name)
{
  const char *str = name;
  int len = strlen(str);
  char *new_str = alloca((len + 10) * sizeof(char));
  int num = 0;
  int dec = 1;
  int i;

  /* separate base name and index */
  for(i=len-1; i>=0; i--) 
  {
    if(len == (i + 1) && str[i] >= '0' && str[i] <= '9')
      num += (str[len = i] - '0') * dec;
    else
      new_str[i] = str[i];
      
    dec *= 10;
  }
  
  /* generate new label name */
  while(midiconfig_label_get_by_name(config, name) != NULL) 
  {
    sprintf(new_str + len, "%d", ++num);
    name = fts_new_symbol(new_str);
  }

  return name;
}

static void
midiconfig_restore(fts_midiconfig_t *config)
{
  fts_objectlist_cell_t *p = fts_objectlist_get_head(&midiconfig_listeners);

  while(p != NULL)
  {
    fts_objectlist_cell_t *next = fts_objectlist_get_next(p);

    fts_object_recompute(fts_objectlist_get_object(p));

    p = next;
  }
}

static void
midiconfig_update_devices(fts_midiconfig_t *config)
{
  int ac;
  fts_atom_t *at;
  
  fts_array_clear(&midiconfig_inputs);
  fts_array_clear(&midiconfig_outputs);

  fts_array_append_symbol(&midiconfig_inputs, fts_s_unconnected);
  fts_array_append_symbol(&midiconfig_outputs, fts_s_unconnected);

  fts_array_append_symbol(&midiconfig_inputs, midiconfig_s_internal_bus);
  fts_array_append_symbol(&midiconfig_outputs, midiconfig_s_internal_bus);

  /* get device names from all midimanagers */
  midimanagers_get_device_names();

  ac = fts_array_get_size(&midiconfig_inputs);
  at = fts_array_get_atoms(&midiconfig_inputs);
  fts_client_send_message((fts_object_t *)config, midiconfig_s_inputs, ac, at);
  
  ac = fts_array_get_size(&midiconfig_outputs);
  at = fts_array_get_atoms(&midiconfig_outputs);
  fts_client_send_message((fts_object_t *)config, midiconfig_s_outputs, ac, at);
}

static void
midiconfig_update_labels(fts_midiconfig_t *config)
{
  fts_midilabel_t *label = config->labels;
  int n = config->n_labels;
  int i;

  /* check inout and output midiports */
  for(i=0; i<n; i++) 
  {
    if(label->input == NULL || fts_object_get_class((fts_object_t *)label->input) != midibus_type) 
    {
      fts_midiport_t *input = NULL;
      fts_midiport_t *output = NULL;
	  
      if(label->input != NULL)
      {
	input = midimanagers_get_input(label->input_name, label->name);
	      
	if(input != label->input) 
	  midiconfig_label_set_input(config, label, i, input, label->input_name);
      }	  

      if(label->output != NULL)
      {
	output = midimanagers_get_output(label->output_name, label->name);
	      
	if(output != label->output) 
	  midiconfig_label_set_output(config, label, i, output, label->output_name);
      }
    }
      
    label = label->next;
  }
}

static void
midiconfig_erase_labels(fts_midiconfig_t *config)
{
  fts_midilabel_t *label = config->labels;
  
  while(label != NULL)
  {
    fts_midilabel_t *next = label->next;
      
    midilabel_delete(label);
    label = next;
  }
  
  config->labels = NULL;
  config->n_labels = 0;
}

void
fts_midiconfig_set_defaults(fts_midiconfig_t* midiconfig)
{
  if(midiconfig != NULL)
  {
    fts_midilabel_t *label = midiconfig_label_get_by_index(midiconfig, 0);
      
    if(label == NULL)
      label = midiconfig_label_insert(midiconfig, 0, fts_s_default);
      
    if(label->input == NULL)
    {
      fts_symbol_t name = midimanagers_get_default_input();
      fts_midiport_t *port = midimanagers_get_input(name, fts_s_default);
	  
      midiconfig_label_set_input(midiconfig, label, 0, port, name);
    }

    if(label->output == NULL)
    {
      fts_symbol_t name = midimanagers_get_default_output();
      fts_midiport_t *port = midimanagers_get_output(name, fts_s_default);
	  
      midiconfig_label_set_output(midiconfig, label, 0, port, name);
    }

    fts_config_set_dirty( (fts_config_t *)fts_config_get(), 0);
  }
}

/* midi manager API */
void
fts_midiconfig_update()
{
  fts_midiconfig_t* midiconfig = (fts_midiconfig_t*)fts_midiconfig_get();
  
  midiconfig_update_labels(midiconfig);
  midiconfig_update_devices(midiconfig);
}

void
fts_midiconfig_add_manager(fts_midimanager_t *mm)
{
  fts_midimanager_t **p = &midimanagers;
  
  while(*p != NULL)
    p = &((*p)->next);

  *p = mm;
  mm->next = NULL;

  /* update default devices */
  fts_midiconfig_set_defaults((fts_midiconfig_t*)fts_midiconfig_get());
}

/* midi objects API */
fts_midiport_t *
fts_midiconfig_get_input(fts_symbol_t name)
{
  fts_midiconfig_t* midiconfig = (fts_midiconfig_t*)fts_midiconfig_get();
  
  if(midiconfig != NULL)
  {
    fts_midilabel_t *label = midiconfig_label_get_by_name(midiconfig, name);
      
    if(label != NULL && label->input != NULL)
      return label->input;
  }

  return midinull;
}

fts_midiport_t *
fts_midiconfig_get_output(fts_symbol_t name)
{
  fts_midiconfig_t* midiconfig = (fts_midiconfig_t*)fts_midiconfig_get();
  
  if(midiconfig != NULL)
  {
    fts_midilabel_t *label = midiconfig_label_get_by_name(midiconfig, name);
      
    if(label != NULL && label->output != NULL)
      return label->output;
  }
  
  return midinull;
}

void
fts_midiconfig_add_listener(fts_object_t *obj)
{
  fts_objectlist_insert(&midiconfig_listeners, obj);
}

void
fts_midiconfig_remove_listener(fts_object_t *obj)
{
  fts_objectlist_remove(&midiconfig_listeners, obj);
}

/****************************************************
 *
 *  MIDI configuration class
 *
 */
fts_class_t *fts_midiconfig_class;

static void
midiconfig_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;

  midiconfig_erase_labels(this);
  midiconfig_label_insert(this, 0, fts_s_default);  

  if(fts_object_has_id( o)) 
    fts_client_send_message( o, fts_s_clear, 0, 0);
  
  fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);
}

static void
midiconfig_restore_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  fts_symbol_t name = fts_get_symbol(at);
  fts_symbol_t input = fts_get_symbol(at + 1);
  fts_symbol_t output = fts_get_symbol(at + 2);
  int index = this->n_labels;
  
  if( name == fts_s_default)
    {
      if( midiconfig_label_get_by_name( this, name) == NULL)
	midiconfig_label_insert(this, 0, name);
      
      midiconfig_set_input(this, 0, input);
      midiconfig_set_output(this, 0, output);
    }  
  else
    {
      /* make sure that first label is "default" */
      if(index == 0)
	{
	  midiconfig_label_insert(this, 0, fts_s_default);
	  index = 1;
	}

      midiconfig_label_insert(this, index, name);
      midiconfig_set_input(this, index, input);
      midiconfig_set_output(this, index, output);
    }
}

static void
midiconfig_insert_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);
  
  /* check if name is not already used */
  if(midiconfig_label_get_by_name(this, name) != NULL)
    name = midiconfig_get_fresh_label_name(this, name);

  midiconfig_label_insert(this, index, name);
}

static void
midiconfig_remove_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  int index = fts_get_int(at);

  midiconfig_label_remove(this, index);

  midiconfig_update_labels(this);
  midiconfig_update_devices(this);
  midiconfig_restore(this);
}

static void
midiconfig_input( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);

  midiconfig_set_input(this, index, name);

  midiconfig_update_labels(this);
  midiconfig_update_devices(this);
  midiconfig_restore(this);
}

static void
midiconfig_output( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);

  midiconfig_set_output(this, index, name);

  midiconfig_update_labels(this);
  midiconfig_update_devices(this);
  midiconfig_restore(this);
}

static void
midiconfig_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  fts_midilabel_t *label = this->labels;
  int i;

  /* upload labels with inputs and outputs */
  for(i=0; i<this->n_labels; i++) 
  {
    fts_atom_t args[4];
      
    fts_set_int(args, i);
    fts_set_symbol(args + 1, label->name);
    fts_set_symbol(args + 2, label->input_name);
    fts_set_symbol(args + 3, label->output_name);
    fts_client_send_message((fts_object_t *)this, fts_s_insert, 4, args);
      
    label = label->next;
  }

  midiconfig_update_labels( this);
  midiconfig_update_devices( this);
}


void
fts_midiconfig_dump( fts_midiconfig_t *this, fts_bmax_file_t *f)
{
  fts_midilabel_t *label = this->labels;
  
  while(label) 
  {
    /*code insert message for each label */
    /*
      here symbol order must be inverse of restore method symbol order 
      so in restore_method (selector fts_s_label):
      label->name = fts_get_symbol(at);
      label->input_name = fts_get_symbol(at+1);
      label->output_name = fts_get_symbol(at+2);
    */
    fts_bmax_code_push_symbol(f, label->output_name);
    fts_bmax_code_push_symbol(f, label->input_name);
    fts_bmax_code_push_symbol(f, label->name);
    fts_bmax_code_push_symbol(f, fts_s_label);
    fts_bmax_code_obj_mess(f, fts_s_midi_config, 4);
    fts_bmax_code_pop_args(f, 4);
      
    label = label->next;	  
  }
}

static void
midiconfig_set_to_defaults( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_clear( o, winlet, fts_s_clear, 0, 0);
  /* this->file_name = NULL; */
  midiconfig_upload( o, winlet, fts_s_upload, 0, 0); 
  
  fts_config_set_dirty( (fts_config_t *)fts_config_get(), 0);
}

static void
midiconfig_print( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  fts_midilabel_t *label = this->labels;
  fts_bytestream_t* stream = fts_get_default_console_stream();
  fts_midimanager_t *mm;
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  fts_spost(stream, "labels\n");
  for(label = this->labels; label != NULL; label = label->next) 
    fts_spost(stream, "  %s: '%s' '%s'\n", label->name, label->input_name, label->output_name);

  /* redirect to MIDI managers */
  for(mm = midimanagers; mm != NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_s_print, ac, at);
}

static void
midiconfig_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  fts_atom_t a;
      
  this->labels = NULL;
  this->n_labels = 0;

  /* modify object description */
  fts_set_symbol(&a, midiconfig_s_name);
  fts_object_set_description(o, 1, &a);
}

static void
midiconfig_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;

  midiconfig_erase_labels(this);
}

static void
midiconfig_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_midiconfig_t), midiconfig_init, midiconfig_delete);

  fts_class_message_varargs(cl, fts_s_clear, midiconfig_clear);
  fts_class_message_varargs(cl, fts_s_default, midiconfig_set_to_defaults);

  fts_class_message_varargs(cl, fts_s_label, midiconfig_restore_label);
  fts_class_message_varargs(cl, fts_s_insert, midiconfig_insert_label);
  fts_class_message_varargs(cl, fts_s_remove, midiconfig_remove_label);
  fts_class_message_varargs(cl, fts_s_input, midiconfig_input);
  fts_class_message_varargs(cl, fts_s_output, midiconfig_output);
  fts_class_message_varargs(cl, fts_s_upload, midiconfig_upload);

  fts_class_message_varargs(cl, fts_s_print, midiconfig_print);
}

/* **********************************************************************
 *
 * Initialization
 *
 */
void
fts_kernel_midi_init(void)
{
  midibus_type = fts_class_install(NULL, midibus_instantiate);
  midinull_type = fts_class_install(NULL, midinull_instantiate);
  
  midiconfig_s_name = fts_new_symbol("__midiconfig");
  midiconfig_s_internal_bus = fts_new_symbol("Internal Bus");
  midiconfig_s_inputs = fts_new_symbol("inputs");
  midiconfig_s_outputs = fts_new_symbol("outputs");  
  fts_array_init(&midiconfig_inputs, 0, 0);
  fts_array_init(&midiconfig_outputs, 0, 0);

  /* midi manager interface messages */
  fts_midimanager_s_get_default_input = fts_new_symbol("_midimanager_get_default_input");
  fts_midimanager_s_get_default_output = fts_new_symbol("_midimanager_get_default_output");
  fts_midimanager_s_append_input_names = fts_new_symbol("_midimanager_append_input_names");
  fts_midimanager_s_append_output_names = fts_new_symbol("_midimanager_append_output_names");
  fts_midimanager_s_get_input = fts_new_symbol("_midimanager_get_input");
  fts_midimanager_s_get_output = fts_new_symbol("_midimanager_get_output");

  /* MIDI configuration class */
  fts_midiconfig_class = fts_class_install(midiconfig_s_name, midiconfig_instantiate);

  /* create global NULL MIDI port */
  midinull = (fts_midiport_t *)fts_object_create(midinull_type, 0, 0);
  fts_object_refer((fts_object_t *)midinull);

  fts_objectlist_init(&midiconfig_listeners);
}
