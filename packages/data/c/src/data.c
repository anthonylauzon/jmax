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

/*****************************************************************
 *
 * (pseudo) generic utils
 *
 */

fts_data_t *
data_create(fts_symbol_t type, int ac, const fts_atom_t *at)
{
  fts_data_t *data = 0;

  if(type == fts_s_integer_vector)
    data = fts_integer_vector_constructor(ac, at);
  else if(type == fts_s_float_vector)
    data = fts_float_vector_constructor(ac, at);
  else if(type == fts_s_atom_array)
    data = fts_atom_array_constructor(ac, at);
  else if(type == fts_s_atom_array)
    data = fts_atom_array_constructor(ac, at);

  return data;
}

void
data_destroy(fts_data_t *data)
{
  if(fts_data_is(data, fts_s_integer_vector))
    fts_integer_vector_delete((fts_integer_vector_t *)data);
  else if(fts_data_is(data, fts_s_float_vector))
    fts_float_vector_delete((fts_float_vector_t *)data);
  else if(fts_data_is(data, fts_s_atom_array))
    fts_atom_array_delete((fts_atom_array_t *)data);
}

fts_data_t *
data_replicate(fts_data_t *data)
{
  fts_data_t *copy;
  fts_symbol_t type = fts_data_get_class_name(data);

  if(type == fts_s_integer_vector)
    {      
      copy = (fts_data_t *)fts_integer_vector_new(0);
      fts_integer_vector_copy((fts_integer_vector_t *)data, (fts_integer_vector_t *)copy);
    }
  else if(type == fts_s_float_vector)
    {      
      copy = (fts_data_t *)fts_float_vector_new(0);
      fts_float_vector_copy((fts_float_vector_t *)data, (fts_float_vector_t *)copy);
    }
  else if(type == fts_s_atom_array)
    {      
      copy = (fts_data_t *)fts_atom_array_new(0);
      fts_atom_array_copy((fts_atom_array_t *)data, (fts_atom_array_t *)copy);
    }

  return copy;
}

void
data_release(fts_data_t *data)
{
  if(fts_data_derefer(data) == 0)
    data_destroy(data);
}


/* get data size in # of atoms */
int 
data_get_size(fts_data_t *data)
{
  if(data->class == fts_integer_vector_data_class)
    return ((fts_integer_vector_t *)data)->size;
  else if(data->class == fts_integer_vector_data_class)
    return ((fts_float_vector_t *)data)->size;
  else if(data->class == fts_atom_array_data_class)
    return ((fts_atom_array_t *)data)->size;

  return 0;
}

/* get data as array of atoms (takes pointer to pre-allocated (!) array) and
   return original size (might be bigger or smaller than pre-allocated array with ac) */
int
data_get_atoms(fts_data_t *data, int ac, fts_atom_t *at)
{
  if(data->class == fts_integer_vector_data_class)
    return fts_integer_vector_get_atoms((fts_integer_vector_t *)data, ac, at);
  else if(data->class == fts_float_vector_data_class)
    return fts_float_vector_get_atoms((fts_float_vector_t *)data, ac, at);
  else if(data->class == fts_atom_array_data_class)
    return fts_atom_array_get_atoms((fts_atom_array_t *)data, ac, at);

  return 0;
}

/*****************************************************************
 *
 *  data module
 *
 */

extern void data_expr_init(void);
extern void data_types_init(void);

extern void operators_init(void);

extern void monops_init(void);

extern void binops_init(void);
extern void binop_arith_init(void);
extern void binop_arith_inplace_init(void);
extern void binop_comp_init(void);

extern void binop_obj_config(void);
extern void atom_obj_config(void);
extern void post_obj_config(void);

extern void getelem_config(void);
extern void getsize_config(void);

static void
data_module_init(void)
{
  data_expr_init();
  data_types_init();

  operators_init();

  monops_init();

  binops_init();
  binop_arith_init();
  binop_arith_inplace_init();
  binop_comp_init();

  /*  monop_obj_config();*/
  binop_obj_config();
  atom_obj_config();
  post_obj_config();

  getelem_config();
  getsize_config();
}

fts_module_t data_module = {"data", "basic data handling classes", data_module_init};
