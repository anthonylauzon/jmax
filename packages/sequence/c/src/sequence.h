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
  /* list of events in sequence */
  sequence_event_t *prev;
  sequence_event_t *next;

  sequence_track_t *track; /* track of event */

  double time; /* time tag */
  sequence_event_field_t *fields; /* list of fields of field_tracks */

  fts_atom_t value;
};

#define sequence_element_get_track

extern void sequence_event_set(sequence_event_t *event, fts_atom_t value);
extern void sequence_event_free(sequence_event_t *event);
extern sequence_event_t *sequence_event_new(fts_atom_t value);
extern void sequence_event_delete(sequence_event_t *event);

/*****************************************************************
 *
 *  track
 *
 */

struct _sequence_track
{ 
  /* list of tracks in sequence */
  sequence_track_t *next;

  sequence_t *sequence; /* sequence of track */

  fts_symbol_t name;
  fts_type_t type; /* type of track */
  int active; /* active flag */
  sequence_field_track_t *field_tracks; /* list of field_tracks */
};

#define sequence_track_get_name(t) ((t)->name)
#define sequence_track_get_type(t) ((t)->type)


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
extern sequence_t *sequence_create(void);
extern void sequence_destroy(sequence_t *sequence);

/* tracks */
extern sequence_track_t *sequence_add_track(sequence_t *sequence, fts_symbol_t name, fts_type_t type);
extern void sequence_remove_track(sequence_t *sequence, int index);
extern sequence_track_t *sequence_get_track_by_index(sequence_t *sequence, int index);
extern sequence_track_t *sequence_get_track_by_name(sequence_t *sequence, fts_symbol_t name);


/* events */
void sequence_add_event(sequence_t *sequence, sequence_track_t *track, double time, sequence_event_t *event);
void sequence_remove_event(sequence_event_t *event);

/* locate events */
extern sequence_event_t *sequence_get_event_by_time(sequence_t *sequence, double time);
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
