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

/** 
 * @defgroup track track: sequence of time-tagged values
 * @ingroup  fts_classes
 *  
 *  sequence of time-tagged values
 */

/** 
 * @var fts_class_t *track_class
 * @brief track_class ... 
 * @ingroup track 
 */
SEQUENCE_API fts_class_t *track_class;

/**
 * @struct _track_
 * @brief track struct
 * @ingroup track
 */
struct _track_
{ 
  fts_object_t o; /**< fts_object ... */

  int active; /**< active flag */
  int open; /**< flag: is 1 if track editor is open */

  struct _track_editor *editor;/**< track editor */
  int save_editor; /**< flag: is 1 if want to save editor state */
  
  fts_class_t *type; /**< type of events */
  event_t *first; /**< pointer to first event */
  event_t *last; /**< pointer to last event */
  int size; /**< # of events in track */

  fts_list_t *gui_listeners; /**< list of guilisteners */

  fts_object_t *load_obj; /**< object (event value) of currently loaded event */
  
  track_t *markers; /**< markers track */
};

/** 
 * @def MARKERS_BAR_TOLERANCE 20.0
 * @brief tolerance for bars ... 
 * @ingroup track 
 */
#define MARKERS_BAR_TOLERANCE 20.0 /* tolerance for bars */

#define track_get_name(t) (NULL)
#define track_set_name(t, n)

#ifdef AVOID_MACROS
/** 
 * @fn fts_class_t *track_get_type(track_t *track)
 * @brief set track type
 * @param track the track
 * @return type of events in track
 * @ingroup track 
 */
fts_class_t *track_get_type(track_t *track);
/** 
 * @fn event_t *track_get_first(track_t *track)
 * @brief get first event in track
 * @param track the track
 * @return first event in track
 * @ingroup track 
 */
event_t *track_get_first(track_t *track);
/** 
 * @fn event_t *track_get_last(track_t *track)
 * @brief get last event in track
 * @param track the track
 * @return last event in track
 * @ingroup track 
 */
event_t *track_get_last(track_t *track);
/** 
 * @fn int track_get_size(track_t *track)
 * @brief get track size
 * @param track the track
 * @return track size
 * @ingroup track 
 */
int track_get_size(track_t *track);
/** 
 * @fn double track_get_last_time(track_t *track)
 * @brief get timetag of last event in track
 * @param track the track
 * @return timetag of last event
 * @ingroup track 
 */
double track_get_last_time(track_t *track);
/** 
 * @fn double track_get_duration(track_t *track)
 * @brief get track duration
 * @param track the track
 * @return track duration
 * @ingroup track 
 */
double track_get_duration(track_t *track);
/** 
 * @fn int track_is_active(track_t *track)
 * @brief tell if track is active
 * @param track the track
 * @return 1 if true, 0 if false
 * @ingroup track 
 */
int track_is_active(track_t *track);
/** 
 * @fn int track_do_save_editor(track_t *track)
 * @brief tell if track editor state is to be saved
 * @param track the track
 * @return 1 if true, 0 if false
 * @ingroup track 
 */
int track_do_save_editor(track_t *track);
/** 
 * @fn void track_set_active(track_t *track)
 * @brief set track as active
 * @param track the track
 * @ingroup track 
 */
void track_set_active(track_t *track);
/** 
 * @fn void track_set_inactive(track_t *track)
 * @brief set track as inactive
 * @param track the track
 * @ingroup track 
 */
void track_set_inactive(track_t *track);
/** 
 * @fn track_set_markers(track_t *track, track_t *markers)
 * @brief set markers for given track
 * @param track the track
 * @param markers track of markers
 * @ingroup track 
 */
void track_set_markers(track_t *track, track_t *markers);
/** 
 * @fn track_t *track_get_markers(track_t *track)
 * @brief get markers track for given track
 * @param track the track
 * @return markers track
 * @ingroup track 
 */
track_t *track_get_markers(track_t *track);
/** 
 * @fn int track_is_marker(track_t *track)
 * @brief tell if track is a merker track
 * @param track the track
 * @return 1 if true, 0 if false
 * @ingroup track 
 */
int track_is_marker(track_t *track);
/** 
 * @fn void track_set_editor_open(track_t *track)
 * @brief set track editor as opened
 * @param track the track
 * @ingroup track 
 */
void track_set_editor_open(track_t *track);
/** 
 * @fn void track_set_editor_close(track_t *track)
 * @brief set track editor as closed
 * @param track the track
 * @ingroup track 
 */
void track_set_editor_close(track_t *track);
#else
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
#endif

