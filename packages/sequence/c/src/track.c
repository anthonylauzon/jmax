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

#ifdef WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

#include <string.h>
#include <fts/fts.h>
#include <ftsconfig.h>
#include <fts/packages/data/data.h>
#include <fts/packages/sequence/sequence.h>
#include "seqmess.h"

#define TRACK_BLOCK_SIZE 256
#define MAX_CLIENT_MSG_ATOMS 64         /* max. number of atoms to send */

fts_class_t *track_class = 0;

static void track_upload_markers(track_t *self);
static void track_upload(track_t *this);

/*********************************************************
*
*  insert events to event list
*
*/

static void
prepend_event(track_t *track, event_t *event)
{
  if(track->size == 0)
  {
    event->next = NULL;
    event->prev = NULL;
    
    track->first = event;
    track->last = event;
    
    track->size = 1;
  }
  else
  {
    event->next = track->first;
    event->prev = NULL;
    
    track->first->prev = event;
    track->first = event;
    
    track->size++;
  }
}

static void
append_event(track_t *track, event_t *event)
{
  if(track->size == 0)
  {
    event->next = NULL;
    event->prev = NULL;
    
    track->first = event;
    track->last = event;
    
    track->size = 1;
  }
  else
  {
    event->prev = track->last;
    event->next = NULL;
    
    track->last->next = event;
    track->last = event;
    
    track->size++;
  }
}

static void
insert_event_before(track_t *track, event_t *here, event_t *event)
{
  if(here == 0)
    append_event(track, event);
  else if(here == track->first)
    prepend_event(track, event);
  else
  {
    event->prev = here->prev;
    event->next = here;
    
    here->prev->next = event;
    here->prev = event;
    
    track->size++;
  }
}

static void
insert_event_behind(track_t *track, event_t *here, event_t *event)
{
  if(here == 0)
    prepend_event(track, event);
  else if(here == track->last)
    append_event(track, event);
  else
  {
    event->next = here->next;
    event->prev = here;
    
    here->next->prev = event;
    here->next = event;
    
    track->size++;
  }
}

static void
cutout_event(track_t *track, event_t *event)
{
  if(event->prev == NULL)
  {
    if(event->next == NULL)
    {
      track->first = NULL;
      track->last = NULL;
    }
    else
    {
      /* event is first of track */
      track->first = event->next;
      track->first->prev = NULL;
    }
  }
  else if(event->next == NULL)
  {
    /* event is last of track */
    track->last = event->prev;
    track->last->next = NULL;
  }
  else
  {
    event->prev->next = event->next;
    event->next->prev = event->prev;
  }
  
  track->size--;
}

/*********************************************************
*
*  get events
*
*/

/* returns first event at or after given time */
event_t *
track_get_event_by_time(track_t *track, double time)
{
  if(track_get_size(track) > 0 && time <= event_get_time(track->last))
  {
    event_t *event = track->first;
    
    while(event != NULL && time > event_get_time(event))
      event = event_get_next(event);
    
    return event;
  }
  else
    return NULL;
}

/* returns next event after given time */
event_t *
track_get_next_by_time(track_t *track, double time)
{
  if(track_get_size(track) > 0 && time < event_get_time(track->last))
  {
    event_t *event = track->first;
    
    while(event != NULL && time >= event_get_time(event))
      event = event_get_next(event);
    
    return event;
  }
  else
    return NULL;
}

/* like above - searching is started from given element (here) */
event_t *
track_get_next_by_time_after(track_t *track, double time, event_t *here)
{
  if(track_get_size(track) > 0 && time <= event_get_time(track->last))
  {
    event_t *event;
    
    if(here)
      event = here;
    else
      event = track->first;
    
    while(event != NULL && time >= event_get_time(event))
      event = event_get_next(event);
    
    return event;
  }
  else
    return NULL;
}


/* get event before or at <time>, 
   search in descending time order starts at event <here>, if not NULL 
   (N.B.: if time is before first event, the first event is returned(?))
*/
event_t *
track_get_left_by_time_from(track_t *track, double time, event_t *here)
{
  event_t *event = NULL;  
  double center = 0.5 * track_get_last_time(track);
  
  if(here != NULL)
    event = here;
  else if(time < center)
    event = track->first;
  else if(time >= center)
    event = track->last;
  
  while(event != NULL && event_get_time(event) > time)
    event = event_get_prev(event);
  
  while(event != NULL && event_get_next(event) != NULL)
  {
    event_t *next = event_get_next(event);
    
    if(event_get_time(next) > time)
      break;
    
    event = next;
  }
  
  return event;
}



/*********************************************************
*
*  event track: add/remove and move
*
*/

static event_t *
track_event_create(int ac, const fts_atom_t *at)
{
  event_t *event = NULL;
  fts_atom_t value;
  
  fts_set_void(&value);
  
  /*  compatibility for class names starting with ":" (skip) */
  if(fts_is_symbol(at) && fts_get_symbol(at) == fts_s_colon)
  {
    ac--;
    at++;
  }
  
  if(fts_is_symbol(at))
  {
    /* primitive value as <class name> <value> or object as <class name> [<property name> <property value> ...] */
    fts_symbol_t class_name = fts_get_symbol(at);
    
    /* skip class name */
    ac--;
    at++;
    
    if(class_name == fts_s_int || class_name == fts_s_float || class_name == fts_s_symbol)
      value = at[1];
    else
    {
      fts_class_t *type = fts_get_class_by_name(class_name);
      
      if(type != NULL)
      {
        fts_object_t *obj = fts_object_create(type, 0, 0);
        
        if(obj != NULL)
        {
          int i;
          
          /* set properties */
          for(i=0; i<ac-1; i+=2)
          {
            if(fts_is_symbol(at + i))
            {
              fts_symbol_t prop = fts_get_symbol(at + i);              
              fts_send_message(obj, prop, 1, at + i + 1, fts_nix);
            }
          }
          
          fts_set_object(&value, obj);
        }
      }
    }
  }
  else
  {
    /* primitive value */
    value = at[0];
  }
  
  if(!fts_is_void(&value))
  {
    event = (event_t *)fts_object_create(event_class, 1, &value);
    return event;
  }
  
  return NULL;
}

void
track_add_event(track_t *track, double time, event_t *event)
{
  event_t *here = track_get_next_by_time(track, time);
  
  insert_event_before(track, here, event);
  fts_object_refer(event);

  if(time < 0.0)
    time = 0.0;
  
  event_set_track(event, track);
  event_set_time(event, time);
}

void
track_add_event_and_upload(track_t *track, double time, event_t *event)
{  
  track_add_event(track, time, event);
  
  if(track_editor_is_open(track))
    track_upload_event( track, event);
  
  fts_object_set_state_dirty((fts_object_t *)track);
}

void
track_add_event_after(track_t *track, double time, event_t *event, event_t *after)
{
  event_t *here = track_get_next_by_time_after(track, time, after);
  
  insert_event_before(track, here, event);
  fts_object_refer(event);
  
  if(time < 0.0)
    time = 0.0;
  
  event_set_track(event, track);
  event_set_time(event, time);
}

void
track_add_event_after_and_upload(track_t *track, double time, event_t *event, event_t *after)
{  
  track_add_event_after(track, time, event, after);
  
  if(track_editor_is_open(track))
    track_upload_event( track, event);
  
  fts_object_set_state_dirty((fts_object_t *)track);
}

void
track_append_event(track_t *track, double time, event_t *event)
{
  append_event(track, event);
  fts_object_refer(event);
  
  if(time < 0.0)
    time = 0.0;
  
  event_set_track(event, track);
  event_set_time(event, time);
}

void
track_remove_event(track_t *track, event_t *event)
{
  cutout_event(track, event);
  
  event->next = event->prev = NULL;
  event_set_track(event, track);
  
  fts_object_release(event);
}

void
track_remove_event_and_upload(track_t *track, event_t *event)
{
  fts_atom_t at;
  
  if(track_editor_is_open(track))
  {
    fts_set_object(&at, (fts_object_t *) event);  
    fts_client_send_message((fts_object_t *) track, seqsym_removeEvents, 1, &at);  
  }
  
  track_remove_event(track, event);
  fts_object_set_state_dirty((fts_object_t *)track);
}

