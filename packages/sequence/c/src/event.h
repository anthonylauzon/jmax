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
#ifndef _EVENT_H_
#define _EVENT_H_

#include "fts.h"

/*****************************************************************
 *
 *  event
 *
 */

extern fts_symbol_t event_symbol;

typedef struct _event_ event_t;

struct _event_
{
  fts_object_t o;

  /* list of events in sequence */
  event_t *prev;
  event_t *next;

  struct _eventtrk_ *track; /* track of event */

  double time; /* time tag */
};

extern void event_init(event_t *event);

#define event_set_time(e, t) ((e)->time = (t))
#define event_get_time(e) ((e)->time)

#define event_set_track(e, t) ((e)->track = (t))
#define event_get_track(e) ((e)->track)

#define event_get_prev(e) ((e)->prev)
#define event_get_next(e) ((e)->next)

#endif