/** 
 * @fn void track_add_event(track_t *track, double time, event_t *event)
 * @brief add event in track at given time
 * @param track the track
 * @param time timetag for event
 * @param event to be added event
 * @ingroup track 
 */
SEQUENCE_API void track_add_event(track_t *track, double time, event_t *event);
/** 
 * @fn void track_add_event_after(track_t *track, double time, event_t *event, event_t *after)
 * @brief add event in track at given time after given event
 * @param track the track
 * @param time timetag for event
 * @param event to be added event
 * @param after add new event after this one ...
 * @ingroup track 
 */
SEQUENCE_API void track_add_event_after(track_t *track, double time, event_t *event, event_t *after);
/** 
 * @fn void track_append_event(track_t *track, double time, event_t *event)
 * @brief append event in track
 * @param track the track
 * @param time timetag for event
 * @param event to be added event
 * @ingroup track 
 */
SEQUENCE_API void track_append_event(track_t *track, double time, event_t *event);
/** 
 * @fn void track_move_event(track_t *track, event_t *event, double time)
 * @brief move event in track
 * @param track the track
 * @param event to be added event
 * @param time new timetag for event
 * @ingroup track 
 */
SEQUENCE_API void track_move_event(track_t *track, event_t *event, double time);
/** 
 * @fn void track_remove_event(track_t *track, event_t *event)
 * @brief remove event from track
 * @param track the track
 * @param event to be removed event
 * @ingroup track 
 */
SEQUENCE_API void track_remove_event(track_t *track, event_t *event);
/** 
 * @fn void track_copy(track_t *org, track_t *copy)
 * @brief copy track content in given track
 * @param org the original track
 * @param copy copy track
 * @ingroup track 
 */
SEQUENCE_API void track_copy(track_t *org, track_t *copy);
/** 
 * @fn void track_merge(track_t *track, track_t *merge)
 * @brief merge track content whit/in given track
 * @param track first track
 * @param merge merge in track
 * @ingroup track 
 */
SEQUENCE_API void track_merge(track_t *track, track_t *merge);
/** 
 * @fn void track_erase_events(track_t *track)
 * @brief remove all events from track
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API void track_erase_events(track_t *track);
/** 
 * @fn void track_clear(track_t *track)
 * @brief remove all events from track and associated markers track
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API void track_clear(track_t *track);
/** 
 * @fn int track_editor_is_open(track_t *track)
 * @brief tell if track editor is open
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API int track_editor_is_open(track_t *track);
/** 
 * @fn int track_is_in_multitrack(track_t *track)
 * @brief tell if track is inside a multitrack
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API int track_is_in_multitrack(track_t *track);

/** 
 * @fn void track_add_event_and_upload(track_t *track, double time, event_t *event)
 * @brief add event in track and upload to client
 * @param track the track
 * @param time timetag for new event
 * @param event new event
 * @ingroup track 
 */
SEQUENCE_API void track_add_event_and_upload(track_t *track, double time, event_t *event);
/** 
 * @fn void track_add_event_after_and_upload(track_t *track, double time, event_t *event, event_t *after)
 * @brief add event in track after given event, and upload to client
 * @param track the track
 * @param time timetag for new event
 * @param event new event
 * @param after add new event after this one ...
 * @ingroup track 
 */
SEQUENCE_API void track_add_event_after_and_upload(track_t *track, double time, event_t *event, event_t *after);
/** 
 * @fn void track_move_event_and_upload(track_t *track, event_t *event, double time)
 * @brief move event in track and upload to client
 * @param track the track
 * @param event the event 
 * @param time new timetag for event
 * @ingroup track 
 */
SEQUENCE_API void track_move_event_and_upload(track_t *track, event_t *event, double time);
/** 
 * @fn void track_remove_event_and_upload(track_t *track, event_t *event)
 * @brief remove event from track and upload to client
 * @param track the track
 * @param event event to be removed
 * @ingroup track 
 */
SEQUENCE_API void track_remove_event_and_upload(track_t *track, event_t *event);
/** 
 * @fn void track_merge_and_upload(track_t *track, track_t *merge)
 * @brief merge track content whit/in given track and upload to client
 * @param track the track
 * @param merge receiving merge track
 * @ingroup track 
 */
SEQUENCE_API void track_merge_and_upload(track_t *track, track_t *merge);
/** 
 * @fn void track_merge_and_upload(track_t *track)
 * @brief remove all events and markers and upload to client
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API void track_clear_and_upload(track_t *track);
/** 
 * @fn void track_update_editor(track_t *track)
 * @brief upload all changed events if editor is visible
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API void track_update_editor(track_t *track);
/** 
 * @fn void track_upload_event(track_t *track, event_t *event)
 * @brief upload event to client
 * @param track the track
 * @param event the event
 * @ingroup track 
 */
