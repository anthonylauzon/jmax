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

#ifndef _MESS_MESSTAB_H_
#define _MESS_MESSTAB_H_

#include "data.h"

typedef struct messtab
{
  fts_object_t o;
  fts_hashtable_t table_symbol;
  fts_hashtable_t table_int;
  fts_symbol_t keep;
} messtab_t;

DATA_API fts_class_t *messtab_class;
DATA_API fts_symbol_t messtab_symbol;
DATA_API fts_metaclass_t *messtab_type;

DATA_API void messtab_get_keys(messtab_t *messtab, fts_array_t *array);

/* messtab atoms */
#define messtab_atom_get(ap) ((messtab_t *)fts_get_object(ap))
#define messtab_atom_is(ap) (fts_is_a((ap), messtab_type))

#endif
