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
#include "data.h"

/******************************************************
 *
 *  object and class
 *
 */

typedef void (*getelem_meth) (fts_object_t *, fts_data_t *, int);

typedef struct 
{
  fts_object_t o;
  int index;
} getelem_index_t;

typedef struct 
{
  fts_object_t o;
  fts_data_t *data;
  int size;
  getelem_meth meth;
} getelem_data_t;

static void getelem_set_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void getelem_set_data(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void getelem_set_atom_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void getelem_set_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

static void
getelem_index_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_set_index(o, 0, 0, 1, at + 1);
}

static void
getelem_data_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_data_t *data = fts_get_data(at + 1);

  if(fts_data_get_class_name(data) == fts_s_atom_array)
    getelem_set_atom_array(o, 0, 0, 1, at + 1);
  else if(fts_data_get_class_name(data) == fts_s_float_vector)
    getelem_set_float_vector(o, 0, 0, 1, at + 1);
}

/******************************************************
 *
 *  user methods
 *
 */

/* class with index argument */

static void
getelem_set_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_index_t *this = (getelem_index_t *)o;
  int index = fts_get_int(at);

  if(index >= 0)
    this->index = index;
  else
    this->index = 0;
}

static void
getelem_atom_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_index_t *this = (getelem_index_t *)o;
  fts_atom_array_t *array = (fts_atom_array_t *)fts_get_data(at);
  int size = fts_atom_array_get_size(array);
  int index = this->index;

  if(size > index)
    {
      fts_atom_t atom = fts_atom_array_get_element(array, index);

      data_atom_outlet(o, 0, atom);
    }
}

static void
getelem_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_index_t *this = (getelem_index_t *)o;
  fts_float_vector_t *vector = (fts_float_vector_t *)fts_get_data(at);
  int size = fts_float_vector_get_size(vector);
  int index = this->index;

  if(size > index)
    fts_outlet_float(o, 0, fts_float_vector_get_element(vector, index));
}

/* class with data argument */

static void
getelem_from_atom_array(fts_object_t *o, fts_data_t *data, int index)
{
  fts_atom_array_t *array = (fts_atom_array_t *)data;
  int size = fts_atom_array_get_size(array);
  
  if(index < size)
    {
      fts_atom_t atom = fts_atom_array_get_element(array, index);
      
      data_atom_outlet(o, 0, atom);
    }
}

static void
getelem_from_float_vector(fts_object_t *o, fts_data_t *data, int index)
{
  fts_float_vector_t *vector = (fts_float_vector_t *)data;
  int size = fts_float_vector_get_size(vector);
  
  if(index < size)
    fts_outlet_float(o, 0, fts_float_vector_get_element(vector, index));
}

static void
getelem_set_atom_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_data_t *this = (getelem_data_t *)o;
  fts_data_t *data = fts_get_data(at); 

  this->data = data;
  this->meth = getelem_from_atom_array;
}

static void
getelem_set_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_data_t *this = (getelem_data_t *)o;
  fts_data_t *data = fts_get_data(at); 

  this->data = data;
  this->meth = getelem_from_float_vector;
}

static void
getelem_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_data_t *this = (getelem_data_t *)o;
  int index = fts_get_int(at);
  fts_atom_t atom;

  if(index >= 0)
    this->meth(o, this->data, index);
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
getelem_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* data argument (array of vector) */
  if(fts_is_data(at + 1))
    {
      fts_data_t *data = fts_get_data(at + 1);

      /* initialize the class */
      fts_class_init(cl, sizeof(getelem_data_t), 2, 1, 0); 
      
      /* define the system methods */
      a[0] = fts_s_symbol;
      a[1] = fts_s_data;
      fts_method_define(cl, fts_SystemInlet, fts_s_init, getelem_data_init, 2, a);

      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, getelem_index, 1, a);

      a[0] = fts_s_data;
      fts_method_define(cl, 1, fts_s_atom_array, getelem_set_atom_array, 1, a);

      a[0] = fts_s_data;
      fts_method_define(cl, 1, fts_s_float_vector, getelem_set_float_vector, 1, a);
    }
  /* integer argument (index) */
  else if(fts_is_int(at + 1))
    {
      /* initialize the class */
      fts_class_init(cl, sizeof(getelem_index_t), 2, 1, 0); 
      
      /* define the system methods */
      a[0] = fts_s_symbol;
      a[1] = fts_s_int;
      fts_method_define(cl, fts_SystemInlet, fts_s_init, getelem_index_init, 2, a);

      a[0] = fts_s_int;
      fts_method_define(cl, 1, fts_s_int, getelem_set_index, 1, a);

      a[0] = fts_s_data;
      fts_method_define(cl, 0, fts_s_atom_array, getelem_atom_array, 1, a);
      
      a[0] = fts_s_data;
      fts_method_define(cl, 0, fts_s_float_vector, getelem_float_vector, 1, a);
    }
  else
    return &fts_CannotInstantiate;
  
  /* define the methods */
  return fts_Success;
}

void
getelem_config(void)
{
  fts_metaclass_create(fts_new_symbol("getelem"), getelem_instantiate, fts_arg_type_equiv);
}
