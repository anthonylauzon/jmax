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

#ifndef _DATA_VEC_H_
#define _DATA_VEC_H_

#include <fts/packages/data/data.h>

DATA_API fts_class_t *vec_type;
DATA_API fts_symbol_t vec_symbol;

typedef mat_t vec_t;

#define vec_set_size(x, n) (mat_set_size((vec_t *)(x), (n), 1))
#define vec_get_size(x) (((mat_t *)(x))->m)

#define vec_get_ptr(x) (((mat_t *)(x))->data)

#define vec_set_element(x, i, v) (mat_set_element((mat_t *)(x), (i), 0, (v)))
#define vec_get_element(x, i) (mat_get_element((mat_t *)(x), (i), 0))

#define vec_set_const(x, a) (mat_set_const((mat_t *)(x), (a)))
#define vec_set_with_onset_from_atoms(mx, x, ac, at) mat_set_with_onset_from_atoms((mat_t *)(mx), (x), (ac), (at))

DATA_API int vec_read_atom_file(vec_t *vec, fts_symbol_t file_name);
DATA_API int vec_write_atom_file(vec_t *vec, fts_symbol_t file_name);

#endif

