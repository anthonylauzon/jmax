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
 *  objects getelem and setelem
 *
 *    getelem <i> <vec>
 *    getelem <i> <j> <mat>
 *
 *    setelem <i> <vec>
 *    setelem <i> <j> <mat>
 *
 */

typedef struct 
{
  fts_object_t o;
  int i;
  int j;
  union {
    vec_t *vec;
    ivec_t *ivec;
    fvec_t *fvec;
    mat_t *mat;
    fts_object_t *obj;
  } ref;
} elem_t;

static void
elem_vec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac == 2)
    {
      this->i = 0;
      this->ref.obj = fts_get_object(at + 1);
    }
  else
    {
      this->i = fts_get_number_int(at + 1);
      this->ref.obj = fts_get_object(at + 2);
    }

  fts_object_refer(this->ref.obj);
}

static void
elem_mat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac == 2)
    {
      this->i = 0;
      this->j = 0;
      this->ref.obj = fts_get_object(at + 1);
    }
  else
    {
      this->i = fts_get_number_int(at + 1);
      this->j = fts_get_number_int(at + 2);
      this->ref.obj = fts_get_object(at + 3);
    }

  fts_object_refer(this->ref.obj);
}

static void
elem_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  fts_object_release(this->ref.obj);
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
  fts_object_t *obj = fts_get_object(at);

  fts_object_release(this->ref.obj);
  this->ref.obj = obj;
  fts_object_refer(obj);
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

/* getelem vec */

static void
getelem_vec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = vec_get_size(this->ref.vec);
  int i = this->i;
  
  if(i >= 0 && i < size)
    {
      fts_atom_t *a = &vec_get_element(this->ref.vec, i);
      
      if(!fts_is_void(a))
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
    }
}

static void
getelem_vec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->i = fts_get_number_int(at);
  getelem_vec(o, 0, 0, 0, 0);
}

static void
getelem_vec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 1 && vec_atom_is(at + 1))
    elem_set_reference(o, 0, 0, 1, at + 1);

  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);

  getelem_vec(o, 0, 0, 0, 0);
}

/* setelem vec */

static void
setelem_vec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = vec_get_size(this->ref.vec);
  int i = this->i;
  
  if(ac > 0 && i >= 0 && i < size)
    vec_set_element(this->ref.vec, i, at[0]);
}

static void
setelem_vec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 2 && vec_atom_is(at + 2))
    elem_set_reference(o, 0, 0, 1, at + 2);

  if(ac > 1 && fts_is_number(at + 1))
    this->i = fts_get_number_int(at + 1);

  setelem_vec(o, 0, 0, 1, at);
}

/* getelem int vec */

static void
getelem_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = ivec_get_size(this->ref.ivec);
  int i = this->i;

  if(i >= 0 && i < size)
    fts_outlet_int(o, 0, ivec_get_element(this->ref.ivec, i));
}

static void
getelem_ivec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->i = fts_get_number_int(at);
  getelem_ivec(o, 0, 0, 0, 0);
}

static void
getelem_ivec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 1 && ivec_atom_is(at + 1))
    elem_set_reference(o, 0, 0, 1, at + 1);

  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);

  getelem_ivec(o, 0, 0, 0, 0);
}

/* setelem int vec */

static void
setelem_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = ivec_get_size(this->ref.ivec);
  int i = this->i;

  if(i >= 0 && i < size)
    ivec_set_element(this->ref.ivec, i, fts_get_number_int(at));
}

static void
setelem_ivec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 2 && ivec_atom_is(at + 2))
    elem_set_reference(o, 0, 0, 1, at + 2);

  if(ac > 1 && fts_is_number(at + 1))
    this->i = fts_get_number_int(at + 1);

  if(ac > 0 && fts_is_number(at))
    setelem_ivec(o, 0, 0, 1, at);
}

/* getelem float vec */

static void
getelem_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = fvec_get_size(this->ref.fvec);
  int i = this->i;

  if(i >= 0 && i < size)
    fts_outlet_float(o, 0, fvec_get_element(this->ref.fvec, i));
}

static void
getelem_fvec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->i = fts_get_number_int(at);
  getelem_fvec(o, 0, 0, 0, 0);
}

static void
getelem_fvec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 1 && fvec_atom_is(at + 1))
    elem_set_reference(o, 0, 0, 1, at + 1);

  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);

  getelem_fvec(o, 0, 0, 0, 0);
}

/* setelem float vec */

static void
setelem_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  int size = fvec_get_size(this->ref.fvec);
  int i = this->i;

  if(i >= 0 && i < size)
    fvec_set_element(this->ref.fvec, i, fts_get_number_float(at));
}

static void
setelem_fvec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 2 && fvec_atom_is(at + 2))
    elem_set_reference(o, 0, 0, 1, at + 2);

  if(ac > 1 && fts_is_number(at + 1))
    this->i = fts_get_number_int(at + 1);

  if(ac > 0 && fts_is_number(at))
    setelem_fvec(o, 0, 0, 1, at);
}

/* getelem mat */

static void
getelem_mat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  mat_t *mat = this->ref.mat;
  int m = mat_get_m(mat);
  int n = mat_get_n(mat);
  int i = this->i;
  int j = this->j;
  
  if(i >= 0 && i < m && j >= 0 && j < n)
    {
      fts_atom_t *a = &mat_get_element(this->ref.mat, i, j);
      
      if(!fts_is_void(a))
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);
    }
}

static void
getelem_mat_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->i = fts_get_number_int(at);
  getelem_mat(o, 0, 0, 0, 0);
}