void
track_move_event(track_t *track, event_t *event, double time)
{
  event_t *next = event_get_next(event);
  event_t *prev = event_get_prev(event);
  
  if(time < 0.0)
    time = 0.0;
  
  if((next && time > next->time) || (prev && time < prev->time))
  {
    cutout_event(track, event);
    
    event_set_time(event, time);
    
    insert_event_before(track, track_get_next_by_time(track, time), event);
  }
  else
    event_set_time(event, time);
}

void
track_move_event_and_upload(track_t *track, event_t *event, double time)
{
  track_move_event(track, event, time);
  
  if(track_editor_is_open(track))
  {
    fts_atom_t a;
    
    fts_set_object(&a, (fts_object_t *)event);
    fts_client_send_message((fts_object_t *)track, seqsym_moveEvents, 1, &a);
  }
  
  fts_object_set_state_dirty((fts_object_t *)track);
}

static void 
track_copy_events(track_t *org, track_t *copy)
{
  event_t *orgevent = track_get_first(org);
  
  while(orgevent != NULL)
  {
    double time  = event_get_time(orgevent);
    fts_atom_t *value = event_get_value(orgevent);
    fts_atom_t a;
    event_t *eventcopy;
    
    fts_set_void(&a);
    fts_atom_copy(value, &a);
    eventcopy = (event_t *) fts_object_create(event_class, 1, &a);
    track_append_event(copy, time, eventcopy);
    fts_atom_release(&a);  /* atom no longer needed (decrement reference count set by fts_atom_copy in case of an object) */
    
    orgevent = event_get_next(orgevent);
  }
}

void 
track_copy(track_t *org, track_t *copy)
{
  event_t *orgevent = track_get_first(org);
  
  track_clear(copy);
  track_copy_events(org, copy);
  
  if(org->markers != NULL)
    track_copy_events(org->markers, track_get_or_make_markers(copy));
}

void
track_merge(track_t *track, track_t *merge)
{
  event_t *here = track_get_first(track);
  event_t *event = track_get_first(merge);
  
  if(here)
  {
    while(event)
    {
      double time = event_get_time(event);
      event_t *next = event_get_next(event);
      
      /* search for good place */
      while(here && time >= event_get_time(here))
        here = event_get_next(here);
      
      /* transfer event */
      insert_event_before(track, here, event);
      
      event_set_track(event, track);
      event_set_time(event, time);
      
      event = next;
    }
  }
  else
  {
    track->first = merge->first;
    track->last = merge->last;
    track->size = merge->size;
  }
  
  /* merge track is empty */
  merge->first = 0;
  merge->last = 0;
  merge->size = 0;
}

void
track_erase_events(track_t *track)
{
  event_t *event = track_get_first(track);
  
  while(event)
  {
    event_t *next = event_get_next(event);
    
    event->next = event->prev = NULL;
    event->track = NULL;
    fts_object_release((fts_object_t *)event);
    
    event = next;
  }
  
  /* merge track is empty */
  track->first = NULL;
  track->last = NULL;
  track->size = 0;    
}

void
track_clear(track_t *track)
{
  track_erase_events(track);
  
  if(track->markers != NULL)
    track_erase_events(track->markers);
}

void
track_merge_and_upload(track_t *track, track_t *merge)
{
  track_merge(track, merge);
  
  if(track_editor_is_open(track))
    track_upload(track);
  
  fts_object_set_state_dirty((fts_object_t *)track);
}

void
track_clear_and_upload(track_t *track)
{
  track_erase_events(track);
  track_clear_at_client(track);
  
  if(track->markers != NULL)
  {
    track_erase_events(track->markers);
    track_clear_at_client(track->markers);
  }
  
  fts_object_set_state_dirty((fts_object_t *)track);
}

/* get segment by time ("after" is the next event after the segment) */
void
track_segment_get(track_t *self, double begin, double end, event_t **first, event_t **after)
{
  event_t *event = track_get_first(self);
  
  *first = *after = NULL;
  
  if(begin < 0.0)
    begin = 0.0;
  
  if(end < begin)
  {
    double b = begin;
    begin = end;
    end = b;
  }
  
  while(event && event_get_time(event) < begin)
    event = event_get_next(event);
  
  *first = event;
  
  while(event && event_get_time(event) < end)
    event = event_get_next(event);
  
  *after = event;
}

void
track_segment_shift(track_t *self, event_t *first, event_t *after, double begin, double end, double shift)
{
  if(first != NULL && shift != 0.0)
  {
    event_t *event = first;
    event_t *prev = event_get_prev(first);    
    double before = 0.0;
    
    if(prev != NULL)
      before = event_get_time(prev);
    
    while(event != after && event_get_time(event) + shift < before)
    {
      double time = event_get_time(event) + shift;
      event_t *next = event_get_next(event);
      
      if(time < 0.0)
        time = 0.0;
      
      track_move_event(self, event, time);
      event = next;
    }
    
    while(event != after && event_get_time(event) + shift < end)
    {
      event_set_time(event, event_get_time(event) + shift);
      event = event_get_next(event);
    }
    
    if(event != NULL)
    {
      prev = event_get_prev(event);
      
      if(after != NULL)
        event = event_get_prev(after);
      else
        event = track_get_last(self);
      
      while(event != prev)
      {
        event_t *prev = event_get_prev(event);
        
        track_move_event(self, event, event_get_time(event) + shift);        
        
        event = prev;      
      }
    }
  }
}

void
track_segment_stretch(track_t *self, event_t *first, event_t *after, double begin, double end, double stretch)
{
  double shift = 0.0;
  fts_atom_t a[2];
  
  if(stretch < 0.0)
    stretch = 0.0;
  
  if(stretch != 1.0)
  {
    fts_method_t meth_stretch = fts_class_get_method_varargs(track_get_type(self), seqsym_stretch);
    event_t *event = first;
    
    fts_set_float(a, stretch);
    
    if(first != track_get_first(self) && meth_stretch != NULL)
    {
      event = track_get_first(self);
      
      while(event != first)
      {
        double time = event_get_time(event);
        double duration = event_get_duration(event);
        
        if(time + duration > begin)
        {
          if(time + duration > end)
            duration = end - begin;
          else
            duration -= begin - time;
          
          fts_set_float(a + 1, duration);
          (*meth_stretch)(fts_get_object( event_get_value(event)), NULL, 2, a, fts_nix);
          
          if( track_editor_is_open(self))
            event_set_at_client(event, 0);
        }
        event = event_get_next(event);
      }
    }
    
    if(first != NULL)
    {
      while(event != after)
      {
        double time = event_get_time(event);
        
        event_set_time(event, begin + (time - begin) * stretch);
        
        if(meth_stretch != NULL)
        {
          double duration = event_get_duration(event);
          
          if(time + duration > end)
            duration = end - time;
          
          fts_set_float(a + 1, duration);
          (*meth_stretch)(fts_get_object( event_get_value(event)), NULL, 2, a, fts_nix);
          
          if( track_editor_is_open(self)) 
            event_set_at_client(event, 0);
        }
        
        event = event_get_next(event);
      }
      
      shift = (end - begin) * (stretch - 1.0);
      
      while(event != NULL)
      {
        event_set_time(event, shift + event_get_time(event));
        event = event_get_next(event);
      }    
    }
  }
}

void
track_segment_quantize(track_t *self, event_t *first, event_t *after, double begin, double end, double quantize)
{
  int quantize_durations = 1;
  
  if(quantize < 0.0)
  {
    quantize *= -1.0;
    quantize_durations = 0;
  }
  
  if(first != NULL && quantize > 0.0)
  {
    event_t *event = first;
    double time = begin;
    
    while(event != after && time < end)
    {
      double next = time + quantize;
      
      while(event && fabs(event_get_time(event) - time) < fabs(event_get_time(event) - next))
      {
        event_t *next = event_get_next(event);
        
        if(time <= end)
          event_set_time(event, time);
        else
          track_move_event(self, event, time);        
        
        if(quantize_durations > 0)
        {
          double dur = event_get_duration(event);
          
          if(dur > quantize)
            event_set_duration(event, quantize * floor(dur / quantize + 0.5));
          else if(dur > 0.0)
            event_set_duration(event, quantize);
        }
        
        event = next;
      }
      
      time = next;
    }
  }
}

int
track_editor_is_open(track_t *self) 
{
  if(track_is_marker(self))
  {
    fts_object_t *container = fts_object_get_container((fts_object_t *)self);

    if(container != NULL && fts_object_is_a(container, track_class))
      return (((track_t *)container)->open != 0);
    else
      return 0;
  }
  else
    return (self->open != 0);
}

