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
#include <fts/packages/data/data.h>
#include <fts/packages/sequence/sequence.h>
#include "seqmidi.h"
#include "seqmess.h"

#define TRACK_BLOCK_SIZE 256
#define MAX_CLIENT_MSG_ATOMS 64		/* max. number of atoms to send */

fts_class_t *track_class = 0;

static void track_upload_event(track_t *this, event_t *event, fts_array_t *temp_array);

/******************************************************
*
*  preset dumper utility
*
*/

typedef struct
{
  fts_dumper_t head;
  fts_dumper_t *dumper;
  fts_symbol_t dump_mess;
} content_dumper_t;

#define content_dumper_set_dump_mess(d, m) ((d)->dump_mess = (m))
#define content_dumper_set_dumper(d, x) ((d)->dumper = (x))

static void
content_dumper_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  content_dumper_t *this = (content_dumper_t *)o;
  fts_message_t *mess = fts_dumper_message_new(this->dumper, this->dump_mess);
  
  fts_message_append_symbol(mess, s);
  fts_message_append(mess, ac, at);
  
  fts_dumper_message_send(this->dumper, mess);
}

static void
content_dumper_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  content_dumper_t *this = (content_dumper_t *)o;
  
  /* init super */
  fts_dumper_init((fts_dumper_t *)this, content_dumper_send);
  
	if(ac>0)
	{
		this->dumper = (fts_dumper_t *)fts_get_object(at);
		fts_object_refer(this->dumper);
	}
}
static void
content_dumper_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  content_dumper_t *this = (content_dumper_t *)o;
  
  fts_object_release(this->dumper);
  
  /* delete super */
  fts_dumper_destroy((fts_dumper_t *)this);
}

static void
content_dumper_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(content_dumper_t), content_dumper_init, content_dumper_delete);
}

