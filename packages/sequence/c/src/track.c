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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */
#include <fts/fts.h>
#include <ftsprivate/client.h>
#include <ftsprivate/patcher.h>
#include <ftsconfig.h>
#include <sequence/c/include/note.h>
#include <sequence/c/include/event.h>
#include <sequence/c/include/track.h>
#include "seqsym.h"
#include "seqmidi.h"
#include "seqmess.h"

#define TRACK_BLOCK_SIZE 256

fts_class_t *track_type = 0;

/*********************************************************
 *
 *  insert events to event list
 *
 */

/* create event value and make a new event */
static event_t *
create_event(int ac, const fts_atom_t *at)
{
  fts_symbol_t type_name = fts_get_symbol(at);
  event_t *event = 0;

  if(type_name == fts_s_int || type_name == fts_s_float || type_name == fts_s_symbol)
    event = (event_t *)fts_object_create(event_type, NULL, 1, at + 1);
  else
    {
      fts_class_t *type = fts_class_get_by_name( NULL, type_name);

      if(type)
	{
	  fts_object_t *obj = fts_object_create(type, NULL, 0, 0);	
	  fts_class_t *class = fts_object_get_class(obj);
	  fts_method_t meth_set = fts_class_get_method(class, fts_s_set_from_array);

	  if(obj && meth_set)
	    {
	      fts_atom_t a;
	      
	      meth_set(obj, 0, 0, ac - 1, at + 1);

	      fts_set_object(&a, obj);
	      event = (event_t *)fts_object_create(event_type, NULL, 1, &a);
	    }
	}
    }

  return event;
}

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
  
  ((fts_object_t *)event)->patcher = 0;
  fts_object_release(event);
}

static void
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
      ((fts_object_t *)event)->patcher = 0;
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
 *  client calls
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
  
void
track_highlight_cluster(track_t *track, event_t *event, event_t *next)
{
  if(track_editor_is_open(track))
    {
      fts_atom_t at[64];
      int ac = 0;

      while(event && ( event != next) && ( ac < 64))
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
      fts_atom_t at[64];
      int ac = 0;
      
      fts_set_object(at + ac, (fts_object_t *)event);
      ac++;
      event = event_get_next(event);

      while(event && ( event_get_time(event) == time) && (ac < 64))
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
 *  client calls
 * 
 */

/* set name of track by client request (ones this will be about variables!!!) */
static void
track_set_name_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t name = fts_get_symbol(at);

  /* check if name is in use in this sequence */
  if(sequence_get_track_by_name(track_get_sequence(this), name))
    {
      fts_symbol_t old_name = track_get_name(this);
      
      if(old_name)
	{
	  fts_atom_t a[1];

	  fts_set_symbol(a, old_name);
	  fts_client_send_message((fts_object_t *)this, seqsym_setName, 1, a);
	}
      else
	fts_client_send_message((fts_object_t *)this, seqsym_setName, 0, 0);
    }
  else
    {
      track_set_name(this, name);
      fts_client_send_message((fts_object_t *)this, seqsym_setName, 1, at);
    }
  
  track_set_dirty( this);
}

static void
track_add_event_at_client(track_t *this, event_t *event, int ac, const fts_atom_t *at)
{
  if(!fts_object_has_id((fts_object_t *)event))
    {
      ((fts_object_t *)event)->patcher = fts_object_get_patcher( (fts_object_t *)this);
      fts_client_register_object((fts_object_t *)event, FTS_NO_ID);
    }

  fts_client_start_message( (fts_object_t *)this, seqsym_addEvents);
  fts_client_add_int( (fts_object_t *)this, fts_get_object_id((fts_object_t *)event));
  fts_client_add_atoms( (fts_object_t *)this, ac, at);
  fts_client_done_message( (fts_object_t *)this);
}

static void
track_event_upload(track_t *this, event_t *event)
{
  fts_symbol_t type = event_get_type(event);
  fts_atom_t a[4];

  if(!fts_object_has_id((fts_object_t *)event))
    {
      ((fts_object_t *)event)->patcher = fts_object_get_patcher( (fts_object_t *)this);
      fts_client_register_object((fts_object_t *)event, FTS_NO_ID);
    

      fts_client_start_message( (fts_object_t *)this, seqsym_addEvents);
      fts_client_add_int( (fts_object_t *)this, fts_get_object_id((fts_object_t *)event));

      if(fts_is_object(&event->value))
	{
	  if(type == seqsym_note)
	    {
	      note_t *note = (note_t *)fts_get_object(&event->value);
	      
	      fts_set_float(a + 0, (float)event_get_time(event));
	      fts_set_symbol(a + 1, seqsym_note);
	      fts_set_int(a + 2, note_get_pitch(note));
	      fts_set_float(a + 3, (float)note_get_duration(note));
	      fts_client_add_atoms( (fts_object_t *)this, 4, a);
	      fts_client_done_message( (fts_object_t *)this);

	      return;
	    }
	  else if(type == seqsym_seqmess)
	    {
	      seqmess_t *seqmess = (seqmess_t *)fts_get_object(&event->value);
	      
	      fts_set_float(a + 0, (float)event_get_time(event));
	      fts_set_symbol(a + 1, seqsym_seqmess);
	      fts_set_symbol(a + 2, seqmess_get_selector(seqmess));
	      fts_set_int(a + 3, seqmess_get_position(seqmess));
	      fts_client_add_atoms( (fts_object_t *)this, 4, a);
	      fts_client_done_message( (fts_object_t *)this);

	      return;
	    }
	}
      else if(!fts_is_void(&event->value))
	{ 
	  fts_set_float(a + 0, (float)event_get_time(event));
	  fts_set_symbol(a + 1, fts_get_class_name(&event->value));
	  a[2] = event->value;
	  fts_client_add_atoms( (fts_object_t *)this, 3, a);
	  fts_client_done_message( (fts_object_t *)this);

	  return;
	}

      /* anything else is uploaded as void event */
      fts_set_float(a + 0, (float)event_get_time(event));
      fts_set_symbol(a + 1, fts_s_void);
      fts_set_symbol(a + 2, type);
      fts_client_add_atoms( (fts_object_t *)this, 3, a);
      fts_client_done_message( (fts_object_t *)this);
    }
}

/* create new event and upload by client request */
static void
track_add_event_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event;
  
  /* make new event object */
  event = create_event(ac - 1, at + 1);
  
  if(event)
    {
      /* add event to track */
      track_add_event(this, time, event);
      
      track_add_event_at_client(this, event, ac, at);
    }
  
  track_set_dirty( this);
}

