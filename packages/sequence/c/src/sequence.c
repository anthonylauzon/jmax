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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */
#include "fts.h"
#include "sequence.h"

/*********************************************************
 *
 *  sequence
 *
 */

void
sequence_init(sequence_t *sequence)
{
  sequence->begin = sequence->end = 0;
  sequence->size = 0;
  sequence->n_tracks = 0;
  sequence->tracks = 0;
}

/*********************************************************
 *
 *  sequence track
 *
 */

void
sequence_track_init(sequence_track_t *track, fts_type_t type, fts_symbol_t name)
{
  track->next = 0;
  track->sequence = 0;

  track->name = name;
  track->type = type;
  track->active = 1;
  track->field_tracks = 0;
}

void 
sequence_track_post(sequence_track_t *track)
{
  fts_symbol_t name = sequence_track_get_name(track);
  fts_type_t type = sequence_track_get_type(track);
  fts_symbol_t type_name = fts_type_get_selector(type);
  
  post("track: %s (%s)\n", fts_symbol_name(name), fts_symbol_name(type_name));
}

/*********************************************************
 *
 *  sequence event
 *
 */

void
sequence_event_init(sequence_event_t *event)
{
  event->prev = 0;
  event->next = 0;
  event->track = 0;
  event->time = 0.0;
  event->fields = 0;
}

/*********************************************************
 *
 *  local functions cutting a single event from a sequence
 *
 */

static sequence_event_t *
sequence_cut_only_event(sequence_t *sequence)
{
  sequence_event_t *event;

  event = sequence->begin;
  event->next = event->prev = 0;

  sequence->begin = 0;
  sequence->end = 0;
  sequence->size = 0;

  return event;
}

static sequence_event_t *
sequence_cut_begin_event(sequence_t *sequence)
{
  switch(sequence->size)
    {
    case 0: /* empty list */
      return 0;
    case 1: /* cut last event left */
      return sequence_cut_only_event(sequence);
    default: /* cut and make the next one being the begin */
      {
	sequence_event_t *event;

	event = sequence->begin;

	sequence->begin = event->next;
	sequence->begin->prev = 0;

	event->next = 0;
	sequence->size--;

	return event;
      }
    }
}

static sequence_event_t *
sequence_cut_end_event(sequence_t *sequence)
{
  switch(sequence->size)
    {
    case 0: /* empty list */
      return 0;
    case 1: /* cut last event left */
      return sequence_cut_only_event(sequence);
    default: /* cut and make the previous one being the end */
      {
	sequence_event_t *event;

	event = sequence->end;
	
	sequence->end = event->prev;
	sequence->end->next = 0;
	
	event->prev = 0;
	sequence->size--;

	return event;
      }
    }
}

static void
sequence_cut_event(sequence_t *sequence, sequence_event_t *event)
{
  if(event == sequence->begin)
    sequence_cut_begin_event(sequence);
  else if(event == sequence->end)
    sequence_cut_end_event(sequence);
  else
    {      
      event->prev->next = event->next;
      event->next->prev = event->prev;
      sequence->size--;
    }
  
  event->next = event->prev = 0;
}

/*********************************************************
 *
 *  extending sequence by a single event
 *
 */

static void
sequence_prepend_event(sequence_t *sequence, sequence_event_t *event)
{
  if(sequence->size == 0)
    {
      sequence->begin = sequence->end = event;
      event->next = event->prev = 0;
      sequence->size = 1;
    }
  else
    {
      event->next = sequence->begin;
      event->prev = 0;
      sequence->begin = event->next->prev = event;
      sequence->size++;
    }
}

static void
sequence_append_event(sequence_t *sequence, sequence_event_t *event)
{
  if(sequence->size == 0)
    {
      sequence->begin = sequence->end = event;
      event->next = event->prev = 0;
      sequence->size = 1;
    }
  else
    {
      event->prev = sequence->end;
      event->next = 0;
      sequence->end = event->prev->next = event;
      sequence->size++;
    }  
}

