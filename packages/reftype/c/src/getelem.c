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
 *  object and class
 *
 */

typedef struct 
{
  fts_object_t o;
  int index;
} getelem_vector_index_t;

typedef struct 
{
  fts_object_t o;
  int i;
  int j;
} getelem_matrix_index_t;

typedef struct 
{
  fts_object_t o;
  vector_t *vec;
} getelem_vector_t;

typedef struct 
{
  fts_object_t o;
  int_vector_t *vec;
} getelem_int_vector_t;

typedef struct 
{
  fts_object_t o;
  float_vector_t *vec;
} getelem_float_vector_t;

typedef struct 
{
  fts_object_t o;
  matrix_t *mat;
} getelem_matrix_t;

static void getelem_vector_index_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void getelem_matrix_index_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void getelem_vector_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void getelem_int_vector_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void getelem_float_vector_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void getelem_matrix_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

static void
getelem_vector_index_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vector_index_set(o, 0, 0, ac - 1, at + 1);
}

static void
getelem_matrix_index_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_matrix_index_set(o, 0, 0, ac - 1, at + 1);
}

static void
getelem_vector_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vector_t *this = (getelem_vector_t *)o;

  this->vec = vector_atom_get(at + 1);
  vector_refer(this->vec);
}

static void
getelem_int_vector_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_int_vector_t *this = (getelem_int_vector_t *)o;

  this->vec = int_vector_atom_get(at + 1);
  int_vector_refer(this->vec);
}

static void
getelem_float_vector_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_float_vector_t *this = (getelem_float_vector_t *)o;

  this->vec = float_vector_atom_get(at + 1);
  float_vector_refer(this->vec);
}

static void
getelem_matrix_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_matrix_t *this = (getelem_matrix_t *)o;

  this->mat = matrix_atom_get(at + 1);
  matrix_refer(this->mat);
}

/******************************************************
 *
 *  user methods
 *
 */

static void
getelem_vector_index_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vector_index_t *this = (getelem_vector_index_t *)o;

  this->index = fts_get_number_int(at);
}

static void
getelem_matrix_index_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_matrix_index_t *this = (getelem_matrix_index_t *)o;
  int i, j;

  if(ac > 1 && fts_is_number(at) && fts_is_number(at + 1))
    {
      this->i = fts_get_number_int(at);
      this->j = fts_get_number_int(at + 1);
    }
}

static void
getelem_vector_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vector_t *this = (getelem_vector_t *)o;
  vector_t *vec = vector_atom_get(at);

  vector_release(this->vec);
  this->vec = vec;
  vector_refer(vec);
}

static void
getelem_int_vector_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_int_vector_t *this = (getelem_int_vector_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);

  int_vector_release(this->vec);
  this->vec = vec;
  int_vector_refer(vec);
}

static void
getelem_float_vector_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_float_vector_t *this = (getelem_float_vector_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  
  float_vector_release(this->vec);
  this->vec = vec;
  float_vector_refer(vec);
}

static void
getelem_matrix_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_matrix_t *this = (getelem_matrix_t *)o;
  matrix_t *mat = matrix_atom_get(at);
  
  matrix_release(this->mat);
  this->mat = mat;
  matrix_refer(mat);
}

static void
getelem_vector_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vector_t *this = (getelem_vector_t *)o;
  int index = fts_get_number_int(at);
  int size = vector_get_size(this->vec);
  
  if(index >= 0 && index < size)
    {
      fts_atom_t *a = &vector_get_element(this->vec, index);
      
      if(!fts_is_void(a))
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
    }
}

static void
getelem_int_vector_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_int_vector_t *this = (getelem_int_vector_t *)o;
  int index = fts_get_number_int(at);
  int size = int_vector_get_size(this->vec);

  if(index >= 0 && index < size)
    fts_outlet_int(o, 0, int_vector_get_element(this->vec, index));
}

static void
getelem_float_vector_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_float_vector_t *this = (getelem_float_vector_t *)o;
  int index = fts_get_number_int(at);
  int size = float_vector_get_size(this->vec);

  if(index >= 0 && index < size)
    fts_outlet_float(o, 0, float_vector_get_element(this->vec, index));
}

static void
getelem_matrix_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_matrix_t *this = (getelem_matrix_t *)o;

  if(ac > 1 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int i = fts_get_number_int(at);
      int j = fts_get_number_int(at + 1);
      fts_atom_t *a = &matrix_get_element(this->mat, i, j);

      if(!fts_is_void(a))
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
    }
}

