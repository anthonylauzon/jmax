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
#ifndef _TRACK_H_
#define _TRACK_H_

#include <fts/fts.h>
#include "track.h"
#include "event.h"

extern fts_class_t *track_class;

typedef struct _track_ track_t;

struct _track_
{ 
  fts_object_t o;

  struct _sequence_ *sequence; /* sequence of track */
  track_t *next; /* list of tracks in sequence */

  fts_symbol_t name;
  int active; /* active flag */

  fts_symbol_t type; /* type of events */
  event_t *first; /* pointer to first event */
  event_t *last; /* pointer to last event */
  int size; /* # of events in track */
  fts_symbol_t keep;
};

#define track_set_sequence(t, s) ((t)->sequence = (s))
#define track_get_sequence(t) ((t)->sequence)

#define track_get_next(t) ((t)->next)
#define track_get_name(t) ((t)->name)
#define track_set_name(t, n) ((t)->name = (n))

#define track_get_type(t) ((t)->type)
#define track_get_first(t) ((t)->first)
#define track_get_last(t) ((t)->last)
#define track_get_size(t) ((t)->size)
#define track_get_duration(t) ((t)->last->time - (t)->first->time)

#define track_is_active(t) ((t)->active != 0)
#define track_is_inactive(t) ((t)->active == 0)

#define track_set_active(t) ((t)->active = 1)
#define track_set_inactive(t) ((t)->active = 0)

#define track_editor_is_open(t) ((t)->sequence && sequence_editor_is_open((t)->sequence))

extern void track_add_event(track_t *track, double time, event_t *event);
extern void track_add_event_after(track_t *track, double time, event_t *event, event_t *here);
extern void track_append_event(track_t *track, double time, event_t *event);
extern void track_remove_event(track_t *track, event_t *event);

extern void track_merge(track_t *track, track_t *merge);
extern void track_clear(track_t *track);

extern event_t *track_get_event_by_time(track_t *track, double time);
extern event_t *track_get_event_by_time_after(track_t *track, double time, event_t *here);

/* high-lighting events in editor */
extern void track_highlight_event(track_t *track, event_t *event);
extern void track_highlight_cluster(track_t *track, event_t *event, event_t *next);
extern event_t *track_highlight_and_next(track_t *track, event_t *event);

extern void track_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
extern void track_add_event_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/* track atoms */
#define track_atom_set(ap, x) fts_set_object_with_type((ap), (x), seqsym_track)
#define track_atom_get(ap) ((track_t *)fts_get_object(ap))
#define track_atom_is(ap) (fts_is_a((ap), seqsym_track))

#endif
