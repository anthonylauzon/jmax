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
 *  objects getelem and setelem
 *
 *    getelem <i> <vector reference>
 *    getelem <i> <j> <matrix reference>
 *
 *    setelem <i> <vector reference>
 *    setelem <i> <j> <matrix reference>
 *
 */

typedef struct 
{
  fts_object_t o;
  int i;
  int j;
  union {
    vector_t *vec;
    int_vector_t *ivec;
    float_vector_t *fvec;
    matrix_t *mat;
    refdata_t *data;
  } ref;
} elem_t;

static void
elem_vector_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac == 2)
    {
      this->i = 0;
      this->ref.data = refdata_atom_get(at + 1);
    }
  else
    {
      this->i = fts_get_number_int(at + 1);
      this->ref.data = refdata_atom_get(at + 2);
    }

  refdata_refer(this->ref.data);
}

static void
elem_matrix_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac == 2)
    {
      this->i = 0;
      this->j = 0;
      this->ref.data = refdata_atom_get(at + 1);
    }
  else
    {
      this->i = fts_get_number_int(at + 1);
      this->j = fts_get_number_int(at + 2);
      this->ref.data = refdata_atom_get(at + 3);
    }

  refdata_refer(this->ref.data);
}

static void
elem_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  refdata_release(this->ref.data);
}

/******************************************************
 *
 *  user methods
 *
 */

static void
elem_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  refdata_t *data = refdata_atom_get(at);

  refdata_release(this->ref.data);
  this->ref.data = data;
  vector_refer(data);
}

static void
setelem_set_i(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  
  this->i = fts_get_number_int(at);
}

static void
setelem_set_j(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  
  this->j = fts_get_number_int(at);
}

static void
setelem_set_ij(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 1 && fts_is_number(at + 1))
    this->j = fts_get_number_int(at + 1);
    
  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);
}

/* getelem vector */

static void
getelem_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = vector_get_size(this->ref.vec);
  int i = this->i;
  
  if(i >= 0 && i < size)
    {
      fts_atom_t *a = &vector_get_element(this->ref.vec, i);
      
      if(!fts_is_void(a))
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
    }
}

static void
getelem_vector_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->i = fts_get_number_int(at);
  getelem_vector(o, 0, 0, 0, 0);
}

static void
getelem_vector_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 1 && vector_atom_is(at + 1))
    elem_set_reference(o, 0, 0, 1, at + 1);

  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);

  getelem_vector(o, 0, 0, 0, 0);
}

/* setelem vector */

static void
setelem_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = vector_get_size(this->ref.vec);
  int i = this->i;
  
  if(ac > 0 && i >= 0 && i < size)
    vector_set_element(this->ref.vec, i, at[0]);
}

static void
setelem_vector_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 2 && vector_atom_is(at + 2))
    elem_set_reference(o, 0, 0, 1, at + 2);

  if(ac > 1 && fts_is_number(at + 1))
    this->i = fts_get_number_int(at + 1);

  setelem_vector(o, 0, 0, 1, at);
}

/* getelem int vector */

static void
getelem_int_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = int_vector_get_size(this->ref.ivec);
  int i = this->i;

  if(i >= 0 && i < size)
    fts_outlet_int(o, 0, int_vector_get_element(this->ref.ivec, i));
}

static void
getelem_int_vector_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->i = fts_get_number_int(at);
  getelem_int_vector(o, 0, 0, 0, 0);
}

static void
getelem_int_vector_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 1 && int_vector_atom_is(at + 1))
    elem_set_reference(o, 0, 0, 1, at + 1);

  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);

  getelem_int_vector(o, 0, 0, 0, 0);
}

/* setelem int vector */

static void
setelem_int_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = int_vector_get_size(this->ref.ivec);
  int i = this->i;

  if(i >= 0 && i < size)
    int_vector_set_element(this->ref.ivec, i, fts_get_number_int(at));
}

static void
setelem_int_vector_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 2 && int_vector_atom_is(at + 2))
    elem_set_reference(o, 0, 0, 1, at + 2);

  if(ac > 1 && fts_is_number(at + 1))
    this->i = fts_get_number_int(at + 1);

  if(ac > 0 && fts_is_number(at))
    setelem_int_vector(o, 0, 0, 1, at);
}

