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
 */

#ifndef _MESS_DICT_H_
#define _MESS_DICT_H_

#include <data/c/include/data.h>

typedef struct dict
{
  data_object_t o;
  fts_hashtable_t table_symbol;
  fts_hashtable_t table_int;
} dict_t;

DATA_API fts_symbol_t dict_symbol;
DATA_API fts_metaclass_t *dict_type;

DATA_API void dict_store(dict_t *dict, const fts_atom_t *key, const fts_atom_t *atom);
DATA_API void dict_store_atoms(dict_t *dict, const fts_atom_t *key, int ac, const fts_atom_t *at);
DATA_API void dict_recall(dict_t *dict, const fts_atom_t *key, fts_atom_t *atom);
DATA_API void dict_get_keys(dict_t *dict, fts_array_t *array);

/* dict atoms */
#define dict_atom_get(ap) ((dict_t *)fts_get_object(ap))
#define dict_atom_is(ap) (fts_is_a((ap), dict_type))

#endif
