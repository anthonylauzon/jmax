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

#ifndef _FTS_ATOMARRAY_H_
#define _FTS_ATOMARRAY_H_

#include <fts/lang/mess/mess_types.h>

typedef struct fts_list
{
  fts_atom_t *atoms;
  int size;	
  int alloc;
} fts_list_t;

extern fts_symbol_t fts_s_list;

extern fts_list_t *fts_list_new(int ac, const fts_atom_t *at);
extern void fts_list_delete(fts_list_t *this);

extern void fts_list_copy(fts_list_t *in, fts_list_t *out);
extern void fts_list_void(fts_list_t *array);

extern void fts_list_set_size(fts_list_t *array, int size);
#define fts_list_get_size(array) ((array)->size)

#define fts_list_get_ptr(array) ((array)->atoms)

#define fts_list_check_index(array, i)  ((i >= 0) && (i < (array)->size))
#define fts_list_is_empty(array) ((array)->size == 0)

#define fts_list_get_element(array, index) ((array)->atoms[index])
#define fts_list_set_element(array, index, value) ((array)->atoms[(index)] = (value))

#endif