int
track_is_in_multitrack(track_t *track)
{
  fts_object_t *container = fts_object_get_container((fts_object_t *)track);
  return (container != NULL && fts_object_is_a(container, multitrack_class));
}

static void
track_copy_function(const fts_object_t *from, fts_object_t *to)
{
  track_copy((track_t *)from, (track_t *)to);
}

static void
track_description_function(fts_object_t *o,  fts_array_t *array)
{
  track_t *self = (track_t *)o;
  fts_class_t *type = track_get_type(self);
  
  fts_array_append_symbol(array, seqsym_track);
  fts_array_append_symbol(array, fts_class_get_name(type));
}

/******************************************************
 *
 *  upload
 *
 */
static void
track_upload_property_list(track_t *self, fts_array_t *temp_array)
{
  fts_class_t *type = track_get_type(self);
  
  if(type)
  {
    if(fts_class_is_primitive(type))
    {
      fts_atom_t a[2];
      
      fts_set_symbol(a, fts_s_value);
      fts_set_symbol(a + 1, fts_class_get_name(type));
      
      fts_client_send_message((fts_object_t *)self, seqsym_properties, 2, a);
    }
    else
    {
      fts_method_t method_get_property_list;
      
      fts_class_instantiate(type);
      method_get_property_list = fts_class_get_method_varargs(type, seqsym_get_property_list);
      
      /* get property list from class by method */
      if(method_get_property_list)
      {
        int size;
        fts_atom_t *atoms;
        fts_atom_t a;
        
        fts_array_set_size(temp_array, 0);
        fts_set_pointer(&a, (void *)temp_array);
        
        /* get properties array from class */
        (*method_get_property_list)((fts_object_t *)self, NULL, 1, &a, fts_nix);
        
        size = fts_array_get_size(temp_array);
        atoms = fts_array_get_atoms(temp_array);
        
        /* send properties to client */
        if(size > 0)
          fts_client_send_message((fts_object_t *)self, seqsym_properties, size, atoms);
      }
      else /* case of fts_object type (ex. fmat) */
      {
        fts_atom_t a[2];
        
        fts_set_symbol(a, seqsym_object);
        fts_set_symbol(a + 1, seqsym_object);
        
        fts_client_send_message((fts_object_t *)self, seqsym_properties, 2, a);
      }
    }
  }
}

static void
track_upload_event_with_array(track_t *self, event_t *event, fts_array_t *temp_array)
{
  fts_class_t *type = event_get_type(event);
  fts_atom_t a[4];
  
  if(fts_object_has_client((fts_object_t *)event) == 0)
  {
    fts_client_register_object((fts_object_t *)event, fts_object_get_client_id((fts_object_t *)self));
    
    if(fts_is_object(&event->value))
    {
      fts_method_t method_append_properties = fts_class_get_method_varargs(type, seqsym_append_properties);
      int size = 0;
      fts_atom_t *atoms = 0;
      
      fts_array_set_size(temp_array, 0);
      fts_array_append_int(temp_array, fts_object_get_id((fts_object_t *)event));
      fts_array_append_float(temp_array, event_get_time(event));
      fts_array_append_symbol(temp_array, fts_get_class_name(&event->value));
      
      /* get array of properties and types from class */
      if(method_append_properties)
      {
        fts_atom_t a;
        
        fts_set_pointer(&a, temp_array);
        
        (*method_append_properties)( fts_get_object( event_get_value( event)), NULL, 1, &a, fts_nix);
      }
      else /* not a score object but other object as fmat */ 
      {
        /* register value and send object id as value-property */
        fts_object_t *valobj = fts_get_object( event_get_value( event));
        
        fts_client_upload_object(valobj, fts_object_get_client_id((fts_object_t *)self));
        
        fts_array_append_symbol(temp_array, seqsym_objid);
        fts_array_append_int(temp_array, fts_object_get_id(valobj));                  
      }
      
      size = fts_array_get_size(temp_array);
      atoms = fts_array_get_atoms(temp_array);
      
      /* send properties to client */
      if(size > 0)
        fts_client_send_message((fts_object_t *)self, seqsym_addEvents, size, atoms);
    }
    else
    {
      fts_set_int(a + 0, fts_object_get_id((fts_object_t *)event));
      fts_set_float(a + 1, event_get_time(event));
      fts_set_symbol(a + 2, fts_get_class_name(&event->value));
      a[3] = event->value;
      fts_client_send_message((fts_object_t *)self, seqsym_addEvents, 4, a);
    }
  }
  else
    event_set_at_client(event, 1);
}

void
track_upload_event(track_t *self, event_t *event)
{
  fts_array_t temp_array;  
  
  fts_array_init(&temp_array, 0, 0);
  track_upload_event_with_array( self, event, &temp_array);
  fts_array_destroy(&temp_array);
}

static void
track_upload(track_t *self)
{
  fts_symbol_t name = track_get_name(self);
  event_t *event = track_get_first(self);
  fts_array_t temp_array;
  fts_atom_t a;
  
  fts_array_init(&temp_array, 0, 0);
  
  fts_set_int(&a, track_get_size( self));
  fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 1, &a);
  
  /* set track name */
  if(name)
  {
    fts_set_symbol(&a, name);
    fts_client_send_message((fts_object_t *)self, seqsym_setName, 1, &a);
  }
  
  fts_set_int(&a, track_is_active(self));
  fts_client_send_message((fts_object_t *)self, seqsym_active, 1, &a);
  
  /* upload array of properties and types */
  track_upload_property_list(self, &temp_array);
  
  while(event)
  {
    /* create event at client */
    if(fts_object_has_client((fts_object_t *)event) == 0)
      track_upload_event_with_array(self, event, &temp_array);
    else
      event_set_at_client(event, 1);/*?????????*/
      
      event = event_get_next(event);
  }
  
  if(self->markers != NULL)
    track_upload_markers(self);
  
  fts_array_destroy(&temp_array);
  
  /* upload editor stuff */
  if(self->save_editor != 0)
  {
    fts_set_int(&a, self->save_editor); 
    fts_client_send_message((fts_object_t *)self, seqsym_save_editor, 1, &a);
  }
  
  if(self->editor != NULL)
  {    
    if(fts_object_has_client((fts_object_t *)self->editor) == 0)
    {
      fts_atom_t a;
      
      fts_client_register_object((fts_object_t *)self->editor, fts_object_get_client_id((fts_object_t *)self));  
      
      fts_set_int(&a, fts_object_get_id((fts_object_t *)self->editor));
      fts_client_send_message((fts_object_t *)self, seqsym_editor, 1, &a);
    }
    
    track_editor_upload(self->editor);
  }  
  
  fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
}

void
track_clear_at_client(track_t *self)
{
  if(fts_object_has_client((fts_object_t *)self))
    fts_client_send_message((fts_object_t *)self, fts_s_clear, 0, 0);
}

/******************************************************
 *
 *  markers
 *
 */
track_t *
track_get_or_make_markers(track_t *track)
{
  track_t *markers = track_get_markers(track);
  
  if(markers == NULL)
  {
    fts_atom_t a;
    
    fts_set_symbol(&a, seqsym_scomark);
    markers = (track_t *)fts_object_create(track_class, 1, &a);
    fts_object_set_context((fts_object_t *)markers, (fts_context_t *)track);
    
    fts_object_refer((fts_object_t *) markers);
    track_set_markers(track, markers);
  }
  else if(fts_object_get_context((fts_object_t *)markers) == NULL)
    fts_object_set_context((fts_object_t *)markers, (fts_context_t *)track);
  
  return markers;
}


static void 
track_upload_markers(track_t *self)
{
  if(self->markers != NULL)
  {
    if(!fts_object_has_client((fts_object_t *)self->markers))
    {
      fts_class_t *markers_type = track_get_type(self->markers);
      fts_atom_t a[2];
      
      fts_client_register_object((fts_object_t *)self->markers, fts_object_get_client_id((fts_object_t *)self));
      
      fts_set_int(a, fts_object_get_id((fts_object_t *)self->markers));
      
      if(markers_type != NULL)
        fts_set_symbol(a + 1, fts_class_get_name(markers_type));
      else
        fts_set_symbol(a + 1, fts_s_void);              
      
      fts_client_send_message((fts_object_t *)self, seqsym_markers, 2, a);
    }
    
    fts_send_message((fts_object_t *)self->markers, fts_s_upload, 0, NULL, fts_nix);
  }
}

