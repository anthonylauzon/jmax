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

struct fts_list
{
  fts_atom_t *atoms;
  int size;	
  int alloc;
};

FTS_API fts_symbol_t fts_s_list;

FTS_API fts_list_t *fts_list_new(int ac, const fts_atom_t *at);
FTS_API void fts_list_delete(fts_list_t *this);

FTS_API void fts_list_init(fts_list_t *list, int ac, const fts_atom_t *at);
FTS_API void fts_list_set(fts_list_t *list, int ac, const fts_atom_t *at);
FTS_API void fts_list_reset(fts_list_t *list);
FTS_API void fts_list_set_size(fts_list_t *list, int size);

FTS_API void fts_list_append(fts_list_t *list, int ac, const fts_atom_t *at);
FTS_API void fts_list_copy(fts_list_t *in, fts_list_t *out);

#define fts_list_get_ptr(l) ((l)->atoms)
#define fts_list_get_size(l) ((l)->size)

#define fts_list_check_index(l, i)  ((i >= 0) && (i < (l)->size))
#define fts_list_is_empty(l) ((l)->size == 0)

#define fts_list_get_element(l, index) ((l)->atoms[index])
#define fts_list_set_element(l, index, value) ((l)->atoms[(index)] = (value))

#endif
