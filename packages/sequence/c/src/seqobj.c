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

typedef struct seqevt_ 
{
  fts_object_t o;
  sequence_event_t evt;
} seqevt_t;

void
seqevt_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqevt_t *this = (seqevt_t *)o;
  sequence_t *sequence = (sequence_t *)fts_get_object(at + 1);
  int track = fts_get_int(at + 2);
  double time = fts_get_float(at + 3);
  
  sequence_event_set(&this->evt, at[4]);
  sequence_add_event(sequence, sequence_get_track_by_index(sequence, track), time, &this->evt);
}

void
seqevt_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqevt_t *this = (seqevt_t *)o;

  sequence_remove_event(&this->evt);
}

static fts_status_t
seqevt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqevt_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqevt_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqevt_delete);
}

/******************************************************
 *
 *  object
 *
 */

typedef struct seqobj_ 
{
  fts_object_t o;
  sequence_t seq;
} seqobj_t;

void
seqobj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqobj_t *this = (seqobj_t *)o;

  sequence_init(&this->seq);
}

void
seqobj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqobj_t *this = (seqobj_t *)o;

  fts_client_message_send(o, sym_destroyEditor, 0, 0);
  sequence_empty(&this->seq);
}

/******************************************************
 *
 *  system methods
 *
 */

void
seqobj_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqobj_t *this = (seqobj_t *)o;

  fts_client_message_send(o, sym_openEditor, 0, 0);
}

void
seqobj_track_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqobj_t *this = (seqobj_t *)o;
  fts_symbol_t name = fts_get_symbol(at + 0);
  fts_symbol_t type = fts_get_symbol(at + 1);

  sequence_add_track(&this->seq, name, fts_type_get_by_name(type));
}

void
seqobj_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqobj_t *this = (seqobj_t *)o;
  int n_tracks = sequence_get_n_tracks(&this->seq);
  int i;
  
  post("sequence: %d tracks\n", n_tracks);

  for(i=0; i<n_tracks; i++)
    {
      sequence_track_t *track = sequence_get_track_by_index(&this->seq, i);
      fts_symbol_t name = sequence_track_get_name(track);
      fts_type_t type = sequence_track_get_type(track);
      fts_symbol_t type_name = fts_type_get_selector(type);
      
      post("  %d: %s (%s)\n", i, fts_symbol_name(name), fts_symbol_name(type_name));
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

      fts_class_init(cl, sizeof(seqobj_t), 1, 0, 0); 

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
  fts_class_install(fts_new_symbol("sequence"), seqobj_instantiate);
  fts_class_install(fts_new_symbol("seqevt"), seqevt_instantiate);
}