scomark_t *
track_insert_marker(track_t *track, double time, fts_symbol_t type)
{
  track_t *markers = track_get_or_make_markers(track);
  event_t *event = NULL;
  scomark_t *scomark = marker_track_insert_marker(markers, time, type, &event);
  
  return scomark;
}

static fts_method_status_t
track_insert_marker_from_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  double time = fts_get_float(at);
  fts_symbol_t sc_type = fts_get_symbol(at + 1);
  track_t *markers = self;
  event_t *event = NULL;
  
  if(!track_is_marker(self))
  {
    markers = track_get_or_make_markers(self);
    track_upload_markers(self);
  }
  
  marker_track_insert_marker(markers, time, sc_type, &event);
  
  track_upload_event(markers, event);
  
  fts_object_set_state_dirty((fts_object_t *)self);
  
  return fts_ok;
}

static fts_method_status_t
_track_get_markers (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (!track_is_marker((track_t *) o))
    fts_set_object(ret, track_get_or_make_markers((track_t *) o));
  else
    fts_set_object(ret, o); /* a track that is already a marker track returns itself */
  
  return fts_ok;
}

/* set marker track for a track */
static fts_method_status_t
_track_set_markers (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *) o;
  track_t *markers = (track_t *) fts_get_object(at);
  
  if (fts_is_a(at, track_class) && track_is_marker(markers))
  {
    /* args are ok */
    if (!track_is_marker(self))
    {
      /* we're a normal track: release old markers, set new */
      if (track_get_markers(self))
        fts_object_release(track_get_markers(self));
      
      track_set_markers(self, markers);
      fts_object_set_context((fts_object_t *)markers, (fts_context_t *)self);
      
      fts_object_refer(markers);
      
      /* update editor */
      if (track_editor_is_open(self))
        fts_send_message((fts_object_t *) markers, fts_s_upload, 0, NULL, fts_nix);
    }
    else
    {
      /* todo: a track that is already a marker track replaces
      itself by the argument within its containing track? */
    }
  }
  
  fts_set_object(ret, o); /* return self */
  
  return fts_ok;
}

static fts_method_status_t
_track_append_marker(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  track_t *markers = track_get_or_make_markers(self);
  double time = fts_get_number_float(at);
  event_t *event = NULL;
  
  marker_track_append_marker(markers, time, ac - 1, at + 1, &event);
  
  return fts_ok;
}

static fts_method_status_t
_track_append_bar(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  track_t *markers = self;
  event_t *new_bar = NULL;
  event_t *start_bar = NULL;
  
  if(!track_is_marker(self))
  {
    markers = track_get_or_make_markers(self);
    track_upload_markers(self);
  }
  
  if((ac > 0) && fts_is_object(at))
    start_bar = (event_t *)fts_get_object(at);
  
  new_bar = marker_track_append_bar( markers, start_bar);
  
  if(new_bar != NULL)
    track_upload_event(markers, new_bar);
  
  return fts_ok;
}

#define MARKERS_BAR_EPSILON 0.001

static fts_method_status_t
_track_make_bars(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  track_t *markers = self->markers;
  
  if(markers != NULL)
  {
    event_t *marker_event = track_get_first(markers);
    scomark_t *scomark = NULL;
    fts_symbol_t meter = NULL;
    int numerator = 0;
    int denominator = 0;
    double tempo = 0.0;
    double time = 0.0;
    double bar_duration = 0.0;
    double next_bar_time = 0.0;
    
    /* get beginning of meter */
    while(marker_event != NULL && next_bar_time == 0.0)
    {
      scomark = (scomark_t *)fts_get_object(event_get_value(marker_event));
      time = event_get_time(marker_event);
      
      scomark_bar_get_meter(scomark, &meter);
      scomark_get_tempo(scomark, &tempo);
      
      if(scomark_is_bar(scomark) && meter != NULL)
      {
        scomark_meter_symbol_get_quotient(meter, &numerator, &denominator);
        scomark_get_tempo(scomark, &tempo);
        
        /* free tempo not handled yet!!! */
        if(tempo > 0.0)
        {
          bar_duration = ((double)numerator * 240000.0) / (tempo * (double)denominator);
          next_bar_time = time + bar_duration;
        }
      } 
      
      marker_event = event_get_next(marker_event);
    }
    
    /* fill with bars until last marker */
    while(marker_event != NULL)
    {
      double old_tempo = tempo;
      
      scomark = (scomark_t *)fts_get_object(event_get_value(marker_event));
      time = event_get_time(marker_event);
      
      scomark_get_tempo(scomark, &tempo);
      
      if(scomark_is_bar(scomark))
      {
        fts_symbol_t meter = NULL;
        
        scomark_bar_get_meter(scomark, &meter);
        scomark_get_tempo(scomark, &tempo);
        
        if(meter != NULL)
          scomark_meter_symbol_get_quotient(meter, &numerator, &denominator);
        
        /* fill with bars until given bar */
        while(time - next_bar_time > MARKERS_BAR_TOLERANCE)
        {
          track_insert_marker(self, next_bar_time, seqsym_bar);
          next_bar_time += bar_duration;
        }
        
        if(next_bar_time - time > MARKERS_BAR_TOLERANCE)
          fts_post("track make_bars: bar @ %g is short by %g msec\n", time, next_bar_time - time);
        
        /* next given bar */
        bar_duration = ((double)numerator * 240000.0) / (tempo * (double)denominator);
        next_bar_time = time + bar_duration;        
      }
      else
      {
        /* current is marker inside bar (tempo & co) */
        while(time - next_bar_time > MARKERS_BAR_EPSILON)
        {
          track_insert_marker(self, next_bar_time, seqsym_bar);
          next_bar_time += bar_duration;
        }
        
        /* recompute bar duration and time of next bar from new tempo */
        if(tempo != old_tempo)
        {
          bar_duration = ((double)numerator * 240000.0) / (tempo * (double)denominator);
          next_bar_time = time + old_tempo * (next_bar_time - time) / tempo;
        }
        
        /* convert marker to bar if falling on next bar time */
        if(next_bar_time <= time + MARKERS_BAR_EPSILON)
        {
          scomark_set_type(scomark, seqsym_bar);
          next_bar_time += bar_duration;          
        }
      }
      
      /* advance to next marker */
      marker_event = event_get_next(marker_event);
    }
    
    /* fill until end of track */
    while(next_bar_time <= track_get_last_time(self) && next_bar_time > 0.0)
    {
      track_insert_marker(self, next_bar_time, seqsym_bar);
      next_bar_time += ((double)numerator * 240000.0) / (tempo * (double)denominator);
    }      
    
    marker_track_renumber_bars(markers, track_get_first(markers), 
                               FIRST_BAR_NUMBER, 0);
    
    if(track_editor_is_open(self))
      fts_send_message((fts_object_t *)self->markers, fts_s_upload, 0, NULL, fts_nix);
  }
  
  return fts_ok;
}


static fts_method_status_t
_track_renumber_bars (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  track_t *markers = self;
  
  if (!track_is_marker(self))
    markers = track_get_or_make_markers(markers);
  
  marker_track_renumber_bars(markers, track_get_first(markers), FIRST_BAR_NUMBER, 1);
  
  return fts_ok;
}

static fts_method_status_t
_track_collapse_markers(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if(track_is_marker(self))
    marker_track_collapse_markers(self, ac, at);
  else
    marker_track_collapse_markers( track_get_or_make_markers(self), ac, at);
  
  return fts_ok;
}

/******************************************************
 * 
 *  track edit utilities
 *
 */

/* make a trill scoob starting from a set of scoob: 
 *     - the first event gives first pitch; 
 *     - the second event gives second pitch (so interval); 
 *     - The duration is time from starting time of first event and end time of last endind event.
 * The set of events is removed and a new event (of type trill) take his place in track.
 */
