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

#include "fts.h"
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "event.h"
#include "eventtrk.h"
#include "seqref.h"

static fts_symbol_t sym_question = 0;

typedef struct _seqfind_
{
  seqref_t o; /* sequence reference object */
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
  eventtrk_t *track = seqref_get_reference(o);

  if(track)
    {
      event_t *event = eventtrk_get_first(track);
      fts_atom_t a[2];
      fts_atom_t atoms[64];
      int n_atoms;
	  
      fts_set_ptr(a, &n_atoms);
      fts_set_ptr(a + 1, atoms);
	  
      while(event)
	{
	  int i;

	  event_get_atoms(event, &n_atoms, atoms);
	      
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
}

static void
seqfind_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;

  seqref_set_reference(o, ac, at);
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

  seqref_init(o, ac, at);
}

static void 
seqfind_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;
}

static fts_status_t
seqfind_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 2 && fts_is_symbol(at) && fts_is_object(at + 1) && fts_is_int(at + 2))
    {
      fts_class_init(cl, sizeof(seqfind_t), 2, 2, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqfind_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqfind_delete);

      fts_method_define_varargs(cl, 0, fts_s_int, seqfind_find);
      fts_method_define_varargs(cl, 0, fts_s_float, seqfind_find);
      fts_method_define_varargs(cl, 0, fts_s_symbol, seqfind_find);
      fts_method_define_varargs(cl, 0, fts_s_list, seqfind_find);

      fts_method_define_varargs(cl, 1, fts_s_list, seqfind_set_reference);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;    
}

void
seqfind_config(void)
{
  sym_question = fts_new_symbol("?");
  fts_metaclass_install(fts_new_symbol("seqfind"), seqfind_instantiate, fts_arg_type_equiv);
}
