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
#include "fts.h"
#include "vector.h"
#include "intvec.h"
#include "floatvec.h"
#include "matrix.h"

/******************************************************
 *
 *  user methods
 *
 */

static void
getsize_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vector_t *vector = vector_atom_get(at);

  fts_outlet_int(o, 0, vector_get_size(vector));
}

static void
getsize_int_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vector = int_vector_atom_get(at);

  fts_outlet_int(o, 0, int_vector_get_size(vector));
}

static void
getsize_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_vector_t *vector = float_vector_atom_get(at);

  fts_outlet_int(o, 0, float_vector_get_size(vector));
}

static void
getsize_matrix(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matrix_t *matrix = matrix_atom_get(at);
  fts_atom_t list[2];

  fts_set_int(list, matrix_get_m(matrix));
  fts_set_int(list + 1, matrix_get_n(matrix));

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
      
      fts_method_define_varargs(cl, 0, vector_symbol, getsize_vector);
      fts_method_define_varargs(cl, 0, int_vector_symbol, getsize_int_vector);
      fts_method_define_varargs(cl, 0, float_vector_symbol, getsize_float_vector);
      fts_method_define_varargs(cl, 0, matrix_symbol, getsize_matrix);
      
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
