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
#include "sequence.h"
#include "seqobj.h"
#include "seqmidi.h"

fts_symbol_t seqevent_symbol = 0;
fts_symbol_t seqtrack_symbol = 0;

static fts_symbol_t sym_openEditor = 0;
static fts_symbol_t sym_destroyEditor = 0;
static fts_symbol_t sym_addEvent = 0;
static fts_symbol_t sym_addTrack = 0;

/******************************************************
 *
 *  seqtrack object
 * 
 *  This is an FTS object representing a sequence track.
 *  It is not intended to show up on screen
 *
 */

void
seqtrack_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_track_t *this = (sequence_track_t *)o;
  fts_symbol_t type = fts_get_symbol(at + 0);
  fts_symbol_t name = fts_get_symbol(at + 1);

  sequence_track_init(this, type, name);
}

void
seqtrack_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
}

/* create new event by client request */
void
seqtrack_event_new(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_track_t *this = (sequence_track_t *)o;
  sequence_t *sequence = sequence_track_get_sequence(this);
  double time = fts_get_float(at + 0);
  fts_object_t *event;
  fts_atom_t a[1];

  /* make new event object */
  fts_object_new(0, ac - 1, at + 1, &event);

  /* add event to sequence */
  sequence_add_event(sequence, this, time, (sequence_event_t *)event);

  /* create event at client (short cut: could also send upload message to event object) */
  fts_client_upload(event, seqevent_symbol, ac, at);

  /* add event to track at client */
  fts_set_object(a, event);    
  fts_client_send_message((fts_object_t *)this, sym_addEvent, 1, a);
}

/* delete event by client request */
void
seqtrack_event_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_track_t *this = (sequence_track_t *)o;
  fts_object_t *event = fts_get_object(at + 0);

  /* remove event from sequence */
  sequence_remove_event((sequence_event_t *)event);

  /* delete event object and remove from client */
  fts_object_delete(event);
}

static fts_status_t
seqtrack_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sequence_track_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqtrack_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqtrack_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("event_new"), seqtrack_event_new);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("event_remove"), seqtrack_event_remove);
  
  return fts_Success;
}

/******************************************************
 *
 *  sequence object
 *
 */

void
seqobj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_init(this);
}

void
seqobj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  sequence_track_t *track = sequence_get_first_track(this);
  sequence_event_t *event = sequence_get_begin(this);  

  while(event)
    {
      sequence_remove_event(event);
      fts_object_delete((fts_object_t *)event);

      event = sequence_get_begin(this);
    }

  while(track)
    {
      sequence_remove_track(track);
      fts_object_delete((fts_object_t *)track);

      track = sequence_get_first_track(this);
    }    
  
  fts_client_send_message(o, sym_destroyEditor, 0, 0);
}

/******************************************************
 *
 *  system methods
 *
 */

/* add new track by client request */
void
seqobj_track_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t type = fts_get_symbol(at + 0);
  fts_symbol_t name = fts_get_symbol(at + 1);
  fts_object_t *track;
  fts_atom_t a[3];
  
  /* make new event object */
  fts_set_symbol(a + 0, seqtrack_symbol);
  fts_set_symbol(a + 1, type);
  fts_set_symbol(a + 2, name);
  fts_object_new(0, 3, a, &track);  

  /* add it to the track */
  sequence_add_track(this, (sequence_track_t *)track);

  /* add track to sequence at client */
  fts_set_object(a + 0, (fts_object_t *)track);	    
  fts_client_send_message(o, sym_addTrack, 1, a);
}

/* remove track by client request */
void
seqobj_track_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  sequence_track_t *track = (sequence_track_t *)fts_get_object(at + 0);
  sequence_event_t *event = sequence_get_begin(this);

  /* empty track */
  while(event)
    {
      sequence_event_t *next = sequence_event_get_next(event);

      if(sequence_event_get_track(event) == track)
	{
	  sequence_remove_event(event);
	  fts_object_delete((fts_object_t *)event);
	}
      
      event = next;
    }

  /* remove track from sequence */
  sequence_remove_track(track);

  /* delete track object */
  fts_object_delete((fts_object_t *)track);
}