static fts_class_t *content_dumper_class = NULL;
static content_dumper_t *content_dumper = NULL;

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
    event->next = 0;
    event->prev = 0;
    
    track->first = event;
    track->last = event;
    
    track->size = 1;
  }
  else
  {
    event->next = track->first;
    event->prev = 0;
    
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
    event->next = 0;
    event->prev = 0;
    
    track->first = event;
    track->last = event;
    
    track->size = 1;
  }
  else
  {
    event->prev = track->last;
    event->next = 0;
    
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
  if(event->prev == 0)
  {
    if(event->next == 0)
    {
      track->first = 0;
      track->last = 0;
    }
    else
    {
      /* event is first of track */
      track->first = event->next;
      track->first->prev = 0;
    }
  }
  else if(event->next == 0)
  {
    /* event is last of track */
    track->last = event->prev;
    track->last->next = 0;
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
      value = at[0];
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
              fts_send_message(obj, prop, 1, at + i + 1);
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
  
  event_set_track(event, track);
  event_set_time(event, time);
}

void
track_add_event_and_upload(track_t *track, double time, event_t *event)
{
  fts_array_t temp_array;
  
  track_add_event(track, time, event);
  
  if(track_editor_is_open(track))
  {
    fts_array_init(&temp_array, 0, 0);
    track_upload_event( track, event, &temp_array);
    fts_array_destroy(&temp_array);
  }
  
  fts_object_set_state_dirty((fts_object_t *)track);
}

void
track_append_event(track_t *track, double time, event_t *event)
{
  append_event(track, event);
  fts_object_refer(event);
  
  event_set_track(event, track);
  event_set_time(event, time);
}

void
track_remove_event(track_t *track, event_t *event)
{
  cutout_event(track, event);
  
  event->next = event->prev = 0;
  
  fts_object_release(event);
}

void
track_remove_event_and_upload(track_t *track, event_t *event)
{
  fts_atom_t at;
  
  fts_set_object(&at, (fts_object_t *) event);
  
  fts_client_send_message((fts_object_t *) track, seqsym_removeEvents, 1, &at);
  
  track_remove_event(track, event);
  
  fts_object_set_state_dirty((fts_object_t *)track);
}

void
track_move_event(track_t *track, event_t *event, double time)
{
  event_t *next = event_get_next(event);
  event_t *prev = event_get_prev(event);
  
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
track_clear(track_t *track)
{
  event_t *event = track_get_first(track);
  
  while(event)
  {
    event_t *next = event_get_next(event);
    
    event->next = event->prev = 0;
    fts_object_release((fts_object_t *)event);
    
    event = next;
  }
  
  /* merge track is empty */
  track->first = 0;
  track->last = 0;
  track->size = 0;
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

static void
track_copy(track_t *org, track_t *copy)
{
  event_t *event = track_get_first(org);
  
  track_clear(copy);
  
  while(event)
  {
    double time = event_get_time(event);
    fts_atom_t *value = event_get_value(event);
    fts_atom_t a;
    event_t *event;
    
    fts_atom_copy(value, &a);
    event = (event_t *)fts_object_create(event_class, 1, &a);
    track_append_event(copy, time, event);
    
    event = event_get_next(event);
  }
}

static void
track_copy_function(const fts_atom_t *from, fts_atom_t *to)
{
  track_copy((track_t *)fts_get_object(from), (track_t *)fts_get_object(to));
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
    
    track_set_markers(track, markers);
  }
  
  return markers;
}

scomark_t *
track_insert_marker(track_t *track, double time, fts_symbol_t type)
{
  track_t *markers = track_get_or_make_markers(track);
  scomark_t *scomark;
  event_t *event;
  fts_atom_t a;
  
  /* create a scomark */
  fts_set_symbol(&a, type);
  scomark = (scomark_t *)fts_object_create(scomark_class, 1, &a);
  
  /* create a new event with the scomark */
  fts_set_object(&a, (fts_object_t *)scomark);
  event = (event_t *)fts_object_create(event_class, 1, &a);
  
  track_add_event(markers, time, event);
  
  return scomark;
}

static void 
track_upload_markers(track_t *this)
{
	if(this->markers != NULL)
  {
    if(!fts_object_has_id((fts_object_t *)this->markers))
    {
      fts_atom_t a[2];
      fts_class_t *markers_type = track_get_type(this->markers);
		
      fts_client_register_object((fts_object_t *)this->markers, fts_object_get_client_id((fts_object_t *)this));
      
      fts_set_int(a, fts_object_get_id((fts_object_t *)this->markers));
		
      if(markers_type != NULL)
        fts_set_symbol(a + 1, fts_class_get_name(markers_type));
      else
        fts_set_symbol(a + 1, fts_s_void);     		
		
      fts_client_send_message((fts_object_t *)this, seqsym_markers, 2, a);
    }
    
		fts_send_message((fts_object_t *)this->markers, fts_s_upload, 0, NULL);
	}
}
  
/******************************************************
 *
 *  client calls
 *
 */
/* create new event and upload by client request */
static void
track_add_event_from_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event = track_event_create(ac - 1, at + 1);
	
  if(event)
    track_add_event_and_upload( this, time, event);
}

static void
track_make_event_from_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event = track_event_create(ac - 1, at + 1);
	
  /* add event to track */
  if(event != NULL)
    track_add_event(this, time, event);
	
  fts_object_set_state_dirty((fts_object_t *)this);
}

static void
track_remove_events_from_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  int i;
	
  /*  remove event objects from client */
  fts_client_send_message((fts_object_t *)this, seqsym_removeEvents, ac, at);
	
  for(i=0; i<ac; i++)
  {
    fts_object_t *event = fts_get_object(at + i);
    track_remove_event(this, (event_t *)event);
  }
	
  fts_object_set_state_dirty((fts_object_t *)this);
}

/* move event by client request */
static void
track_move_events_from_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  int i;
	
  for(i=0; i<ac; i+=2)
  {
    event_t *event = (event_t *)fts_get_object(at + i);
    float time = fts_get_float(at + i + 1);
		
    track_move_event(this, (event_t *)event, time);
  }
	
  fts_client_send_message((fts_object_t *)this, seqsym_moveEvents, ac, at);
	
  fts_object_set_state_dirty((fts_object_t *)this);
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
    
    while(time > event_get_time(event))
      event = event_get_next(event);
    
    return event;
  }
  else
    return 0;
}

