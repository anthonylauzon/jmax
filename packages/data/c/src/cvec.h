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
 */

#ifndef _DATA_CVEC_H_
#define _DATA_CVEC_H_

#include "data.h"

typedef struct _cvec_
{
  fts_object_t o;
  complex *values;
  int size;
  int alloc;
  fts_symbol_t keep;
} cvec_t;

DATA_API fts_symbol_t cvec_symbol;
DATA_API fts_metaclass_t *cvec_type;

#define cvec_get_size(v) ((v)->size)
DATA_API void cvec_set_size(cvec_t *vector, int size);

#define cvec_get_ptr(v) ((v)->values)

#define cvec_get_element(v, i) ((v)->values[i])
#define cvec_set_element(v, i, x) ((v)->values[i] = (x))

DATA_API void cvec_copy(cvec_t *org, cvec_t *copy);

DATA_API void cvec_set_const(cvec_t *vector, complex c);
#define cvec_zero(v) cvec_set_const((v), CZERO)

DATA_API void cvec_set_with_onset_from_atoms(cvec_t *vector, int offset, int ac, const fts_atom_t *at);

/* cvec atoms */
#define cvec_atom_get(ap) ((cvec_t *)fts_get_object(ap))
#define cvec_atom_is(ap) (fts_is_a((ap), cvec_type))

#endif