/* getelem float vector */

static void
getelem_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = float_vector_get_size(this->ref.fvec);
  int i = this->i;

  if(i >= 0 && i < size)
    fts_outlet_float(o, 0, float_vector_get_element(this->ref.fvec, i));
}

static void
getelem_float_vector_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->i = fts_get_number_int(at);
  getelem_float_vector(o, 0, 0, 0, 0);
}

static void
getelem_float_vector_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 1 && float_vector_atom_is(at + 1))
    elem_set_reference(o, 0, 0, 1, at + 1);

  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);

  getelem_float_vector(o, 0, 0, 0, 0);
}

/* setelem float vector */

static void
setelem_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = float_vector_get_size(this->ref.fvec);
  int i = this->i;

  if(i >= 0 && i < size)
    float_vector_set_element(this->ref.fvec, i, fts_get_number_float(at));
}

static void
setelem_float_vector_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 2 && float_vector_atom_is(at + 2))
    elem_set_reference(o, 0, 0, 1, at + 2);

  if(ac > 1 && fts_is_number(at + 1))
    this->i = fts_get_number_int(at + 1);

  if(ac > 0 && fts_is_number(at))
    setelem_float_vector(o, 0, 0, 1, at);
}

/* getelem matrix */

static void
getelem_matrix(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  matrix_t *mat = this->ref.mat;
  int m = matrix_get_m(mat);
  int n = matrix_get_n(mat);
  int i = this->i;
  int j = this->j;
  
  if(i >= 0 && i < m && j >= 0 && j < n)
    {
      fts_atom_t *a = &matrix_get_element(this->ref.mat, i, j);
      
      if(!fts_is_void(a))
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
    }
}

static void
getelem_matrix_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->i = fts_get_number_int(at);
  getelem_matrix(o, 0, 0, 0, 0);
}

static void
getelem_matrix_col(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->j = fts_get_number_int(at);
}

static void
getelem_matrix_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 2 && matrix_atom_is(at + 2))
    elem_set_reference(o, 0, 0, 1, at + 2);

  if(ac > 1 && fts_is_number(at + 1))
    this->j = fts_get_number_int(at + 1);
    
  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);

  getelem_matrix(o, 0, 0, 0, 0);
}

/* setelem matrix */

static void
setelem_matrix(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  matrix_t *mat = this->ref.mat;
  int m = matrix_get_m(mat);
  int n = matrix_get_n(mat);
  int i = this->i;
  int j = this->j;
  
  if(ac > 0 && i >= 0 && i < m && j >= 0 && j < n)
    matrix_set_element(this->ref.mat, i, j, at[0]);
}

