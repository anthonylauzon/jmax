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
 */
#include <fts/fts.h>
#include "vec.h"
#include <data/c/include/ivec.h>
#include <data/c/include/fvec.h>
#include "mat.h"

/******************************************************
 *
 *  user methods
 *
 */

static void
getsize_vec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *vec = (vec_t *)fts_get_object(at);

  fts_outlet_int(o, 0, vec_get_size(vec));
}

static void
getsize_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = (ivec_t *)fts_get_object(at);

  fts_outlet_int(o, 0, ivec_get_size(vec));
}

static void
getsize_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *vec = (fvec_t *)fts_get_object(at);

  fts_outlet_int(o, 0, fvec_get_size(vec));
}

static void
getsize_mat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *mat = (mat_t *)fts_get_object(at);
  fts_atom_t list[2];

  fts_set_int(list, mat_get_m(mat));
  fts_set_int(list + 1, mat_get_n(mat));

  fts_outlet_varargs(o, 0, 2, list);
}

/******************************************************
 *
 *  class
 *
 */

static void
getsize_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), NULL, NULL); 
  
  fts_class_inlet(cl, 0, vec_type, getsize_vec);
  fts_class_inlet(cl, 0, ivec_type, getsize_ivec);
  fts_class_inlet(cl, 0, fvec_type, getsize_fvec);
  fts_class_inlet(cl, 0, mat_type, getsize_mat);
  
  fts_class_outlet_number(cl, 0);
}

void
getsize_config(void)
{
  fts_class_install(fts_new_symbol("getsize"), getsize_instantiate);
}
