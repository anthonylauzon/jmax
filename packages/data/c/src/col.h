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

#ifndef _DATA_COL_H_
#define _DATA_COL_H_

#include "data.h"

DATA_API fts_type_t col_type;
DATA_API fts_symbol_t col_symbol;
DATA_API fts_class_t *col_class;

typedef struct
{
  fts_object_t head;
  mat_t *mat; /* matrix */
  int j; /* column index */
} col_t;

#define col_get_size(x) ((x)->n)

DATA_API void col_set_element(col_t *col, int i, fts_atom_t atom);
#define col_get_element(x, i) ((x)->mat->data[(i) * (x)->mat->n + (x)->(j)])

#define col_check(x) ((x)->j < (x)->mat->n)
#define col_get_onset(x) 
#define col_get_step(x) 

DATA_API void col_void(col_t *col);
DATA_API void col_set_const(col_t *col, fts_atom_t atom);

DATA_API void col_set_from_list(col_t *col, int ac, const fts_atom_t *at);

/* col atoms */
#define col_atom_set(ap, x) fts_set_object_with_type((ap), (x), col_type)
#define col_atom_get(ap) ((col_t *)fts_get_object(ap))
#define col_atom_is(ap) (fts_is_a((ap), col_type))

#endif