static void
setelem_matrix_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 3 && matrix_atom_is(at + 3))
    elem_set_reference(o, 0, 0, 1, at + 3);

  if(ac > 2 && fts_is_number(at + 2))
    this->j = fts_get_number_int(at + 2);
    
  if(ac > 1 && fts_is_number(at + 1))
    this->i = fts_get_number_int(at + 1);
  
  setelem_matrix(o, 0, 0, 1, at);
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

  if((ac == 2 || (ac == 3 && fts_is_number(at + 1))) &&
     (vector_atom_is(at + ac - 1) || int_vector_atom_is(at + ac - 1) || float_vector_atom_is(at + ac - 1)))
    {
      fts_class_init(cl, sizeof(elem_t), 2, 1, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, elem_vector_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, elem_delete);
      
      if(vector_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, getelem_vector_list);

	  fts_method_define_varargs(cl, 0, fts_s_bang, getelem_vector);
	  fts_method_define_varargs(cl, 0, fts_s_int, getelem_vector_index);
	  fts_method_define_varargs(cl, 0, fts_s_float, getelem_vector_index);
	  fts_method_define_varargs(cl, 1, vector_symbol, elem_set_reference);
	}
      else if(int_vector_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, getelem_int_vector_list);

	  fts_method_define_varargs(cl, 0, fts_s_bang, getelem_int_vector);
	  fts_method_define_varargs(cl, 0, fts_s_int, getelem_int_vector_index);
	  fts_method_define_varargs(cl, 0, fts_s_float, getelem_int_vector_index);
	  fts_method_define_varargs(cl, 1, int_vector_symbol, elem_set_reference);
	}
      else if(float_vector_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, getelem_float_vector_list);

	  fts_method_define_varargs(cl, 0, fts_s_bang, getelem_float_vector);
	  fts_method_define_varargs(cl, 0, fts_s_int, getelem_float_vector_index);
	  fts_method_define_varargs(cl, 0, fts_s_float, getelem_float_vector_index);
	  fts_method_define_varargs(cl, 1, float_vector_symbol, elem_set_reference);
	}
      else
	return &fts_CannotInstantiate;
    }
  else if((ac == 2 || (ac == 4 && fts_is_number(at + 1) && fts_is_number(at + 2))) &&
	  (matrix_atom_is(at + ac - 1)))
    {
      fts_class_init(cl, sizeof(elem_t), 3, 1, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, elem_matrix_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, elem_delete);
      
      if(matrix_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, getelem_matrix_list);

	  fts_method_define_varargs(cl, 0, fts_s_bang, getelem_matrix);
	  fts_method_define_varargs(cl, 0, fts_s_int, getelem_matrix_row);
	  fts_method_define_varargs(cl, 0, fts_s_float, getelem_matrix_row);
	  fts_method_define_varargs(cl, 1, fts_s_int, getelem_matrix_col);
	  fts_method_define_varargs(cl, 1, fts_s_float, getelem_matrix_col);
	  fts_method_define_varargs(cl, 2, matrix_symbol, elem_set_reference);
	}
      else
	return &fts_CannotInstantiate;
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

static fts_status_t
setelem_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if((ac == 2 || (ac == 3 && fts_is_number(at + 1))) &&
     (vector_atom_is(at + ac - 1) || int_vector_atom_is(at + ac - 1) || float_vector_atom_is(at + ac - 1)))
    {
      fts_class_init(cl, sizeof(elem_t), 3, 0, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, elem_vector_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, elem_delete);
      
      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);

      if(vector_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, setelem_vector_list);

	  fts_method_define_varargs(cl, 0, fts_s_anything, setelem_vector);
	  fts_method_define_varargs(cl, 2, vector_symbol, elem_set_reference);
	}
      else if(int_vector_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, setelem_int_vector_list);

	  fts_method_define_varargs(cl, 0, fts_s_int, setelem_int_vector);
	  fts_method_define_varargs(cl, 0, fts_s_float, setelem_int_vector);
	  fts_method_define_varargs(cl, 2, int_vector_symbol, elem_set_reference);
	}
      else if(float_vector_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, setelem_float_vector_list);

	  fts_method_define_varargs(cl, 0, fts_s_int, setelem_float_vector);
	  fts_method_define_varargs(cl, 0, fts_s_float, setelem_float_vector);
	  fts_method_define_varargs(cl, 2, float_vector_symbol, elem_set_reference);
	}
    }
  else if((ac == 2 || (ac == 4 && fts_is_number(at + 1) && fts_is_number(at + 2))) &&
	  (matrix_atom_is(at + ac - 1)))
    {
      fts_class_init(cl, sizeof(elem_t), 4, 0, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, elem_matrix_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, elem_delete);
      
      fts_method_define_varargs(cl, 1, fts_s_list, setelem_set_ij);
      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);
      fts_method_define_varargs(cl, 2, fts_s_int, setelem_set_j);
      fts_method_define_varargs(cl, 2, fts_s_float, setelem_set_j);

      if(matrix_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, setelem_matrix_list);

	  fts_method_define_varargs(cl, 0, fts_s_anything, setelem_matrix);
	  fts_method_define_varargs(cl, 3, matrix_symbol, elem_set_reference);
	}
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

void
getelem_config(void)
{
  fts_metaclass_install(fts_new_symbol("getelem"), getelem_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("setelem"), setelem_instantiate, fts_arg_type_equiv);
}
