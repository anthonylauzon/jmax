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

#ifndef _DATA_IVEC_H_
#define _DATA_IVEC_H_

#include <fts/packages/data/data.h>

typedef struct _ivec_
{
  fts_object_t o;

  int *values;
  int size;
  int alloc;
  int opened; /* non zero if editor open */
  
  fts_object_t *editor;

} ivec_t;

DATA_API fts_symbol_t ivec_symbol;
DATA_API fts_class_t *ivec_class;
#define ivec_type ivec_class

DATA_API void ivec_set_with_onset_from_atoms(ivec_t *vector, int offset, int ac, const fts_atom_t *at);

#define ivec_get_size(v) ((v)->size)
DATA_API void ivec_set_size(ivec_t *vector, int size);

#define ivec_get_ptr(v) ((v)->values)

#define ivec_get_element(v, i) ((v)->values[i])
#define ivec_set_element(v, i, x) ((v)->values[i] = (x))

#define ivec_set_editor_open(v) ((v)->opened = 1)
#define ivec_set_editor_close(v) ((v)->opened = 0)
#define ivec_editor_is_open(v) ((v)->opened)

DATA_API void ivec_set_const(ivec_t *vector, int c);
DATA_API void ivec_copy(ivec_t *org, ivec_t *copy);

DATA_API void ivec_set_with_onset_from_atoms(ivec_t *vector, int offset, int ac, const fts_atom_t *at);

DATA_API int ivec_get_sum(ivec_t *vector);
DATA_API int ivec_get_sub_sum(ivec_t *vector, int from, int to);
DATA_API int ivec_get_min_value(ivec_t *vector);
DATA_API int ivec_get_max_value(ivec_t *vector);
DATA_API int ivec_get_max_abs_value_in_range(ivec_t *vector, int a, int b);
DATA_API int ivec_get_max_value_in_range(ivec_t *vector, int a, int b);
DATA_API int ivec_get_min_value_in_range(ivec_t *vector, int a, int b);

#endif