/* returns next event after given time */
event_t *
track_get_next_by_time(track_t *track, double time)
{
  if(track_get_size(track) > 0 && time < event_get_time(track->last))
  {
    event_t *event = track->first;
    
    while(time >= event_get_time(event))
      event = event_get_next(event);
    
    return event;
  }
  else
    return 0;
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
    
    while(time >= event_get_time(event))
      event = event_get_next(event);
    
    return event;
  }
  else
    return 0;
}



/******************************************************
*
*  client calls: highlighting
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

/******************************************************
*
*  user methods
*
*/

static void
track_active(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  this->active = fts_get_number_int(at);
  
  if(track_editor_is_open(this))
    fts_client_send_message((fts_object_t *)this, seqsym_active, 1, at);
}

static void
track_clear_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  if(track_editor_is_open(this))
    fts_client_send_message(o, fts_s_clear, 0, 0);
  
  track_clear(this);
  
  if(this->markers)
    track_clear(this->markers);
  
  fts_object_set_state_dirty(o);
}

static void
track_insert(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  if(ac >= 2 && fts_is_number(at))
  {
    fts_class_t *track_type = track_get_type(this);
    
    if(track_type == NULL || fts_get_class(at + 1) == track_type)
    {
      double time = fts_get_number_float(at);
      event_t *event = (event_t *)fts_object_create(event_class, 1, at + 1);
      
      /* add event to track */
      track_add_event(this, time, event);
    }
    else
      fts_object_error(o, "insert: event type missmatch");
  }
  else
    fts_object_error(o, "insert: bad time value");
}

static void
track_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  event_t *event = track_get_first(this);
  
  while(event)
  {
    event_t *next = event_get_next(event);
    
    if(fts_atom_equals(event_get_value(event), at))
      track_remove_event(this, event);
    
    event = next;
  }
}

static void
track_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  fts_class_t *track_type = track_get_type(this);
  
  if(track_type == NULL)
    fts_spost(stream, "<track>");
  else
  {
    fts_spost(stream, "<track ");
    fts_spost_symbol(stream, fts_class_get_name(track_type));
    fts_spost(stream, ">");
  }
}

static void
track_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  fts_class_t *track_type = track_get_type(this);
  int track_size = track_get_size(this);
  event_t *event = track_get_first(this);
  track_t *markers = track_get_markers(this);
  
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
          
          fts_spost(stream, "  @%.7g ------", time);
          scomark_post(fts_get_object(a), 0, NULL, ac, at);
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
}

/******************************************************
*
*  upload
*
*/

static void
track_upload_property_list(track_t *this, fts_array_t *temp_array)
{
  fts_class_t *type = track_get_type(this);
  
  if(type)
  {
    if(fts_class_is_primitive(type))
    {
      fts_atom_t a[2];
      
      fts_set_symbol(a, fts_s_value);
      fts_set_symbol(a + 1, fts_class_get_name(type));
      
      fts_client_send_message((fts_object_t *)this, seqsym_properties, 2, a);
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
        (*method_get_property_list)((fts_object_t *)this, 0, 0, 1, &a);
        
        size = fts_array_get_size(temp_array);
        atoms = fts_array_get_atoms(temp_array);
        
        /* send properties to client */
        if(size > 0)
          fts_client_send_message((fts_object_t *)this, seqsym_properties, size, atoms);
      }
    }
  }
}

static void
track_upload_event(track_t *this, event_t *event, fts_array_t *temp_array)
{
  fts_class_t *type = event_get_type(event);
  fts_atom_t a[4];
  
  if(!fts_object_has_id((fts_object_t *)event))
  {
    fts_client_register_object((fts_object_t *)event, fts_object_get_client_id((fts_object_t *)this));
    
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
        
        (*method_append_properties)( fts_get_object( event_get_value( event)), 0, 0, 1, &a);
      }
      
      size = fts_array_get_size(temp_array);
      atoms = fts_array_get_atoms(temp_array);
      
      /* send properties to client */
      if(size > 0)
        fts_client_send_message((fts_object_t *)this, seqsym_addEvents, size, atoms);
    }
    else
    {
      fts_set_int(a + 0, fts_object_get_id((fts_object_t *)event));
      fts_set_float(a + 1, event_get_time(event));
      fts_set_symbol(a + 2, fts_get_class_name(&event->value));
      a[3] = event->value;
      fts_client_send_message((fts_object_t *)this, seqsym_addEvents, 4, a);
    }
  }
}

