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
#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include <fts/fts.h>

typedef struct _segment_
{
  fts_object_t o;
  fts_atom_t value;
  double duration;
  double attack;
  double release;
} segment_t;

#define segment_get_value(s) (&(s)->value)
#define segment_get_duration(s) ((s)->duration)
#define segment_get_attack(s) ((s)->attack)
#define segment_get_release(s) ((s)->release)

/* segment atoms */
#define segment_atom_set(ap, x) fts_set_object_with_type((ap), (x), seqsym_segment)
#define segment_atom_get(ap) ((segment_t *)fts_get_object(ap))
#define segment_atom_is(ap) (fts_is_a((ap), seqsym_segment))

#endif
