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
#include "value.h"
#include "ivec.h"
#include "fvec.h"
#include "vec.h"
#include "mat.h"

/******************************************************
 *
 *  objects getval and setval
 *
 *    getval <value>
 *    getelem <i> <vec>
 *    getelem <i> <j> <mat>
 *
 *    setval <value>
 *    setelem <i> <vec>
 *    setelem <i> <j> <mat>
 *
 */

typedef struct 
{
  fts_object_t o;
  fts_object_t *obj;
} getval_t;

typedef struct 
{
  fts_object_t o;
  fts_object_t *obj;
  int i;
} getelem_vec_t;

typedef struct 
{
  fts_object_t o;
  fts_object_t *obj;
  int i;
  int j;
} getelem_mat_t;

static void
getval_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getval_t *this = (getval_t *)o;

  this->obj = fts_get_object(at + 1);
  fts_object_refer(this->obj);
}

static void
getelem_vec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  if(ac == 2)
    {
      this->i = 0;
      this->obj = fts_get_object(at + 1);
    }
  else
    {
      this->i = fts_get_number_int(at + 1);
      this->obj = fts_get_object(at + 2);
    }

  fts_object_refer(this->obj);
}

static void
getelem_mat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;

  if(ac == 2)
    {
      this->i = 0;
      this->j = 0;
      this->obj = fts_get_object(at + 1);
    }
  else if(ac == 3)
    {
      this->i = 0;

      if(fts_is_number(at + 1))
	this->j = fts_get_number_int(at + 1);
      else
	fts_object_set_error(o, "Invalid index argument");

      this->obj = fts_get_object(at + 2);
    }
  else
    {
      if(fts_is_number(at + 1) && fts_is_number(at + 2))
	{
	  this->i = fts_get_number_int(at + 1);
	  this->j = fts_get_number_int(at + 2);
	}
      else
	fts_object_set_error(o, "Invalid index argument");

      this->obj = fts_get_object(at + 3);
    }

  fts_object_refer(this->obj);
}

static void
getval_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getval_t *this = (getval_t *)o;

  fts_object_release(this->obj);
}

/******************************************************
 *
 *  set indices and reference
 *
 */

static void
getval_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getval_t *this = (getval_t *)o;
  fts_object_t *obj = fts_get_object(at);

  fts_object_release(this->obj);
  this->obj = obj;
  fts_object_refer(obj);
}

static void
setelem_set_i(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  
  this->i = fts_get_number_int(at);
}

static void
setelem_set_j(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;
  
  this->j = fts_get_number_int(at);
}

static void
setelem_set_ij(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;

  if(ac > 1 && fts_is_number(at + 1))
    this->j = fts_get_number_int(at + 1);
    
  if(ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);
}

/******************************************************
 *
 *  value
 *
 */

static void
getval_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getval_t *this = (getval_t *)o;
  fts_atom_t out = value_get((value_t *)this->obj);

  if(fts_is_object(&out))
    {
      fts_refer(&out);
      fts_outlet_send(o, 0, fts_get_selector(&out), 1, &out);
      fts_release(&out);
    }
  else if(!fts_is_void(&out))
    fts_outlet_send(o, 0, fts_get_selector(&out), 1, &out);
}

static void
getval_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getval_t *this = (getval_t *)o;
  
  getval_set_reference(o, 0, 0, 1, at);
  getval_bang(o, 0, 0, 0, 0);
}

static void
setval_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getval_t *this = (getval_t *)o;
  
  if(ac > 0 && s == fts_get_selector(at))
    value_set((value_t *)this->obj, at[0]);
}

static void
setval_value_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getval_t *this = (getval_t *)o;

  switch(ac)
    {
    default:
    case 2:
      if(value_atom_is(at + 1))
	getval_set_reference(o, 0, 0, 1, at + 1);
    case 1:
      value_set((value_t *)this->obj, at[0]);
    case 0:
      break;
    }
}

/******************************************************
 *
 *  ivec
 *
 */

static void
getval_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  int size = ivec_get_size((ivec_t *)this->obj);
  int i = this->i;

  if(i >= 0 && i < size)
    fts_outlet_int(o, 0, ivec_get_element((ivec_t *)this->obj, i));
}

static void
getval_ivec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  this->i = fts_get_number_int(at);
  getval_ivec(o, 0, 0, 0, 0);
}

static void
getval_ivec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  switch(ac)
    {
    default:
    case 2:
      if(ivec_atom_is(at + 1))
	getval_set_reference(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	this->i = fts_get_number_int(at);
    case 0:
      break;
    }
}

static void
setval_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  int size = ivec_get_size((ivec_t *)this->obj);
  int i = this->i;

  if(i >= 0 && i < size)
    ivec_set_element((ivec_t *)this->obj, i, fts_get_number_int(at));
}

