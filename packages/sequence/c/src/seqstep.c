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

typedef struct _seqstep_
{
  fts_object_t o;
  sequence_t *sequence;
  int index;
  eventtrk_t *track;
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
      event_t *next = event_get_next(event);
      double time = event_get_time(event);
      fts_atom_t a[2];
      fts_atom_t atoms[64];
      int n_atoms;
  
      fts_set_object(atoms, (fts_object_t *)event);
      n_atoms = 1;
      
      while(next && event_get_time(next) == time)
	{
	  fts_set_object(atoms + n_atoms, (fts_object_t *)next);
	  n_atoms++;
	  next = event_get_next(next);
	}
      
      this->prev = event_get_prev(event);
      this->next = next;
      
      if(sequence_editor_is_open(this->sequence))
	fts_client_send_message((fts_object_t *)this->track, seqsym_highlightEvents, n_atoms, atoms);

      fts_outlet_float(o, 1, (float)time);

      fts_set_ptr(a, &n_atoms);
      fts_set_ptr(a + 1, atoms);
      
      do{
	fts_send_message((fts_object_t *)event, fts_SystemInlet, seqsym_get_atoms, 2, a);
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
      event_t *prev = event_get_prev(event);
      double time = event_get_time(event);
      fts_atom_t a[2];
      fts_atom_t atoms[64];
      int n_atoms;

      fts_set_object(atoms, (fts_object_t *)event);
      n_atoms = 1;
      
      while(prev && event_get_time(prev) == time)
	{
	  fts_set_object(atoms + n_atoms, (fts_object_t *)prev);
	  n_atoms++;
	  prev = event_get_prev(prev);
	}
      
      this->prev = prev;
      this->next = event_get_next(event);
      
      if(sequence_editor_is_open(this->sequence))
	fts_client_send_message((fts_object_t *)this->track, seqsym_highlightEvents, n_atoms, atoms);
      
      fts_outlet_float(o, 1, (float)time);

      fts_set_ptr(a, &n_atoms);
      fts_set_ptr(a + 1, atoms);
      
      do{
	fts_send_message((fts_object_t *)event, fts_SystemInlet, seqsym_get_atoms, 2, a);
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

  if(this->track)
    {
      track_unlock((track_t *)this->track);

      this->track = 0;
      this->prev = 0;
      this->next = 0;
    }
}

static void 
seqstep_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;
  double locate;

  seqstep_stop(o, 0, 0, 0, 0);
  
  if(ac && fts_is_number(at))
    locate = fts_get_number_float(at);
  else
    locate = 0.0;

  if(this->sequence)
    {
      eventtrk_t *track = (eventtrk_t *)sequence_get_track_by_index(this->sequence, this->index);
      
      if(track)
	{
	  event_t *event = eventtrk_get_event_by_time(track, locate);
	  
	  if(event)
	    {
	      double time = event_get_time(event);

	      track_lock((track_t *)track);
	  
	      this->track = track;

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
}

static void 
seqstep_set_sequence(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;

  if(ac && fts_is_object(at))
    {
      fts_object_t *seqobj = fts_get_object(at);
      
      if(fts_object_get_class_name(seqobj) == seqsym_sequence)
	this->sequence = (sequence_t *)seqobj;
      else
	this->sequence = 0;
    }
}

static void 
seqstep_set_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;

  this->index = fts_get_int(at);
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
  fts_object_t *seqobj = fts_get_object(at + 1);
  int index = fts_get_int(at + 2);

  if(fts_object_get_class_name(seqobj) == seqsym_sequence)
    this->sequence = (sequence_t *)seqobj;
  else
    this->sequence = 0;

  this->index = index;
  this->track = 0;
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
      fts_class_init(cl, sizeof(seqstep_t), 3, 2, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqstep_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqstep_delete);

      fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), seqstep_locate);
      fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqstep_stop);
      fts_method_define_varargs(cl, 0, fts_new_symbol("prev"), seqstep_prev);
      fts_method_define_varargs(cl, 0, fts_new_symbol("next"), seqstep_next);

      fts_method_define_varargs(cl, 1, fts_s_object, seqstep_set_sequence);
      fts_method_define_varargs(cl, 2, fts_s_symbol, seqstep_set_index);

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