static fts_method_status_t
_track_make_trill(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if(track_get_type(self) == scoob_class && ac > 1)
  {
    int i;
    event_t *evt;
    int too_much_pitch = 0;
    double time, start, end, duration;
    double first_pitch = 0.0;
    double second_pitch = 0.0;
    double pitch = 0.0;
    fts_atom_t a[9];
    
    /* first object */
    evt = (event_t *)fts_get_object(at);
    start = event_get_time(evt);
    end = start + event_get_duration(evt);
    
    first_pitch = scoob_get_pitch((scoob_t *)fts_get_object(event_get_value(evt)));
    second_pitch = scoob_get_pitch((scoob_t *)fts_get_object(event_get_value((event_t *)fts_get_object(at+1))));
    if(first_pitch > second_pitch) 
    {
      double temp = first_pitch;
      first_pitch = second_pitch;
      second_pitch = temp;
    }
    
    /* find start time and duration */
    for(i = 1; i<ac; i++)
    {
      evt = (event_t *)fts_get_object(at+i);
      time = event_get_time(evt);
      duration = event_get_duration(evt);
      if(time < start) start = time;
      if(time + duration > end) end = time + duration;
      
      pitch =  scoob_get_pitch((scoob_t *)fts_get_object(event_get_value(evt)));
      if(pitch != first_pitch && pitch != second_pitch)
      {
        too_much_pitch = 1;
        break;
      }
    }
    
    if(too_much_pitch == 0)
    {      
      fts_set_int(a, 0);
      fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 1, a);
      
      /* create new event and add to track */
      fts_set_symbol(a, seqsym_scoob);
      fts_set_symbol(a+1, seqsym_type);
      fts_set_symbol(a+2, seqsym_trill);
      fts_set_symbol(a+3, seqsym_pitch);
      fts_set_int(a+4, first_pitch);
      fts_set_symbol(a+5, seqsym_interval);
      fts_set_int(a+6, second_pitch - first_pitch);
      fts_set_symbol(a+7, seqsym_duration);
      fts_set_float(a+8, end-start);
      evt = track_event_create( 9, a);
      if(evt)
        track_add_event_and_upload( self, start, evt);    
    
      /* remove events */
      for(i = 0/*1*/; i<ac ; i++)
        track_remove_event(self, (event_t *)fts_get_object(at+i));
      
      /* remove events at client */
      fts_client_send_message((fts_object_t *)self, seqsym_removeEvents, ac, at);      
      
      fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
    }
    else
      fts_post("error: more than two pitches in selection, impossible to make a trill !\n");
  }
  
  return fts_ok;
}

/******************************************************
 *
 *  persistence compatibility
 *
 */
static fts_method_status_t
track_compatible_add_event_from_file(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  double time = fts_get_float(at);
  event_t *event = track_event_create(ac - 1, at + 1);
  
  /* add event to track (strictly ordered by time) */
  if(event != NULL)
  {
    fts_atom_t *a = event_get_value(event);
    
    if(fts_is_object(a))
      self->load_obj = fts_get_object(a);
    
    track_append_event(self, time, event);
  }
  
  return fts_ok;
}

static fts_method_status_t
track_compatible_add_marker_from_file(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  track_t *markers = track_get_or_make_markers(self);
  double time = fts_get_float(at);
  event_t *marker_event = track_event_create(ac - 1, at + 1);
  fts_object_set_context((fts_object_t *)marker_event, (fts_context_t *)markers);
  
  /* add event to track (strictly ordered by time) */
  if(marker_event != NULL)
  {
    fts_atom_t *a = event_get_value(marker_event);
    
    if(fts_is_object(a))
    {
      self->load_obj = fts_get_object(a);
      track_append_event(markers, time, marker_event);
      fts_object_set_context(self->load_obj, (fts_context_t *)marker_event);
    }
  }
  
  return fts_ok;
}

static fts_method_status_t
track_compatible_event_dump_mess(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  /* send dump message to last loaded object */
  if(self->load_obj != NULL)
  {
    fts_symbol_t selector = fts_get_symbol(at);
    fts_send_message(self->load_obj, selector, ac - 1, at + 1, fts_nix);
  }
  
  return fts_ok;
}

/******************************************************
 *
 *  client calls
 *
 */
/* create new event and upload by client request */
static fts_method_status_t
track_add_event_from_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event = track_event_create(ac - 1, at + 1);

  if(event)
    track_add_event_and_upload( self, time, event);
    
  return fts_ok;
}

static fts_method_status_t
track_make_event_from_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event = track_event_create(ac - 1, at + 1);
  
  /* add event to track */
  if(event != NULL)
    track_add_event(self, time, event);
  
  fts_object_set_state_dirty((fts_object_t *)self);
  
  return fts_ok;
}

static fts_method_status_t
track_remove_events_from_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  int i;
  
  if(track_is_marker(self))
  {
    marker_track_unset_tempo_on_selection(self, ac, at);
    marker_track_remove_events(self, ac, at);
  }
  else
  {
    for(i=0; i<ac; i++)
    {
      fts_object_t *event = fts_get_object(at + i);
      track_remove_event(self, (event_t *)event);
    }
  }
  /*  remove event objects from client */
  fts_client_send_message((fts_object_t *)self, seqsym_removeEvents, ac, at);
  
  fts_object_set_state_dirty((fts_object_t *)self);
  
  return fts_ok;
}

/* move event by client request */
static fts_method_status_t
track_move_events_from_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  int i;
  
  for(i=0; i<ac; i+=2)
  {
    event_t *event = (event_t *)fts_get_object(at + i);
    float time = fts_get_float(at + i + 1);
    
    track_move_event(self, (event_t *)event, time);
  }
  fts_client_send_message((fts_object_t *)self, seqsym_moveEvents, ac, at);
  fts_object_set_state_dirty((fts_object_t *)self);
  
  return fts_ok;
}

/******************************************************
*
*  client calls: highlight event, move segment
*
*/

void
track_highlight_event(track_t *track, event_t *event)
{
  if(track_editor_is_open(track))
  {
    fts_atom_t a;
    
    fts_set_object(&a, (fts_object_t *)event);
    fts_client_send_message((fts_object_t *)track, seqsym_highlightEvents, 1, &a);
  }
}

void track_highlight_time(track_t *track, double time)
{
  if (track_editor_is_open(track))
  {
    fts_atom_t a;
    
    fts_set_float(&a, time);
    fts_client_send_message((fts_object_t *) track,
                            seqsym_highlightEventsAndTime, 1, &a);
  }
}

void track_highlight_events(track_t *track, int n, event_t *event[])
{
  if (track_editor_is_open(track))
  {
    fts_atom_t a[MAX_CLIENT_MSG_ATOMS];
    int        i;
    
    if (n > MAX_CLIENT_MSG_ATOMS)
    {
      fts_post("too many events for client message in track_highlight_events, ignored %d\n", n - MAX_CLIENT_MSG_ATOMS);
      n = MAX_CLIENT_MSG_ATOMS;
    }
    
    for (i = 0; i < n; i++)
      fts_set_object(&a[i], (fts_object_t *) event[i]);
    
    fts_client_send_message((fts_object_t *) track,
                            seqsym_highlightEvents, n, a);
  }
}


void track_highlight_events_and_time(track_t *track, double time, int n, event_t *event[])
{
  if (track_editor_is_open(track))
  {
    fts_atom_t a[MAX_CLIENT_MSG_ATOMS + 1];
    int        i;
    
    if (n > MAX_CLIENT_MSG_ATOMS)
    {
      fts_post("too many events for client message in track_highlight_events_and_time, ignored %d\n", n - MAX_CLIENT_MSG_ATOMS);
      n = MAX_CLIENT_MSG_ATOMS;
    }
    
    fts_set_float(a, time);
    
    for (i = 0; i < n; i++)
      fts_set_object(&a[i+1], (fts_object_t *) event[i]);
    
    fts_client_send_message((fts_object_t *) track, seqsym_highlightEventsAndTime, n + 1, a);
  }
}

void
track_highlight_cluster(track_t *track, event_t *event, event_t *next)
{
  if(track_editor_is_open(track))
  {
    fts_atom_t at[MAX_CLIENT_MSG_ATOMS];
    int ac = 0;
    
    while(event && ( event != next) && ( ac < MAX_CLIENT_MSG_ATOMS))
    {
      fts_set_object(at + ac, (fts_object_t *)event);
      ac++;
      event = event_get_next(event);
    }
    
    if(ac)
      fts_client_send_message((fts_object_t *)track, seqsym_highlightEvents, ac, at);
  }
}