/* create new event by client request without uploading */
static void
track_make_event_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event;
  
  /* make new event object */
  event = create_event(ac - 1, at + 1);
  
  /* add event to track */
  if(event)
    track_add_event(this, time, event);

  track_set_dirty( this);
}

/* delete event by client request */
static void
track_remove_events_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  int i;
  
  /*  remove event objects from client */
  fts_client_send_message(o, seqsym_removeEvents, ac, at);
  
  for(i=0; i<ac; i++)
    {
      fts_object_t *event = fts_get_object(at + i);
      
      track_remove_event(this, (event_t *)event);
    }

  track_set_dirty( this);
}

/* move event by client request */
static void
track_move_events_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  int i;
  
  for(i=0; i<ac; i+=2)
    {
      event_t *event = (event_t *)fts_get_object(at + i);
      float time = fts_get_float(at + i + 1);
      
      track_move_event(this, (event_t *)event, time);
    }
  
  fts_client_send_message(o, seqsym_moveEvents, ac, at);

  track_set_dirty( this);
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
  
  this->active = fts_get_int(at);

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

  track_set_dirty( this);
}

static void 
track_insert(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  if(ac >= 2 && fts_is_number(at))
    {
      fts_symbol_t track_type = track_get_type(this);
      
      if(track_type == fts_s_void || fts_get_class_name(at + 1) == track_type)
	{
	  double time = fts_get_number_float(at);
	  event_t *event = (event_t *)fts_object_create(event_type, NULL, 1, at + 1);
	  
	  /* add event to track */
	  track_add_event(this, time, event);
	}
      else
	fts_object_signal_runtime_error(o, "insert: event type missmatch\n");
    }
}
  
static void 
track_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  event_t *event = track_get_first(this);

  while(event)
    {
      event_t *next = event_get_next(event);

      if(fts_atom_compare(event_get_value(event), at))
	track_remove_event(this, event);
      
      event = next;
    }  
}
  
static void 
track_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  fts_symbol_t track_type = track_get_type(this);
  
  if(track_type == fts_s_void)
    fts_spost(stream, "(:track)");
  else
    {
      fts_spost(stream, "(:track ");
      fts_spost_symbol(stream, track_type);
      fts_spost(stream, ")");
    }
}

static void 
track_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  fts_symbol_t track_type = track_get_type(this);
  int track_size = track_get_size(this);
  event_t *event = track_get_first(this);  

  if(track_size == 0)
    {
      if(track_type == fts_s_void)
	fts_spost(stream, "<empty track>\n");
      else
	fts_spost(stream, "<empty %s track>\n", track_type);
    }
  else 
    {
      if(track_type == fts_s_void)
	fts_spost(stream, "<%s ", track_type);
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
	  fts_spost(stream, "track of %d events>\n", track_size);
	  fts_spost(stream, "{\n");
      
	  while(event)
	    {
	      fts_spost(stream, "@%.7g, ", event_get_time(event));
	      fts_spost_atoms(stream, 1, event_get_value(event));
	      fts_spost(stream, "\n");
	      
	      event = event_get_next(event);
	    }  
	  
	  fts_spost(stream, "}\n");      
	}
    }
}

