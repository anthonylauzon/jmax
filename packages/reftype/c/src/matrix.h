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

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "fts.h"
#include "refdata.h"

extern fts_type_t matrix_type;
extern fts_symbol_t matrix_symbol;
extern reftype_t *matrix_reftype;

typedef struct
{
  refdata_t type;
  fts_atom_t *data; /* matrix data */
  int m; /* # of rows */
  int n; /* # of columns */
  int alloc; /* current alloc size for lazy allocation */
} matrix_t;

extern matrix_t *matrix_new(int m, int n);
extern void matrix_delete(matrix_t *mx);

extern void matrix_set_size(matrix_t *mx, int m, int n);
#define matrix_get_m(mx) ((mx)->m)
#define matrix_get_n(mx) ((mx)->n)

#define matrix_get_ptr(vec) ((mx)->data)

extern void matrix_set_element(matrix_t *mx, int i, int j, fts_atom_t atom);
#define matrix_get_element(mx, i, j) ((mx)->data[(i) * (mx)->n + (j)])
extern void matrix_void_element(matrix_t *mx, int i, int j);

#define matrix_get_row(mx, i) ((mx)->data + (i) * (mx)->n)

extern void matrix_void(matrix_t *mx);
extern void matrix_fill(matrix_t *mx, fts_atom_t atom);

void matrix_set_from_atom_list(matrix_t *mx, int offset, int ac, const fts_atom_t *at);

extern int matrix_read_atom_file_newline(matrix_t *mx, fts_symbol_t file_name);
extern int matrix_write_atom_file_newline(matrix_t *mx, fts_symbol_t file_name);

extern int matrix_read_atom_file_separator(matrix_t *mx, fts_symbol_t file_name, 
					      fts_symbol_t separator, int ac, const fts_atom_t *at);
extern int matrix_write_atom_file_separator(matrix_t *mx, fts_symbol_t file_name, fts_symbol_t separator);

/* refdata */
#define matrix_get_constructor(ac, at) (reftype_get_constructor(matrix_reftype, (ac), (at)))
#define matrix_create(ac, at) ((matrix_t *)refdata_create(matrix_reftype, (ac), (at)))
#define matrix_refer(v) refdata_refer(v)
#define matrix_release(v) refdata_release(v)
#define matrix_no_reference(v) refdata_no_reference(v)
#define matrix_set_creator(v, c) refdata_set_creator((v), (c))

/* atom matrix atoms */
#define matrix_atom_set(ap, x) \
  do {fts_set_type(ap, matrix_type); fts_word_set_ptr(fts_atom_value(ap), ((void *)x));} while (0)
#define matrix_atom_get(ap) ((matrix_t *)fts_word_get_ptr(fts_atom_value(ap)))
#define matrix_atom_is(ap) (fts_is_a((ap), matrix_type))

#endif
