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

#include "fts.h"

typedef struct _fvec_
{
  fts_object_t o;
  float *values;
  int size;
  int alloc;
} fvec_t;

extern fts_class_t *fvec_class;
extern fts_symbol_t fvec_symbol;
extern fts_type_t fvec_type;

extern void fvec_set_from_atom_list(fvec_t *vector, int offset, int ac, const fts_atom_t *at);
extern int fvec_get_atoms(fvec_t *vector, int ac, fts_atom_t *at);

extern void fvec_copy(fvec_t *in, fvec_t *out);
extern void fvec_zero(fvec_t *vector);

#define fvec_get_size(vector) ((vector)->size)
extern void fvec_set_size(fvec_t *vector, int size);

#define fvec_is_empty(vector) ((vector)->size == 0)

#define fvec_get_element(vector, index) ((vector)->values[index])
#define fvec_set_element(vector, index, value) ((vector)->values[index] = (value))

extern void fvec_set_const(fvec_t *vector, float c);
extern void fvec_zero(fvec_t *vector);

#define fvec_get_ptr(vector) ((vector)->values)
extern void fvec_set_from_ptr(fvec_t *vector, float *ptr, int size);

extern float fvec_get_sum(fvec_t *vector);
extern float fvec_get_sub_sum(fvec_t *vector, int from, int to);
extern float fvec_get_min_value(fvec_t *vector);
extern float fvec_get_max_value(fvec_t *vector);

/* fvec atoms */
#define fvec_atom_set(ap, x) fts_set_object_with_type((ap), (x), fvec_type)
#define fvec_atom_get(ap) ((fvec_t *)fts_get_object(ap))
#define fvec_atom_is(ap) (fts_is_a((ap), fvec_type))

#endif
