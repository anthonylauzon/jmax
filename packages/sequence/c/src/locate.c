/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <float.h>

#include <fts/fts.h>
#include <fts/packages/sequence/sequence.h>

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

      if(track_get_size(track) > 0 && time - epsilon <= event_get_time(track_get_last(track)))
	{
	  event_t *event = track_get_first(track);

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
      
      if(track_get_size(track) > 0 &&
         time - epsilon <= event_get_time(track_get_last(track)) + event_get_duration(track_get_last(track)))
	{
	  event_t *event = track_get_first(track);
	  
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
locate_set_mode_event(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  locate_t *this = (locate_t *)o;

  this->mode = seqsym_event;
}

static void
locate_set_mode_segment(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  locate_t *this = (locate_t *)o;

  this->mode = seqsym_segment;
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

  this->track = 0;
  this->epsilon = 0.0;
  this->mode = seqsym_segment;

  if(ac > 0 && fts_is_a(at, track_class))
    locate_set_track(o, 0, 0, ac, at);
  else
    fts_object_error(o, "argument of track required");
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

  fts_class_message_varargs(cl, fts_new_symbol("event"), locate_set_mode_event);
  fts_class_message_varargs(cl, fts_new_symbol("segment"), locate_set_mode_segment);

  fts_class_message_varargs(cl, fts_new_symbol("epsilon"), locate_set_epsilon);

  fts_class_inlet_number(cl, 0, locate_locate);
  fts_class_inlet(cl, 1, track_class, locate_set_track);

  fts_class_outlet_varargs(cl, 0);
  fts_class_outlet_float(cl, 1);  
}

void
locate_config(void)
{
  /*fts_class_install(fts_new_symbol("locate"), locate_instantiate);*/
}
