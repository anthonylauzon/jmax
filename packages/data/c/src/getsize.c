/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

/******************************************************
 *
 *  user methods
 *
 */

static void
getsize_atom_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_array_t *array = (fts_atom_array_t *)fts_get_data(at);

  fts_outlet_int(o, 0, fts_atom_array_get_size(array));
}

static void
getsize_integer_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vector = (fts_integer_vector_t *)fts_get_data(at);

  fts_outlet_int(o, 0, fts_integer_vector_get_size(vector));
}

static void
getsize_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vector = (fts_float_vector_t *)fts_get_data(at);

  fts_outlet_int(o, 0, fts_float_vector_get_size(vector));
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

  /* initialize the class */
  fts_class_init(cl, sizeof(fts_object_t), 1, 1, 0); 
  
  a[0] = fts_s_data;
  fts_method_define(cl, 0, fts_s_atom_array, getsize_atom_array, 1, a);
  
  a[0] = fts_s_data;
  fts_method_define(cl, 0, fts_s_integer_vector, getsize_integer_vector, 1, a);

  a[0] = fts_s_data;
  fts_method_define(cl, 0, fts_s_float_vector, getsize_float_vector, 1, a);

  /* type the outlet */
  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);

  return fts_Success;
}

void
getsize_config(void)
{
  fts_metaclass_create(fts_new_symbol("getsize"), getsize_instantiate, fts_always_equiv);
}