static void
setval_ivec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(ivec_atom_is(at + 2))
	getval_set_reference(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->i = fts_get_number_int(at + 1);
    case 1:
      if(fts_is_number(at))
	setval_ivec(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

/******************************************************
 *
 *  fvec
 *
 */

static void
getval_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  int size = fvec_get_size((fvec_t *)this->obj);
  int i = this->i;

  if(i >= 0 && i < size)
    fts_outlet_float(o, 0, fvec_get_element((fvec_t *)this->obj, i));
}

static void
getval_fvec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  this->i = fts_get_number_int(at);
  getval_fvec(o, 0, 0, 0, 0);
}

static void
getval_fvec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  switch(ac)
    {
    default:
    case 2:
      if(fvec_atom_is(at + 1))
	getval_set_reference(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	this->i = fts_get_number_int(at);
    case 0:
      getval_fvec(o, 0, 0, 0, 0);
      break;
    }
}

static void
setval_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  int size = fvec_get_size((fvec_t *)this->obj);
  int i = this->i;

  if(i >= 0 && i < size)
    fvec_set_element((fvec_t *)this->obj, i, fts_get_number_float(at));
}

static void
setval_fvec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(fvec_atom_is(at + 2))
	getval_set_reference(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->i = fts_get_number_int(at + 1);
    case 1:
      if(fts_is_number(at))
	setval_fvec(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

/******************************************************
 *
 *  vec
 *
 */

static void
getelem_vec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  int size = vec_get_size((vec_t *)this->obj);
  int i = this->i;
  
  if(i >= 0 && i < size)
    {
      fts_atom_t out = vec_get_element((vec_t *)this->obj, i);
      
      if(fts_is_object(&out))
	{
	  fts_refer(&out);
	  fts_outlet_send(o, 0, fts_get_selector(&out), 1, &out);
	  fts_release(&out);
	}
      else if(!fts_is_void(&out))
	fts_outlet_send(o, 0, fts_get_selector(&out), 1, &out);
    }
}

static void
getelem_vec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  this->i = fts_get_number_int(at);
  getelem_vec(o, 0, 0, 0, 0);
}

static void
getelem_vec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  switch(ac)
    {
    default:
    case 2:
      if(vec_atom_is(at + 1))
	getval_set_reference(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	this->i = fts_get_number_int(at);
    case 0:
      getelem_vec(o, 0, 0, 0, 0);
      break;
    }
}

static void
setelem_vec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  int size = vec_get_size((vec_t *)this->obj);
  int i = this->i;
  
  if(ac > 0 && s == fts_get_selector(at) && i >= 0 && i < size)
    vec_set_element((vec_t *)this->obj, i, at[0]);
}

static void
setelem_vec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(vec_atom_is(at + 2))
	getval_set_reference(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->i = fts_get_number_int(at + 1);
    case 1:
      if(fts_is_number(at))
	setelem_vec(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

/******************************************************
 *
 *  mat
 *
 */

static void
getelem_mat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;
  mat_t *mat = (mat_t *)this->obj;
  int m = mat_get_m(mat);
  int n = mat_get_n(mat);
  int i = this->i;
  int j = this->j;
  
  if(i >= 0 && i < m && j >= 0 && j < n)
    {
      fts_atom_t out = mat_get_element((mat_t *)this->obj, i, j);
      
      if(fts_is_object(&out))
	{
	  fts_refer(&out);
	  fts_outlet_send(o, 0, fts_get_selector(&out), 1, &out);
	  fts_release(&out);
	}
      else if(!fts_is_void(&out))
	fts_outlet_send(o, 0, fts_get_selector(&out), 1, &out);
    }
}

static void
getelem_mat_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;

  this->i = fts_get_number_int(at);
  getelem_mat(o, 0, 0, 0, 0);
}

static void
getelem_mat_col(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;

  this->j = fts_get_number_int(at);
}

static void
getelem_mat_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(mat_atom_is(at + 2))
	getval_set_reference(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->j = fts_get_number_int(at + 1);
    case 1:
      if(fts_is_number(at))
	this->i = fts_get_number_int(at);
    case 0:
      getelem_mat(o, 0, 0, 0, 0);
      break;
    }
}

static void
setelem_mat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;
  mat_t *mat = (mat_t *)this->obj;
  int m = mat_get_m(mat);
  int n = mat_get_n(mat);
  int i = this->i;
  int j = this->j;
  
  if(ac > 0 && s == fts_get_selector(at) && i >= 0 && i < m && j >= 0 && j < n)
    mat_set_element((mat_t *)this->obj, i, j, at[0]);
}

