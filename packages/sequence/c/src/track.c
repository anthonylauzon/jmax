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
#include <ftsconfig.h>
#include "seqsym.h"
#include "event.h"
#include "track.h"
#include "seqmidi.h"

#define TRACK_BLOCK_SIZE 256

fts_class_t *track_class = 0;

/*********************************************************
 *
 *  insert events to event list
 *
 */

/* create event value and make a new event */
static event_t *
create_event(int ac, const fts_atom_t *at)
{
  fts_symbol_t type = fts_get_symbol(at);
  event_t *event = 0;

  if(type == fts_s_int || type == fts_s_float || type == fts_s_symbol)
    event = (event_t *)fts_object_create(event_class, 1, at + 1);
  else
    {
      fts_class_t *class = fts_class_get_by_name(type);

      if(class)
	{
	  fts_object_t *obj = fts_object_create(class, ac - 1, at + 1);
	  
	  if(obj)
	    {
	      fts_atom_t a[1];
	      
	      fts_set_object_with_type(a, obj, fts_get_class_name(class));
	      event = (event_t *)fts_object_create(event_class, 1, a);
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
    append_event(track, event);
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
  event_t *here = track_get_event_by_time(track, time);

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
track_add_event_after(track_t *track, double time, event_t *event, event_t *after)
{
  event_t *here = track_get_event_by_time_after(track, time, after);
  
  insert_event_before(track, here, event);
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

static void
track_move_event(track_t *track, event_t *event, double time)
{
  event_t *next = event->next;
  event_t *prev = event->prev;

  if((next && time > next->time) || (prev && time < prev->time))
    {
      cutout_event(track, event);

      event_set_time(event, time);      

      insert_event_before(track, track_get_event_by_time(track, time), event);
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
  event_t *event = track_get_first(merge);
  
  while(event)
    {
      event_t *next = event_get_next(event);

      /* move event from merge track to this */
      track_add_event(track, event_get_time(event), event);
      
      event = next;
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
  if(track_get_size(track) > 0 && time <= track->last->time)
    {
      event_t *event = track->first;
      
      while(time > event->time)
	event = event->next;

      return event;  
    }
  else
    return 0;
}

/* like above - searching is started from given element (here) */
event_t *
track_get_event_by_time_after(track_t *track, double time, event_t *here)
{
  if(track_get_size(track) > 0 && time <= track->last->time)
    {
      event_t *event;

      if(here)
	event = here;
      else
	event = track->first;
      
      while(time > event->time)
	event = event->next;

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
  if(sequence_editor_is_open(track_get_sequence((track_t *)track)))
    {
      fts_atom_t a;
      
      fts_set_object(&a, (fts_object_t *)event);
      fts_client_send_message((fts_object_t *)track, seqsym_highlightEvents, 1, &a);
    }
}
  
void
track_highlight_cluster(track_t *track, event_t *event, event_t *next)
{
  if(sequence_editor_is_open(track_get_sequence((track_t *)track)))
    {
      fts_atom_t at[64];
      int ac = 0;

      while(event && event != next)
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

  if(sequence_editor_is_open(track_get_sequence((track_t *)track)))
    {
      fts_atom_t at[64];
      int ac = 0;
      
      fts_set_object(at + ac, (fts_object_t *)event);
      ac++;
      event = event_get_next(event);

      while(event && event_get_time(event) == time)
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
void
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
}

/* create new event and upload by client request */
void
track_add_event_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  double time = fts_get_float(at + 0);
  fts_array_t array;
  event_t *event;
  
  /* make new event object */
  event = create_event(ac - 1, at + 1);
  
  if(event)
    {
      fts_atom_t a[1];
      
      /* add event to track */
      track_add_event(this, time, event);
      
      /* create event at client (short cut: could also send upload message to event object) */
      fts_client_upload((fts_object_t *)event, seqsym_event, ac, at);
      
      /* add event to track at client */
      fts_set_object(a, (fts_object_t *)event);    
      fts_client_send_message((fts_object_t *)this, seqsym_addEvents, 1, a);
    }
}

/* create new event by client request without uploading */
void
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
}

/* delete event by client request */
void
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
}

/* move event by client request */
void
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
}

/******************************************************
 *
 *  user methods
 * 
 */

void
track_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  event_t *event = track_get_first(this);
  fts_atom_t a[TRACK_BLOCK_SIZE];
  fts_array_t array;
  int n = 0;
  
  if(!fts_object_has_id((fts_object_t *)this))
    {
      fts_symbol_t name = track_get_name(this); 
     
      /* create track at client */
      fts_set_symbol(a, track_get_type(this));
      fts_client_upload((fts_object_t *)this, seqsym_track, 1, a);

      if(name)
	{
	  fts_set_symbol(a, name);
	  fts_client_send_message(o, seqsym_setName, 1, a);
	}
    }
  
  fts_array_init(&array, 0, 0);

  while(event)
    {
      if(!fts_object_has_id((fts_object_t *)event))
	{
	  /* create event at client */
	  int ac;
	  fts_atom_t *at;

	  fts_array_clear(&array);
	  event_get_array(event, &array);
	  
	  ac = fts_array_get_size(&array);
	  at = fts_array_get_atoms(&array);
	  fts_client_upload((fts_object_t *)event, seqsym_event, ac, at);

	  fts_set_object(a + n, (fts_object_t *)event);
	  n++;

	  if(n == TRACK_BLOCK_SIZE)
	    {
	      fts_client_send_message((fts_object_t *)this, seqsym_addEvents, n, a);
	      n = 0;
	    }
	}
      
      event = event_get_next(event);
    }

  fts_array_destroy(&array);
 
  if(n > 0)
    fts_client_send_message((fts_object_t *)this, seqsym_addEvents, n, a);    
}

static void 
track_clear_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  
  fts_client_send_message(o, fts_s_clear, 0, 0);
  
  track_clear(this);
}

static void 
track_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t track_name = track_get_name(this);
  fts_symbol_t track_type = this->type;
  event_t *event = track_get_first(this);  
  fts_array_t array;
  
  post("track of %s (%s): %d event(s)\n", fts_symbol_name(track_type), track_name? fts_symbol_name(track_name): "untitled", track_get_size(this));
  
  fts_array_init(&array, 0, 0);

  while(event)
    {
      int size;
      fts_atom_t *atoms;
           
      fts_array_clear(&array);
      event_get_array(event, &array);

      size = fts_array_get_size(&array);
      atoms = fts_array_get_atoms(&array);

      post("  @%lf: ", fts_get_float(atoms + 0));
      post("<%s> ", fts_symbol_name(fts_get_symbol(atoms + 1)));
      post_atoms(size - 2, atoms + 2);
      post("\n");

      event = event_get_next(event);
    }  

  fts_array_destroy(&array);
}

/******************************************************
 *
 *  MIDI files
 * 
 */

static void 
track_export_to_midifile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t name = fts_get_symbol(at);
  fts_midifile_t *file = fts_midifile_open_write(name);

  if(file)
    {
      int size = track_export_midifile(this, file);

      if(fts_midifile_get_error(file))
	post("track: error writing MIDI file %s (%s)\n", file->name, (file->error)? file->error: "unknown");
      else if(size <= 0)
	post("track: couldn't export to MIDI file %s\n", file->name);
      
      fts_midifile_close(file);
    }
  else
    post("track: cannot open file %s\n", file->name);
}

static void 
track_export_to_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_symbol_t track_name = track_get_name(this);
  fts_symbol_t default_name;
  char str[1024];
  fts_atom_t a[4];

  snprintf(str, 1024, "%s.mid", track_name? fts_symbol_name(track_name): "untitled");
  default_name = fts_new_symbol_copy(str);
  fts_set_symbol(a, seqsym_export_midifile);
  fts_set_symbol(a + 1, fts_new_symbol("Save standard MIDI file"));
  fts_set_symbol(a + 2, fts_project_get_dir());
  fts_set_symbol(a + 3, default_name);
  fts_client_send_message((fts_object_t *)this, seqsym_openFileDialog, 4, a);
}

/******************************************************
 *
 *  bmax files
 * 
 */

void 
track_write_to_dumper(track_t *this, fts_dumper_t *dumper)
{
  event_t *event = track_get_first(this);
  fts_message_t *mess;

  while(event)
    {
      int ac;
      fts_atom_t *at;

      /* get new add event message */
      mess = fts_dumper_message_new(dumper, seqsym_add_event);

      /* get event as array */
      event_get_array(event, fts_message_get_args(mess));

      /* dump add event message */
      fts_dumper_message_send(dumper, mess);

      event = event_get_next(event);
    }  
}

static void 
track_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);  

  track_write_to_dumper(this, dumper);
}

