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

#ifndef _DATA_FVEC_H_
#define _DATA_FVEC_H_

#include "data.h"
#include "fmat.h"

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

#endif
