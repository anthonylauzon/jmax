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

#ifndef _SEQUENCE_TRACK_EDITOR_H_
#define _SEQUENCE_TRACK_EDITOR_H_

#include <fts/fts.h>
#include <fts/packages/sequence/sequence.h>

extern fts_class_t *track_editor_class;

struct _track_editor
{ 
  fts_object_t o;

  track_t *track; /* the track */

	int win_x; /* editor window's bounds */
  int win_y;
  int win_w;
  int win_h;
  fts_symbol_t label; /* property event label */
  float zoom; /* editor zoom_factor */
  int transp; /* editor x_transposition */	
};

extern void track_editor_upload(track_editor_t *this);
extern void track_editor_dump_gui(track_editor_t *this, fts_dumper_t *dumper);

#endif
