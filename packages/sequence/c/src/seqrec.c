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

typedef struct _seqrec_
{
  fts_object_t o;
  sequence_t *sequence;
  int index;
  fts_class_t *class;
  eventtrk_t *track;
  event_t *event;
  double start_location;
  double start_time;
} seqrec_t;

/************************************************************
 *
 *  user methods
 *
 */

static void 
seqrec_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;

  if(this->track)
    {
      /* upload and unlock track after recording */
      /*fts_send_message((fts_object_t *)this->track, fts_SystemInlet, fts_s_upload, 0, 0);*/
      track_unlock((track_t *)this->track);

      this->track = 0;
      this->event = 0;
      this->start_time = 0.0;
    }
}

static void 
seqrec_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;
  double locate;

  seqrec_stop(o, 0, 0, 0, 0);
  
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
	  fts_symbol_t type = eventtrk_get_type(track);
	  
	  if(type == seqsym_int || type == seqsym_float || type == seqsym_symbol)
	    this->class = 0;
	  else
	    this->class = fts_class_get_by_name(type);

	  this->track = track;
	  this->event = event;
	  
	  this->start_location = locate;
	  this->start_time = 0.0;

	  track_lock((track_t *)track);
	}
    }
}

static void 
seqrec_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;

  if(this->start_time == 0.0)
    {
      double now = fts_get_time_in_msecs();    
      
      if(!this->track)
	seqrec_locate(o, 0, 0, 0, 0);
      
      if(this->track)
	this->start_time = now;
    }
}

static void 
seqrec_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;
  double now = fts_get_time_in_msecs();
      
  this->start_location += now - this->start_time;
}

static void 
seqrec_set_sequence(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;

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
seqrec_set_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;

  this->index = fts_get_int(at);
}

static void 
seqrec_record(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqrec_t *this = (seqrec_t *)o;

  if(this->start_time != 0.0)
    {
      double time = fts_get_time_in_msecs() - this->start_time + this->start_location;
      event_t *event = 0;

      if(this->class)
	{
	  fts_object_t *obj = fts_object_create(this->class, ac, at);
	  fts_atom_t a[1];

	  fts_set_object(a, obj);
	  event = (event_t *)fts_object_create(event_class, 1, a);
	}
      else
	event = (event_t *)fts_object_create(event_class, 1, at);
      
      /* add event to track (look for right position starting from last event) */
      eventtrk_add_event_after(this->track, time, event, this->event);

      if(sequence_editor_is_open(this->sequence))
	{
	  fts_atom_t a[1];

	  event_upload(event);

	  fts_set_object(a, (fts_object_t *)event);    
	  fts_client_send_message((fts_object_t *)this->track, seqsym_addEvents, 1, a);
	}

      this->event = event;
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
seqrec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;
  fts_object_t *seqobj = fts_get_object(at + 1);
  int index = fts_get_int(at + 2);

  if(fts_object_get_class_name(seqobj) == seqsym_sequence)
    this->sequence = (sequence_t *)seqobj;
  else
    this->sequence = 0;

  this->index = index;
  this->track = 0;
  this->event = 0;
  this->start_location = 0.0;
  this->start_time = 0.0;
}

static void 
seqrec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;
}

static fts_status_t
seqrec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 2 && fts_is_symbol(at) && fts_is_object(at + 1) && fts_is_int(at + 2))
    {
      fts_class_init(cl, sizeof(seqrec_t), 3, 0, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqrec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqrec_delete);

      fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), seqrec_locate);
      fts_method_define_varargs(cl, 0, fts_new_symbol("start"), seqrec_start);
      fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), seqrec_pause);
      fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqrec_stop);

      fts_method_define_varargs(cl, 0, fts_s_int, seqrec_record);
      fts_method_define_varargs(cl, 0, fts_s_float, seqrec_record);
      fts_method_define_varargs(cl, 0, fts_s_symbol, seqrec_record);
      fts_method_define_varargs(cl, 0, fts_s_list, seqrec_record);

      fts_method_define_varargs(cl, 1, fts_s_object, seqrec_set_sequence);
      fts_method_define_varargs(cl, 2, fts_s_symbol, seqrec_set_index);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;    
}

void
seqrec_config(void)
{
  fts_metaclass_install(fts_new_symbol("seqrec"), seqrec_instantiate, fts_arg_type_equiv);
}
