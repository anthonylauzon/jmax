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

#ifndef _FLOATVEC_H_
#define _FLOATVEC_H_

#include "fts.h"
#include "refdata.h"

typedef struct _float_vector_
{
  refdata_t refdata;
  float *values;
  int size;
  int alloc;
} float_vector_t;

extern fts_data_class_t *float_vector_data_class;
extern fts_symbol_t float_vector_symbol;
extern fts_type_t float_vector_type;
extern reftype_t *float_vector_reftype;

extern float_vector_t *float_vector_new(int size);
extern void float_vector_delete(float_vector_t *vector);

extern void float_vector_set_from_atom_list(float_vector_t *vector, int offset, int ac, const fts_atom_t *at);
extern int float_vector_get_atoms(float_vector_t *vector, int ac, fts_atom_t *at);

extern void float_vector_copy(float_vector_t *in, float_vector_t *out);
extern void float_vector_zero(float_vector_t *vector);

#define float_vector_get_size(vector) ((vector)->size)
extern void float_vector_set_size(float_vector_t *vector, int size);

#define float_vector_is_empty(vector) ((vector)->size == 0)

#define float_vector_get_element(vector, index) ((vector)->values[index])
#define float_vector_set_element(vector, index, value) ((vector)->values[index] = (value))

#define float_vector_set_const(vector, constant) fts_vec_ffill((constant), (vector)->values, (vector)->size)

#define float_vector_get_ptr(vector) ((vector)->values)
extern void float_vector_set_from_ptr(float_vector_t *vector, float *ptr, int size);

extern float float_vector_get_sum(float_vector_t *vector);
extern float float_vector_get_sub_sum(float_vector_t *vector, int from, int to);
extern float float_vector_get_min_value(float_vector_t *vector);
extern float float_vector_get_max_value(float_vector_t *vector);

/* save in bmax format */
extern void float_vector_save_bmax(float_vector_t *vector, fts_bmax_file_t *f);

/* refdata */
#define float_vector_get_constructor(ac, at) reftype_get_constructor(float_vector_reftype, (ac), (at))
#define float_vector_create(ac, at) ((float_vector_t *)refdata_create(float_vector_reftype, (ac), (at)))
#define float_vector_refer(v) refdata_refer(v)
#define float_vector_release(v) refdata_release(v)
#define float_vector_set_creator(v, c) refdata_set_creator((v), (c))

/* float_vector atoms */
#define float_vector_atom_set(ap, x) \
  do {fts_set_type(ap, float_vector_type); fts_word_set_ptr(fts_atom_value(ap), ((void *)x));} while (0)
#define float_vector_atom_get(ap) ((float_vector_t *)fts_word_get_ptr(fts_atom_value(ap)))
#define float_vector_atom_is(ap) (fts_is_a((ap), float_vector_type))

#endif