static void
track_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t name = track_get_name(this);
  event_t *event = track_get_first(this);
  fts_array_t temp_array;
  fts_atom_t a;
  
  fts_array_init(&temp_array, 0, 0);
  
  fts_set_int(&a, track_get_size( this));
  fts_client_send_message((fts_object_t *)this, fts_s_start_upload, 1, &a);
  
  /* set track name */
  if(name)
  {
    fts_set_symbol(&a, name);
    fts_client_send_message((fts_object_t *)this, seqsym_setName, 1, &a);
  }
  
  fts_set_int(&a, track_is_active(this));
  fts_client_send_message((fts_object_t *)this, seqsym_active, 1, &a);
  
  /* upload array of properties and types */
  track_upload_property_list(this, &temp_array);
  
  while(event)
  {
    /* create event at client */
    if(!fts_object_has_id((fts_object_t *)event))
      track_upload_event(this, event, &temp_array);
    
    event = event_get_next(event);
  }
	
	if( this->markers != NULL /*&& !fts_object_has_id((fts_object_t *)this->markers)*/)
		track_upload_markers(this);
	
  fts_client_send_message((fts_object_t *)this, fts_s_end_upload, 0, 0);
  
  fts_array_destroy(&temp_array);
}

static void
track_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_atom_t a;
  
	if(this->type != NULL)
  {
    fts_set_symbol(&a, fts_class_get_name(this->type));
    fts_client_send_message(o, fts_s_type, 1, &a);
  }
	
  if(this->save_editor != 0)
  {
    fts_set_int(&a, this->save_editor); 
    fts_client_send_message(o, seqsym_save_editor, 1, &a);
  }
	
	if(this->editor!=NULL)
	{
		if(!fts_object_has_id((fts_object_t *)this->editor))
		{
			fts_atom_t a;
			fts_client_register_object((fts_object_t *)this->editor, fts_object_get_client_id(o));	
			
			fts_set_int(&a, fts_object_get_id((fts_object_t *)this->editor));
			fts_client_send_message(o, seqsym_editor, 1, &a);
		}
    
		track_editor_upload(this->editor);
	}
}

static void
_track_get_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  fts_return_float(track_get_duration(this));
}

static void
_track_get_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  fts_return_int(track_get_size(this));
}

#define MARKERS_BAR_TOLERANCE 20.0 /* tolerance for bars */
#define MARKERS_BAR_EPSILON 0.1

