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

#ifndef _DATA_FVEC_H_
#define _DATA_FVEC_H_

#include <fts/packages/data/data.h>

DATA_API fts_symbol_t fvec_symbol;
DATA_API fts_class_t *fvec_type;

typedef fmat_t fvec_t;

#define fvec_get_size(v) ((v)->m)
DATA_API void fvec_set_size(fvec_t *vector, int size);

#define fvec_get_ptr(v) ((v)->values)

#define fvec_get_element(v, i) ((v)->values[i])
#define fvec_set_element(v, i, x) ((v)->values[i] = (x))

DATA_API void fvec_copy(fvec_t *org, fvec_t *copy);

DATA_API void fvec_set_const(fvec_t *vector, float c);

DATA_API void fvec_set_with_onset_from_atoms(fvec_t *vector, int offset, int ac, const fts_atom_t *at);

DATA_API float fvec_get_max_value(fvec_t *vector);
DATA_API float fvec_get_max_abs_value_in_range(fvec_t *vector, int a, int b);
DATA_API float fvec_get_max_value_in_range(fvec_t *vector, int a, int b);
DATA_API float fvec_get_min_value_in_range(fvec_t *vector, int a, int b);

#define fvec_set_editor_open(v) ((v)->opened = 1)
#define fvec_set_editor_close(v) ((v)->opened = 0)
#define fvec_editor_is_open(v) ((v)->opened)

/**
 * If another object changed our data, do the necessary stuff
 * (update editor, set data dirty)
 */
DATA_API void fvec_changed(fvec_t *this);

#endif
