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

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "fts.h"
#include "refdata.h"
#include "matrix.h"

extern fts_type_t vector_type;
extern fts_symbol_t vector_symbol;
extern reftype_t *vector_reftype;

typedef matrix_t vector_t;

extern vector_t *vector_new(int size);
#define vector_delete(vec) (matrix_delete((matrix_t *)(vec)))

#define vector_set_size(vec, n) (matrix_set_size((vector_t *)(vec), (n), 1))
#define vector_get_size(vec) (((matrix_t *)(vec))->m)

#define vector_get_ptr(vec) (((matrix_t *)(vec))->data)

#define vector_set_element(vec, i, v) (matrix_set_element(vec, i, 0, v))
#define vector_get_element(vec, i) (matrix_get_element(vec, i, 0))
#define vector_void_element(vec, i) (matrix_void_element(vec, i, 0))

#define vector_void(vec) (matrix_void((matrix_t *)(vec)))
#define vector_fill(vec, a) (matrix_fill((matrix_t *)(vec), (a)))
#define vector_set_from_atom_list(mx, x, ac, at) matrix_set_from_atom_list((matrix_t *)(mx), (x), (ac), (at))

extern int vector_import_ascii(vector_t *vec, fts_symbol_t file_name);
extern int vector_export_ascii(vector_t *vec, fts_symbol_t file_name);

/* refdata */
#define vector_get_constructor(ac, at) (reftype_get_constructor(vector_reftype, (ac), (at)))
#define vector_create(ac, at) ((vector_t *)refdata_create(vector_reftype, (ac), (at)))
#define vector_refer(v) refdata_refer(v)
#define vector_release(v) refdata_release(v)
#define vector_no_reference(v) refdata_no_reference(v)
#define vector_set_creator(v, c) refdata_set_creator((v), (c))

/* atom array atoms */
#define vector_atom_set(ap, x) \
  do {fts_set_type(ap, vector_type); fts_word_set_ptr(fts_atom_value(ap), ((void *)x));} while (0)
#define vector_atom_get(ap) ((vector_t *)fts_word_get_ptr(fts_atom_value(ap)))
#define vector_atom_is(ap) (fts_is_a((ap), vector_type))
#endif