static void
track_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t name = track_get_name(this);  
  event_t *event = track_get_first(this);
  fts_atom_t a[TRACK_BLOCK_SIZE];
  
  fts_set_int(a, track_get_size( this));
  fts_client_send_message((fts_object_t *)this, fts_s_start_upload, 1, a);  

  /* set track name */
  if(name)
    {
      fts_set_symbol(a, name);
      fts_client_send_message((fts_object_t *)this, seqsym_setName, 1, a);
    }

  fts_set_int(a, track_is_active(this));
  fts_client_send_message((fts_object_t *)this, seqsym_active, 1, a);  

  while(event)
    {
      if(!fts_object_has_id((fts_object_t *)event))
	{
	  /* create event at client */
	  track_event_upload(this, event);
	}
	  
      event = event_get_next(event);
    }
  fts_client_send_message((fts_object_t *)this, fts_s_end_upload, 0, 0);  
}

static void
track_persistence(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  if(ac > 0)
    {
      /* set persistence flag */
      if(fts_is_number(at) && this->persistence >= 0)
	{
	  this->persistence = (fts_get_number_int(at) != 0);
	  fts_client_send_message(o, fts_s_persistence, 1, at);
	}
    }
  else
    {
      /* return persistence flag */
      fts_atom_t a;

      fts_set_int(&a, this->persistence);
      fts_return(&a);
    }
}

static void
track_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_atom_t a;

  fts_set_symbol(&a, this->type);
  fts_client_send_message(o, fts_s_type, 1, &a);

  fts_set_int(&a, (this->persistence > 0));
  fts_client_send_message(o, fts_s_persistence, 1, &a);
  
  fts_name_gui_method(o, 0, 0, 0, 0);
}

static void
track_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  if(ac == 0)
    {
      fts_atom_t a;

      fts_set_int(&a, track_get_duration(this));
      fts_return(&a);      
    }
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

      track_clear(this);

      size = track_import_from_midifile(this, file);
      error = fts_midifile_get_error(file);
      
      if(error)
	fts_object_signal_runtime_error(o, "import: read error in \"%s\" (%s)\n", name, error);
      else if(size <= 0)
	fts_object_signal_runtime_error(o, "import: couldn't get any data from \"%s\"\n", name);
      
      fts_midifile_close(file);
      
      if(track_editor_is_open(this))
	track_upload(o, 0, 0, 0, 0);
    }
  else
    fts_object_signal_runtime_error(o, "import: cannot open \"%s\"\n", name);
}
      
static void
track_import_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t track_name = track_get_name(this);
  fts_symbol_t default_name;
  char str[1024];
  fts_atom_t a[4];

  snprintf(str, 1024, "%s.mid", track_name? track_name: "untitled");
  default_name = fts_new_symbol(str);

  fts_set_symbol(a, seqsym_import_midifile);
  fts_set_symbol(a + 1, fts_new_symbol("Open standard MIDI file"));
  fts_set_symbol(a + 2, fts_project_get_dir());
  fts_set_symbol(a + 3, default_name);

  fts_client_send_message((fts_object_t *)this, seqsym_openFileDialog, 4, a);
}

static void
track_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t type = track_get_type(this);

  if(type == fts_s_midievent || type == seqsym_note || type == fts_s_void)
    {
      if(ac == 0)
	track_import_midifile_dialog(o, 0, 0, 0, 0);
      else if(ac == 1 && fts_is_symbol(at))
	track_import_midifile(o, 0, 0, 1, at);
      else
	fts_object_signal_runtime_error(o, "import: wrong arguments");  
    }
  else
    fts_object_signal_runtime_error(o, "import: cannot import MIDI file to track of type %s", type);
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
	fts_object_signal_runtime_error(o, "export: write error in \"%s\" (%s)\n", error, name);
      else if(size <= 0)
	fts_object_signal_runtime_error(o, "export: couldn't write any data to \"%s\"\n", name);
      
      fts_midifile_close(file);
    }
  else
    fts_object_signal_runtime_error(o, "export: cannot open \"%s\"\n", name);
}

static void 
track_export_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t track_name = track_get_name(this);
  fts_symbol_t default_name;
  char str[1024];
  fts_atom_t a[4];

  snprintf(str, 1024, "%s.mid", track_name? track_name: "untitled");
  default_name = fts_new_symbol(str);

  fts_set_symbol(a, seqsym_export_midifile);
  fts_set_symbol(a + 1, fts_new_symbol("Save standard MIDI file"));
  fts_set_symbol(a + 2, fts_project_get_dir());
  fts_set_symbol(a + 3, default_name);

  fts_client_send_message((fts_object_t *)this, seqsym_openFileDialog, 4, a);
}

