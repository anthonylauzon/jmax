/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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
  fts_symbol_t a[3];

  if(ac == 1)
    {
      /* initialize the class */
      fts_class_init(cl, sizeof(fts_object_t), 1, 1, 0); 
      
      fts_method_define_varargs(cl, 0, vec_symbol, getsize_vec);
      fts_method_define_varargs(cl, 0, ivec_symbol, getsize_ivec);
      fts_method_define_varargs(cl, 0, fvec_symbol, getsize_fvec);
      fts_method_define_varargs(cl, 0, mat_symbol, getsize_mat);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
getsize_config(void)
{
  fts_class_install(fts_new_symbol("getsize"), getsize_instantiate);
}
