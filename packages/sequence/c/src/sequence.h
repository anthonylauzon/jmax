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

#include <fts/fts.h>
#include "track.h"

/*****************************************************************
 *
 *  sequence
 *
 */

typedef struct _sequence_
{ 
  fts_object_t o;

  track_t *tracks; /* list of tracks */ 
  int size; /* # of tracks */ 

  int open; /* flag: is 1 if sequence editor is open */
  track_t *currently_loaded_event_track; /* its more like a hack for loading bmax files */
} sequence_t;

#define sequence_get_size(s) ((s)->size)
#define sequence_get_first_track(s) ((s)->tracks)

#define sequence_set_editor_open(s) ((s)->open = 1)
#define sequence_set_editor_close(s) ((s)->open = 0)
#define sequence_editor_is_open(s) ((s)->open != 0)

extern void sequence_add_track(sequence_t *sequence, track_t *track);
extern void sequence_remove_track(sequence_t *sequence, track_t *track);

extern track_t *sequence_get_track_by_name(sequence_t *sequence, fts_symbol_t name);
extern track_t *sequence_get_track_by_index(sequence_t *sequence, int index);

#endif



