/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _INTVEC_H_
#define _INTVEC_H_

typedef struct fts_integer_vector
{
  fts_data_t dataobj;
  int *values;
  int size;	
  int alloc;
  fts_symbol_t name;
} fts_integer_vector_t;

extern fts_data_class_t *fts_integer_vector_data_class;
extern fts_symbol_t fts_s_integer_vector;

extern fts_integer_vector_t *fts_integer_vector_new(int size);
extern void fts_integer_vector_delete(fts_integer_vector_t *this);

fts_data_t *fts_integer_vector_constructor(int ac, const fts_atom_t *at);

extern void fts_integer_vector_set_from_atom_list(fts_integer_vector_t *vector, int offset, int ac, const fts_atom_t *at);
extern int fts_integer_vector_get_atoms(fts_integer_vector_t *vector, int ac, fts_atom_t *at);

extern void fts_integer_vector_copy(fts_integer_vector_t *in, fts_integer_vector_t *out);
extern void fts_integer_vector_zero(fts_integer_vector_t *vector);

#define fts_integer_vector_get_size(vector) ((vector)->size)
#define fts_integer_vector_get_name(vector) ((vector)->name)

extern void fts_integer_vector_set_size(fts_integer_vector_t *vector, int size);

#define fts_integer_vector_is_empty(vector) ((vector)->size == 0)

#define fts_integer_vector_get_element(vector, index) ((vector)->values[index])
#define fts_integer_vector_set_element(vector, index, value) ((vector)->values[index] = (value))

#define fts_integer_vector_set_const(vector, constant) fts_vec_ifill((constant), (int *)((vector)->values), (vector)->size)

#define fts_integer_vector_get_ptr(vector) ((vector)->values)
extern void fts_integer_vector_set_from_ptr(fts_integer_vector_t *vector, int *ptr, int size);

extern int fts_integer_vector_get_sum(fts_integer_vector_t *vector);
extern int fts_integer_vector_get_sub_sum(fts_integer_vector_t *vector, int from, int to);
extern int fts_integer_vector_get_min_value(fts_integer_vector_t *vector);
extern int fts_integer_vector_get_max_value(fts_integer_vector_t *vector);

/* common data methods */
extern void fts_integer_vector_save_bmax(fts_integer_vector_t *this, fts_bmax_file_t *f);

#endif