void
track_add_event_from_array(track_t *this, int ac, const fts_atom_t *at)
{
  double time = fts_get_float(at + 0);
  event_t *event;
  
  /* make new event object */
  event = create_event(ac - 1, at + 1);
  
  /* add event to track (strictly ordered by time) */
  if(event)
    track_append_event(this, time, event);
}

static void
track_add_event_from_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  track_add_event_from_array(this, ac, at);
}

/******************************************************
 *
 *  properties
 * 
 */

static void
track_set_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  track_t *this = (track_t *)obj;

  if(this->keep != fts_s_args && fts_is_symbol(value))
    this->keep = fts_get_symbol(value);
}

static void
track_get_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  track_t *this = (track_t *)obj;

  if(this->sequence)
    fts_set_symbol(value, sequence_get_keep(this->sequence));
  else
    fts_set_symbol(value, this->keep);
}

static void
track_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  track_t *this = (track_t *)o;

  fts_set_object_with_type(value, this, seqsym_track);
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

  ac--;
  at++;

  this->sequence = 0;
  this->next = 0;

  this->name = 0;
  this->active = 1;

  this->first = 0;
  this->last = 0;
  this->size = 0;
  this->type = seqsym_note;
  
  this->keep = fts_s_no;

  if(ac > 0)
    {
      if(fts_is_symbol(at))
	this->type = fts_get_symbol(at);
      else
	fts_object_set_error(o, "Wrong argument");
    }
}

void
track_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_t *this = (track_t *)o;

  track_clear(this);
}

static fts_status_t
track_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(track_t), 1, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, track_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, track_delete);

  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, track_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, track_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, track_get_state);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, track_dump);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_add_event, track_add_event_from_dump);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload, track_upload);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, track_print);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_clear, track_clear_method);

  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_export_midifile_dialog, track_export_to_midifile_dialog);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_export_midifile, track_export_to_midifile);
    
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_setName, track_set_name_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_addEvent, track_add_event_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_makeEvent, track_make_event_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_removeEvents, track_remove_events_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_moveEvents, track_move_events_by_client_request);

  return fts_Success;
}

void
track_config(void)
{
  fts_class_install(seqsym_track, track_instantiate);
  track_class = fts_class_get_by_name(seqsym_track);
}
