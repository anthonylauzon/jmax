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

#ifndef _FLOATVECTOR_H_
#define _FLOATVECTOR_H_

typedef struct fts_float_vector
{
  fts_data_t dataobj;
  float *values;
  int size;
  int alloc;
} fts_float_vector_t;

extern fts_data_class_t *fts_float_vector_data_class;
extern fts_symbol_t fts_s_float_vector;

extern fts_float_vector_t *fts_float_vector_new(int size);
extern void fts_float_vector_delete(fts_float_vector_t *vector);

fts_data_t *fts_float_vector_constructor(int ac, const fts_atom_t *at);

extern void fts_float_vector_set_from_atom_list(fts_float_vector_t *vector, int offset, int ac, const fts_atom_t *at);
extern int fts_float_vector_get_atoms(fts_float_vector_t *vector, int ac, fts_atom_t *at);

extern void fts_float_vector_copy(fts_float_vector_t *in, fts_float_vector_t *out);
extern void fts_float_vector_zero(fts_float_vector_t *vector);

#define fts_float_vector_get_size(vector) ((vector)->size)
extern void fts_float_vector_set_size(fts_float_vector_t *vector, int size);

#define fts_float_vector_is_empty(vector) ((vector)->size == 0)

#define fts_float_vector_get_element(vector, index) ((vector)->values[index])
#define fts_float_vector_set_element(vector, index, value) ((vector)->values[index] = (value))

#define fts_float_vector_set_const(vector, constant) fts_vec_ffill((constant), (vector)->values, (vector)->size)

#define fts_float_vector_get_ptr(vector) ((vector)->values)
extern void fts_float_vector_set_from_ptr(fts_float_vector_t *vector, float *ptr, int size);

extern float fts_float_vector_get_sum(fts_float_vector_t *vector);
extern float fts_float_vector_get_sub_sum(fts_float_vector_t *vector, int from, int to);
extern float fts_float_vector_get_min_value(fts_float_vector_t *vector);
extern float fts_float_vector_get_max_value(fts_float_vector_t *vector);

extern void fts_float_vector_save_bmax(fts_float_vector_t *vector, fts_bmax_file_t *f);

#endif

