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
#include "eventtrk.h"
#include "seqref.h"

typedef struct _seqplay_
{
  seqref_t o; /* sequence reference object */
  event_t *event;
  double start_location;
  double start_time;
  fts_alarm_t alarm;
} seqplay_t;

/************************************************************
 *
 *  user methods
 *
 */

static void 
seqplay_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(this->event)
    {
      fts_alarm_unarm(&this->alarm);

      seqref_unlock(o);

      this->event = 0;
    }
}

static void 
seqplay_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  double locate;
  eventtrk_t *track;

  seqplay_stop(o, 0, 0, 0, 0);
  
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
	  seqref_lock(o, track);
	  
	  this->event = event;
	  
	  this->start_location = locate;      
	}
    }
}

static void 
seqplay_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(!fts_alarm_is_armed(&this->alarm))
    {
      double now = fts_get_time_in_msecs();    
      
      if(!this->event)
	seqplay_locate(o, 0, 0, 0, 0);
      
      if(this->event)
	{
	  this->start_time = now;
	  
	  fts_alarm_set_time(&this->alarm, now + event_get_time(this->event) - this->start_location);
	  fts_alarm_arm(&this->alarm);
	}
    }
}

static void 
seqplay_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  double now = fts_get_time_in_msecs();
      
  this->start_location += now - this->start_time;
  fts_alarm_unarm(&this->alarm);
}

static void 
seqplay_sync(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  event_t *event = this->event;

  if(event && ac && fts_is_number(at))
    {
      double time = fts_get_number_float(at);
      int n_atoms;
      fts_atom_t atoms[64];

      while(event && time >= event_get_time(event))
	{
	  event_get_atoms(event, &n_atoms, atoms);
	  fts_outlet_send(o, 0, fts_s_list, n_atoms, atoms);
	  
	  event = event_get_next(event);
	}

      if(event)
	{
	  this->event = event;
	  this->start_time = fts_get_time_in_msecs();
	  this->start_location = time;

	  if(fts_alarm_is_armed(&this->alarm))
	    fts_alarm_set_time(&this->alarm, fts_get_time_in_msecs() + event_get_time(this->event) - time);
	}
      else
	seqplay_stop(o, 0, 0, 0, 0);
    }
}

static void
seqplay_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;

  seqplay_stop(o, 0, 0, 0, 0);
  seqref_set_reference(o, ac, at);
}

/************************************************************
 *
 *  alarm
 *
 */

static void
seqplay_alarm_tick(fts_alarm_t *alarm, void *o)
{
  seqplay_t *this = (seqplay_t *)o;
  double current_location = fts_get_time_in_msecs() - this->start_time + this->start_location;
  event_t *event = this->event;
  event_t *next;

  /* highlight current group and get next event (event after current_location) */
  next = seqref_get_next_and_highlight(o, event, current_location);

  if(next)
    {
      this->event = next;
      fts_alarm_set_time(alarm, this->start_time + event_get_time(next) - this->start_location);
      fts_alarm_arm(alarm);
    }
  else
    seqplay_stop(o, 0, 0, 0, 0);

  {
    int ac;
    fts_atom_t at[64];
    
    do{
      event_get_atoms(event, &ac, at);

      fts_outlet_send((fts_object_t *)o, 0, fts_s_list, ac, at);
      
      event = event_get_next(event);
    }
    while(event != next);
  }
}

/************************************************************
 *
 *  class
 *
 */

static void
seqplay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  seqref_init(o, ac, at);

  this->event = 0;
  this->start_location = 0.0;
  this->start_time = 0.0;

  fts_alarm_init(&this->alarm, 0, seqplay_alarm_tick, this);
}

static void 
seqplay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  fts_alarm_unarm(&this->alarm);
}

static fts_status_t
seqplay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 2 && fts_is_symbol(at) && fts_is_object(at + 1) && fts_is_int(at + 2))
    {
      fts_class_init(cl, sizeof(seqplay_t), 2, 1, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqplay_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqplay_delete);

      fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), seqplay_locate);
      fts_method_define_varargs(cl, 0, fts_new_symbol("start"), seqplay_start);
      fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), seqplay_pause);
      fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqplay_stop);
      fts_method_define_varargs(cl, 0, fts_new_symbol("sync"), seqplay_sync);

      fts_method_define_varargs(cl, 1, fts_s_list, seqplay_set_reference);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;    
}

void
seqplay_config(void)
{
  fts_metaclass_install(fts_new_symbol("seqplay"), seqplay_instantiate, fts_arg_type_equiv);
}