event_t *
track_highlight_and_next(track_t *track, event_t *event)
{
  double time = event_get_time(event);
  
  if(track_editor_is_open(track))
  {
    fts_atom_t at[MAX_CLIENT_MSG_ATOMS];
    int ac = 0;
    
    fts_set_object(at + ac, (fts_object_t *)event);
    ac++;
    event = event_get_next(event);
    
    while(event && ( event_get_time(event) == time) && (ac < MAX_CLIENT_MSG_ATOMS))
    {
      fts_set_object(at + ac, (fts_object_t *)event);
      ac++;
      event = event_get_next(event);
    }
    
    if(ac)
      fts_client_send_message((fts_object_t *)track, seqsym_highlightEvents, ac, at);
  }
  else
  {
    event = event_get_next(event);
    
    while(event && event_get_time(event) == time)
      event = event_get_next(event);
  }
  
  return event;
}

void
track_move_events_at_client(track_t *self, event_t *first, event_t *after)
{
  if(track_editor_is_open(self) && first != NULL)
  {
    event_t *event = first;
    fts_array_t array;
    
    fts_array_init(&array, 0, NULL);
    
    while(event != after && event != NULL)
    {
      fts_array_append_object(&array, (fts_object_t *)event);
      fts_array_append_float(&array, event_get_time(event));
      event = event_get_next(event);
    }
    
    fts_client_send_message((fts_object_t *)self, seqsym_moveEventsFromServer, fts_array_get_size(&array), fts_array_get_atoms(&array));
    
    fts_array_destroy(&array);
  }
}

/******************************************************
*
*  user methods
*
*/

static fts_method_status_t
track_active(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  self->active = fts_get_number_int(at);
  
  if(track_editor_is_open(self))
    fts_client_send_message((fts_object_t *)self, seqsym_active, 1, at);
  
  return fts_ok;
}

static fts_method_status_t
track_ping(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_client_send_message( o, fts_s_ping, 1, at);
  
  return fts_ok;
}

static fts_method_status_t
_track_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t sym = fts_get_symbol(at);
    
    if(sym == seqsym_track)
    {
      track_erase_events(self);
      track_clear_at_client(self);      
    }
    else if(sym == seqsym_markers && self->markers != NULL)
    {
      track_erase_events(self->markers);
      track_clear_at_client(self->markers);
    }
    else
      fts_object_error(o, "invalid argument for message clear");
  }
  else
    track_clear_and_upload(self);
    
  return fts_ok;
}


static fts_method_status_t
_track_merge (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *track = (track_t *) o;
  track_t *merge = (track_t *) fts_get_object(at);
  /* checked by message handler */
  
  track_merge_and_upload(track, merge);
  
  return fts_ok;
}


static fts_method_status_t
_track_insert(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if(ac >= 2 && fts_is_number(at))
  {
    fts_class_t *track_type = track_get_type(self);
    
    if(fts_get_class(at + 1) == track_type)
    {
      double time = fts_get_number_float(at);
      event_t *event = (event_t *)fts_object_create(event_class, 1, at + 1);
      
      /* add event to track and upload it if editor is open */
      track_add_event_and_upload(self, time, event);
    }
    else
      fts_object_error(o, "insert: event type missmatch");
  }
  else
    fts_object_error(o, "insert: bad time value");
  
  return fts_ok;
}


static fts_method_status_t
_track_append(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  double duration = track_get_last_time(self);
  
  if(ac >= 2 && fts_is_number(at))
  {
    fts_class_t *track_type = track_get_type(self);
    
    if(fts_get_class(at + 1) == track_type)
    {
      double time = fts_get_number_float(at);
      event_t *event = (event_t *)fts_object_create(event_class, 1, at + 1);
      
      if(time < duration)
        time = duration;
      
      /* add event to track and upload it if editor is open */
      track_append_event(self, time, event);
    }
    else
      fts_object_error(o, "insert: event type missmatch");
  }
  else
    fts_object_error(o, "insert: bad time value");
  
  return fts_ok;
}

static fts_method_status_t
_track_remove(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  event_t *event = track_get_first(self);
  
  while(event)
  {
    event_t *next = event_get_next(event);
    
    if(fts_atom_equals(event_get_value(event), at))
      track_remove_event(self, event);
    
    event = next;
  }
  
  return fts_ok;
}

static fts_method_status_t
_track_shift(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  double begin = 0.0;
  double end = 2.0 * track_get_duration(self);
  event_t *first = NULL;
  event_t *after = NULL;
  double shift = 0.0;
  
  switch(ac)
  {
    default:
    case 3:
      if(fts_is_number(at + 2))
        end = fts_get_number_float(at + 2);
    case 2:
      if(fts_is_number(at + 1))
        begin = fts_get_number_float(at + 1);
    case 1:
      if(fts_is_number(at))
        shift = fts_get_number_float(at);
    case 0:
      break;
  }
  
  track_segment_get(self, begin, end, &first, &after);
  
  if(first != NULL)
  {
    track_segment_shift(self, first, after, begin, end, shift);
    track_move_events_at_client(self, first, after);
    fts_object_set_state_dirty(o);
  }
  
  return fts_ok;
}

static fts_method_status_t
_track_stretch(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  double begin = 0.0;
  double end = 2.0 * track_get_last_time(self);
  event_t *first = NULL;
  event_t *after = NULL;
  double stretch = 1.0;
  
  switch(ac)
  {
    default:
    case 3:
      if(fts_is_number(at + 2))
        end = fts_get_number_float(at + 2);
    case 2:
      if(fts_is_number(at + 1))
        begin = fts_get_number_float(at + 1);
    case 1:
      if(fts_is_number(at))
        stretch = fts_get_number_float(at);
    case 0:
      break;
  }
  
  track_segment_get(self, begin, end, &first, &after);
  if(first != NULL)
  {
    track_segment_stretch(self, first, after, begin, end, stretch);
    track_move_events_at_client(self, first, NULL);
    fts_object_set_state_dirty(o);
  }
  
  return fts_ok;
}

static fts_method_status_t
_track_quantize(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  double begin = 0.0;
  double end = 2.0 * track_get_last_time(self);
  event_t *first = NULL;
  event_t *after = NULL;
  double quantize = 0.0;
  
  switch(ac)
  {
    default:
    case 3:
      if(fts_is_number(at + 2))
        end = fts_get_number_float(at + 2);
    case 2:
      if(fts_is_number(at + 1))
        begin = fts_get_number_float(at + 1);
    case 1:
      if(fts_is_number(at))
        quantize = fts_get_number_float(at);
    case 0:
      break;
  }
  
  track_segment_get(self, begin, end, &first, &after);
  if(first != NULL)
  {
    track_segment_quantize(self, first, after, begin, end, quantize);
    track_move_events_at_client(self, first, after);
    fts_object_set_state_dirty(o);
  }
  
  return fts_ok;
}

static fts_method_status_t
_track_print(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  fts_class_t *track_type = track_get_type(self);
  int track_size = track_get_size(self);
  event_t *event = track_get_first(self);
  track_t *markers = track_get_markers(self);
  fts_bytestream_t* stream = fts_get_default_console_stream();
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  if(track_size == 0)
  {
    if(track_type == NULL)
      fts_spost(stream, "<empty track>\n");
    else
      fts_spost(stream, "<empty %s track>\n", fts_symbol_name(fts_class_get_name(track_type)));
  }
  else
  {
    if(track_type != NULL)
      fts_spost(stream, "<%s ", fts_symbol_name(fts_class_get_name(track_type)));
    else
      fts_spost(stream, "<");
    
    if(track_size == 1)
    {
      fts_spost(stream, "track of 1 event: @%.7g, ", event_get_time(event));
      fts_spost_atoms(stream, 1, event_get_value(event));
      fts_spost(stream, ">\n", event_get_time(event));
    }
    else
    {
      event_t *marker_event = NULL;
      
      if(markers != NULL)
        marker_event = track_get_first(markers);
      
      fts_spost(stream, "track of %d events>\n", track_size);
      fts_spost(stream, "{\n");
      
      while(event)
      {
        double time = event_get_time(event);
        
        /* print markers first */
        while(marker_event != NULL && event_get_time(marker_event) <= time)
        {
          fts_atom_t *a = event_get_value(marker_event);
          
          fts_spost(stream, "  @%.7g ------", event_get_time(marker_event));
          scomark_spost(fts_get_object(a), stream);
          fts_spost(stream, "-------\n");
          
          marker_event = event_get_next(marker_event);
        }
        
        fts_spost(stream, "  @%.7g, ", time);
        fts_spost_atoms(stream, 1, event_get_value(event));
        fts_spost(stream, "\n");
        
        event = event_get_next(event);
      }
      
      fts_spost(stream, "}\n");
    }
  }
  
  return fts_ok;
}

