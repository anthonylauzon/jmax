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
#include "fts.h"
#include "seqsym.h"
#include "event.h"
#include "eventtrk.h"
#include "seqmidi.h"

#define EVENTTRK_ADD_BLOCK_SIZE 256

/*********************************************************
 *
 *  event track: add/remove and move event
 *
 */

static void
eventtrk_cut_event(eventtrk_t *track, event_t *event)
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
  
  event->next = event->prev = 0;
  track->size--;
}

static void
eventtrk_prepend_event(eventtrk_t *track, event_t *event)
{
  if(track->size == 0)
    {
      track->first = track->last = event;
      event->next = event->prev = 0;
      track->size = 1;
    }
  else
    {
      event->next = track->first;
      event->prev = 0;
      track->first = event->next->prev = event;
      track->size++;
    }
}

static void
eventtrk_append_event(eventtrk_t *track, event_t *event)
{
  if(track->size == 0)
    {
      track->first = track->last = event;
      event->next = event->prev = 0;
      track->size = 1;
    }
  else
    {
      event->prev = track->last;
      event->next = 0;
      track->last = event->prev->next = event;
      track->size++;
    }  
}

static void
eventtrk_insert_event_before(eventtrk_t *track, event_t *here, event_t *event)
{
  if(here == 0)
    eventtrk_append_event(track, event);
  else if(here == track->first)
    eventtrk_prepend_event(track, event);
  else
    {
      event->prev = here->prev;
      event->next = here;

      here->prev->next = here->prev = event;
      track->size++;
    }  
}

static void
eventtrk_insert_event_behind(eventtrk_t *track, event_t *here, event_t *event)
{
  if(here == track->last)
    eventtrk_append_event(track, event);
  else
    {
      event->next = here->next;
      event->prev = here;

      here->next->prev = here->next = event;
      track->size++;
    }  
}

static event_t *
eventtrk_get_event_by_time(eventtrk_t *track, double time)
{
  event_t *event = 0;

  if(eventtrk_get_size(track) > 0 && time <= track->last->time)
    {
      event = track->first;
      
      while(time > event->time)
	event = event->next;
    }
  
  return event;  
}

static event_t *
eventtrk_get_event_by_time_after(eventtrk_t *track, double time, event_t *here)
{
  event_t *event = 0;

  if(eventtrk_get_size(track) > 0 && time <= track->last->time)
    {
      event = here;
      
      while(time > event->time)
	event = event->next;
    }
  
  return event;  
}


/*********************************************************
 *
 *  event track: add/remove and move event
 *
 */

void
eventtrk_add_event(eventtrk_t *track, double time, event_t *event)
{
  event_t *next = eventtrk_get_event_by_time(track, time);
  
  eventtrk_insert_event_before(track, next, event);

  event_set_track(event, track);
  event_set_time(event, time);
}

void
eventtrk_add_event_after(eventtrk_t *track, double time, event_t *event, event_t *here)
{
  event_t *next = eventtrk_get_event_by_time_after(track, time, here);
  
  eventtrk_insert_event_before(track, next, event);

  event_set_track(event, track);
  event_set_time(event, time);
}

void
eventtrk_move_event(event_t *event, double time)
{
  eventtrk_t *track = event->track;
  event_t *next = event->next;
  event_t *prev = event->prev;

  if((next && time > next->time) || (prev && time < prev->time))
    {
      eventtrk_cut_event(track, event);
      event_set_time(event, time);      
      eventtrk_insert_event_before(track, eventtrk_get_event_by_time(track, time), event);
    }
  else
    event_set_time(event, time);
}


void
eventtrk_remove_event(event_t *event)
{
  eventtrk_t *track = event->track;
  eventtrk_cut_event(track, event);
}

/******************************************************
 *
 *  object
 * 
 */

void
eventtrk_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;
  fts_symbol_t name = fts_get_symbol(at + 1);
  fts_symbol_t type = fts_get_symbol(at + 2);

  track_init(&this->head, name);

  this->first = 0;
  this->last = 0;
  this->size = 0;
  this->type = type;
}

void
eventtrk_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;
  event_t *event = eventtrk_get_first(this);

  while(event)
    {
      event_t *next = event_get_next(event);

      fts_object_delete((fts_object_t *)event);
      
      event = next;
    }
}

/* create new event and upload by client request */
void
eventtrk_add_event_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;

  if(!track_is_locked(&this->head))
    {
      double time = fts_get_float(at + 0);
      fts_object_t *event;
      fts_atom_t a[1];

      /* make new event object */
      fts_object_new(0, ac - 1, at + 1, &event);

      /* add event to track */
      eventtrk_add_event(this, time, (event_t *)event);

      /* create event at client (short cut: could also send upload message to event object) */
      fts_client_upload(event, seqsym_event, ac, at);

      /* add event to track at client */
      fts_set_object(a, event);    
      fts_client_send_message((fts_object_t *)this, seqsym_addEvents, 1, a);
    }
}

/* create new event by client request without uploading */
void
eventtrk_make_event_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;

  if(!track_is_locked(&this->head))
    {
      double time = fts_get_float(at + 0);
      fts_object_t *event;
      fts_atom_t a[1];

      /* make new event object */
      fts_object_new(0, ac - 1, at + 1, &event);

      /* add event to track */
      eventtrk_add_event(this, time, (event_t *)event);
    }
}