static void
getelem_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vector_index_t *this = (getelem_vector_index_t *)o;
  vector_t *vec = vector_atom_get(at);
  int size = vector_get_size(vec);
  int index = this->index;

  if(ac > 1 && fts_is_number(at + 1))
     this->index = fts_get_number_int(at + 1);

  if(index >= 0 && index < size)
    {
      fts_atom_t *a = &vector_get_element(vec, index);
      
      if(!fts_is_void(a))
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
    }
}

static void
getelem_int_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vector_index_t *this = (getelem_vector_index_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int index = this->index;

  if(index >= 0 && index < size)
    fts_outlet_int(o, 0, int_vector_get_element(vec, index));
}

static void
getelem_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vector_index_t *this = (getelem_vector_index_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  int size = float_vector_get_size(vec);
  int index = this->index;

  if(index >= 0 && index < size)
    fts_outlet_float(o, 0, float_vector_get_element(vec, index));
}

static void
getelem_matrix(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_matrix_index_t *this = (getelem_matrix_index_t *)o;
  matrix_t *mat = matrix_atom_get(at);
  int m = matrix_get_m(mat);
  int n = matrix_get_n(mat);
  int i = this->i;
  int j = this->j;

  if(i >= 0 && i < m && j >= 0 && j < n)
    {
      fts_atom_t *a = &matrix_get_element(mat, this->i, this->j);
      
      if(!fts_is_void(a))
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
    }
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

  if(ac == 2 && fts_is_number(at + 1))  /* integer argument (index) */
    {
      fts_class_init(cl, sizeof(getelem_vector_index_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vector_index_init);

      fts_method_define_varargs(cl, 0, vector_symbol, getelem_vector);
      fts_method_define_varargs(cl, 0, int_vector_symbol, getelem_int_vector);
      fts_method_define_varargs(cl, 0, float_vector_symbol, getelem_float_vector);
      fts_method_define_varargs(cl, 1, fts_s_int, getelem_vector_index_set);
      fts_method_define_varargs(cl, 1, fts_s_float, getelem_vector_index_set);
    }
  else if(ac == 3 && fts_is_number(at + 1) && fts_is_number(at + 1))  /* integer argument (index) */
    {
      fts_class_init(cl, sizeof(getelem_matrix_index_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_matrix_index_init);

      fts_method_define_varargs(cl, 0, matrix_symbol, getelem_matrix);
      fts_method_define_varargs(cl, 1, fts_s_list, getelem_matrix_index_set);
    }
  else if(ac == 2 && vector_atom_is(at + 1))
    {
      fts_class_init(cl, sizeof(getelem_vector_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vector_init);

      fts_method_define_varargs(cl, 0, fts_s_int, getelem_vector_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getelem_vector_index);
      fts_method_define_varargs(cl, 1, vector_symbol, getelem_vector_set);
    }
  else if(ac == 2 && int_vector_atom_is(at + 1))
    {
      fts_class_init(cl, sizeof(getelem_int_vector_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_int_vector_init);

      fts_method_define_varargs(cl, 0, fts_s_int, getelem_int_vector_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getelem_int_vector_index);
      fts_method_define_varargs(cl, 1, int_vector_symbol, getelem_int_vector_set);
    }
  else if(ac == 2 && float_vector_atom_is(at + 1))
    {
      fts_class_init(cl, sizeof(getelem_float_vector_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_float_vector_init);

      fts_method_define_varargs(cl, 0, fts_s_int, getelem_float_vector_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getelem_float_vector_index);
      fts_method_define_varargs(cl, 1, float_vector_symbol, getelem_float_vector_set);
    }
  else if(ac == 2 && matrix_atom_is(at + 1))
    {
      fts_class_init(cl, sizeof(getelem_matrix_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_matrix_init);

      fts_method_define_varargs(cl, 0, fts_s_list, getelem_matrix_index);
      fts_method_define_varargs(cl, 1, matrix_symbol, getelem_matrix_set);
    }
  else
    return &fts_CannotInstantiate;
  
  /* define the methods */
  return fts_Success;
}

void
getelem_config(void)
{
  fts_metaclass_install(fts_new_symbol("getelem"), getelem_instantiate, fts_arg_type_equiv);
}
