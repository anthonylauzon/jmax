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

#ifndef _DATA_PARAM_H_
#define _DATA_PARAM_H_

#include "data.h"

typedef struct _param_
{
  fts_object_t o;
  fts_array_t atoms;
  fts_objectlist_t listeners;
  fts_symbol_t keep;
} param_t;

DATA_API void param_add_listener(param_t *param, fts_object_t *listener);
DATA_API void param_remove_listener(param_t *param, fts_object_t *listener);
DATA_API void param_call_listeners(param_t *param, int ac, const fts_atom_t *at);

DATA_API fts_class_t *param_class;
DATA_API fts_symbol_t param_symbol;
DATA_API fts_type_t param_type;

/* param atoms */
#define param_atom_set(ap, x) fts_set_object_with_type((ap), (x), param_type)
#define param_atom_get(ap) ((param_t *)fts_get_object(ap))
#define param_atom_is(ap) (fts_is_a((ap), param_type))

#endif