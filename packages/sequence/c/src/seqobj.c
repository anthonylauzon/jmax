/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "sequence.h"

static fts_symbol_t sym_openEditor = 0;
static fts_symbol_t sym_destroyEditor = 0;
static fts_symbol_t sym_addEvent = 0;

static fts_symbol_t sym_seqevent = 0;

void
seqevt_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_event_t *this = (sequence_event_t *)o;
  double time = fts_get_float(at + 1);
  fts_symbol_t selector = fts_get_symbol(at + 2);

  sequence_event_init(this, time, selector, ac - 3, at + 3);
}

void
seqevt_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_event_t *this = (sequence_event_t *)o;

  sequence_event_reset_value(this);
}

static fts_status_t
seqevt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sequence_event_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqevt_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqevt_delete);

  return fts_Success;
}

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
  sequence_empty(this);
}

/******************************************************
 *
 *  system methods
 *
 */

void
seqobj_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  fts_client_send_message(o, sym_openEditor, 0, 0);
}

void
seqobj_track_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t name = fts_get_symbol(at + 0);
  fts_symbol_t type = fts_get_symbol(at + 1);

  sequence_add_track(this, name, fts_type_get_by_name(type));
}

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

  /* and delete it */
  sequence_remove_track(this, track_name);
}

void
seqobj_event_new(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t track_name = fts_get_symbol(at + 0);
  double time = fts_get_float(at + 1);
  fts_symbol_t selector = fts_get_symbol(at + 2);
  fts_object_t *evtobj;

  /* create event object */
  {
    fts_atom_t a[64];
    int i;

    fts_set_symbol(a + 0, sym_seqevent);    
    fts_set_float(a + 1, time);
    fts_set_symbol(a + 2, selector);
    
    for(i=3; i<ac; i++)
      a[i] = at[i];
    
    fts_object_new(0, ac, a, &evtobj);
  }

  /* add event to sequence */
  sequence_add_event(this, sequence_get_track_by_name(this, track_name), (sequence_event_t *)evtobj);

  /* create event at client */
  fts_client_upload(evtobj, sym_seqevent, ac - 1, at + 1);

  /* add event to sequence at client */
  {
    fts_atom_t a[2];

    fts_set_symbol(a + 0, track_name);
    fts_set_object(a + 1, evtobj);
    
    fts_client_send_message(o, sym_addEvent, 2, a);
  }
}

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
seqobj_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  /* print tracks */
  {
    int n_tracks = sequence_get_n_tracks(this);
    int i;
    
    post("sequence: %d tracks\n", n_tracks);
    
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
	sequence_event_post(evt);
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
      sym_openEditor = fts_new_symbol("openEditor");
      sym_destroyEditor = fts_new_symbol("destroyEditor");
      sym_addEvent = fts_new_symbol("addEvent");

      fts_class_init(cl, sizeof(sequence_t), 1, 0, 0); 

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqobj_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqobj_delete);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), seqobj_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("track_add"), seqobj_track_add);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("event_new"), seqobj_event_new);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("event_remove"), seqobj_event_remove);

      fts_method_define_varargs(cl, 0, fts_new_symbol("print"), seqobj_print);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
seqobj_config(void)
{
  sym_seqevent = fts_new_symbol("seqevent");

  fts_class_install(fts_new_symbol("sequence"), seqobj_instantiate);
  fts_class_install(sym_seqevent, seqevt_instantiate);
}








