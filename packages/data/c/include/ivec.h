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

#ifndef _DATA_IVEC_H_
#define _DATA_IVEC_H_

#include "data.h"

typedef struct _ivec_
{
  data_object_t o;
  int *values;
  int size;
  int alloc;
  int opened; /* non zero if editor open */
  int vsize; /* visible points */
  int vindex; /* first visible point */
  float zoom; /* current zoom */
  int pixsize; /* visible pixels size */
  struct _ivec_ *copy;
} ivec_t;

DATA_API fts_symbol_t ivec_symbol;
DATA_API fts_metaclass_t *ivec_type;

DATA_API void ivec_set_with_onset_from_atoms(ivec_t *vector, int offset, int ac, const fts_atom_t *at);

#define ivec_get_size(v) ((v)->size)
DATA_API void ivec_set_size(ivec_t *vector, int size);

#define ivec_get_ptr(v) ((v)->values)

#define ivec_get_element(v, i) ((v)->values[i])
#define ivec_set_element(v, i, x) ((v)->values[i] = (x))

DATA_API void ivec_set_const(ivec_t *vector, int c);
DATA_API void ivec_copy(ivec_t *org, ivec_t *copy);

DATA_API void ivec_set_with_onset_from_atoms(ivec_t *vector, int offset, int ac, const fts_atom_t *at);

DATA_API int ivec_get_sum(ivec_t *vector);
DATA_API int ivec_get_sub_sum(ivec_t *vector, int from, int to);
DATA_API int ivec_get_min_value(ivec_t *vector);
DATA_API int ivec_get_max_value(ivec_t *vector);

#endif
