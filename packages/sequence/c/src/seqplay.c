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
#include "event.h"
#include "eventtrk.h"

#define NO_FILTER -1

/************************************************************
 *
 *  object
 *
 */

typedef struct _seqplay_
{
  fts_object_t o;
  sequence_t *sequence;
  fts_symbol_t track_name;
  eventtrk_t *track;
  event_t *event;
  fts_alarm_t alarm;
} seqplay_t;

static void seqplay_alarm_tick(fts_alarm_t *alarm, void *o);

static void
seqplay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  fts_object_t *seqobj = fts_get_object(at + 1);
  fts_symbol_t track_name = fts_get_symbol(at + 2);

  if(fts_object_get_class_name(seqobj) == seqsym_sequence)
    this->sequence = (sequence_t *)seqobj;
  else
    this->sequence = 0;

  this->track_name = track_name;
  this->track = 0;
  this->event = 0;

  fts_alarm_init(&this->alarm, 0, seqplay_alarm_tick, this);
}

static void 
seqplay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  fts_alarm_unarm(&this->alarm);
}

/************************************************************
 *
 *  Alarm
 *
 */

static void
seqplay_alarm_tick(fts_alarm_t *alarm, void *o)
{
  seqplay_t *this = (seqplay_t *)o;
  double now = fts_get_time_in_msecs();
  event_t *event = this->event;
  event_t *next = event_get_next(event);
  fts_atom_t at[64];
  int ac;
  fts_atom_t a[2];

  fts_set_object(at, (fts_object_t *)event);
  ac = 1;

  while(next && event_get_time(next) <= now)
    {
      fts_set_object(at + ac, (fts_object_t *)event);
      ac++;

      next = event_get_next(next);
    }

  if(next)
    {
      /* playing */
      this->event = next;
      fts_alarm_set_delay(alarm, event_get_time(next) - now);
      fts_alarm_arm(alarm);
    }
  else
    {
      /* stop */
      fts_send_message((fts_object_t *)this->track, fts_SystemInlet, seqsym_unlock, 0, 0);            
      this->track = 0;
      this->event = 0;
    }

  if(sequence_editor_is_open(this->sequence))
     fts_client_send_message((fts_object_t *)this->track, seqsym_highlightEvents, ac, at);

  fts_set_ptr(a, &ac);
  fts_set_ptr(a + 1, at);

  do{
    fts_send_message((fts_object_t *)event, fts_SystemInlet, seqsym_get_atoms, 2, a);
    fts_outlet_send((fts_object_t *)o, 0, fts_s_list, ac, at);
    
    event = event_get_next(event);
  }
  while(event != next);
}



/************************************************************
 *
 *  user methods
 *
 */

static void 
seqplay_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(this->sequence)
    {
      eventtrk_t *track = (eventtrk_t *)sequence_get_track_by_name(this->sequence, this->track_name);
      
      if(track)
	{
	  event_t *event = eventtrk_get_first(track);
	  
	  if(event)
	    {
	      fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_lock, 0, 0);
	  
	      this->track = track;
	      this->event = event;

	      fts_alarm_set_delay(&this->alarm, event_get_time(event));
	      fts_alarm_arm(&this->alarm);
	    }
	}
    }
}

static void 
seqplay_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(this->track)
    {
      fts_alarm_unarm(&this->alarm);
      fts_send_message((fts_object_t *)this->track, fts_SystemInlet, seqsym_unlock, 0, 0);      

      this->track = 0;
      this->event = 0;
    }
}

/************************************************************
 *
 *  class
 *
 */

static fts_status_t
seqplay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 2 && fts_is_symbol(at) && fts_is_object(at + 1) && fts_is_symbol(at + 2))
    {
      fts_class_init(cl, sizeof(seqplay_t), 2, 1, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqplay_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqplay_delete);

      fts_method_define_varargs(cl, 0, fts_new_symbol("start"), seqplay_start);
      fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqplay_stop);

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