SEQUENCE_API void track_upload_event(track_t *track, event_t *event);
/** 
 * @fn void track_clear_at_client(track_t *track)
 * @brief notify track clear to the client
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API void track_clear_at_client(track_t *track);
/** 
 * @fn void track_upload(track_t *track)
 * @brief upload track content to the client
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API void track_upload(track_t *track);
/** 
 * @fn void track_set_dirty(track_t *track)
 * @brief set root patcher for track as dirty (to be saved)
 * @param track the track
 * @ingroup track 
 */
SEQUENCE_API void track_set_dirty(track_t *track);
/** 
 * @fn event_t *track_get_event_by_time(track_t *track, double time)
 * @brief get first event at or after given time
 * @param track the track
 * @param time the time
 * @return the event
 * @ingroup track 
 */
SEQUENCE_API event_t *track_get_event_by_time(track_t *track, double time);
/** 
 * @fn event_t *track_get_next_by_time(track_t *track, double time)
 * @brief get next event after given time
 * @param track the track
 * @param time the time
 * @return the event
 * @ingroup track 
 */
SEQUENCE_API event_t *track_get_next_by_time(track_t *track, double time);
/** 
 * @fn event_t *track_get_next_by_time_after(track_t *track, double time, event_t *here)
 * @brief get next event after given time. The search is started from given element.
 * @param track the track
 * @param time the time
 * @param here search start from this event
 * @return the event
 * @ingroup track 
 */
SEQUENCE_API event_t *track_get_next_by_time_after(track_t *track, double time, event_t *here);
/** 
 * @fn  event_t *track_get_left_by_time_from(track_t *track, double time, event_t *here)
 * @brief get event before or at given time. Search in descending time order starts at given event
 * @param track the track
 * @param time the time
 * @param here search starts from this event
 * @return the event
 * @ingroup track 
 * 
 * get event before or at <time>, search in descending time order starts at event <here>, if not NULL 
 * (N.B.: if time is before first event, the first event is returned(?)
 */
SEQUENCE_API event_t *track_get_left_by_time_from(track_t *track, double time, event_t *here);
/** 
 * @fn track_t *track_get_or_make_markers(track_t *track)
 * @brief get markers track (and creates it if needed)
 * @param track the track
 * @return markers track
 * @ingroup track 
 */
SEQUENCE_API track_t *track_get_or_make_markers(track_t *track);
/** 
 * @fn scomark_t *track_insert_marker(track_t *track, double time, fts_symbol_t type)
 * @brief insert marker of given type at given time
 * @param track the track
 * @param time the time
 * @param type the type
 * @return new scomark
 * @ingroup track 
 */
SEQUENCE_API scomark_t *track_insert_marker(track_t *track, double time, fts_symbol_t type);
/** 
 * @fn void track_segment_get(track_t *track, double begin, double end, event_t **first, event_t **after)
 * @brief  get segment by time
 * @param track the track
 * @param begin starting time
 * @param end end time
 * @param first start search from this event
 * @param after next event after the segment
 * @ingroup track 
 */
SEQUENCE_API void track_segment_get(track_t *track, double begin, double end, event_t **first, event_t **after);
/** 
 * @fn void track_segment_shift(track_t *track, event_t *first, event_t *after, double begin, double end, double shift)
 * @brief move every event in segment of given time
 * @param track the track
 * @param first start search from this event
 * @param after next event after the segment
 * @param begin starting time
 * @param end end time
 * @param shift shift in milliseconds
 * @ingroup track 
 */
SEQUENCE_API void track_segment_shift(track_t *track, event_t *first, event_t *after, double begin, double end, double shift);
/** 
 * @fn void track_segment_stretch(track_t *track, event_t *first, event_t *after, double begin, double end, double stretch)
 * @brief stretch every event in segment of given stretch arg
 * @param track the track
 * @param first start search from this event
 * @param after next event after the segment
 * @param begin starting time
 * @param end end time
 * @param stretch ...
 * @ingroup track 
 */
SEQUENCE_API void track_segment_stretch(track_t *track, event_t *first, event_t *after, double begin, double end, double stretch);
/** 
 * @fn void track_segment_quantize(track_t *track, event_t *first, event_t *after, double begin, double end, double quantize)
 * @brief quantize segment ...
 * @param track the track
 * @param first start search from this event
 * @param after next event after the segment
 * @param begin starting time
 * @param end end time
 * @param quantize ...
 * @ingroup track 
 */
