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
#ifndef _SEQREF_H_
#define _SEQREF_H_

#include <fts/fts.h>
#include "eventtrk.h"

typedef struct _seqref_
{
  fts_object_t o; /* sequence reference object */
  sequence_t *sequence;
  eventtrk_t *track;
  int index;
} seqref_t;

#define seqref_lock(o, t) (track_lock((track_t *)(t)), ((seqref_t *)(o))->track = (eventtrk_t *)(t))
#define seqref_unlock(o) (track_unlock((track_t *)((seqref_t *)(o))->track), ((seqref_t *)(o))->track = 0)
#define seqref_is_locked(o) (((seqref_t *)(o))->track != 0)

#define seqref_get_track(o) (((seqref_t *)(o))->track)

extern void seqref_set_reference(fts_object_t *o, int ac, const fts_atom_t *at);
extern eventtrk_t *seqref_get_reference(fts_object_t *o);
extern void seqref_init(fts_object_t *o, int ac, const fts_atom_t *at);

extern void seqref_upload(fts_object_t *o);

extern void seqref_highlight_event(fts_object_t *o, event_t *event);
extern event_t *seqref_get_next_and_highlight(fts_object_t *o, event_t *event, double time);
extern event_t *seqref_get_prev_and_highlight(fts_object_t *o, event_t *event, double time);

#endif