static void
setelem_mat_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;

  switch(ac)
    {
    default:
    case 4:
      if(mat_atom_is(at + 3))
	getval_set_reference(o, 0, 0, 1, at + 3);
    case 3:
      if(fts_is_number(at + 2))
	this->j = fts_get_number_int(at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->i = fts_get_number_int(at + 1);
    case 1:
      setelem_mat(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
getval_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 2 && value_atom_is(at + 1))
    {
      fts_class_init(cl, sizeof(getval_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getval_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, getval_bang);
      fts_method_define_varargs(cl, 0, value_symbol, getval_value);

      fts_method_define_varargs(cl, 1, value_symbol, getval_set_reference);
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

static fts_status_t
getelem_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac >= 2 && vec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_list, getelem_vec_list);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, getelem_vec);
      fts_method_define_varargs(cl, 0, fts_s_int, getelem_vec_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getelem_vec_index);
      fts_method_define_varargs(cl, 1, vec_symbol, getval_set_reference);
    }
  else if(ac >= 2 && ivec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_list, getval_ivec_list);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, getval_ivec);
      fts_method_define_varargs(cl, 0, fts_s_int, getval_ivec_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getval_ivec_index);
      fts_method_define_varargs(cl, 1, ivec_symbol, getval_set_reference);
    }
  else if(ac >= 2 && fvec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_list, getval_fvec_list);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, getval_fvec);
      fts_method_define_varargs(cl, 0, fts_s_int, getval_fvec_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getval_fvec_index);
      fts_method_define_varargs(cl, 1, fvec_symbol, getval_set_reference);
    }
  else if(ac >= 2 && mat_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_mat_t), 3, 1, 0);
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_mat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_list, getelem_mat_list);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, getelem_mat);
      fts_method_define_varargs(cl, 0, fts_s_int, getelem_mat_row);
      fts_method_define_varargs(cl, 0, fts_s_float, getelem_mat_row);
      fts_method_define_varargs(cl, 1, fts_s_int, getelem_mat_col);
      fts_method_define_varargs(cl, 1, fts_s_float, getelem_mat_col);
      fts_method_define_varargs(cl, 2, mat_symbol, getval_set_reference);
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

static fts_status_t
setval_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 2 && value_atom_is(at + 1))
    {
      fts_class_init(cl, sizeof(getval_t), 2, 0, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getval_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_list, setval_value_list);
      fts_method_define_varargs(cl, 0, fts_s_anything, setval_value);
      
      fts_method_define_varargs(cl, 1, value_symbol, getval_set_reference);
    }
  else
    return &fts_CannotInstantiate;
    
  return fts_Success;
}

static fts_status_t
setelem_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac >= 2 && vec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 3, 0, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
	  
      fts_method_define_varargs(cl, 0, fts_s_list, setelem_vec_list);
      fts_method_define_varargs(cl, 0, fts_s_anything, setelem_vec);

      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);

      fts_method_define_varargs(cl, 2, vec_symbol, getval_set_reference);
    }
  else if(ac >= 2 && ivec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 3, 0, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
	  
      fts_method_define_varargs(cl, 0, fts_s_list, setval_ivec_list);
      fts_method_define_varargs(cl, 0, fts_s_int, setval_ivec);
      fts_method_define_varargs(cl, 0, fts_s_float, setval_ivec);

      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);

      fts_method_define_varargs(cl, 2, ivec_symbol, getval_set_reference);
    }
  else if(ac >= 2 && fvec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 3, 0, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
	  
      fts_method_define_varargs(cl, 0, fts_s_list, setval_fvec_list);
      fts_method_define_varargs(cl, 0, fts_s_int, setval_fvec);
      fts_method_define_varargs(cl, 0, fts_s_float, setval_fvec);

      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);

      fts_method_define_varargs(cl, 2, fvec_symbol, getval_set_reference);
    }
  else if(ac >= 2 && mat_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_mat_t), 4, 0, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_mat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_list, setelem_mat_list);
      fts_method_define_varargs(cl, 0, fts_s_anything, setelem_mat);
      
      fts_method_define_varargs(cl, 1, fts_s_list, setelem_set_ij);
      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);
      
      fts_method_define_varargs(cl, 2, fts_s_int, setelem_set_j);
      fts_method_define_varargs(cl, 2, fts_s_float, setelem_set_j);
      
      fts_method_define_varargs(cl, 3, mat_symbol, getval_set_reference);
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

void
getval_config(void)
{
  fts_metaclass_install(fts_new_symbol("getval"), getval_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("setval"), setval_instantiate, fts_arg_type_equiv);

  fts_metaclass_install(fts_new_symbol("getelem"), getelem_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("setelem"), setelem_instantiate, fts_arg_type_equiv);
}
