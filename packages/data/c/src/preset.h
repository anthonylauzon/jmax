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
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#ifndef _DATA_PRESET_H_
#define _DATA_PRESET_H_

#include "data.h"

typedef struct 
{
  fts_object_t o;
  fts_object_t **objects;
  fts_hashtable_t hash;
  int n_objects;
  fts_object_t **current; /* current preset (for loading) */
  fts_symbol_t keep;
} preset_t;

DATA_API fts_symbol_t preset_symbol;
DATA_API fts_metaclass_t *preset_type;

DATA_API void preset_get_keys(preset_t *this, fts_array_t *array);

/* preset atoms */
#define preset_atom_get(ap) ((preset_t *)fts_get_object(ap))
#define preset_atom_is(ap) (fts_is_a((ap), preset_type))

#endif