static void
_track_make_bars(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  track_t *markers = this->markers;
  
  if(markers != NULL)
  {
    event_t *marker_event = track_get_first(markers);
    scomark_t *scomark = NULL;
    int numerator = 0;
    int denominator = 0;
    double tempo = 0.0;
    double time = 0.0;
    double bar_duration = 0.0;
    double next_bar_time = 0.0;
    
    while(marker_event != NULL)
    {
      /* get beginning of meter */
      while(marker_event != NULL && next_bar_time == 0.0)
      {
        scomark = (scomark_t *)fts_get_object(event_get_value(marker_event));
        time = event_get_time(marker_event);
        tempo = scomark_get_tempo(scomark);
        
        if(scomark_is_bar(scomark) && scomark_get_meter_num(scomark) > 0)
        {
          numerator = scomark_get_meter_num(scomark);
          denominator = scomark_get_meter_den(scomark);
          
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
        
        if(scomark_get_tempo(scomark) > 0.0)
          tempo = scomark_get_tempo(scomark);
        
        if(scomark_is_bar(scomark))
        {
          /* current is bar */
          if(scomark_get_meter_num(scomark) > 0)
          {
            numerator = scomark_get_meter_num(scomark);
            denominator = scomark_get_meter_den(scomark);
          }
          
          /* fill with bars until given bar */
          while(time - next_bar_time > MARKERS_BAR_TOLERANCE)
          {
            track_insert_marker(this, next_bar_time, seqsym_bar);
            next_bar_time += bar_duration;
          }
          
          if(next_bar_time - time < MARKERS_BAR_TOLERANCE)
            fts_post("warning: track make-bars: @ %g, bar short by %g msec", time, next_bar_time - time);
          
          /* next given bar */
          bar_duration = ((double)numerator * 240000.0) / (tempo * (double)denominator);
          next_bar_time = time + bar_duration;
        }
        else
        {
          /* current is marker inside bar (tempo & co) */
          while(time - next_bar_time > MARKERS_BAR_EPSILON)
          {
            track_insert_marker(this, next_bar_time, seqsym_bar);
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
    }
  
    /* fill until end of track */
    while(next_bar_time <= track_get_duration(this) && next_bar_time > 0.0)
    {
      track_insert_marker(this, next_bar_time, seqsym_bar);
      next_bar_time += ((double)numerator * 240000.0) / (tempo * (double)denominator);
    }      
    
    if(track_editor_is_open(this))
      fts_send_message((fts_object_t *)this->markers, fts_s_upload, 0, NULL);
  }
}

static void
_track_get_markers(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  fts_return_object((fts_object_t *)track_get_markers(this));
}

/******************************************************
*
*  MIDI files
*
*/

static void
track_import_midifile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t name = fts_get_symbol(at);
  fts_midifile_t *file = fts_midifile_open_read(name);
  
  if(file)
  {
    int size;
    char *error;
    
    track_clear_method(o, 0, NULL, 0, NULL);
    
    size = track_import_from_midifile(this, file);
    error = fts_midifile_get_error(file);
    
    if(error)
      fts_object_error(o, "import: read error in \"%s\" (%s)", fts_symbol_name(name), error);
    else if(size <= 0)
      fts_object_error(o, "import: couldn't get any data from \"%s\"", fts_symbol_name(name));
    
    fts_midifile_close(file);
    
    if(track_editor_is_open(this))
      track_upload(o, 0, NULL, 0, NULL);
  }
  else
    fts_object_error(o, "import: cannot open \"%s\"", fts_symbol_name(name));
}

static void
track_import_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_open_dialog(o, seqsym_import_midifile, fts_new_symbol("Open standard MIDI file"));
}

static void
track_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_class_t *type = track_get_type(this);
  
  if(type == fts_midievent_type || type == scoob_class || type == fts_int_class || type == NULL)
    track_import_midifile(o, 0, 0, 1, at);
  else
    fts_object_error(o, "import: cannot import MIDI file to track of type %s", fts_symbol_name(fts_class_get_name(type)));
}

static void
track_import_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_class_t *type = track_get_type(this);
  
  if(type == fts_midievent_type || type == scoob_class || type == fts_int_class || type == NULL)
    track_import_midifile_dialog(o, 0, 0, 0, 0);
  else
    fts_object_error(o, "import: cannot import MIDI file to track of type %s", fts_symbol_name(fts_class_get_name(type)));
}

static void
track_export_midifile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t name = fts_get_symbol(at);
  fts_midifile_t *file = fts_midifile_open_write(name);
  
  if(file)
  {
    int size = track_export_to_midifile(this, file);
    char *error = fts_midifile_get_error(file);
    
    if(error)
      fts_object_error(o, "export: write error in \"%s\" (%s)", error, fts_symbol_name(name));
    else if(size <= 0)
      fts_object_error(o, "export: couldn't write any data to \"%s\"", fts_symbol_name(name));
    
    fts_midifile_close(file);
  }
  else
    fts_object_error(o, "export: cannot open \"%s\"", fts_symbol_name(name));
}

static void
track_export_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t track_name = track_get_name(this);
  fts_symbol_t default_name;
  char str[1024];
  
  snprintf(str, 1024, "%s.mid", track_name? fts_symbol_name(track_name): "untitled");
  default_name = fts_new_symbol(str);
  fts_object_save_dialog(o, seqsym_export_midifile, fts_new_symbol("Save standard MIDI file"), fts_project_get_dir(), default_name);
}

static void
track_export_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac == 0)
    track_export_midifile_dialog(o, 0, 0, 0, 0);
}

static void
track_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_export_midifile(o, 0, 0, 1, at);
}

