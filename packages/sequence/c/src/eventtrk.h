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
#ifndef _EVENTTRK_H_
#define _EVENTTRK_H_

#include "fts.h"
#include "track.h"
#include "event.h"

/*****************************************************************
 *
 *  track of events
 *
 */

extern fts_symbol_t eventtrk_symbol;

typedef struct _eventtrk_ eventtrk_t;

struct _eventtrk_
{ 
  track_t head; /* generic sequence track */

  fts_symbol_t type; /* type of events */

  event_t *first; /* pointer to first event */
  event_t *last; /* pointer to last event */
  int size; /* # of events in track */
};

#define eventtrk_get_type(track) ((track)->type)

#define eventtrk_get_first(track) ((track)->first)
#define eventtrk_get_last(track) ((track)->last)
#define eventtrk_get_size(track) ((track)->size)

#define eventtrk_get_duration(track) ((track)->last->time - (track)->first->time)

extern void eventtrk_add_event(eventtrk_t *track, double time, event_t *event);
extern void eventtrk_add_event_after(eventtrk_t *track, double time, event_t *event, event_t *here);
extern void eventtrk_move_event(event_t *event, double time);
extern void eventtrk_remove_event(event_t *event);

#endif