void
seqobj_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  sequence_track_t *track = sequence_get_first_track(this);
  sequence_event_t *event = sequence_get_begin(this);

  while(track)
    {
      if(!fts_object_has_id((fts_object_t *)track))
	{
	  sequence_t *sequence = sequence_track_get_sequence(track);
	  fts_atom_t a[2];

	  /* create track at client */
	  fts_set_symbol(a + 0, sequence_track_get_type(track));	    
	  fts_set_symbol(a + 1, sequence_track_get_name(track));
	  fts_client_upload((fts_object_t *)track, seqtrack_symbol, 2, a);
	  
	  /* add track to sequence at client */
	  fts_set_object(a + 0, (fts_object_t *)track);	    
	  fts_client_send_message((fts_object_t *)sequence, sym_addTrack, 1, a);
	}

      track = sequence_track_get_next(track);
    }

  while(event)
    {
      if(!fts_object_has_id((fts_object_t *)event))
	{
	  sequence_track_t *track = sequence_event_get_track(event);
	  fts_atom_t a[1];
	    
	  /* create event at client */
	  fts_send_message((fts_object_t *)event, fts_SystemInlet, fts_s_upload, 0, 0);
	  
	  /* add event to sequence at client */
	  fts_set_object(a, (fts_object_t *)event);
	  fts_client_send_message((fts_object_t *)track, sym_addEvent, 1, a);
	}

      event = sequence_event_get_next(event);
    }
}

void
seqobj_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  seqobj_update(o, 0, 0, 0, 0);
  fts_client_send_message(o, sym_openEditor, 0, 0);
}

void
seqobj_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);

  if(name)
    sequence_read_midifile(this, name);

  seqobj_update(o, 0, 0, 0, 0);
}
void
seqobj_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  sequence_track_t *track = sequence_get_first_track(this);
  sequence_event_t *event = sequence_get_begin(this);  

  post("sequence: %d track(s)\n", sequence_get_n_tracks(this));
  
  while(track)
    {
      sequence_track_post(track);
      track = sequence_track_get_next(track);      
    }    
  
  post("sequence: %d events\n", sequence_get_size(this));
  
  while(event)
    {
      sequence_track_t *track = sequence_event_get_track(event);
      fts_symbol_t track_name = sequence_track_get_name(track);
      fts_symbol_t class_name = fts_object_get_class_name((fts_object_t *)event);
      
      post("@%lf (%s): <%s> ", sequence_event_get_time(event), fts_symbol_name(track_name), fts_symbol_name(class_name));
      fts_send_message((fts_object_t *)event, fts_SystemInlet, fts_s_print, 0, 0);
      post("\n");	
      event = sequence_event_get_next(event);
    }
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
seqobj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1)
    {
      fts_class_init(cl, sizeof(sequence_t), 1, 0, 0); 

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqobj_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqobj_delete);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), seqobj_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("track_add"), seqobj_track_add);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("track_remove"), seqobj_track_remove);

      fts_method_define_varargs(cl, 0, fts_s_print, seqobj_print);
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), seqobj_import);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
seqobj_config(void)
{
  seqevent_symbol = fts_new_symbol("seqevent");
  seqtrack_symbol = fts_new_symbol("seqtrack");

  sym_openEditor = fts_new_symbol("openEditor");
  sym_destroyEditor = fts_new_symbol("destroyEditor");
  sym_addEvent = fts_new_symbol("addEvent");
  sym_addTrack = fts_new_symbol("addTrack");

  fts_class_install(fts_new_symbol("sequence"), seqobj_instantiate);
  fts_class_install(fts_new_symbol("seqtrack"), seqtrack_instantiate);
}