static void
track_set_editor_at_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
	track_t *this = (track_t *)o;
	fts_atom_t a;
	
	if(this->editor == NULL)
	{
		fts_set_object(&a, o);
		this->editor = (track_editor_t *)fts_object_create(track_editor_class, 1, &a);	
    fts_object_refer((fts_object_t *)this->editor);
	}
  
	if(!fts_object_has_id((fts_object_t *)this->editor))
		fts_client_register_object((fts_object_t *)this->editor, fts_object_get_client_id(o));	
		
	fts_set_int(&a, fts_object_get_id((fts_object_t *)this->editor));
	fts_client_send_message(o, seqsym_editor, 1, &a);		
}	

static void
track_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
	track_t *this = (track_t *)o;
	
	if(this->editor == NULL)
		track_set_editor_at_client(o, 0, 0, 0, 0);
	
  track_set_editor_open( this);
  fts_client_send_message( o, fts_s_openEditor, 0, 0);
  track_upload(o, 0, 0, 0, 0);
	track_editor_upload(this->editor);
}

static void
track_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
	
	if( track_editor_is_open(this))
		track_set_editor_close( this);
}

static void
track_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  if( track_editor_is_open(this))
  {
    track_set_editor_close(this);
    fts_client_send_message( o, fts_s_closeEditor, 0, 0);
  }
}

static void
track_end_paste(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  if( track_editor_is_open(this))
    fts_client_send_message( o, seqsym_endPaste, 0, 0);
}

static void
track_end_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(track_editor_is_open((track_t *)o))
    fts_client_send_message( o, fts_new_symbol("endUpdate"), 0, 0);
}

static void
track_set_save_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
	track_t *this = (track_t *)o;
	if(ac == 1) 
	{
		this->save_editor = fts_get_int(at);
		
		if(track_editor_is_open(this))
		  fts_client_send_message(o, seqsym_save_editor, 1, at);
		
		fts_object_set_dirty(o);
	}
}

/******************************************************
*
*  bmax files
*
*/

void
track_event_dump_mess(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  /* send dump message to last loaded object */
  if(this->load_obj != NULL)
  {
    fts_symbol_t selector = fts_get_symbol(at);
    fts_send_message(this->load_obj, selector, ac - 1, at + 1);
  }
}

void
track_editor_dump_mess(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t selector;
  
  if(this->editor == NULL)
  {
    fts_atom_t a;
    fts_set_object(&a, o);
    this->editor = (track_editor_t *)fts_object_create(track_editor_class, 1, &a);
    fts_object_refer((fts_object_t *)this->editor);
  }
  
  selector = fts_get_symbol(at);
  fts_send_message((fts_object_t *)this->editor, selector, ac - 1, at + 1);
}

void
track_add_event_from_file(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  double time = fts_get_float(at);
  event_t *event = track_event_create(ac - 1, at + 1);
  
  /* add event to track (strictly ordered by time) */
  if(event != NULL)
  {
    fts_atom_t *a = event_get_value(event);
    
    if(fts_is_object(a))
      this->load_obj = fts_get_object(a);

    track_append_event(this, time, event);
  }
}

void
track_add_marker_from_file(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  track_t *markers = track_get_or_make_markers(this);
  double time = fts_get_float(at);
  event_t *marker_event = track_event_create(ac - 1, at + 1);
  
  /* add event to track (strictly ordered by time) */
  if(marker_event != NULL)
  {
    fts_atom_t *a = event_get_value(marker_event);
    
    if(fts_is_object(a))
    {
      this->load_obj = fts_get_object(a);
      track_append_event(markers, time, marker_event);
    }
  }
}

