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

#include "fts.h"

typedef struct _track_ track_t;

struct _track_
{ 
  fts_object_t o;

  /* list of tracks in sequence */
  track_t *next;

  struct _sequence_ *sequence; /* sequence of track */

  fts_symbol_t name;

  int active; /* active flag */
  int lock; /* lock counter (non-zero: no delete of track or events) */
};

extern void track_init(track_t *track);

#define track_get_sequence(t) ((t)->sequence)
#define track_get_index(t) ((t)->name)
#define track_get_name(t) ((t)->name)
#define track_get_next(t) ((t)->next)

#define track_is_locked(t) ((t)->lock != 0)

extern void track_lock(track_t *track);
extern void track_unlock(track_t *track);

#define track_set_name(t, n) ((t)->name = (n))

#endif
