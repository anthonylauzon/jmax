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
#include <sequence.h>
#include <track.h>
#include <event.h>
#include <seqsym.h>

typedef struct _seqplay_
{
  fts_object_t head;
  fts_timebase_t *timebase; /* underlying timebase */
  track_t *track; /* the track to play */

  /* status */
  event_t *event; /* current event */
  enum seqplay_status {status_reset, status_ready, status_playing, status_looping} status;
  double begin;
  double end;
  double speed; /* set speed */
  double last_time;
  double last_location;
  double last_speed;
  int sync_speed;
} seqplay_t;

static void seqplay_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void seqplay_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

static void
seqplay_reset(seqplay_t *this)
{
  if(this->status >= status_playing || this->sync_speed)
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
seqplay_position(seqplay_t *this, double there)
{
  if(this->track && this->end > there)
    {
      event_t *event = track_get_event_by_time(this->track, there);
      
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
seqplay_go(seqplay_t *this)
{ 
  fts_object_t *o = (fts_object_t *)this;

  if(track_is_active(this->track) && this->status == status_ready && this->speed > 0.0 && this->end > this->begin)
    {
      /* set start time and speed */
      this->last_time = fts_timebase_get_time(this->timebase);
      this->last_speed = this->speed;
      
      /* schedule event */
      fts_timebase_add_call(this->timebase, o, seqplay_next, 0, (event_get_time(this->event) - this->last_location) / this->speed);
      
      /* play */
      this->status = status_playing;
    }
}

static void
seqplay_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;
  event_t *event = this->event;
  double time = event_get_time(event);
  event_t *next = track_highlight_and_next(this->track, event);
  
  if(track_is_active(this->track))
    {
      while(event && event != next)
	{
	  fts_atom_t *a = event_get_value(event);
	  
	  fts_outlet_varargs(o, 0, 1, a);
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
	fts_timebase_add_call(this->timebase, o, seqplay_end, 0, (end_time - time) / speed);
      else
	{
	  /* claim next event */
	  this->event = next;
	  fts_object_refer(next);
	  
	  /* schedule next output */
	  fts_timebase_add_call(this->timebase, o, seqplay_next, 0, (next_time - time) / speed);
	}
    }
  else
    seqplay_end(o, 0, 0, 0, 0);
}

static void
seqplay_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;

  if(this->status == status_looping)
    {
      seqplay_position(this, this->begin);
      seqplay_go(this);
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

  /* re-schedule event with new speed */
  if(track_is_active(this->track))
    fts_timebase_add_call(this->timebase, o, seqplay_next, 0, (event_get_time(this->event) - here) / speed);
}

static void 
seqplay_halt(seqplay_t *this)
{ 
  if(this->status >= status_playing)
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
seqplay_set_track(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;
  track_t *track = (track_t *)fts_get_object(at);

  seqplay_reset(this);  
  
  if(this->track)
    fts_object_release(this->track);
  
  this->track = track;
  
  fts_object_refer(track);
}

static void 
seqplay_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;

  if(fts_is_number(at))
    {
      double begin = fts_get_number_float(at);
      
      if(begin < 0.0)
	begin = 0.0;
      
      this->begin = begin;
    }
}

static void 
seqplay_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;

  if(fts_is_number(at))
    {
      double end = fts_get_number_float(at);
      
      if(end < 0.0)
	end = 0.0;
      
      this->end = end;
      
      if(this->status >= status_playing)
	{
	  double time = fts_timebase_get_time(this->timebase);
	  double here = this->last_location + (time - this->last_time) * this->last_speed;
	  
	  if(end <= here)
	    seqplay_end(o, 0, 0, 0, 0);
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

  if(speed <= 0.0)
    {
      this->speed = 0.0;

      seqplay_halt(this);
    }
  else if(this->status >= status_playing)
    {
      this->speed = speed;

      /* schedule speed sync call */
      if(!this->sync_speed)
	{
	  fts_timebase_add_call(this->timebase, o, seqplay_sync_speed, 0, fts_timebase_get_step(this->timebase));
	  this->sync_speed = 1;
	}
    }
  else
    this->speed = this->last_speed = speed;
}

static void
seqplay_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch(ac)
    {
    default:
    case 4:
      seqplay_set_speed(o, 0, 0, 1, at + 3);
    case 3:
      seqplay_set_end(o, 0, 0, 1, at + 2);
    case 2:
      seqplay_set_begin(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_a(at, track_type))
	seqplay_set_track(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

static void 
seqplay_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;
  double duration = fts_get_number_float(at);
  double begin = this->begin;
  double end = this->end;
  
  if(duration > 0.0)
    this->speed = (end - begin) / duration;
  else
    this->speed = 1.0;
}

static void 
seqplay_jump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  double time = 0.0;
  
  if(fts_is_number(at))
    time = fts_get_number_float(at);

  if(this->status > status_reset)
    seqplay_reset(this);  

  seqplay_position(this, time);
  seqplay_go(this);
}
  
static void 
seqplay_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  
  switch(this->status)
    {
    case status_reset:
      seqplay_position(this, this->begin);
    case status_ready:
      seqplay_go(this);
    case status_playing:
      break;
    case status_looping:
      this->status = status_playing;
      break;
    }
}

static void 
seqplay_loop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;
  
  switch(this->status)
    {
    case status_reset:
      seqplay_position(this, this->begin);
    case status_ready:
      seqplay_go(this);
    case status_playing:
      this->status = status_looping;
      break;
    case status_looping:
      break;
    }
}

static void 
seqplay_segment(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplay_t *this = (seqplay_t *)o;

  if(this->status > status_reset)
    seqplay_reset(this);  

  seqplay_set(o, 0, 0, ac, at);
  seqplay_position(this, this->begin);
  seqplay_go(this);
}

static void 
seqplay_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplay_t *this = (seqplay_t *)o;

  seqplay_halt(this);
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

	  if(this->status >= status_playing)
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
		fts_outlet_varargs(o, 0, 1, a);

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
    seqplay_set(o, 0, 0, ac, at);
  else
    fts_object_set_error(o, "argument of track required");
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

static void
seqplay_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(seqplay_t), seqplay_init, seqplay_delete);

  fts_class_message_varargs(cl, fts_new_symbol("locate"), seqplay_jump);
  fts_class_message_varargs(cl, fts_new_symbol("play"), seqplay_play);
  fts_class_message_varargs(cl, fts_new_symbol("stop"), seqplay_stop);
  fts_class_message_varargs(cl, fts_new_symbol("pause"), seqplay_pause);
  fts_class_message_varargs(cl, fts_new_symbol("loop"), seqplay_loop);

  fts_class_message_varargs(cl, fts_new_symbol("begin"), seqplay_set_begin);
  fts_class_message_varargs(cl, fts_new_symbol("end"), seqplay_set_end);
  fts_class_message_varargs(cl, fts_new_symbol("speed"), seqplay_set_speed);

  fts_class_message_varargs(cl, fts_new_symbol("duration"), seqplay_set_duration);
  fts_class_message_varargs(cl, fts_s_set, seqplay_set);

  fts_class_message_varargs(cl, fts_new_symbol("sync"), seqplay_sync);

  fts_class_message_varargs(cl, fts_s_bang, seqplay_segment);
  
  fts_class_inlet_varargs(cl, 0, seqplay_segment);
  fts_class_inlet_number(cl, 1, seqplay_set_begin);
  fts_class_inlet_number(cl, 2, seqplay_set_end);
  fts_class_inlet_number(cl, 3, seqplay_set_speed);

  fts_class_outlet_varargs(cl, 0);
  fts_class_outlet_bang(cl, 1);
}

void
seqplay_config(void)
{
  fts_class_install(fts_new_symbol("play"), seqplay_instantiate);
}
