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
  sequence_t *sequence = (sequence_t *)fts_get_object(at + 0);
  fts_symbol_t track_name = fts_get_symbol(at + 1);
  double time = fts_get_float(at + 2);
  fts_symbol_t selector = fts_get_symbol(at + 3);

  sequence_event_set_time(this, time);
  sequence_event_set_value(this, selector, ac - 4, at + 4);

  sequence_add_event(sequence, sequence_get_track_by_name(sequence, track_name), this);
}

void
seqevt_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_event_t *this = (sequence_event_t *)o;

  sequence_event_reset_value(this);
}

void
seqevt_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_event_t *this = (sequence_event_t *)o;
  sequence_track_t *track = sequence_event_get_track(this);
  fts_atom_t a[64];  
  int i;

  fts_set_object(a + 0, (fts_object_t *)sequence_track_get_sequence(track));
  fts_set_symbol(a + 1, sequence_track_get_name(track));
  fts_set_float(a + 2, sequence_event_get_time(this));
  fts_set_symbol(a + 3, this->s);

  for(i=0; i<this->ac; i++)
    a[i + 4] = this->at[i];

  /* send event to client */
  fts_client_upload(o, sym_seqevent, ac, at);
}

static fts_status_t
seqevt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sequence_event_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("upload"), seqevt_upload);

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