void
track_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  event_t *event = track_get_first(this);
  fts_atom_t dumper_atom;
  
	/* to be sent to event */
  fts_set_object(&dumper_atom, (fts_object_t *)content_dumper);
	
	content_dumper_set_dumper(content_dumper, dumper);
  content_dumper_set_dump_mess(content_dumper, seqsym_dump_mess);
		
  /* save events */
  while(event)
  {
    fts_atom_t *value = event_get_value(event);
    
    if(fts_is_object(value))
    {
      fts_object_t *obj = fts_get_object(value);
      fts_symbol_t class_name = fts_object_get_class_name(obj);
      
      if(class_name != NULL)
      {
        fts_message_t *mess = fts_dumper_message_new(dumper, seqsym_add_event);
        
        /* save event time and class name */
        fts_message_append_float(mess, event_get_time(event));
        fts_message_append_symbol(mess, class_name);
        fts_dumper_message_send(dumper, mess);
        
        /* dump object messages */
        fts_send_message_varargs(obj, fts_s_dump_state, 1, &dumper_atom);
      }
    }
    else
    {
      fts_message_t *mess = fts_dumper_message_new(dumper, seqsym_add_event);
      
      /* save event time and (primitive) value */
      fts_message_append_float(mess, event_get_time(event));
      fts_message_append(mess, 1, value);
      fts_dumper_message_send(dumper, mess);
    }
    
    event = event_get_next(event);
  }

  /* save markers */
  if(track_get_markers(this) != NULL)
  {
    track_t *markers = track_get_markers(this);
    event_t *marker_event = track_get_first(markers);
    
    while(marker_event)
    {
      fts_atom_t *value = event_get_value(marker_event);      
      fts_object_t *obj = fts_get_object(value);
      fts_message_t *mess = fts_dumper_message_new(dumper, seqsym_add_marker);
      
      /* save event time and class name */
      fts_message_append_float(mess, event_get_time(marker_event));
      fts_message_append_symbol(mess, seqsym_scomark);
      fts_dumper_message_send(dumper, mess);
      
      /* dump object messages */
      fts_send_message_varargs(obj, fts_s_dump_state, 1, &dumper_atom);
      
      marker_event = event_get_next(marker_event);
    }
  }
}

static void
track_dump_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
	
	if(this->save_editor == 1)
	{
		fts_atom_t a;
		fts_set_int(&a, this->save_editor);
		fts_dumper_send(dumper, seqsym_save_editor, 1, &a);
	}
	
	if(this->editor != NULL && this->save_editor)
  {
    content_dumper_set_dumper(content_dumper, dumper);
    content_dumper_set_dump_mess(content_dumper, seqsym_editor);
    
		track_editor_dump_gui(this->editor, (fts_dumper_t *)content_dumper);
  }  
}

static void
track_add_gui_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  if(ac > 0 && fts_is_object(at))
    this->gui_listeners = fts_list_append( this->gui_listeners, at);
}

static void
track_remove_gui_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  if(ac > 0 && fts_is_object(at))
    this->gui_listeners = fts_list_remove( this->gui_listeners, at);
}

static void
track_notify_gui_listeners(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_atom_t a;
  fts_iterator_t i;
  fts_list_get_values( this->gui_listeners, &i);
  
  while( fts_iterator_has_more( &i))
  {
    fts_iterator_next( &i, &a);
    fts_send_message_varargs( fts_get_object(&a), fts_s_send, ac, at);
  }
}

/******************************************************
*
*  class
*
*/
void
track_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
	
  this->active = 1;
  
  this->first = 0;
  this->last = 0;
  this->size = 0;
  this->type = NULL;
  
  this->gui_listeners = 0;
  
	this->editor = NULL;
	this->save_editor = 0;
  
  this->markers = NULL;
	
  if(ac > 0)
  {
    if(fts_is_symbol(at))
    {
      fts_symbol_t class_name = fts_get_symbol(at);
      
      if(class_name != fts_s_void)
      {
        if(class_name == fts_s_int)
          this->type = fts_int_class;
        else if(class_name == fts_s_float)
          this->type = fts_float_class;
        else if(class_name == fts_s_symbol)
          this->type = fts_symbol_class;
        else
          this->type = fts_get_class_by_name(class_name);
        
        if(this->type == NULL)
          fts_object_error(o, "cannot create track of %s", fts_symbol_name(class_name));
      }
    }
    else
      fts_object_error(o, "bad argument");
  }
}

void
track_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  track_clear(this);
  
  if(this->markers != NULL)
    fts_object_release((fts_object_t *)this->markers);
  
  if(this->editor != NULL)
    fts_object_release((fts_object_t *)this->editor);
}

