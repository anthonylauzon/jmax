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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include <fts/fts.h>
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "event.h"

static fts_symbol_t sym_question = 0;

typedef struct _seqfind_
{
  fts_object_t head; /* sequence reference object */
  track_t *track;
  fts_array_t array;
} seqfind_t;

/************************************************************
 *
 *  user methods
 *
 */

static void 
seqfind_find(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;
  event_t *event = track_get_first(this->track);
	  
  while(event)
    {
      fts_atom_t *atoms;
      int n_atoms;
      int i;
      
      fts_array_clear(&this->array);
      event_get_array(event, &this->array);
      
      n_atoms = fts_array_get_size(&this->array);
      atoms = fts_array_get_atoms(&this->array);
      
      if(ac > n_atoms)
	ac = n_atoms;
      
      for(i=0; i<ac; i++)
	{
	  if(fts_is_symbol(at + i) && fts_get_symbol(at + i) == sym_question)
	    continue;
	  else if(fts_is_int(at + i) && fts_is_float(atoms + i) && fts_get_int(at + i) == (int)fts_get_float(atoms + i))
	    continue;
	  else if(fts_is_float(at + i) && fts_is_int(atoms + i) && fts_get_float(at + i) == (float)fts_get_int(atoms + i))
	    continue;
	  else if(fts_get_type(at + i) != fts_get_type(atoms + i) || fts_get_int(at + i) != fts_get_int(atoms + i))
	    break;
	}
      
      if(i == ac)
	{
	  fts_outlet_float(o, 1, (float)event_get_time(event));
	  fts_outlet_send(o, 0, fts_s_list, n_atoms, atoms);
	  break;
	}
      
      event = event_get_next(event);
    }
}

static void
seqfind_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;

  fts_object_release(this->track);
  this->track = track_atom_get(at);
  fts_object_refer(this->track);
}

/************************************************************
 *
 *  class
 *
 */

static void
seqfind_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;

  ac--;
  at++;

  this->track = 0;

  if(track_atom_is(at))
    {
      this->track = (track_t *)fts_get_object(at);
      fts_object_refer(this->track);
      
      fts_array_init(&this->array, 0, 0);
    }
  else
    fts_object_set_error(o, "Argument of event track required");
}

static void 
seqfind_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;

  if(this->track)
    {
      fts_object_release(this->track);
      fts_array_destroy(&this->array);
    }
}

static fts_status_t
seqfind_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqfind_t), 2, 2, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqfind_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqfind_delete);
  
  fts_method_define_varargs(cl, 0, fts_s_int, seqfind_find);
  fts_method_define_varargs(cl, 0, fts_s_float, seqfind_find);
  fts_method_define_varargs(cl, 0, fts_s_symbol, seqfind_find);
  fts_method_define_varargs(cl, 0, fts_s_list, seqfind_find);
  
  fts_method_define_varargs(cl, 1, seqsym_track, seqfind_set_reference);
  
  return fts_Success;
}

void
seqfind_config(void)
{
  sym_question = fts_new_symbol("?");
  fts_metaclass_install(fts_new_symbol("seqfind"), seqfind_instantiate, fts_arg_type_equiv);
}
