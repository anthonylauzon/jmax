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
 */

#ifndef _SEQUENCE_TRACK_H_
#define _SEQUENCE_TRACK_H_

#include <fts/fts.h>
#include <fts/packages/sequence/sequence.h>

extern fts_class_t *track_class;

struct _track_
{ 
  fts_object_t o;

  int persistence;

  struct _sequence_ *sequence; /* sequence of track */
  track_t *next; /* list of tracks in sequence */

  fts_symbol_t name;
  int active; /* active flag */

  int open; /* flag: is 1 if track editor is open */

  fts_class_t *type; /* type of events */
  event_t *first; /* pointer to first event */
  event_t *last; /* pointer to last event */
  int size; /* # of events in track */

  fts_list_t *gui_listeners; /* list of guilisteners */

  fts_object_t *load_obj; /* object (event value) of currently loaded event */
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
#define track_get_duration(t) (((t)->last)? (t)->last->time: 0.0)

#define track_is_active(t) ((t)->active != 0)

#define track_set_active(t) ((t)->active = 1)
#define track_set_inactive(t) ((t)->active = 0)

#define track_set_editor_open(t) ((t)->open = 1)
#define track_set_editor_close(t) ((t)->open = 0)
#define track_editor_is_open(t) (((t)->sequence && sequence_editor_is_open((t)->sequence)) || ((!(t)->sequence) && ((t)->open != 0)))

extern void track_add_event(track_t *track, double time, event_t *event);
extern void track_add_event_after(track_t *track, double time, event_t *event, event_t *here);
extern void track_append_event(track_t *track, double time, event_t *event);
extern void track_remove_event(track_t *track, event_t *event);

extern void track_merge(track_t *track, track_t *merge);
extern void track_clear(track_t *track);

extern void track_set_dirty(track_t *track);

extern event_t *track_get_event_by_time(track_t *track, double time);
extern event_t *track_get_next_by_time(track_t *track, double time);
extern event_t *track_get_next_by_time_after(track_t *track, double time, event_t *here);


/** @name highlighting events in editor
 *  @{ */

/**
 * Highlight one event in track editor and set cursor on its starting time.
 * Previous highlighting is removed.
 *
 * @param event	pointer to track event
 */
extern void track_highlight_event(track_t *track, event_t *event);

/**
 * Highlight events in track editor (up to 64) and set cursor to start
 * time of group.
 * Previous highlighting is removed.
 *
 * @param n	number of events in event array
 * @param event	array of track events
 */
extern void track_highlight_events(track_t *track, int n, event_t *event[]);

/**
 * Set cursor to arbitrary time in track editor.
 * Previous highlighting is removed.
 *
 * @param time	time to display in editor
 */
extern void track_highlight_time(track_t *track, double time);

/**
 * Highlight events in track editor (up to 64) and set cursor to arbitrary time.
 * Set n to 0 and event to NULL to only display time.
 * Previous highlighting is removed.
 *
 * @param time	time to display in editor
 * @param n	number of events in event array
 * @param event	array of track events
 */
extern void track_highlight_events_and_time(track_t *track, double time, 
					    int n, event_t *event[]);

/**
 * Highlight sequential event group in track editor.
 * Previous highlighting is removed.
 *
 * @param event pointer to first event
 * @param event pointer to last event
 */
extern void track_highlight_cluster(track_t *track, event_t *event, event_t *next);

/**
 * Highlight one event in track editor, set cursor on its starting time.
 * Previous highlighting is removed.
 *
 * @return next event in track
 * @param event	pointer to track event
 */
extern event_t *track_highlight_and_next(track_t *track, event_t *event);

/** @}	end of doc group highlighting */

extern void track_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
extern void track_add_event_from_file(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
extern void track_event_dump_mess(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

#endif
