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
 * Authors: Norbert Schnell, Riccardo Borghesi.
 *
 */
#ifndef _SEQUENCE_EVENT_H_
#define _SEQUENCE_EVENT_H_

#include <fts/fts.h>
#include <fts/packages/sequence/sequence.h>

extern fts_class_t *event_class;

struct _event_
{
  fts_object_t o;
  double time; /* time tag */
  fts_atom_t value; /* value */
  struct _track_ *track; /* event track */
  event_t *prev; /* previous in track */
  event_t *next; /* next in track */
};

#define event_set_time(e, t) ((e)->time = (t))
#define event_get_time(e) ((e)->time)

#define event_get_value(e) (&(e)->value)

#define event_set_track(e, t) ((e)->track = (t))
#define event_get_track(e) ((e)->track)

#define event_get_prev(e) ((e)->prev)
#define event_get_next(e) ((e)->next)

#define event_set_int(e, i) fts_set_int(&(e)->value, i)
#define event_set_float(e, f) fts_set_float(&(e)->value, f)
#define event_set_object(e, o) fts_set_object(&(e)->value, o)

#define event_get_int(e) fts_get_int(&(e)->value)
#define event_get_float(e) fts_get_float(&(e)->value)
#define event_get_object(e) fts_get_object(&(e)->value)

#define event_get_type(e) (fts_get_class_name(&(e)->value))

extern double event_get_duration(event_t *event);

#endif
