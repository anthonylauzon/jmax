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
#include "ivec.h"
#include "fvec.h"
#include "mat.h"

/******************************************************
 *
 *  user methods
 *
 */

static void
getsize_vec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *vec = vec_atom_get(at);

  fts_outlet_int(o, 0, vec_get_size(vec));
}

static void
getsize_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ivec_atom_get(at);

  fts_outlet_int(o, 0, ivec_get_size(vec));
}

static void
getsize_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *vec = fvec_atom_get(at);

  fts_outlet_int(o, 0, fvec_get_size(vec));
}

static void
getsize_mat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *mat = mat_atom_get(at);
  fts_atom_t list[2];

  fts_set_int(list, mat_get_m(mat));
  fts_set_int(list + 1, mat_get_n(mat));

  fts_outlet_send(o, 0, fts_s_list, 2, list);
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
getsize_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  /* initialize the class */
  fts_class_init(cl, sizeof(fts_object_t), 1, 1, 0); 
  
  fts_method_define_varargs(cl, 0, vec_symbol, getsize_vec);
  fts_method_define_varargs(cl, 0, ivec_symbol, getsize_ivec);
  fts_method_define_varargs(cl, 0, fvec_symbol, getsize_fvec);
  fts_method_define_varargs(cl, 0, mat_symbol, getsize_mat);
  
  return fts_Success;
}

void
getsize_config(void)
{
  fts_class_install(fts_new_symbol("getsize"), getsize_instantiate);
}