static fts_method_status_t
_track_upload(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  track_upload(self);
  
  return fts_ok;
}

static fts_method_status_t
track_update_gui(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  fts_atom_t a;
  
  if(self->type != NULL)
  {
    fts_set_symbol(&a, fts_class_get_name(self->type));
    fts_client_send_message(o, fts_s_type, 1, &a);
  }
  
  return fts_ok;
}

static fts_method_status_t
_track_get_duration(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  fts_set_float(ret, track_get_duration(self));
  
  return fts_ok;
}

static fts_method_status_t
_track_get_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  fts_set_int(ret, track_get_size(self));
  
  return fts_ok;
}

/* editor */
static fts_method_status_t
track_set_editor_at_client(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  fts_atom_t a;
  
  if(self->editor == NULL)
  {
    fts_set_object(&a, o);
    self->editor = (track_editor_t *)fts_object_create(track_editor_class, 1, &a);  
    fts_object_refer((fts_object_t *)self->editor);
  }
  
  if(fts_object_has_client((fts_object_t *)self->editor) == 0)
    fts_client_register_object((fts_object_t *)self->editor, fts_object_get_client_id(o));  
  
  fts_set_int(&a, fts_object_get_id((fts_object_t *)self->editor));
  fts_client_send_message(o, seqsym_editor, 1, &a);        
  
  return fts_ok;
}       

static fts_method_status_t
track_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if(self->editor == NULL)
    track_set_editor_at_client(o, NULL, 0, NULL, fts_nix);
  
  track_set_editor_open( self);
  fts_client_send_message( o, fts_s_createEditor, 0, 0);
  track_upload(self);
  track_editor_upload(self->editor);
  fts_client_send_message( o, fts_s_openEditor, 0, 0);
  
  return fts_ok;
}

static fts_method_status_t
track_destroy_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if( track_editor_is_open(self))
    track_set_editor_close( self);
  
  return fts_ok;
}

static fts_method_status_t
track_close_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if( track_editor_is_open(self))
  {
    track_set_editor_close(self);
    fts_client_send_message( o, fts_s_closeEditor, 0, 0);
  }
  
  return fts_ok;
}


/* upload all changed events if editor is visible */
void 
track_update_editor (track_t *self)
{
  if (track_editor_is_open(self))
    track_upload(self);
}


static fts_method_status_t
track_end_paste(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if( track_editor_is_open(self))
    fts_client_send_message( o, seqsym_endPaste, 0, 0);
  
  return fts_ok;
}

static fts_method_status_t
track_end_update(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(track_editor_is_open((track_t *)o))
    fts_client_send_message( o, fts_new_symbol("endUpdate"), 0, 0);
  
  return fts_ok;
}

static fts_method_status_t
track_set_save_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  if(ac == 1) 
  {
    self->save_editor = fts_get_int(at);
    
    if(track_editor_is_open(self))
      fts_client_send_message(o, seqsym_save_editor, 1, at);
    
    if( track_is_in_multitrack(self))
      fts_object_set_dirty(fts_object_get_container(o));
    else
      fts_object_set_dirty(o);
  }
  
  return fts_ok;
}

/******************************************************
*
*  persistence
*
*/

static fts_method_status_t
track_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  fts_symbol_t selector;
  
  if(self->editor == NULL)
  {
    fts_atom_t a;
    fts_set_object(&a, o);
    self->editor = (track_editor_t *)fts_object_create(track_editor_class, 1, &a);
    fts_object_refer((fts_object_t *)self->editor);
  }
  
  selector = fts_get_symbol(at);
  fts_send_message((fts_object_t *)self->editor, selector, ac - 1, at + 1, fts_nix);
  
  return fts_ok;
}

fts_method_status_t
track_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  event_t *event = track_get_first(self);
  track_t *markers = track_get_markers(self);
  fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_class, 0, 0);
  
  fts_object_refer((fts_object_t *)mess);  
  
  /* save events */
  while(event)
  {
    fts_atom_t *value = event_get_value(event);
    
    fts_message_set(mess, fts_s_append, 0, 0);    
    fts_message_append_float(mess, event_get_time(event));
    fts_message_append(mess, 1, value);
    fts_dumper_message_send(dumper, mess);
    
    event = event_get_next(event);
  }
  
  /* dump marker track */
  if (markers != NULL)
  {
    fts_message_set(mess, seqsym_markers, 0, 0);
    
    /* recursively save marker track */
    fts_message_append_object(mess, (fts_object_t *) markers);
    fts_dumper_message_send(dumper, mess);
  }
  
  if(self->save_editor == 1)
  {
    fts_atom_t a;
    fts_set_int(&a, self->save_editor);
    fts_dumper_send(dumper, seqsym_save_editor, 1, &a);
  }
  
  if (self->editor != NULL && self->save_editor)
    track_editor_dump_gui(self->editor, dumper);
  
  fts_object_release((fts_object_t *)mess);  
  
  return fts_ok;
}


static fts_method_status_t
track_add_gui_listener(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if(ac > 0 && fts_is_object(at))
    self->gui_listeners = fts_list_append( self->gui_listeners, at);
  
  return fts_ok;
}

static fts_method_status_t
track_remove_gui_listener(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if(ac > 0 && fts_is_object(at))
    self->gui_listeners = fts_list_remove( self->gui_listeners, at);
  
  return fts_ok;
}

static fts_method_status_t
track_notify_gui_listeners(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  fts_atom_t a;
  fts_iterator_t i;
  fts_list_get_values( self->gui_listeners, &i);
  
  while( fts_iterator_has_more( &i))
  {
    fts_iterator_next( &i, &a);
    fts_send_message( fts_get_object(&a), fts_s_send, ac, at, fts_nix);
  }
  
  return fts_ok;
}




/*******************************************************************************
*
*  foreign class methods
*
*/

#define TRACK_ITER_BEGIN(track)				\
{							\
    event_t *orgevent = track_get_first(track);		\
    int      i        = 0;				\
							\
    while (orgevent != NULL)				\
    {							\
	double      time  = event_get_time(orgevent);	\
	fts_atom_t *value = event_get_value(orgevent);

#define TRACK_ITER_END					\
	orgevent = event_get_next(orgevent);		\
	i++;						\
    }							\
}


static fts_method_status_t
_track_copy_to_mat (fts_object_t *o, fts_symbol_t s, int ac, 
                    const fts_atom_t *at, fts_atom_t *ret)
{
    mat_t   *mat      = (mat_t *)   o;
    track_t *track    = (track_t *) fts_get_object(at);
  
    mat_set_size(mat, track_get_size(track), 2);
  
    TRACK_ITER_BEGIN(track)
        fts_atom_t  a[2];

        fts_set_float(a, time);
        fts_set_void (a+1);
        fts_atom_copy(value, a+1);
        mat_set_element(mat, i, 0, a);
        mat_set_element(mat, i, 1, a+1);
        fts_atom_release(a+1);  /* atom no longer needed, decrement reference count
                                   set by fts_atom_copy in case of an object */
    TRACK_ITER_END

    /* TODO: set column names */
    fts_object_changed(o);
    fts_set_object(ret, o);

    return fts_ok;
}


static fts_method_status_t
_track_copy_to_fmat (fts_object_t *o, fts_symbol_t s, int ac, 
                     const fts_atom_t *at, fts_atom_t *ret)
{
    fmat_t  *fmat     = (fmat_t *)  o;
    track_t *track    = (track_t *) fts_get_object(at);

    fmat_set_size(fmat, track_get_size(track), 2);

    if (track_get_type(track) == fts_float_class)
    {
	TRACK_ITER_BEGIN(track)
	    fmat_set_element(fmat, i, 0, time);
	    fmat_set_element(fmat, i, 1, fts_get_float(value));
	TRACK_ITER_END
    }
    else if (track_get_type(track) == fts_int_class)
    {
	TRACK_ITER_BEGIN(track)
	    fmat_set_element(fmat, i, 0, time);
	    fmat_set_element(fmat, i, 1, fts_get_int(value));
	TRACK_ITER_END
    }
    else
    {   /* do something which makes sense: get times */
	TRACK_ITER_BEGIN(track)
	    fmat_set_element(fmat, i, 0, time);
	    fmat_set_element(fmat, i, 1, 0);
	TRACK_ITER_END
    }

    /* TODO: set column names */
    fts_object_changed(o);
    fts_set_object(ret, o);

    return fts_ok;
}


