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

#include <float.h>

#include <fts/fts.h>
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "event.h"

typedef struct _segplay_
{
  fts_object_t head;
  fts_timebase_t *timebase; /* underlying timebase */
  track_t *track; /* the track to play */

  /* status */
  event_t *event; /* current event */
  enum segplay_status {status_reset, status_ready, status_playing} status;
  double begin;
  double end;
  double speed; /* set speed */
  double last_time;
  double last_location;
  double last_speed;
  int sync_speed;
} segplay_t;

static void
segplay_reset(segplay_t *this)
{
  if(this->status == status_playing || this->sync_speed)
    {
      fts_timebase_remove_object(this->timebase, (fts_object_t *)this);
      this->sync_speed = 0;
    }

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
segplay_position(segplay_t *this, double there)
{
  if(this->track && this->end > there)
    {
      event_t *event = track_get_event_with_duration_by_time(this->track, there);
      
      if(event)
	{
	  /* claim event */
	  this->event = event;
	  fts_object_refer(event);
	  
	  /* set start time */
	  this->last_location = there;
	  
	  this->status = status_ready;
	}
      else
	this->status = status_reset;
    }
}

static void
segplay_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;

  segplay_reset(this);  
  fts_outlet_bang(o, 2);  
}

static void
segplay_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;
  event_t *event = this->event;
  double time = event_get_time(event);
  event_t *next = track_highlight_and_next(this->track, event);
  
  if(track_is_active(this->track))
    {
      while(event && event != next)
	{
	  fts_atom_t *a = event_get_value(event);
	  
	  fts_outlet_float(o, 1, event_get_time(event));
	  fts_outlet_send(o, 0, fts_get_selector(a), 1, a);

	  event = event_get_next(event);
	}
    }
  
  /* release current event */
  fts_object_release(this->event);
  this->event = 0;
  
  if(next)
    {
      double speed = this->last_speed;
      double next_time = event_get_time(next);
      double end_time = this->end;
      
      if(next_time > end_time)
	fts_timebase_add_call(this->timebase, o, segplay_end, 0, (end_time - time) / speed);
      else
	{
	  /* claim next event */
	  this->event = next;
	  fts_object_refer(next);
	  
	  /* schedule next output */
	  fts_timebase_add_call(this->timebase, o, segplay_next, 0, (next_time - time) / speed);
	}
    }
  else
    segplay_end(o, 0, 0, 0, 0);
}

static void
segplay_sync_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;
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

  /* re-schedule event with new speed */
  if(track_is_active(this->track))
    fts_timebase_add_call(this->timebase, o, segplay_next, 0, (event_get_time(this->event) - here) / speed);
}

static void 
segplay_go(segplay_t *this)
{ 
  fts_object_t *o = (fts_object_t *)this;

  if(track_is_active(this->track) && this->status == status_ready && this->speed > 0.0 && this->end > this->begin)
    {
      event_t *event = this->event;

      /* output incomplete segments */
      if(event && this->last_location > event_get_time(event))
	{
	  fts_atom_t *a = event_get_value(event);
	  
	  fts_outlet_float(o, 1, event_get_time(event));
	  fts_outlet_send(o, 0, fts_get_selector(a), 1, a);	  
	}

      /* look for further segments activ at given time */
      while(event && this->last_location > event_get_time(event))
	{
	  if(this->last_location < event_get_time(event) + event_get_duration(event))
	    {
	      fts_atom_t *a = event_get_value(event);
	      
	      fts_outlet_float(o, 1, event_get_time(event));
	      fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
	    }

	  event = event_get_next(event);
	}
      

      /* set start time and speed */
      this->last_time = fts_timebase_get_time(this->timebase);
      this->last_speed = this->speed;
      
      /* schedule event */
      fts_timebase_add_call(this->timebase, o, segplay_next, 0, (event_get_time(this->event) - this->last_location) / this->speed);
      
      /* play */
      this->status = status_playing;
    }
}

static void 
segplay_halt(segplay_t *this)
{ 
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

/***********************************************************
 *
 *  user methods
 *
 */

static void
segplay_set_track(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;
  track_t *track = track_atom_get(at);

  segplay_reset(this);  
  
  if(this->track)
    fts_object_release(this->track);
  
  this->track = track;
  
  fts_object_refer(track);
}

static void 
segplay_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;

  if(fts_is_number(at))
    {
      double begin = fts_get_number_float(at);
      
      if(begin < 0.0)
	begin = 0.0;
      
      this->begin = begin;
    }
}

static void 
segplay_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;

  if(fts_is_number(at))
    {
      double end = fts_get_number_float(at);
      
      if(end < 0.0)
	end = 0.0;
      
      this->end = end;
      
      if(this->status == status_playing)
	{
	  double time = fts_timebase_get_time(this->timebase);
	  double here = this->last_location + (time - this->last_time) * this->last_speed;
	  
	  if(end <= here)
	    segplay_end(o, 0, 0, 0, 0);
	}
    }
}

static void
segplay_set_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;
  double speed = 0.0;

  if(fts_is_number(at))
    speed = fts_get_number_float(at);

  if(speed <= 0.0)
    {
      this->speed = 0.0;

      segplay_halt(this);
    }
  else if(this->status == status_playing)
    {
      this->speed = speed;

      /* schedule speed sync call */
      if(!this->sync_speed)
	{
	  fts_timebase_add_call(this->timebase, o, segplay_sync_speed, 0, fts_timebase_get_step(this->timebase));
	  this->sync_speed = 1;
	}
    }
  else
    this->speed = this->last_speed = speed;
}