static void
sequence_insert_event_before(sequence_t *sequence, sequence_event_t *here, sequence_event_t *event)
{
  if(here == 0)
    sequence_append_event(sequence, event);
  else if(here == sequence->begin)
    sequence_prepend_event(sequence, event);
  else
    {
      event->prev = here->prev;
      event->next = here;

      here->prev->next = here->prev = event;
      sequence->size++;
    }  
}

static void
sequence_insert_event_behind(sequence_t *sequence, sequence_event_t *here, sequence_event_t *event)
{
  if(here == sequence->end)
    sequence_append_event(sequence, event);
  else
    {
      event->next = here->next;
      event->prev = here;

      here->next->prev = here->next = event;
      sequence->size++;
    }  
}

/*********************************************************
 *
 *  add/remove tracks
 *
 */

void
sequence_add_track(sequence_t *sequence, sequence_track_t *track)
{
  if(!sequence->tracks)
    {
      /* first track */
      sequence->tracks = track;
      sequence->n_tracks = 1;
    }
  else
    {
      /* append at end */
      sequence_track_t *last = sequence->tracks;

      while(last->next)
	last = last->next;

      last->next = track;
      sequence->n_tracks++;
    }
  
  track->sequence = sequence;
  track->next = 0;
}

void
sequence_remove_track(sequence_track_t *track)
{
  sequence_t *sequence = sequence_track_get_sequence(track);

  if(track == sequence->tracks)
    {
      /* first track */
      sequence->tracks = track->next;
      sequence->n_tracks--;
    }
  else
    {
      sequence_track_t *prev = sequence->tracks;
      sequence_track_t *this = sequence->tracks->next;

      while(this && this != track)
	{
	  prev = this;
	  this = this->next;
	}

      if(this)
	{
	  prev->next = this->next;
	  sequence->n_tracks--;	  
	}
    }
}

/*********************************************************
 *
 *  sequence add/remove event
 *
 */

void
sequence_add_event(sequence_t *sequence, sequence_track_t *track, double time, sequence_event_t *event)
{
  sequence_event_t *next = sequence_get_event_by_time(sequence, time);
  
  sequence_insert_event_before(sequence, next, event);

  sequence_event_set_track(event, track);
  sequence_event_set_time(event, time);
}

void
sequence_add_event_after(sequence_t *sequence, sequence_track_t *track, double time, sequence_event_t *event, sequence_event_t *here)
{
  sequence_event_t *next = sequence_get_event_by_time_after(sequence, time, here);
  
  sequence_insert_event_before(sequence, next, event);

  sequence_event_set_track(event, track);
  sequence_event_set_time(event, time);
}

void
sequence_remove_event(sequence_event_t *event)
{
  sequence_t *sequence = event->track->sequence;
  sequence_cut_event(sequence, event);
}

void
sequence_move_event(sequence_event_t *event, double time)
{
  sequence_t *sequence = event->track->sequence;
  sequence_event_t *next = event->next;
  sequence_event_t *prev = event->prev;

  if((next && time > next->time) || (prev && time < prev->time))
    {
      sequence_cut_event(sequence, event);
      sequence_event_set_time(event, time);      
      sequence_insert_event_before(sequence, sequence_get_event_by_time(sequence, time), event);
    }
  else
    sequence_event_set_time(event, time);
}

/*********************************************************
 *
 *  get an event from a sequence
 *
 */

sequence_event_t *
sequence_get_event_by_time(sequence_t *sequence, double time)
{
  sequence_event_t *event = 0;

  if(sequence_get_size(sequence) > 0 && time <= sequence->end->time)
    {
      event = sequence->begin;
      
      while(time > event->time)
	event = event->next;
    }
  
  return event;  
}

sequence_event_t *
sequence_get_event_by_time_after(sequence_t *sequence, double time, sequence_event_t *here)
{
  sequence_event_t *event = 0;

  if(sequence_get_size(sequence) > 0 && time <= sequence->end->time)
    {
      event = here;
      
      while(time > event->time)
	event = event->next;
    }
  
  return event;  
}

sequence_event_t *
sequence_get_event_in_track_by_time(sequence_t *sequence, sequence_track_t *track, double time)
{
  sequence_event_t *event = 0;

  if(time <= sequence->end->time)
    {
      event = sequence->begin;

      while(event && time > event->time && event->track != track)
	event = event->next;
    }

  return event;  
}
