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

#ifndef _DATA_FVEC_H_
#define _DATA_FVEC_H_

#include "data.h"

typedef struct _fvec_
{
  fts_object_t o;
  float *values;
  int size;
  int alloc;
  fts_symbol_t keep;
  float sr; /* sr > 0: force sample rate when loading sample files, sr <= 0: sample rate of current file */
} fvec_t;

DATA_API fts_symbol_t fvec_symbol;
DATA_API fts_metaclass_t *fvec_type;

#define fvec_get_sr(v) ((((v)->sr) > 0)? ((v)->sr): (-(v)->sr))

#define fvec_get_size(v) ((v)->size)
DATA_API void fvec_set_size(fvec_t *vector, int size);

#define fvec_get_ptr(v) ((v)->values)

#define fvec_get_element(v, i) ((v)->values[i])
#define fvec_set_element(v, i, x) ((v)->values[i] = (x))

DATA_API void fvec_set_const(fvec_t *vector, float c);
#define fvec_zero(v) fvec_set_const((v), 0.0)

DATA_API void fvec_set_with_onset_from_atoms(fvec_t *vector, int offset, int ac, const fts_atom_t *at);

DATA_API float fvec_get_sum(fvec_t *vector);
DATA_API float fvec_get_sub_sum(fvec_t *vector, int from, int to);
DATA_API float fvec_get_min_value(fvec_t *vector);
DATA_API float fvec_get_max_value(fvec_t *vector);

/* fvec atoms */
#define fvec_atom_get(ap) ((fvec_t *)fts_get_object(ap))
#define fvec_atom_is(ap) (fts_is_a((ap), fvec_type))

#endif