SEQUENCE_API void track_segment_quantize(track_t *track, event_t *first, event_t *after, double begin, double end, double quantize);
/** 
 * @fn void track_move_events_at_client(track_t *track, event_t *first, event_t *after)
 * @brief notify move evnts ti the client
 * @param track the track
 * @param first first moved event
 * @param after first event after moved segment
 * @ingroup track 
 */
SEQUENCE_API void track_move_events_at_client(track_t *track, event_t *first, event_t *after);

/** @name Functions for highlight events in track editor
 *  @{ */

/**
 * @fn void track_highlight_event(track_t *track, event_t *event)
 * @brief highlight event in track editor
 * @param track the track
 * @param event	pointer to track event
 * @ingroup track
 *
 * Highlight one event in track editor and set cursor on its starting time.
 * Previous highlighting is removed.
 */
SEQUENCE_API void track_highlight_event(track_t *track, event_t *event);

/**
 * @fn void track_highlight_events(track_t *track, int n, event_t *event[])
 * @brief highlight events in track editor
 * @param track the track
 * @param n	number of events in event array
 * @param event	array of track events
 * @ingroup track
 *
 * Highlight events in track editor (up to 64) and set cursor to start time of group.
 * Previous highlighting is removed.
 */
SEQUENCE_API void track_highlight_events(track_t *track, int n, event_t *event[]);

/**
 * @fn void track_highlight_time(track_t *track, double time)
 * @brief set highlight cursor to arbitrary time in track editor
 * @param track the track
 * @param time	time to display in editor
 * @ingroup track
 *
 * Set cursor to arbitrary time in track editor.
 * Previous highlighting is removed.
 */
SEQUENCE_API void track_highlight_time(track_t *track, double time);

/**
 * @fn void track_highlight_events_and_time(track_t *track, double time, int n, event_t *event[])
 * @brief highlight events in track editor  and set cursor to given time
 * @param track the track
 * @param time	time to display in editor
 * @param n	number of events in event array
 * @param event	array of track events
 * @ingroup track
 *
 * Highlight events in track editor (up to 64) and set cursor to arbitrary time.
 * Set n to 0 and event to NULL to only display time.
 * Previous highlighting is removed.
 */
SEQUENCE_API void track_highlight_events_and_time(track_t *track, double time, int n, event_t *event[]);

/**
 * @fn void track_highlight_cluster(track_t *track, event_t *event, event_t *next)
 * @brief highlight sequential event group in track editor
 * @param track the track
 * @param event pointer to first event
 * @param event pointer to last event
 * @ingroup track
 *
 * Highlight sequential event group in track editor.
 * Previous highlighting is removed.
 */
SEQUENCE_API void track_highlight_cluster(track_t *track, event_t *event, event_t *next);

/**
 * @fn event_t *track_highlight_and_next(track_t *track, event_t *event)
 * @brief highlight event in track editor and get next event
 * @param track the track
 * @param event the event
 * @return next event in track
 * @ingroup track
 *
 * Highlight one event in track editor, set cursor on its starting time.
 * Previous highlighting is removed.
 */
SEQUENCE_API event_t *track_highlight_and_next(track_t *track, event_t *event);

/**
 * @fn void track_highlight_reset(track_t *track)
 * @brief reset highlighted events in track
 * @param track	pointer to track
 * @ingroup track
 */
SEQUENCE_API void track_highlight_reset(track_t *track);

/** @}	end of doc group highlighting */

/**
 * @fn fts_method_status_t track_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief track dump state method
 * @param o the track object
 * @param s selector
 * @param ac arguments count
 * @param at arguments
 * @param ret atom to store return value
 * @return fts_method_status_t ...
 * @ingroup track
 */
SEQUENCE_API fts_method_status_t track_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
/**
 * @fn fts_method_status_t track_add_event_from_file(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief track dump state method
 * @param o the track object
 * @param s selector
 * @param ac arguments count
 * @param at arguments
 * @param ret atom to store return value
 * @return fts_method_status_t ...
 * @ingroup track
 */
SEQUENCE_API fts_method_status_t track_add_event_from_file(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
/**
 * @fn fts_method_status_t track_event_dump_mess(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief track event dump method
 * @param o the track object
 * @param s selector
 * @param ac arguments count
 * @param at arguments
 * @param ret atom to store return value
 * @return fts_method_status_t ...
 * @ingroup track
 */
SEQUENCE_API fts_method_status_t track_event_dump_mess(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

#endif  /* _SEQUENCE_TRACK_H_ */
