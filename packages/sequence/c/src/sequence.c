/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "sequence.h"

/*********************************************************
 *
 *  local new/init/delete sequence event functions
 *
 */

void
sequence_event_set(sequence_event_t *event, fts_atom_t value)
{
  event->value = value;
}

void
sequence_event_free(sequence_event_t *event)
{
  if(fts_is_object(&event->value))
    fts_object_delete(fts_get_object(&event->value));
}

sequence_event_t *
sequence_event_new(fts_atom_t value)
{
  sequence_event_t *event = (sequence_event_t *)fts_block_zalloc(sizeof(sequence_event_t));

  sequence_event_set(event, value);

  return event;
}

void
sequence_event_delete(sequence_event_t *event)
{
  sequence_event_free(event);
  fts_block_free(event, sizeof(sequence_event_t));
}

/*********************************************************
 *
 *  local new/delete sequence tracks
 *
 */

static sequence_track_t *
sequence_track_new(fts_symbol_t name, fts_type_t type)
{
  sequence_track_t *track = (sequence_track_t *)fts_block_zalloc(sizeof(sequence_track_t));

  track->next = 0;
  track->sequence = 0;

  track->name = name;
  track->type = type;
  track->active = 1;
  track->field_tracks = 0;

  return track;
}

static void
sequence_track_delete(sequence_track_t *track)
{
  fts_block_free(track, sizeof(sequence_track_t));
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
	sequence->size--;

	event->next = 0;

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
	sequence->size--;
	
	event->prev = 0;

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
  if(here == sequence->begin)
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
sequence_insert_event_after(sequence_t *sequence, sequence_event_t *here, sequence_event_t *event)
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

sequence_track_t *
sequence_get_track_by_index(sequence_t *sequence, int index)
{
  sequence_track_t *track = sequence->tracks;
  int i = 0;

  while(track && i < index)
    track = track->next;

  return track;
}

sequence_track_t *
sequence_get_track_by_name(sequence_t *sequence, fts_symbol_t name)
{
  sequence_track_t *track = sequence->tracks;
  
  while(track && track->name != name)
    track = track->next;

  return track;
}

sequence_track_t *
sequence_add_track(sequence_t *sequence, fts_symbol_t name, fts_type_t type)
{
  sequence_track_t *track = sequence_track_new(name, type);

  if(!sequence->tracks)
    {
      sequence->tracks = track;
      track->sequence = sequence;
    }
  else
    {
      sequence_track_t *last = sequence->tracks;

      while(last->next)
	last = last->next;

      track->next = 0;
      last->next = track;
    }

  return track;
}

void
sequence_remove_track(sequence_t *sequence, int index)
{
  if(index == 0 && sequence->tracks)
    {
      sequence_track_t *track = sequence->tracks;

      sequence->tracks = track->next;
      sequence_track_delete(track);      
    }
  else
    {
      sequence_track_t *prev = sequence_get_track_by_index(sequence, index - 1);
      
      if(prev && prev->next)
	{
	  sequence_track_t *track = prev->next;

	  prev->next = track->next;
	  sequence_track_delete(track);
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
  
  if(next)
    sequence_insert_event_before(sequence, next, event);
  else
    sequence_append_event(sequence, event);
}

void
sequence_remove_event(sequence_event_t *event)
{
  sequence_t *sequence = event->track->sequence;

  sequence_cut_event(sequence, event);
}

/*********************************************************
 *
 *  new/delete sequence
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

void
sequence_empty(sequence_t *sequence)
{
  while(sequence->size)
    {
      sequence_event_t *event = sequence_cut_begin_event(sequence);
      sequence_event_delete(event);
    }

  while(sequence->tracks)
    sequence_remove_track(sequence, 0);
}

sequence_t *
sequence_create(void)
{
  sequence_t *sequence;

  sequence = (sequence_t *)fts_malloc(sizeof(sequence_t));
  sequence_init(sequence);
  
  return sequence;
}

void
sequence_destroy(sequence_t *sequence)
{
  sequence_empty(sequence);
  fts_free(sequence);
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

  if(time <= sequence->end->time)
    {
      event = sequence->begin;

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
