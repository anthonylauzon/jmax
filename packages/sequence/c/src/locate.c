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

typedef struct _locate_
{
  fts_object_t head;
  track_t *track;
  double epsilon;
  fts_symbol_t mode;
} locate_t;

/***********************************************************
 *
 *  user methods
 *
 */

static void
locate_set_track(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  locate_t *this = (locate_t *)o;
  track_t *track = (track_t *)fts_get_object(at);

  if(this->track)
    fts_object_release(this->track);
  
  this->track = track;
  
  fts_object_refer(track);
}

static void 
locate_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  locate_t *this = (locate_t *)o;
  track_t *track = this->track;
  double time = 0.0;
  
  if(fts_is_number(at))
    time = fts_get_number_float(at);

  if(time < 0.0)
    time = 0.0;

  if(this->mode == seqsym_event)
    {
      double epsilon = this->epsilon;

      if(track_get_size(track) > 0 && time - epsilon <= event_get_time(track->last))
	{
	  event_t *event = track->first;
	  event_t *next = event_get_next(event);
	  
	  while(time - epsilon > event_get_time(event))
	    event = event_get_next(event);

	  while(event && time + epsilon >= event_get_time(event))
	    {
	      fts_atom_t *value = event_get_value(event);
	      
	      fts_outlet_float(o, 1, (float)event_get_time(event));
	      fts_outlet_varargs(o, 0, 1, value);
	      
	      event = event_get_next(event);
	    }
	}
    }
  else
    {
      double epsilon = this->epsilon;
      
      if(track_get_size(track) > 0 && time - epsilon <= event_get_time(track->last) + event_get_duration(track->last))
	{
	  event_t *event = track->first;
	  
	  while(time - epsilon >= event_get_time(event) + event_get_duration(event))
	    event = event_get_next(event);

	  while(event && time + epsilon >= event_get_time(event))
	    {
	      if(time - epsilon < event_get_time(event) + event_get_duration(event))
		{
		  fts_atom_t *value = event_get_value(event);

		  fts_outlet_float(o, 1, (float)event_get_time(event));
		  fts_outlet_varargs(o, 0, 1, value);
		}

	      event = event_get_next(event);
	    }	  
	}
    }
}
  
static void
locate_set_epsilon(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  locate_t *this = (locate_t *)o;
  double epsilon = fts_get_number_float(at);

  if(epsilon < 0.0)
    epsilon = 0.0;

  this->epsilon = epsilon;
}

static void
locate_set_epsilon_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  locate_set_epsilon(o, 0, 0, 1, value);
}

static void
locate_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  locate_t *this = (locate_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
    {
      fts_symbol_t mode = fts_get_symbol(at);
      
      if(mode == seqsym_event)
	this->mode = seqsym_event;
      else if(mode == seqsym_segment)
	this->mode = seqsym_segment;
      else
	fts_object_signal_runtime_error(o, "doesn't understand mode %s", mode);
    }
  else
    fts_object_signal_runtime_error(o, "symbol argument required for message mode");
}

static void
locate_set_mode_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  locate_set_mode(o, 0, 0, 1, value);
}

/************************************************************
 *
 *  class
 *
 */

static void
locate_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  locate_t *this = (locate_t *)o;
  fts_timebase_t *pseudo = (fts_timebase_t *)o;
  
  this->track = 0;
  this->epsilon = 0.0;
  this->mode = seqsym_event;

  if(ac > 0 && fts_is_a(at, track_type))
    locate_set_track(o, 0, 0, ac, at);
  else
    fts_object_set_error(o, "Argument of track required");
}

static void 
locate_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  locate_t *this = (locate_t *)o;

  if(this->track)
    fts_object_release(this->track);
}

static void
locate_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(locate_t), locate_init, locate_delete);

  fts_class_method_varargs(cl, fts_new_symbol("mode"), locate_set_mode);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), locate_set_mode_prop);

  fts_class_method_varargs(cl, fts_new_symbol("epsilon"), locate_set_epsilon);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("epsilon"), locate_set_epsilon_prop);

  fts_class_inlet_number(cl, 0, locate_locate);
  fts_class_inlet(cl, 1, track_type, locate_set_track);

  fts_class_outlet_varargs(cl, 0);
  fts_class_outlet_float(cl, 1);  
}

void
locate_config(void)
{
  fts_class_install(fts_new_symbol("locate"), locate_instantiate);
}