static void
track_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(track_t), track_init, track_delete);
  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
	fts_class_message_varargs(cl, seqsym_save_editor, track_set_save_editor);
	fts_class_message_varargs(cl, seqsym_set_editor, track_set_editor_at_client);
  
  fts_class_message_varargs(cl, fts_s_dump_state, track_dump_state);
  fts_class_message_varargs(cl, fts_s_dump_gui, track_dump_gui);
  
  fts_class_message_varargs(cl, fts_s_update_gui, track_update_gui);
	
  fts_class_message_varargs(cl, seqsym_add_event, track_add_event_from_file);
  fts_class_message_varargs(cl, seqsym_add_marker, track_add_marker_from_file);
  fts_class_message_varargs(cl, seqsym_dump_mess, track_event_dump_mess);
  fts_class_message_varargs(cl, seqsym_editor, track_editor_dump_mess);
  
  fts_class_message_varargs(cl, fts_s_upload, track_upload);
  fts_class_message_varargs(cl, fts_s_post, track_post);
  
  fts_class_message_varargs(cl, fts_s_print, track_print);
  
  fts_class_message_varargs(cl, seqsym_import_midifile_dialog, track_import_midifile_dialog);
  fts_class_message_varargs(cl, seqsym_import_midifile, track_import_midifile);
  
  fts_class_message_varargs(cl, seqsym_export_midifile_dialog, track_export_midifile_dialog);
  fts_class_message_varargs(cl, seqsym_export_midifile, track_export_midifile);
  
  fts_class_message_varargs(cl, seqsym_endPaste, track_end_paste);
  fts_class_message_varargs(cl, fts_new_symbol("endUpdate"), track_end_update);
  
  fts_class_message_varargs(cl, fts_s_openEditor, track_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, track_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, track_close_editor);
  
  fts_class_message_varargs(cl, fts_s_add_gui_listener, track_add_gui_listener);
  fts_class_message_varargs(cl, fts_s_remove_gui_listener, track_remove_gui_listener);
  fts_class_message_varargs(cl, fts_s_notify_gui_listeners, track_notify_gui_listeners);
  
  fts_class_message_number(cl, seqsym_active, track_active);
  
  fts_class_message_void(cl, fts_s_clear, track_clear_method);
	
	fts_class_message_varargs(cl, seqsym_addEvent, track_add_event_from_client);
  fts_class_message_varargs(cl, seqsym_makeEvent, track_make_event_from_client);
  fts_class_message_varargs(cl, seqsym_removeEvents, track_remove_events_from_client);
  fts_class_message_varargs(cl, seqsym_moveEvents, track_move_events_from_client);
  
  fts_class_message_varargs(cl, seqsym_insert, track_insert);  
  fts_class_message_atom(cl, seqsym_remove, track_remove);
  
  fts_class_message_void(cl, fts_s_import, track_import_dialog);
  fts_class_message_symbol(cl, fts_s_import, track_import);
  
  fts_class_message_void(cl, fts_s_export, track_export_dialog);
  fts_class_message_symbol(cl, fts_s_export, track_export);
  
  fts_class_message_void(cl, fts_new_symbol("duration"), _track_get_duration);
  fts_class_message_void(cl, fts_new_symbol("size"), _track_get_size);
  
  /* DON'T EVER TELL ANYBODY!! */
  fts_class_message_void(cl, seqsym_markers, _track_get_markers);
  fts_class_message_void(cl, fts_new_symbol("make-bars"), _track_make_bars);
  
  fts_class_inlet_thru(cl, 0);
  
  fts_class_set_copy_function(cl, track_copy_function);
  
  fts_class_doc(cl, seqsym_track, "[<sym: type>]", "sequence of time-tagged values");
  fts_class_doc(cl, fts_s_clear, NULL, "erase all events");
  fts_class_doc(cl, seqsym_insert, "<num: time> <any: value>", "insert event at given time");
  fts_class_doc(cl, fts_new_symbol("duration"), NULL, "get duration");
  fts_class_doc(cl, fts_new_symbol("size"), NULL, "get duration");
  fts_class_doc(cl, fts_s_import, "[<sym: file name>]", "import from standard MIDI file");
  fts_class_doc(cl, fts_s_export, "[<sym: file name>]", "export to standard MIDI file");
  fts_class_doc(cl, fts_s_print, NULL, "print");
}  

void
track_config(void)
{
  track_class = fts_class_install(seqsym_track, track_instantiate);
  
  content_dumper_class = fts_class_install(NULL, content_dumper_instantiate);
  content_dumper = (content_dumper_t *)fts_object_create(content_dumper_class, 0, 0);
}
