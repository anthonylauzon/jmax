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

#ifndef _DATA_ROW_H_
#define _DATA_ROW_H_

#include "data.h"
#include "mat.h"

DATA_API fts_type_t row_type;
DATA_API fts_symbol_t row_symbol;
DATA_API fts_class_t *row_class;

typedef struct
{
  fts_object_t head;
  mat_t *mat; /* matrix */
  int i; /* row index */
} row_t;

#define row_get_size(x) ((x)->mat->n)

#define row_set_element(x, j, v) (mat_set_element((x)->mat, (x)->i, (j), (v)))
#define row_get_element(x, j) (mat_get_element((x)->mat, (x)->i, (j)))
#define row_void_element(x, j) (mat_void_element((x)->mat, (x)->i, (j)))

#define row_get_onset(x) ((x)->i * n)

DATA_API void row_void(row_t *col);
DATA_API void row_set_const(row_t *col, fts_atom_t atom);
DATA_API void row_set_from_atoms(row_t *col, int onset, int ac, const fts_atom_t *at);

/* col atoms */
#define row_atom_set(ap, x) fts_set_object_with_type((ap), (x), row_type)
#define row_atom_get(ap) ((row_t *)fts_get_object(ap))
#define row_atom_is(ap) (fts_is_a((ap), row_type))

#endif