static void
track_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac == 0)
    track_export_midifile_dialog(o, 0, 0, 0, 0);
  else if(ac == 1 && fts_is_symbol(at))
    track_export_midifile(o, 0, 0, 1, at);
  else
    fts_object_signal_runtime_error(o, "export: wrong arguments");  
}

static void
track_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  track_set_editor_open( this);
  fts_client_send_message( o, fts_s_openEditor, 0, 0);
  track_upload(o, 0, 0, 0, 0);
}

static void
track_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

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

void
track_set_dirty(track_t *track)
{
  if( track_get_sequence( track))
    sequence_set_dirty( track_get_sequence( track));
  else
    if(track->persistence == 1)
      fts_patcher_set_dirty(  fts_object_get_patcher( (fts_object_t *)track), 1);
}

static void
track_end_paste(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  if( track_editor_is_open(this))
    {
      fts_client_send_message( o, seqsym_endPaste, 0, 0);
    }
}

/******************************************************
 *
 *  bmax files
 * 
 */

static void 
track_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  if(this->persistence == 1)
    {
      fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);  
      event_t *event = track_get_first(this);
      
      while(event)
	{
	  event_dump(event, dumper);
	  event = event_get_next(event);
	}  
    }

  fts_name_dump_method(o, 0, 0, ac, at);
}

void 
track_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  if(this->persistence == 1)
    {
      fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
      fts_atom_t a;

      track_dump_state(o, 0, 0, ac, at);

      /* save persistence flag */
      fts_set_int(&a, 1);
      fts_dumper_send(dumper, fts_s_persistence, 1, &a);      
    }

  fts_name_dump_method(o, 0, 0, ac, at);
}

void
track_add_event_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event;
  
  /* make new event object */
  if(ac == 2)
    event = (event_t *)fts_object_create(event_type, NULL, 1, at + 1);
  else
    event = create_event(ac - 1, at + 1);
  
  /* add event to track (strictly ordered by time) */
  if(event)
    track_append_event(this, time, event);
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

  this->persistence = 0;

  this->sequence = 0;
  this->next = 0;

  this->name = 0;
  this->active = 1;

  this->open = 0;

  this->first = 0;
  this->last = 0;
  this->size = 0;
  this->type = fts_s_void;
  
  if(ac > 0)
    {
      if(fts_is_symbol(at))
	this->type = fts_get_symbol(at);
      else
	fts_object_set_error(o, "bad argument");
    }
}

void
track_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  track_clear(this);
}

static void
track_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(track_t), track_init, track_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_persistence, track_persistence);
  fts_class_message_varargs(cl, fts_s_update_gui, track_update_gui); 
  fts_class_message_varargs(cl, fts_s_dump, track_dump);

  fts_class_message_varargs(cl, seqsym_add_event, track_add_event_from_array);

  fts_class_message_varargs(cl, fts_s_upload, track_upload);
  fts_class_message_varargs(cl, fts_s_post, track_post);
  fts_class_message_varargs(cl, fts_s_print, track_print);

  fts_class_message_varargs(cl, seqsym_import_midifile_dialog, track_import_midifile_dialog);
  fts_class_message_varargs(cl, seqsym_import_midifile, track_import_midifile);

  fts_class_message_varargs(cl, seqsym_export_midifile_dialog, track_export_midifile_dialog);
  fts_class_message_varargs(cl, seqsym_export_midifile, track_export_midifile);

  fts_class_message_varargs(cl, seqsym_setName, track_set_name_by_client_request);
  fts_class_message_varargs(cl, seqsym_addEvent, track_add_event_by_client_request);
  fts_class_message_varargs(cl, seqsym_makeEvent, track_make_event_by_client_request);
  fts_class_message_varargs(cl, seqsym_removeEvents, track_remove_events_by_client_request);
  fts_class_message_varargs(cl, seqsym_moveEvents, track_move_events_by_client_request);

  fts_class_message_varargs(cl, seqsym_active, track_active);

  fts_class_message_varargs(cl, fts_s_clear, track_clear_method);
  fts_class_message_varargs(cl, seqsym_insert, track_insert);
  fts_class_message_varargs(cl, seqsym_remove, track_remove);
  fts_class_message_varargs(cl, fts_s_import, track_import);
  fts_class_message_varargs(cl, fts_s_export, track_export);

  fts_class_message_varargs(cl, seqsym_endPaste, track_end_paste);

  fts_class_message_varargs(cl, fts_s_openEditor, track_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, track_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, track_close_editor);

  fts_class_message_varargs(cl, fts_new_symbol("duration"), track_duration);

  fts_class_inlet_anything(cl, 0);
}

void
track_config(void)
{
  track_type = fts_class_install(seqsym_track, track_instantiate);
}
