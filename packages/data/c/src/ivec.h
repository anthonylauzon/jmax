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

#ifndef _DATA_IVEC_H_
#define _DATA_IVEC_H_

#include <fts/fts.h>

typedef struct _ivec_
{
  fts_object_t o;
  int *values;
  int size;
  int alloc;
  int opened; /* non zero if editor open */
  fts_symbol_t file;
  int vsize; /* visible points */
  int vindex; /* first visible point */
} ivec_t;

extern fts_class_t *ivec_class;
extern fts_symbol_t ivec_symbol;
extern fts_type_t ivec_type;

extern void ivec_set_from_atom_list(ivec_t *vector, int offset, int ac, const fts_atom_t *at);
extern int ivec_get_atoms(ivec_t *vector, int ac, fts_atom_t *at);

extern void ivec_copy(ivec_t *in, ivec_t *out);
extern void ivec_zero(ivec_t *vector);

#define ivec_get_size(vector) ((vector)->size)
extern void ivec_set_size(ivec_t *vector, int size);

#define ivec_is_empty(vector) ((vector)->size == 0)

#define ivec_get_element(vector, index) ((vector)->values[index])
#define ivec_set_element(vector, index, value) ((vector)->values[index] = (value))

void ivec_set_const(ivec_t *vector, int c);
void ivec_zero(ivec_t *vector);

#define ivec_get_ptr(vector) ((vector)->values)
extern void ivec_set_from_ptr(ivec_t *vector, int *ptr, int size);

extern int ivec_get_sum(ivec_t *vector);
extern int ivec_get_sub_sum(ivec_t *vector, int from, int to);
extern int ivec_get_min_value(ivec_t *vector);
extern int ivec_get_max_value(ivec_t *vector);

extern void  ivec_save_bmax(ivec_t *vec, fts_bmax_file_t *f);

/* ivec atoms */
#define ivec_atom_set(ap, x) fts_set_object_with_type((ap), (x), ivec_type)
#define ivec_atom_get(ap) ((ivec_t *)fts_get_object(ap))
#define ivec_atom_is(ap) (fts_is_a((ap), ivec_type))

#endif
