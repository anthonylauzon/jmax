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
#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#include "fts.h"

/*****************************************************************
 *
 *  forward structures
 *
 */

typedef struct _sequence_event sequence_event_t;
typedef struct _sequence_track sequence_track_t;

typedef struct _sequence_event_field sequence_event_field_t;
typedef struct _sequence_field_track sequence_field_track_t;

typedef struct _sequence_marker sequence_marker_t;

typedef struct _sequence sequence_t;
typedef struct _sequence_ptr sequence_ptr_t;


/*****************************************************************
 *
 *  event
 *
 */

struct _sequence_event
{
  fts_object_t o;

  /* list of events in sequence */
  sequence_event_t *prev;
  sequence_event_t *next;

  sequence_track_t *track; /* track of event */

  double time; /* time tag */
  sequence_event_field_t *fields; /* list of fields of field_tracks */
};

extern void sequence_event_init(sequence_event_t *event);

#define sequence_event_set_time(e, t) ((e)->time = (t))
#define sequence_event_get_time(e) ((e)->time)

#define sequence_event_set_track(e, t) ((e)->track = (t))
#define sequence_event_get_track(e) ((e)->track)

#define sequence_event_get_prev(e) ((e)->prev)
#define sequence_event_get_next(e) ((e)->next)

/*****************************************************************
 *
 *  track
 *
 */

struct _sequence_track
{ 
  fts_object_t o;

  /* list of tracks in sequence */
  sequence_track_t *next;

  sequence_t *sequence; /* sequence of track */

  fts_symbol_t name;
  fts_type_t type; /* type of track */
  int active; /* active flag */
  sequence_field_track_t *field_tracks; /* list of field_tracks */
};

#define sequence_track_get_sequence(t) ((t)->sequence)
#define sequence_track_get_name(t) ((t)->name)
#define sequence_track_get_type(t) ((t)->type)

extern void sequence_track_post(sequence_track_t *track);

/*****************************************************************
 *
 *  field
 *
 */

struct _sequence_event_field
{
  /* list of fields in event */
  struct _sequence_event_field_t *next;

  struct _sequence_field_track_t *track; /* field_track of the field */
  struct _sequence_event_t *event; /* event the field is associated to */

  fts_atom_t value; /* fields value */
};


/*****************************************************************
 *
 *  field track
 *
 */

struct _sequence_field_track
{ 
  /* list of field_tracks in track */
  sequence_track_t *next; /* next field_track of track */
  fts_symbol_t name; /* name of track */

  sequence_track_t *track; /* track of field_track */

  fts_type_t type; /* type of field_track */
  int active; /* active flag */
};


/*****************************************************************
 *
 *  marker
 *
 */

struct _sequence_marker
{
  double time;
  sequence_event_t *event;
};


/*****************************************************************
 *
 *  sequence
 *
 */

struct _sequence
{ 
  fts_object_t o;

  sequence_event_t *begin; /* first event in sequence */
  sequence_event_t *end; /* last even in sequence */

  int size; /* # of events */

  sequence_track_t *tracks; /* list of tracks */ 
  int n_tracks; /* # of tracks */ 
};

#define sequence_get_size(s) ((s)->size)
#define sequence_get_n_tracks(s) ((s)->n_tracks)

/* new/delete sequence */
extern void sequence_init(sequence_t *sequence);
extern void sequence_empty(sequence_t *sequence);

/* tracks */
extern sequence_track_t *sequence_add_track(sequence_t *sequence, fts_symbol_t name, fts_type_t type);
extern void sequence_remove_track(sequence_t *sequence, fts_symbol_t name);
extern sequence_track_t *sequence_get_track_by_index(sequence_t *sequence, int index);
extern sequence_track_t *sequence_get_track_by_name(sequence_t *sequence, fts_symbol_t name);

/* events */
extern void sequence_add_event(sequence_t *sequence, sequence_track_t *track, double time, sequence_event_t *event);
extern void sequence_add_event_after(sequence_t *sequence, sequence_track_t *track, double time, sequence_event_t *event, sequence_event_t *here);
extern void sequence_remove_event(sequence_event_t *event);
extern void sequence_move_event(sequence_event_t *event, double time);

/* locate events */
extern sequence_event_t *sequence_get_event_by_time(sequence_t *sequence, double time);
extern sequence_event_t *sequence_get_event_by_time_after(sequence_t *sequence, double time, sequence_event_t *here);
extern sequence_event_t *sequence_get_event_in_track_by_time(sequence_t *sequence, sequence_track_t *track, double time);

#define sequence_get_size(sequence) ((sequence)->size)
#define sequence_get_duration(sequence) ((sequence)->end->time - (sequence)->begin->time)
#define sequence_get_begin(sequence) ((sequence)->begin)
#define sequence_get_end(sequence) ((sequence)->end)

/*****************************************************************
 *
 *  sequence pointer
 *
 */
/*
struct _sequence_ptr
{
  sequence_t *sequence;
  sequence_event_t *ptr;
};

extern void sequence_ptr_init(sequence_ptr_t *ptr, sequence_t *sequence);
extern void sequence_ptr_free(sequence_ptr_t *ptr, sequence_t *sequence);

extern void sequence_ptr_set_to_begin(sequence_ptr_t *ptr);
extern void sequence_ptr_set_to_end(sequence_ptr_t *ptr);
extern void sequence_ptr_set_by_time(sequence_ptr_t *ptr, double time);

extern fts_sequence_event_t *sequence_ptr_get_event(sequence_ptr_t *ptr);
*/

#endif
