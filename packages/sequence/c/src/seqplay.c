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

typedef struct _seqplay_
{
  fts_object_t head;
  enum {status_reset, status_ready, status_playing} status;
  track_t *track;
  event_t *event;
  double start_location;
  double start_time;
  fts_timer_t *timer;
  fts_array_t array;
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

  if(this->status == status_playing)
    fts_timer_reset(this->timer);

  if(this->event)
    {
      fts_object_release(this->event);
      this->event = 0;
    }

  this->status = status_reset;
}

static void 
seqplay_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  event_t *event;
  double locate;

  seqplay_stop(o, 0, 0, 0, 0);
  
  if(ac && fts_is_number(at))
    locate = fts_get_number_float(at);
  else
    locate = 0.0;

  event = track_get_event_by_time(this->track, locate);
      
  if(event)
    {
      this->event = event;
      fts_object_refer(this->event);
      
      this->start_location = locate;
      this->status = status_ready;
    }
}

static void 
seqplay_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(this->status != status_playing)
    {
      double now = fts_get_time();    
      
      if(!this->event)
	seqplay_locate(o, 0, 0, 0, 0);
      
      if(this->event)
	{
	  this->start_time = now;	  
	  fts_timer_set_alarm(this->timer, now + event_get_time(this->event) - this->start_location, 0);
	  this->status = status_playing;
	}
    }
}

static void 
seqplay_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(this->status == status_playing)
    {
      double now = fts_get_time();
      
      this->start_location += now - this->start_time;
      fts_timer_reset(this->timer);

      this->status = status_ready;
    }
}

static void 
seqplay_sync(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  event_t *event = this->event;

  if(event && ac && fts_is_number(at))
    {
      double time = fts_get_number_float(at);

      fts_object_release(this->event);

      while(event && time >= event_get_time(event))
	{
	  fts_array_clear(&this->array);
	  event_append_state_to_array(event, &this->array);

	  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->array), fts_array_get_atoms(&this->array));
	  
	  event = event_get_next(event);
	}

      if(event)
	{
	  this->event = event;
	  fts_object_refer(event);
	  
	  if(this->status == status_playing)
	    {
	      /* restart at new position */
	      this->start_time = fts_get_time();
	      this->start_location = time;
	      
	      fts_timer_set_alarm(this->timer, this->start_time + event_get_time(this->event) - time, 0);
	    }
	}
      else
	this->status = status_reset;
    }
}

static void
seqplay_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;

  seqplay_stop(o, 0, 0, 0, 0);

  fts_object_release(this->track);
  this->track = track_atom_get(at);
  fts_object_refer(this->track);
}

/************************************************************
 *
 *  alarm
 *
 */

static void
seqplay_output_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;
  double current_location = fts_get_time() - this->start_time + this->start_location;
  event_t *event = this->event;
  event_t *next;

  /* highlight current group and get next event (event after current_location) */
  next = track_get_next_and_highlight(this->track, event, current_location);

  if(next)
    {
      fts_object_release(this->event);
      this->event = next;
      fts_object_refer(this->event);

      fts_timer_set_alarm(this->timer, this->start_time + event_get_time(next) - this->start_location, 0);
    }
  else
    seqplay_stop(o, 0, 0, 0, 0);

  {
    do{
      fts_array_clear(&this->array);
      event_append_state_to_array(event, &this->array);

      fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->array), fts_array_get_atoms(&this->array));
      
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
  
  ac--;
  at++;

  this->status = status_reset;
  this->track = 0;
  this->event = 0;
  this->start_location = 0.0;
  this->start_time = 0.0;

  if(track_atom_is(at))
    {
      this->track = (track_t *)fts_get_object(at);
      fts_object_refer(this->track);
      
      this->timer = fts_timer_new(o, 0);
      fts_array_init(&this->array, 0, 0);
    }
  else
    fts_object_set_error(o, "Argument of event track required");
}

static void 
seqplay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(this->track)
    {
      fts_object_release(this->track);
      fts_timer_delete(this->timer);
      fts_array_destroy(&this->array);
    }
}

static fts_status_t
seqplay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqplay_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqplay_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqplay_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_timer_alarm, seqplay_output_next);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), seqplay_locate);
  fts_method_define_varargs(cl, 0, fts_new_symbol("start"), seqplay_start);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), seqplay_pause);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqplay_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("sync"), seqplay_sync);
  
  fts_method_define_varargs(cl, 1, seqsym_track, seqplay_set_reference);
  
  return fts_Success;
}

void
seqplay_config(void)
{
  fts_metaclass_install(fts_new_symbol("seqplay"), seqplay_instantiate, fts_arg_type_equiv);
}
