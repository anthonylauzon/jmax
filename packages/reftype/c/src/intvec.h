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

#ifndef _INTVEC_H_
#define _INTVEC_H_

#include "fts.h"
#include "refdata.h"

typedef struct _int_vector_
{
  refdata_t refdata;
  int *values;
  int size;	
  int alloc;
} int_vector_t;

extern fts_type_t int_vector_type;
extern fts_symbol_t int_vector_symbol;
extern fts_data_class_t *int_vector_data_class;
extern reftype_t *int_vector_reftype;

extern int_vector_t *int_vector_new(int size);
extern void int_vector_delete(int_vector_t *this);

extern void int_vector_set_from_atom_list(int_vector_t *vector, int offset, int ac, const fts_atom_t *at);
extern int int_vector_get_atoms(int_vector_t *vector, int ac, fts_atom_t *at);

extern void int_vector_copy(int_vector_t *in, int_vector_t *out);
extern void int_vector_zero(int_vector_t *vector);

#define int_vector_get_size(vector) ((vector)->size)
#define int_vector_get_aloc_size(vector) ((vector)->size)
extern void int_vector_set_size(int_vector_t *vector, int size);

#define int_vector_is_empty(vector) ((vector)->size == 0)

#define int_vector_get_element(vector, index) ((vector)->values[index])
#define int_vector_set_element(vector, index, value) ((vector)->values[index] = (value))

void int_vector_set_const(int_vector_t *vector, int c);
void int_vector_zero(int_vector_t *vector);

#define int_vector_get_ptr(vector) ((vector)->values)
extern void int_vector_set_from_ptr(int_vector_t *vector, int *ptr, int size);

extern int int_vector_get_sum(int_vector_t *vector);
extern int int_vector_get_sub_sum(int_vector_t *vector, int from, int to);
extern int int_vector_get_min_value(int_vector_t *vector);
extern int int_vector_get_max_value(int_vector_t *vector);

/* files */
extern int int_vector_read_atom_file(int_vector_t *vec, fts_symbol_t file_name);
extern int int_vector_write_atom_file(int_vector_t *vec, fts_symbol_t file_name);

/* save in bmax format */
extern void int_vector_save_bmax(int_vector_t *this, fts_bmax_file_t *f);

/* refdata */
#define int_vector_get_constructor(ac, at) (reftype_get_constructor(int_vector_reftype, (ac), (at)))
#define int_vector_create(ac, at) ((int_vector_t *)refdata_create(int_vector_reftype, (ac), (at)))
#define int_vector_refer(v) refdata_refer(v)
#define int_vector_release(v) refdata_release(v)
#define int_vector_no_reference(v) refdata_no_reference(v)
#define int_vector_set_creator(v, c) refdata_set_creator((v), (c))

/* int_vector atoms */
#define int_vector_atom_set(ap, x) \
  do {fts_set_type(ap, int_vector_type); fts_word_set_ptr(fts_atom_value(ap), ((void *)x));} while (0)
#define int_vector_atom_get(ap) ((int_vector_t *)fts_word_get_ptr(fts_atom_value(ap)))
#define int_vector_atom_is(ap) (fts_is_a((ap), int_vector_type))

#endif