/* delete event by client request */
void
eventtrk_remove_event_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;

  if(!track_is_locked(&this->head))
    {
      fts_object_t *event = fts_get_object(at + 0);
      fts_atom_t a[1];

      eventtrk_cut_event(this, (event_t *)event);
      
      /* add track to sequence at client */
      fts_set_object(a + 0, (fts_object_t *)event);
      fts_client_send_message(o, seqsym_deleteEvents, 1, a);
      
      /* delete event object and remove from client */
      fts_object_delete(event);
    }
}

void
eventtrk_lock(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;

  track_lock(&this->head);
}

void
eventtrk_unlock(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;

  track_unlock(&this->head);
}

void
eventtrk_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;
  event_t *event = eventtrk_get_first(this);
  fts_atom_t a[EVENTTRK_ADD_BLOCK_SIZE];
  int n = 0;
  
  if(!fts_object_has_id((fts_object_t *)this))
    {
      fts_atom_t a[2];
      
      /* create track at client */
      fts_set_symbol(a + 0, track_get_name(&this->head));
      fts_set_symbol(a + 1, eventtrk_get_type(this));
      fts_client_upload((fts_object_t *)this, seqsym_track, 2, a);
    }

  while(event)
    {
      if(!fts_object_has_id((fts_object_t *)event))
	{
	  /* create event at client */
	  fts_send_message((fts_object_t *)event, fts_SystemInlet, fts_s_upload, 0, 0);

	  fts_set_object(a + n, (fts_object_t *)event);
	  
	  n++;

	  if(n == EVENTTRK_ADD_BLOCK_SIZE)
	    {
	      fts_client_send_message((fts_object_t *)this, seqsym_addEvents, n, a);
	      n = 0;
	    }
	}
      
      event = event_get_next(event);
    }
 
  if(n > 0)
    fts_client_send_message((fts_object_t *)this, seqsym_addEvents, n, a);    
}

static void 
eventtrk_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;
  fts_symbol_t track_name = track_get_name(&this->head);
  event_t *event = eventtrk_get_first(this);  
  
  post("track %s: %d event(s)\n", fts_symbol_name(track_name), eventtrk_get_size(this));

  while(event)
    {
      fts_symbol_t class_name = fts_object_get_class_name((fts_object_t *)event);
      
      post("  @%lf: <%s> ", event_get_time(event), fts_symbol_name(class_name));
      fts_send_message((fts_object_t *)event, fts_SystemInlet, fts_s_print, 0, 0);
      event = event_get_next(event);
    }  
}

static void 
eventtrk_export_to_midifile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;
  fts_symbol_t file_name = fts_get_symbol(at);

  seqmidi_write_midifile_from_event_track(this, file_name);
}

static void 
eventtrk_export_to_midifile_with_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;
  fts_symbol_t track_name = track_get_name(&this->head);
  fts_symbol_t default_name;
  char str[1024];
  fts_atom_t a[4];

  snprintf(str, 1024, "%s.mid", fts_symbol_name(track_name));
  default_name = fts_new_symbol_copy(str);
      
  fts_set_symbol(a, seqsym_export_midi);
  fts_set_symbol(a + 1, fts_new_symbol("Save standard MIDI file"));
  fts_set_symbol(a + 2, fts_get_project_dir());
  fts_set_symbol(a + 3, default_name);
  fts_client_send_message((fts_object_t *)this, seqsym_openFileDialog, 4, a);
}

static void
eventtrk_add_event_from_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;
  double time = fts_get_float(at + 0);
  fts_object_t *event;
  
  /* make new event object */
  fts_object_new(0, ac - 1, at + 1, &event);
  
  /* add event to track */
  eventtrk_append_event(this, (event_t *)event);

  event_set_track((event_t *)event, this);
  event_set_time((event_t *)event, time);
}

static void 
eventtrk_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  eventtrk_t *this = (eventtrk_t *)o;
  fts_bmax_file_t *file = (fts_bmax_file_t *) fts_get_ptr(at);  
  event_t *event = eventtrk_get_first(this);

  fts_bmax_code_push_symbol(file, this->type);
  fts_bmax_code_push_symbol(file, track_get_name(&this->head));

  fts_bmax_code_obj_mess(file, fts_SystemInlet, seqsym_bmax_add_track, 2);
  fts_bmax_code_pop_args(file, 2);
  
  while(event)
    {
      fts_send_message((fts_object_t *)event, fts_SystemInlet, fts_s_save_bmax, ac, at);
      event = event_get_next(event);
    }  
}

static fts_status_t
eventtrk_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(eventtrk_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, eventtrk_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, eventtrk_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, eventtrk_save_bmax);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_bmax_add_event, eventtrk_add_event_from_bmax);

  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_lock, eventtrk_lock);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_unlock, eventtrk_unlock);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload, eventtrk_upload);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, eventtrk_print);

  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_export_midi_dialog, eventtrk_export_to_midifile_with_dialog);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_export_midi, eventtrk_export_to_midifile);
    
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("make_event"), eventtrk_make_event_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_event"), eventtrk_add_event_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("remove_event"), eventtrk_remove_event_by_client_request);

  return fts_Success;
}

void
eventtrk_config(void)
{
  fts_class_install(seqsym_eventtrk, eventtrk_instantiate);
}