static fts_method_status_t
_track_copy_to_fvec (fts_object_t *o, fts_symbol_t s, int ac, 
                     const fts_atom_t *at, fts_atom_t *ret)
{
    fvec_t  *fvec     = (fvec_t *)  o;
    track_t *track    = (track_t *) fts_get_object(at);
    event_t *orgevent = track_get_first(track);
    float   *ptr;
    int      size, stride;
    int      i, j;
  
    fvec_get_vector(fvec, &ptr, &size, &stride);
    
    /* copy not more events than underlying matrix size
       TBD: resize fvec if less events than size??? */
    if (track_get_type(track) == fts_float_class  ||
	track_get_type(track) == fts_int_class)
    {
	for (i = 0, j = 0; i < size  &&  orgevent != NULL; i++, j += stride)
	{
	    ptr[j]  = fts_get_number_float(event_get_value(orgevent));
	    
	    orgevent = event_get_next(orgevent);
	}
    }
    else
    {
	fmat_set_const((fmat_t *) fvec, 0);  /* yes, it works on fvec, too */
    }

    fts_object_changed(o);
    fts_set_object(ret, o);

    return fts_ok;
}




/******************************************************
*
*  class
*
*/
static fts_method_status_t
track_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  self->active = 1;
  
  self->first = 0;
  self->last = 0;
  self->size = 0;
  self->type = NULL;
  
  self->gui_listeners = 0;
  
  self->editor = NULL;
  self->save_editor = 0;
  
  self->markers = NULL;
  
  if(ac > 0)
  {
    if(fts_is_symbol(at))
    {
      fts_symbol_t class_name = fts_get_symbol(at);
      
      if(class_name != fts_s_void)
      {
        if(class_name == fts_s_int)
          self->type = fts_int_class;
        else if(class_name == fts_s_float)
          self->type = fts_float_class;
        else if(class_name == fts_s_symbol)
          self->type = fts_symbol_class;
        else
          self->type = fts_get_class_by_name(class_name);
        
        if(self->type == NULL)
          fts_object_error(o, "cannot create track of %s", fts_symbol_name(class_name));
      }
    }
    else
      fts_object_error(o, "bad argument");
  }
  
  if(self->type == NULL)
    self->type = fts_int_class;
  
  return fts_ok;
}

static fts_method_status_t
track_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  track_erase_events(self);
  
  if(self->markers != NULL)
    fts_object_release((fts_object_t *)self->markers);
  
  if(self->editor != NULL)
    fts_object_release((fts_object_t *)self->editor);
  
  return fts_ok;
}

static void
track_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(track_t), track_init, track_delete);
  
  fts_class_set_copy_function(cl, track_copy_function);
  fts_class_set_description_function(cl, track_description_function);
  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, seqsym_save_editor, track_set_save_editor);
  fts_class_message_varargs(cl, seqsym_set_editor, track_set_editor_at_client);
  
  fts_class_message_varargs(cl, fts_s_dump_state, track_dump_state);
  fts_class_message_varargs(cl, fts_s_update_gui, track_update_gui);
  
  /* persistence compatibility */
  fts_class_message_varargs(cl, seqsym_add_event, track_compatible_add_event_from_file);
  fts_class_message_varargs(cl, seqsym_add_marker, track_compatible_add_marker_from_file);
  fts_class_message_varargs(cl, seqsym_dump_mess, track_compatible_event_dump_mess);
  
  fts_class_message_varargs(cl, seqsym_editor, track_editor);
  
  fts_class_message_varargs(cl, fts_s_upload, _track_upload);
  
  fts_class_message_varargs(cl, fts_s_print, _track_print);
  
  fts_class_message_varargs(cl, seqsym_endPaste, track_end_paste);
  fts_class_message_varargs(cl, fts_new_symbol("endUpdate"), track_end_update);
  
  fts_class_message_varargs(cl, fts_s_openEditor, track_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, track_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, track_close_editor);
  
  fts_class_message_varargs(cl, fts_s_add_gui_listener, track_add_gui_listener);
  fts_class_message_varargs(cl, fts_s_remove_gui_listener, track_remove_gui_listener);
  fts_class_message_varargs(cl, fts_s_notify_gui_listeners, track_notify_gui_listeners);
  
  fts_class_message_number(cl, seqsym_active, track_active);
  
  fts_class_message_varargs(cl, fts_s_clear, _track_clear);
  fts_class_message(cl, fts_new_symbol("merge"), cl, _track_merge);
  
  fts_class_message_varargs(cl, seqsym_insert, _track_insert);  
  fts_class_message_varargs(cl, fts_s_append, _track_append);
  fts_class_message_varargs(cl, seqsym_remove, _track_remove);
  
  fts_class_message_varargs(cl, fts_s_ping, track_ping);
  
  /* fts_class_message_number(cl, fts_s_get_element, track_get_element); */
  
  fts_class_message_varargs(cl, seqsym_shift, _track_shift);
  fts_class_message_varargs(cl, seqsym_stretch, _track_stretch);
  fts_class_message_varargs(cl, seqsym_quantize, _track_quantize);
  fts_class_message_void(cl, fts_new_symbol("duration"), _track_get_duration);
  fts_class_message_void(cl, fts_new_symbol("size"), _track_get_size);
  
  /* markers */
  fts_class_message_void(cl, seqsym_markers, _track_get_markers);
  fts_class_message_varargs(cl, seqsym_markers, _track_set_markers);
  fts_class_message_void(cl, fts_new_symbol("make_bars"), _track_make_bars);
  fts_class_message_void(cl, fts_new_symbol("renumber_bars"), _track_renumber_bars);
  fts_class_message_varargs(cl, fts_new_symbol("append_bar"), _track_append_bar);
  fts_class_message_varargs(cl, fts_new_symbol("make_trill"), _track_make_trill);
  fts_class_message_varargs(cl, fts_new_symbol("collapse_markers"), _track_collapse_markers);
  fts_class_message_varargs(cl, seqsym_marker, _track_append_marker);
  
  fts_class_inlet_thru(cl, 0);
  
  fts_class_message_varargs(cl, seqsym_addEvent, track_add_event_from_client);
  fts_class_message_varargs(cl, seqsym_makeEvent, track_make_event_from_client);
  fts_class_message_varargs(cl, seqsym_removeEvents, track_remove_events_from_client);
  fts_class_message_varargs(cl, seqsym_moveEvents, track_move_events_from_client);
  
  fts_class_message_varargs(cl, fts_new_symbol("insert_marker"), track_insert_marker_from_client);
  
  
  /*
   * track class documentation
   */
  
  fts_class_doc(cl, seqsym_track, "[<sym: type>]", "sequence of time-tagged values");
  fts_class_doc(cl, fts_s_clear, NULL, "erase all events");
  fts_class_doc(cl, seqsym_insert, "<num: time> <any: value>", "insert event at given time");
  fts_class_doc(cl, fts_new_symbol("duration"), NULL, "get duration");
  fts_class_doc(cl, fts_new_symbol("size"), NULL, "get duration");
  fts_class_doc(cl, fts_s_print, NULL, "print");


  /*
   *  add some methods to foreign classes from the data package
   *  (do it here to avoid dependencies of data on sequence)
   */

  fts_class_instantiate(mat_class);
  fts_class_instantiate(fmat_class);
  fts_class_instantiate(fvec_class);

  fts_class_message(mat_class,  fts_s_set, track_class, _track_copy_to_mat);
  fts_class_message(fmat_class, fts_s_set, track_class, _track_copy_to_fmat);
  fts_class_message(fvec_class, fts_s_set, track_class, _track_copy_to_fvec);

  fts_class_doc(mat_class,  fts_s_set, "<track: src>", "copy track to mat with 2 columns (time, event)");
  fts_class_doc(fmat_class, fts_s_set, "<track: src>", "copy float track to fmat with 2 columns (time, value)");
  fts_class_doc(fvec_class, fts_s_set, "<track: src>", "copy float track values to fvec");
}  



FTS_MODULE_INIT(track)
{
  track_class = fts_class_install(seqsym_track, track_instantiate);
}
