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

typedef struct _seqstep_
{
  seqref_t o; /* sequence reference object */
  event_t *prev;
  event_t *next;
} seqstep_t;

/************************************************************
 *
 *  user methods
 *
 */

static void
seqstep_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqstep_t *this = (seqstep_t *)o;
  event_t *event = this->next;

  if(event)
    {
      double time = event_get_time(event);
      event_t *next = seqref_get_next_and_highlight(o, event, time);
      
      this->prev = event_get_prev(event);
      this->next = next;
      
      fts_outlet_float(o, 1, (float)time);

      do{
	fts_atom_t atoms[64];
	int n_atoms;

	event_get_atoms(event, &n_atoms, atoms);
	fts_outlet_send(o, 0, fts_s_list, n_atoms, atoms);	
	event = event_get_next(event);
      }
      while(event != next);
    }
}

static void
seqstep_prev(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqstep_t *this = (seqstep_t *)o;
  event_t *event = this->prev;

  if(event)
    {
      double time = event_get_time(event);
      event_t *prev = seqref_get_prev_and_highlight(o, event, time);
      
      this->prev = prev;
      this->next = event_get_next(event);
      
      fts_outlet_float(o, 1, (float)time);

      do{
	fts_atom_t atoms[64];
	int n_atoms;

	event_get_atoms(event, &n_atoms, atoms);
      	fts_outlet_send((fts_object_t *)o, 0, fts_s_list, n_atoms, atoms);
	event = event_get_prev(event);
      }
      while(event != prev);
    }
}

static void 
seqstep_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;

  if(seqref_is_locked(o))
    {
      seqref_unlock(o);

      this->prev = 0;
      this->next = 0;
    }
}

static void 
seqstep_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;
  eventtrk_t *track;
  double locate;

  seqstep_stop(o, 0, 0, 0, 0);
  
  if(ac && fts_is_number(at))
    locate = fts_get_number_float(at);
  else
    locate = 0.0;

  track = seqref_get_reference(o);

  if(track)
    {
      event_t *event = eventtrk_get_event_by_time(track, locate);
      
      if(event)
	{
	  double time = event_get_time(event);
	  
	  seqref_lock(o, track);
	  
	  if(locate == time)
	    {
	      this->next = event;
	      this->prev = event;
	      seqstep_next(o, 0, 0, 0, 0);
	    }
	  else
	    {
	      this->next = event;
	      this->prev = event_get_prev(event);
	    }
	}
    }
}

static void
seqstep_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqstep_t *this = (seqstep_t *)o;

  seqstep_stop(o, 0, 0, 0, 0);
  seqref_set_reference(o, ac, at);
}

/************************************************************
 *
 *  class
 *
 */

static void
seqstep_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;

  seqref_init(o, ac, at);

  this->prev = 0;
  this->next = 0;
}

static void 
seqstep_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;
}

static fts_status_t
seqstep_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 2 && fts_is_symbol(at) && fts_is_object(at + 1) && fts_is_int(at + 2))
    {
      fts_class_init(cl, sizeof(seqstep_t), 2, 2, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqstep_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqstep_delete);

      fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), seqstep_locate);
      fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqstep_stop);
      fts_method_define_varargs(cl, 0, fts_new_symbol("prev"), seqstep_prev);
      fts_method_define_varargs(cl, 0, fts_new_symbol("next"), seqstep_next);

      fts_method_define_varargs(cl, 1, fts_s_list, seqstep_set_reference);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;    
}

void
seqstep_config(void)
{
  fts_metaclass_install(fts_new_symbol("seqstep"), seqstep_instantiate, fts_arg_type_equiv);
}