static void
segplay_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;  
  
  switch(ac)
    {
    default:
    case 4:
      segplay_set_speed(o, 0, 0, 1, at + 3);
    case 3:
      segplay_set_end(o, 0, 0, 1, at + 2);
    case 2:
      segplay_set_begin(o, 0, 0, 1, at + 1);
    case 1:
      if(track_atom_is(at))
	segplay_set_track(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

static void 
segplay_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;
  double duration = fts_get_number_float(at);
  double begin = this->begin;
  double end = this->end;
  
  if(duration > 0.0)
    this->speed = (end - begin) / duration;
  else
    this->speed = 1.0;
}

static void 
segplay_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  segplay_t *this = (segplay_t *)o;
  double time = 0.0;
  
  if(fts_is_number(at))
    time = fts_get_number_float(at);

  if(this->status > status_reset)
    segplay_reset(this);  

  segplay_position(this, time);
}
  
static void 
segplay_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  segplay_t *this = (segplay_t *)o;
  
  switch(this->status)
    {
    case status_reset:
      segplay_position(this, this->begin);
    case status_ready:
      segplay_go(this);
    case status_playing:
      break;
    }
}

static void 
segplay_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segplay_t *this = (segplay_t *)o;

  if(this->status > status_reset)
    segplay_reset(this);  

  segplay_set(o, 0, 0, ac, at);
  segplay_position(this, this->begin);
  segplay_go(this);
}

static void 
segplay_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  segplay_t *this = (segplay_t *)o;

  segplay_halt(this);
}

static void 
segplay_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  segplay_t *this = (segplay_t *)o;

  segplay_reset(this);
}

static void 
segplay_sync(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  segplay_t *this = (segplay_t *)o;

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
		{
		  fts_outlet_float(o, 1, event_get_time(event));
		  fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
		}

	      event = event_get_next(event);
	    }
	  
	  if(event)
	    {
	      double speed = this->last_speed;
	      
	      /* claim next event */
	      this->event = event;
	      fts_object_refer(event);
	      
	      /* schedule next output */
	      fts_timebase_add_call(this->timebase, o, segplay_next, 0, (event_get_time(event) - time) / speed);
	    }
	  else
	    segplay_reset(this);  
	}
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
segplay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  segplay_t *this = (segplay_t *)o;
  fts_timebase_t *pseudo = (fts_timebase_t *)o;
  
  ac--;
  at++;

  fts_timebase_init(pseudo); /* init local (pseudo) timebase */

  this->timebase = fts_get_timebase();

  this->track = 0;
  this->event = 0;

  this->status = status_reset;
  this->begin = 0.0;
  this->end = DBL_MAX;
  this->speed = 1.0;

  this->last_time = 0.0;
  this->last_location = 0.0;
  this->last_speed = 0.0;

  this->sync_speed = 0;

  if(ac > 0)
    segplay_set(o, 0, 0, ac, at);
  else
    fts_object_set_error(o, "Argument of track required");
}

static void 
segplay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  segplay_t *this = (segplay_t *)o;

  if(this->track)
    {
      segplay_reset(this);
      fts_object_release(this->track);
    }
}

static fts_status_t
segplay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(segplay_t), 4, 3, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, segplay_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, segplay_delete);

  fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), segplay_locate);
  fts_method_define_varargs(cl, 0, fts_new_symbol("play"), segplay_play);
  /* fts_method_define_varargs(cl, 0, fts_new_symbol("loop"), segplay_loop); */
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), segplay_pause);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), segplay_stop);

  fts_method_define_varargs(cl, 0, fts_new_symbol("begin"), segplay_set_begin);
  fts_method_define_varargs(cl, 0, fts_new_symbol("end"), segplay_set_end);
  fts_method_define_varargs(cl, 0, fts_new_symbol("speed"), segplay_set_speed);
  fts_method_define_varargs(cl, 0, fts_new_symbol("duration"), segplay_set_duration);
  fts_method_define_varargs(cl, 0, fts_s_set, segplay_set);

  fts_method_define_varargs(cl, 0, fts_new_symbol("sync"), segplay_sync);

  fts_method_define_varargs(cl, 0, fts_new_symbol("start"), segplay_start);
  fts_method_define_varargs(cl, 0, fts_s_bang, segplay_start);
  fts_method_define_varargs(cl, 0, seqsym_track, segplay_start);
  fts_method_define_varargs(cl, 0, fts_s_list, segplay_start);
  
  fts_method_define_varargs(cl, 1, fts_s_int, segplay_set_begin);
  fts_method_define_varargs(cl, 1, fts_s_float, segplay_set_begin);

  fts_method_define_varargs(cl, 2, fts_s_int, segplay_set_end);
  fts_method_define_varargs(cl, 2, fts_s_float, segplay_set_end);

  fts_method_define_varargs(cl, 3, fts_s_int, segplay_set_speed);
  fts_method_define_varargs(cl, 3, fts_s_float, segplay_set_speed);
  
  return fts_Success;
}

void
segplay_config(void)
{
  fts_metaclass_install(fts_new_symbol("segplay"), segplay_instantiate, fts_arg_type_equiv);
}
