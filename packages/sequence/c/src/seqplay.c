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

typedef struct _seqplay_
{
  fts_object_t head;
  fts_timebase_t *timebase; /* underlying timebase */
  track_t *track; /* the track to play */

  /* status */
  event_t *event; /* current event */
  enum {status_reset, status_ready, status_playing} status;
  double last_time;
  double last_location; /* start time */
  double last_speed;
  double speed; /* set speed */
  int sync_speed;
} seqplay_t;

static void
seqplay_reset(seqplay_t *this)
{
  if(this->status == status_playing || this->sync_speed)
    fts_timebase_remove_object(this->timebase, (fts_object_t *)this);

  /* release event */
  if(this->event)
    {
      fts_object_release(this->event);
      this->event = 0;
    }

  /* reset status */
  this->status = status_reset;
}

static void
seqplay_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;
  event_t *event = this->event;
  double time = event_get_time(event);
  event_t *next = track_highlight_and_next(this->track, event);
  
  /* release current event */
  fts_object_release(event);
  this->event = 0;
  
  if(track_is_active(this->track))
    {
      while(event && event != next)
	{
	  fts_atom_t *a = event_get_value(event);
	  
	  fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
	  event = event_get_next(event);
	}
    }
  
  if(next)
    {
      double speed = this->last_speed;
      
      /* claim next event */
      this->event = next;
      fts_object_refer(next);
      
      /* schedule next output */
      fts_timebase_add_call(this->timebase, o, seqplay_next, 0, (event_get_time(next) - time) / speed);
    }
  else
    {
      seqplay_reset(this);  
      fts_outlet_bang(o, 1);
    }
}

static void
seqplay_sync_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;
  double time = fts_timebase_get_time(this->timebase);
  double speed = this->speed;
  double here = this->last_location + (time - this->last_time) * this->last_speed;

  this->sync_speed = 0;

  /* remove scheduled event */
  fts_timebase_remove_object(this->timebase, o);

  /* set time and speed */
  this->last_location = here;
  this->last_time = time;
  this->last_speed = speed;

  /* re-schedule event */
  if(track_is_active(this->track))
    fts_timebase_add_call(this->timebase, o, seqplay_next, 0, (event_get_time(this->event) - here) / speed);
}

static void 
seqplay_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  double time = 0.0;
  event_t *event;
      
  if(this->status > status_reset)
    seqplay_reset(this);  

  if(ac > 0 && fts_is_number(at))
    time = fts_get_number_float(at);
  
  /* locate */
  event = track_get_event_by_time(this->track, time);
  
  if(event)
    {
      /* claim event */
      this->event = event;
      fts_object_refer(event);
      
      /* set start time */
      this->last_location = time;

      this->status = status_ready;
    }
}

static void 
seqplay_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  
  if(track_is_active(this->track) && this->speed > 0.0)
    {
      switch(this->status)
	{
	case status_reset:
	  /* locate */
	  seqplay_locate(o, 0, 0, ac, at);

	  if(this->status != status_ready)
	    return;
	  
	case status_ready:
	  /* set start time and speed */
	  this->last_time = fts_timebase_get_time(this->timebase);
	  this->last_speed = this->speed;

	  /* schedule event */
	  fts_timebase_add_call(this->timebase, o, seqplay_next, 0, 
				(event_get_time(this->event) - this->last_location) / this->speed);
	  
	  /* play */
	  this->status = status_playing;
	  
	case status_playing:
	  break;
	}
    }
}

static void 
seqplay_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(this->status == status_playing)
    {
      double time = fts_timebase_get_time(this->timebase);
      double here = this->last_location + (time - this->last_time) * this->last_speed;

      /* stop scheduled event */
      fts_timebase_remove_object(this->timebase, (fts_object_t *)this);

      /* remind current time */
      this->last_location = here;
      this->status = status_ready;
    }
}

static void 
seqplay_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  seqplay_reset(this);
}

static void 
seqplay_sync(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(fts_is_number(at))
    {
      double sync = fts_get_number_float(at);
      double time = fts_timebase_get_time(this->timebase);
      double here = (time - this->last_time) * this->last_speed;

      if(sync > here)
	{
	  event_t *event = this->event;

	  if(this->status == status_playing)
	    {
	      fts_timebase_remove_object(this->timebase, (fts_object_t *)this);
	      this->last_time = time;
	      this->last_speed = this->speed;
	    }
	  
	  /* release current event */
	  fts_object_release(event);
	  this->event = 0;
	  
	  while(event && event_get_time(event) <= here)
	    {
	      fts_atom_t *a = event_get_value(event);

	      if(track_is_active(this->track))
		fts_outlet_send(o, 0, fts_get_selector(a), 1, a);

	      event = event_get_next(event);
	    }
	  
	  if(event)
	    {
	      double speed = this->last_speed;
	      
	      /* claim next event */
	      this->event = event;
	      fts_object_refer(event);
	      
	      /* schedule next output */
	      fts_timebase_add_call(this->timebase, o, seqplay_next, 0, (event_get_time(event) - time) / speed);
	    }
	  else
	    seqplay_reset(this);  
	}
    }
}

static void
seqplay_set_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;
  double speed = 0.0;

  if(fts_is_number(at))
    speed = fts_get_number_float(at);

  this->speed = speed;

  if(speed <= 0.0)
    seqplay_pause(o, 0, 0, 0, 0);
  else if(this->status == status_playing)
    {
      /* schedule speed sync call */
      if(!this->sync_speed)
	{
	  fts_timebase_add_call(this->timebase, o, seqplay_sync_speed, 0, fts_timebase_get_step(this->timebase));
	  this->sync_speed = 1;
	}
    }
  else
    this->last_speed = speed;
}

static void
seqplay_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;

  seqplay_reset(this);

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
seqplay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  fts_timebase_t *pseudo = (fts_timebase_t *)o;
  
  ac--;
  at++;

  fts_timebase_init(pseudo); /* init local (pseudo) timebase */

  this->timebase = fts_get_timebase();

  this->track = 0;
  this->event = 0;

  this->status = status_reset;
  this->last_time = 0.0;
  this->last_location = 0.0;
  this->last_speed = 0.0;
  this->speed = 1.0;
  this->sync_speed = 0;

  if(ac == 2 && fts_is_number(at))
    {
      seqplay_set_speed(o, 0, 0, 1, at);

      /* skip speed argument */
      ac--;
      at++;
    }

  if(ac == 1 && track_atom_is(at))
    {
      this->track = (track_t *)fts_get_object(at);
      fts_object_refer(this->track);
    }
  else
    fts_object_set_error(o, "Argument of track required");
}

static void 
seqplay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  if(this->track)
    {
      seqplay_reset(this);
      fts_object_release(this->track);
    }
}

static fts_status_t
seqplay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqplay_t), 3, 2, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqplay_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqplay_delete);

  fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), seqplay_locate);
  fts_method_define_varargs(cl, 0, fts_s_start, seqplay_start);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), seqplay_pause);
  fts_method_define_varargs(cl, 0, fts_s_stop, seqplay_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("sync"), seqplay_sync);
  
  fts_method_define_varargs(cl, 1, fts_s_float, seqplay_set_speed);
  fts_method_define_varargs(cl, 2, seqsym_track, seqplay_set_reference);
  
  return fts_Success;
}

void
seqplay_config(void)
{
  fts_metaclass_install(fts_new_symbol("play"), seqplay_instantiate, fts_arg_type_equiv);
}
