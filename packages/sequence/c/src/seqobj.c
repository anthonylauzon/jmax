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
#include "seqmidi.h"

static fts_symbol_t sym_openEditor = 0;
static fts_symbol_t sym_destroyEditor = 0;
static fts_symbol_t sym_addEvent = 0;
static fts_symbol_t sym_addTrack = 0;

static fts_symbol_t sym_seqevent = 0;
static fts_symbol_t sym_upload = 0;
static fts_symbol_t sym_print = 0;

/******************************************************
 *
 *  object
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
  fts_symbol_t name = fts_get_symbol(at + 0);
  fts_symbol_t type = fts_get_symbol(at + 1);
  fts_object_t *track;
  
  

  sequence_add_track(this, name, fts_type_get_by_name(type));
}

/* remove track by client request */
void
seqobj_track_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t track_name = fts_get_symbol(at + 0);
  sequence_track_t *track = sequence_get_track_by_name(this, track_name);
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

  /* delete track object */
  fts_object_delete((fts_object_t *)track);

  /* remove track from sequence */
  sequence_remove_track(this, track_name);
}

/* create new event by client request */
void
seqobj_event_new(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t track_name = fts_get_symbol(at + 0);
  double time = fts_get_float(at + 1);
  fts_object_t *event;

  /* make new event object */
  fts_object_new(0, ac - 2, at + 2, &event);

  /* add event to sequence */
  sequence_add_event(this, sequence_get_track_by_name(this, track_name), time, (sequence_event_t *)event);

  /* create event at client */
  fts_client_upload(event, sym_seqevent, ac - 1, at + 1);

  /* add event to sequence at client */
  {
    fts_atom_t a[2];

    fts_set_symbol(a + 0, track_name);
    fts_set_object(a + 1, event);
    
    fts_client_send_message(o, sym_addEvent, 2, a);
  }
}

/* delete event by client request */
void
seqobj_event_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_object_t *event = fts_get_object(at);

  /* remove event from sequence */
  sequence_remove_event((sequence_event_t *)event);

  /* delete event object and remove from client */
  fts_object_delete(event);
}

void
seqobj_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  sequence_event_t *event = sequence_get_begin(this);

  /* empty track */
  while(event)
    {
      if(!fts_object_has_id((fts_object_t *)event))
	{
	  /* create event at client */
	  fts_send_message((fts_object_t *)event, fts_SystemInlet, sym_upload, 0, 0);
	  
	  /* add event to sequence at client */
	  {
	    sequence_track_t *track = sequence_event_get_track(event);
	    fts_atom_t a[2];
	    
	    fts_set_symbol(a + 0, sequence_track_get_name(track));
	    fts_set_object(a + 1, (fts_object_t *)event);
	    
	    fts_client_send_message(o, sym_addEvent, 2, a);
	  }
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

  /* print tracks */
  {
    int n_tracks = sequence_get_n_tracks(this);
    int i;
    
    post("sequence: %d track(s)\n", n_tracks);
    
    for(i=0; i<n_tracks; i++)
      {
	sequence_track_t *track = sequence_get_track_by_index(this, i);
	sequence_track_post(track);
      }
  }
  
  /* print events */
  {
    int size = sequence_get_size(this);
    sequence_event_t *evt = sequence_get_begin(this);
    
    post("sequence: %d events\n", size);
    
    while(evt)
      {
	sequence_track_t *track = sequence_event_get_track(evt);
	fts_symbol_t track_name = sequence_track_get_name(track);
	fts_symbol_t class_name = fts_object_get_class_name((fts_object_t *)evt);

	post("@%lf (%s): %s ", sequence_event_get_time(evt), fts_symbol_name(track_name), fts_symbol_name(class_name));
	fts_send_message((fts_object_t *)evt, fts_SystemInlet, fts_s_print, 0, 0);
	post("\n");	
	evt = sequence_event_get_next(evt);
      }
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
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("event_new"), seqobj_event_new);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("event_remove"), seqobj_event_remove);

      fts_method_define_varargs(cl, 0, sym_print, seqobj_print);
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), seqobj_import);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
seqobj_config(void)
{
  sym_openEditor = fts_new_symbol("openEditor");
  sym_destroyEditor = fts_new_symbol("destroyEditor");
  sym_addEvent = fts_new_symbol("addEvent");
  sym_addEvent = fts_new_symbol("addTrack");

  sym_seqevent = fts_new_symbol("seqevent");
  sym_upload = fts_new_symbol("upload");
  sym_print = fts_new_symbol("print");

  fts_class_install(fts_new_symbol("sequence"), seqobj_instantiate);
}
