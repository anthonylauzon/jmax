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
#include "track.h"

typedef struct _seqstep_
{
  fts_object_t head;
  track_t *track;
  event_t *prev;
  event_t *next;
  fts_array_t array;
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
      event_t *next = track_get_next_and_highlight(this->track, event, time);
      
      fts_object_release(event);

      if(this->prev)
	fts_object_release(this->prev);

      this->prev = event_get_prev(event);
      this->next = next;

      if(this->prev)
	fts_object_refer(this->prev);

      if(this->next)
	fts_object_refer(this->next);
      
      fts_outlet_float(o, 1, (float)time);

      do{
	fts_array_clear(&this->array);
	event_append_state_to_array(event, &this->array);
	
	fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->array), fts_array_get_atoms(&this->array));

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
      event_t *prev = track_get_prev_and_highlight(this->track, event, time);
      
      fts_object_release(event);

      if(this->next)
	fts_object_release(this->next);

      this->prev = prev;
      this->next = event_get_next(event);
      
      if(this->prev)
	fts_object_refer(this->prev);

      if(this->next)
	fts_object_refer(this->next);
      
      fts_outlet_float(o, 1, (float)time);

      do{
	fts_array_clear(&this->array);
	event_append_state_to_array(event, &this->array);
	
	fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->array), fts_array_get_atoms(&this->array));

	event = event_get_prev(event);
      }
      while(event != prev);
    }
}

static void 
seqstep_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;

  if(this->prev)
    {
      fts_object_release(this->prev);
      this->prev = 0;
    }
 
  if(this->next)
    {
      fts_object_release(this->next);
      this->next = 0;
    }
}

static void 
seqstep_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;
  event_t *event;
  double locate;
  double time;

  seqstep_stop(o, 0, 0, 0, 0);
  
  if(ac && fts_is_number(at))
    locate = fts_get_number_float(at);
  else
    locate = 0.0;

  event = track_get_event_by_time(this->track, locate);
  
  if(event)
    {
      time = event_get_time(event);
      
      if(locate == time)
	{
	  this->next = event;
	  this->prev = event;

	  fts_object_refer(event);
	  fts_object_refer(event);

	  seqstep_next(o, 0, 0, 0, 0);
	}
      else
	{
	  this->next = event;
	  this->prev = event_get_prev(event);

	  fts_object_refer(event);

	  if(this->prev)
	    fts_object_refer(this->prev);
	}
    }
}

static void
seqstep_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqstep_t *this = (seqstep_t *)o;

  seqstep_stop(o, 0, 0, 0, 0);

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
seqstep_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;

  ac--;
  at++;

  this->prev = 0;
  this->next = 0;

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
seqstep_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqstep_t *this = (seqstep_t *)o;

  if(this->track)
    {
      fts_object_release(this->track);
      fts_array_destroy(&this->array);
    }
}

static fts_status_t
seqstep_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqstep_t), 2, 2, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqstep_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqstep_delete);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), seqstep_locate);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqstep_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("prev"), seqstep_prev);
  fts_method_define_varargs(cl, 0, fts_new_symbol("next"), seqstep_next);
  
  fts_method_define_varargs(cl, 1, seqsym_track, seqstep_set_reference);
  
  return fts_Success;
}

void
seqstep_config(void)
{
  fts_metaclass_install(fts_new_symbol("seqstep"), seqstep_instantiate, fts_arg_type_equiv);
}
