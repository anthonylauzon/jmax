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

#ifndef _DATA_FMAT_H_
#define _DATA_FMAT_H_

#include "data.h"

typedef struct _fmat_
{
  data_object_t o;
  float *values;
  int m;
  int n;
  int alloc;
} fmat_t;

DATA_API fts_symbol_t fmat_symbol;
DATA_API fts_metaclass_t *fmat_type;

#define fmat_get_m(x) ((x)->m)
#define fmat_get_n(x) ((x)->n)

DATA_API void fmat_set_size(fmat_t *fmat, int m, int n);

#define fmat_get_ptr(m) ((m)->values)

#define fmat_get_element(m, i, j) ((m)->values[(i) * (m)->n + (j)])
#define fmat_set_element(m, i, j, x) ((m)->values[(i) * (m)->n + (j)] = (x))

DATA_API void fmat_set_const(fmat_t *mat, float c);
#define fmat_zero(m) fmat_set_const((m), 0.0)

DATA_API void fmat_set_with_onset_from_atoms(fmat_t *mat, int offset, int ac, const fts_atom_t *at);

/* fmat atoms */
#define fmat_atom_get(ap) ((fmat_t *)fts_get_object(ap))
#define fmat_atom_is(ap) (fts_is_a((ap), fmat_type))

#endif