static void
getelem_mat_col(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  this->j = fts_get_number_int(at);
}

static void
getelem_mat_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 2 && mat_atom_is(at + 2))
    elem_set_reference(o, 0, 0, 1, at + 2);

  if(ac > 1 && fts_is_number(at + 1))
    this->j = fts_get_number_int(at + 1);
    
  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);

  getelem_mat(o, 0, 0, 0, 0);
}

/* setelem mat */

static void
setelem_mat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;
  mat_t *mat = this->ref.mat;
  int m = mat_get_m(mat);
  int n = mat_get_n(mat);
  int i = this->i;
  int j = this->j;
  
  if(ac > 0 && i >= 0 && i < m && j >= 0 && j < n)
    mat_set_element(this->ref.mat, i, j, at[0]);
}

static void
setelem_mat_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  elem_t *this = (elem_t *)o;

  if(ac > 3 && mat_atom_is(at + 3))
    elem_set_reference(o, 0, 0, 1, at + 3);

  if(ac > 2 && fts_is_number(at + 2))
    this->j = fts_get_number_int(at + 2);
    
  if(ac > 1 && fts_is_number(at + 1))
    this->i = fts_get_number_int(at + 1);
  
  setelem_mat(o, 0, 0, 1, at);
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
     (vec_atom_is(at + ac - 1) || ivec_atom_is(at + ac - 1) || fvec_atom_is(at + ac - 1)))
    {
      fts_class_init(cl, sizeof(elem_t), 2, 1, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, elem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, elem_delete);
      
      if(vec_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, getelem_vec_list);

	  fts_method_define_varargs(cl, 0, fts_s_bang, getelem_vec);
	  fts_method_define_varargs(cl, 0, fts_s_int, getelem_vec_index);
	  fts_method_define_varargs(cl, 0, fts_s_float, getelem_vec_index);
	  fts_method_define_varargs(cl, 1, vec_symbol, elem_set_reference);
	}
      else if(ivec_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, getelem_ivec_list);

	  fts_method_define_varargs(cl, 0, fts_s_bang, getelem_ivec);
	  fts_method_define_varargs(cl, 0, fts_s_int, getelem_ivec_index);
	  fts_method_define_varargs(cl, 0, fts_s_float, getelem_ivec_index);
	  fts_method_define_varargs(cl, 1, ivec_symbol, elem_set_reference);
	}
      else if(fvec_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, getelem_fvec_list);

	  fts_method_define_varargs(cl, 0, fts_s_bang, getelem_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, getelem_fvec_index);
	  fts_method_define_varargs(cl, 0, fts_s_float, getelem_fvec_index);
	  fts_method_define_varargs(cl, 1, fvec_symbol, elem_set_reference);
	}
      else
	return &fts_CannotInstantiate;
    }
  else if((ac == 2 || (ac == 4 && fts_is_number(at + 1) && fts_is_number(at + 2))) &&
	  (mat_atom_is(at + ac - 1)))
    {
      fts_class_init(cl, sizeof(elem_t), 3, 1, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, elem_mat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, elem_delete);
      
      if(mat_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, getelem_mat_list);

	  fts_method_define_varargs(cl, 0, fts_s_bang, getelem_mat);
	  fts_method_define_varargs(cl, 0, fts_s_int, getelem_mat_row);
	  fts_method_define_varargs(cl, 0, fts_s_float, getelem_mat_row);
	  fts_method_define_varargs(cl, 1, fts_s_int, getelem_mat_col);
	  fts_method_define_varargs(cl, 1, fts_s_float, getelem_mat_col);
	  fts_method_define_varargs(cl, 2, mat_symbol, elem_set_reference);
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
     (vec_atom_is(at + ac - 1) || ivec_atom_is(at + ac - 1) || fvec_atom_is(at + ac - 1)))
    {
      fts_class_init(cl, sizeof(elem_t), 3, 0, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, elem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, elem_delete);
      
      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);

      if(vec_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, setelem_vec_list);

	  fts_method_define_varargs(cl, 0, fts_s_anything, setelem_vec);
	  fts_method_define_varargs(cl, 2, vec_symbol, elem_set_reference);
	}
      else if(ivec_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, setelem_ivec_list);

	  fts_method_define_varargs(cl, 0, fts_s_int, setelem_ivec);
	  fts_method_define_varargs(cl, 0, fts_s_float, setelem_ivec);
	  fts_method_define_varargs(cl, 2, ivec_symbol, elem_set_reference);
	}
      else if(fvec_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, setelem_fvec_list);

	  fts_method_define_varargs(cl, 0, fts_s_int, setelem_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, setelem_fvec);
	  fts_method_define_varargs(cl, 2, fvec_symbol, elem_set_reference);
	}
    }
  else if((ac == 2 || (ac == 4 && fts_is_number(at + 1) && fts_is_number(at + 2))) &&
	  (mat_atom_is(at + ac - 1)))
    {
      fts_class_init(cl, sizeof(elem_t), 4, 0, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, elem_mat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, elem_delete);
      
      fts_method_define_varargs(cl, 1, fts_s_list, setelem_set_ij);
      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);
      fts_method_define_varargs(cl, 2, fts_s_int, setelem_set_j);
      fts_method_define_varargs(cl, 2, fts_s_float, setelem_set_j);

      if(mat_atom_is(at + ac - 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_list, setelem_mat_list);

	  fts_method_define_varargs(cl, 0, fts_s_anything, setelem_mat);
	  fts_method_define_varargs(cl, 3, mat_symbol, elem_set_reference);
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
