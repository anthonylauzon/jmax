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

#ifndef _SEQUENCE_TRACK_H_
#define _SEQUENCE_TRACK_H_

#include <fts/fts.h>
#include <fts/packages/sequence/marker.h>
#include <fts/packages/sequence/sequence.h>
#include <math.h>

#ifdef WIN32 
#define fmax(a,b) (((a)>(b))? (a) : (b))
#endif

SEQUENCE_API fts_class_t *track_class;

struct _track_
{ 
  fts_object_t o;

  int active; /* active flag */
  int open; /* flag: is 1 if track editor is open */

  struct _track_editor *editor;
  int save_editor; /* flag: is 1 if want to save editor state */
  
  fts_class_t *type; /* type of events */
  event_t *first; /* pointer to first event */
  event_t *last; /* pointer to last event */
  int size; /* # of events in track */

  fts_list_t *gui_listeners; /* list of guilisteners */

  fts_object_t *load_obj; /* object (event value) of currently loaded event */
  
  track_t *markers;
};

#define MARKERS_BAR_TOLERANCE 20.0 /* tolerance for bars */

#define track_get_name(t) (NULL)
#define track_set_name(t, n)

#define track_get_type(t) ((t)->type)
#define track_get_first(t) ((t)->first)
#define track_get_last(t) ((t)->last)
#define track_get_size(t) ((t)->size)
#define track_get_last_time(t) (((t)->last)? (t)->last->time: 0.0)

#define track_get_duration(t) \
  ((track_get_markers(t))? (fmax(track_get_last_time(track_get_markers(t)), track_get_last_time(t))): (track_get_last_time(t)))

#define track_is_active(t) ((t)->active != 0)
#define track_do_save_editor(t) ((t)->save_editor != 0)

#define track_set_active(t) ((t)->active = 1)
#define track_set_inactive(t) ((t)->active = 0)

#define track_set_markers(t, m) ((t)->markers = (m))
#define track_get_markers(t) ((t)->markers)
#define track_is_marker(t) (track_get_type(t) == scomark_class)

#define track_set_editor_open(t) ((t)->open = 1)
#define track_set_editor_close(t) ((t)->open = 0)

SEQUENCE_API void track_add_event(track_t *track, double time, event_t *event);
SEQUENCE_API void track_add_event_after(track_t *track, double time, event_t *event, event_t *after);
SEQUENCE_API void track_append_event(track_t *track, double time, event_t *event);
SEQUENCE_API void track_move_event(track_t *track, event_t *event, double time);
SEQUENCE_API void track_remove_event(track_t *track, event_t *event);
SEQUENCE_API void track_copy(track_t *org, track_t *copy);
SEQUENCE_API void track_merge(track_t *track, track_t *merge);
SEQUENCE_API void track_erase_events(track_t *track);
SEQUENCE_API void track_clear(track_t *track);

SEQUENCE_API int track_editor_is_open(track_t *track);
SEQUENCE_API int track_is_in_multitrack(track_t *track);

SEQUENCE_API void track_add_event_and_upload(track_t *track, double time, event_t *event);
SEQUENCE_API void track_add_event_after_and_upload(track_t *track, double time, event_t *event, event_t *after);
SEQUENCE_API void track_move_event_and_upload(track_t *track, event_t *event, double time);
SEQUENCE_API void track_remove_event_and_upload(track_t *track, event_t *event);
SEQUENCE_API void track_merge_and_upload(track_t *track, track_t *merge);
SEQUENCE_API void track_clear_and_upload(track_t *track);

SEQUENCE_API void track_update_editor(track_t *self);
SEQUENCE_API void track_upload_event(track_t *self, event_t *event);
SEQUENCE_API void track_clear_at_client(track_t *self);

SEQUENCE_API void track_set_dirty(track_t *track);

SEQUENCE_API event_t *track_get_event_by_time(track_t *track, double time);
SEQUENCE_API event_t *track_get_next_by_time(track_t *track, double time);
SEQUENCE_API event_t *track_get_next_by_time_after(track_t *track, double time, event_t *here);
SEQUENCE_API event_t *track_get_left_by_time_from(track_t *track, double time, event_t *here);

SEQUENCE_API track_t *track_get_or_make_markers(track_t *track);
SEQUENCE_API scomark_t *track_insert_marker(track_t *track, double time, fts_symbol_t type);

SEQUENCE_API void track_segment_get(track_t *self, double begin, double end, event_t **first, event_t **after);
SEQUENCE_API void track_segment_shift(track_t *self, event_t *first, event_t *after, double begin, double end, double shift);
SEQUENCE_API void track_segment_stretch(track_t *self, event_t *first, event_t *after, double begin, double end, double stretch);
SEQUENCE_API void track_segment_quantize(track_t *self, event_t *first, event_t *after, double begin, double end, double quantize);
SEQUENCE_API void track_move_events_at_client(track_t *self, event_t *first, event_t *after);

/** @name highlighting events in editor
 *  @{ */

/**
 * Highlight one event in track editor and set cursor on its starting time.
 * Previous highlighting is removed.
 *
 * @param event	pointer to track event
 */
SEQUENCE_API void track_highlight_event(track_t *track, event_t *event);

/**
 * Highlight events in track editor (up to 64) and set cursor to start
 * time of group.
 * Previous highlighting is removed.
 *
 * @param n	number of events in event array
 * @param event	array of track events
 */
SEQUENCE_API void track_highlight_events(track_t *track, int n, event_t *event[]);

/**
 * Set cursor to arbitrary time in track editor.
 * Previous highlighting is removed.
 *
 * @param time	time to display in editor
 */
SEQUENCE_API void track_highlight_time(track_t *track, double time);

/**
 * Highlight events in track editor (up to 64) and set cursor to arbitrary time.
 * Set n to 0 and event to NULL to only display time.
 * Previous highlighting is removed.
 *
 * @param time	time to display in editor
 * @param n	number of events in event array
 * @param event	array of track events
 */
SEQUENCE_API void track_highlight_events_and_time(track_t *track, double time, 
					    int n, event_t *event[]);

/**
 * Highlight sequential event group in track editor.
 * Previous highlighting is removed.
 *
 * @param event pointer to first event
 * @param event pointer to last event
 */
SEQUENCE_API void track_highlight_cluster(track_t *track, event_t *event, event_t *next);

/**
 * Highlight one event in track editor, set cursor on its starting time.
 * Previous highlighting is removed.
 *
 * @return next event in track
 * @param event	pointer to track event
 */
SEQUENCE_API event_t *track_highlight_and_next(track_t *track, event_t *event);

/** @}	end of doc group highlighting */

SEQUENCE_API fts_method_status_t track_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
SEQUENCE_API fts_method_status_t track_add_event_from_file(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
SEQUENCE_API fts_method_status_t track_event_dump_mess(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

#endif  /* _SEQUENCE_TRACK_H_ */
