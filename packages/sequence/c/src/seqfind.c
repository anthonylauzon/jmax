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

#include <fts/fts.h>
#include <fts/packages/sequence/sequence.h>

typedef struct _seqfind_
{
  fts_object_t head;
  track_t *track;
} seqfind_t;

/************************************************************
 *
 *  user methods
 *
 */

static void
seqfind_all_values(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;
  event_t *event = track_get_first(this->track);

  while(event)
  {
    fts_atom_t *value = event_get_value(event);

    if(fts_atom_equals(value, at))
    {
      fts_outlet_float(o, 1, (float)event_get_time(event));
      fts_outlet_atom(o, 0, value);
    }

    event = event_get_next(event);
  }
}

static void
seqfind_first_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;
  event_t *event = track_get_first(this->track);

  while(event)
  {
    fts_atom_t *value = event_get_value(event);

    if(fts_atom_equals(value, at))
    {
      fts_outlet_float(o, 1, (float)event_get_time(event));
      fts_outlet_atom(o, 0, value);

      break;
    }

    event = event_get_next(event);
  }
}

static void
seqfind_event(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;
  track_t *track = this->track;
  double time = 0.0;
  double epsilon = 0.0;

  if(ac > 0 && fts_is_number(at))
    time = fts_get_number_float(at);

  if(time < 0.0)
    time = 0.0;

  if(ac > 1 && fts_is_number(at + 1))
    epsilon = fts_get_number_float(at + 1);

  if(epsilon < 0.0)
    epsilon = 0.0;

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

static void
seqfind_segment(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;
  track_t *track = this->track;
  double time = 0.0;
  double epsilon = 0.0;

  if(ac > 0 && fts_is_number(at))
    time = fts_get_number_float(at);

  if(time < 0.0)
    time = 0.0;

  if(ac > 1 && fts_is_number(at + 1))
    epsilon = fts_get_number_float(at + 1);

  if(epsilon < 0.0)
    epsilon = 0.0;

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

static void
seqfind_set_track(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;

  fts_object_release(this->track);
  this->track = (track_t *)fts_get_object(at);
  fts_object_refer(this->track);
}

/************************************************************
 *
 *  class
 *
 */

static void
seqfind_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;

  this->track = 0;

  if(ac > 0 && fts_is_a(at, track_class))
    {
      this->track = (track_t *)fts_get_object(at);
      fts_object_refer(this->track);
    }
  else
    fts_object_error(o, "argument of event track required");
}

static void 
seqfind_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;

  if(this->track)
    fts_object_release(this->track);
}

static void
seqfind_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(seqfind_t), seqfind_init, seqfind_delete);

  fts_class_message_varargs(cl, fts_new_symbol("first"), seqfind_first_value);
  fts_class_message_varargs(cl, fts_new_symbol("event"), seqfind_event);
  fts_class_message_varargs(cl, fts_new_symbol("segment"), seqfind_segment);

  fts_class_inlet_atom(cl, 0, seqfind_all_values);
  fts_class_inlet(cl, 1, track_class, seqfind_set_track);

  fts_class_outlet_atom(cl, 0);
  fts_class_outlet_float(cl, 1);
}

void
seqfind_config(void)
{
  fts_class_install(fts_new_symbol("find"), seqfind_instantiate);
}
