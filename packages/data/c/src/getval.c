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
#include "value.h"
#include "ivec.h"
#include "fvec.h"
#include "vec.h"
#include "mat.h"
#include "dict.h"

/******************************************************
 *
 *  objects getval and setval
 *
 *    getval <value>
 *    getelem [<i>] <vec>
 *    getelem [<i> <j>] <mat>
 *
 *    setval <value>
 *    setelem [<i>] <vec>
 *    setelem [<i> <j>] <mat>
 *
 *    putmess <dict>
 *    getmess <dict>
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

  this->obj = fts_get_object(at);
  fts_object_refer(this->obj);
}

static void
getelem_vec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  if(ac == 1)
    {
      this->i = 0;
      this->obj = fts_get_object(at);
    }
  else
    {
      this->i = fts_get_number_int(at + 0);
      this->obj = fts_get_object(at + 1);
    }

  fts_object_refer(this->obj);
}

static void
getelem_mat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_mat_t *this = (getelem_mat_t *)o;

  if(ac == 1)
    {
      this->i = 0;
      this->j = 0;
      this->obj = fts_get_object(at);
    }
  else if(ac == 2)
    {
      this->i = 0;

      if(fts_is_number(at))
	this->j = fts_get_number_int(at);
      else
	fts_object_set_error(o, "Invalid index argument");

      this->obj = fts_get_object(at + 1);
    }
  else
    {
      if(fts_is_number(at + 0) && fts_is_number(at + 1))
	{
	  this->i = fts_get_number_int(at + 0);
	  this->j = fts_get_number_int(at + 1);
	}
      else
	fts_object_set_error(o, "Invalid index argument");

      this->obj = fts_get_object(at + 2);
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

  fts_outlet_atom(o, 0, value_get((value_t *)this->obj));
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
    value_set((value_t *)this->obj, at);
}

/******************************************************
 *
 *  ivec
 *
 */

static void
getelem_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  int size = ivec_get_size((ivec_t *)this->obj);
  int i = this->i;

  if(i >= 0 && i < size)
    fts_outlet_int(o, 0, ivec_get_element((ivec_t *)this->obj, i));
}

static void
getelem_ivec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  this->i = fts_get_number_int(at);
  getelem_ivec(o, 0, 0, 0, 0);
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

/******************************************************
 *
 *  fvec
 *
 */

static void
getelem_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;
  int size = fvec_get_size((fvec_t *)this->obj);
  int i = this->i;

  if(i >= 0 && i < size)
    fts_outlet_float(o, 0, fvec_get_element((fvec_t *)this->obj, i));
}

static void
getelem_fvec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  this->i = fts_get_number_int(at);
  getelem_fvec(o, 0, 0, 0, 0);
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
    fts_outlet_atom(o, 0, vec_get_element((vec_t *)this->obj, i));
}

static void
getelem_vec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getelem_vec_t *this = (getelem_vec_t *)o;

  this->i = fts_get_number_int(at);
  getelem_vec(o, 0, 0, 0, 0);
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
    fts_outlet_atom(o, 0, mat_get_element((mat_t *)this->obj, i, j));
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

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
getval_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1 && value_atom_is(at))
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
  if(ac >= 1 && vec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, getelem_vec);
      fts_method_define_varargs(cl, 0, fts_s_int, getelem_vec_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getelem_vec_index);

      fts_method_define_varargs(cl, 1, vec_symbol, getval_set_reference);
    }
  else if(ac >= 1 && ivec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, getelem_ivec);
      fts_method_define_varargs(cl, 0, fts_s_int, getelem_ivec_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getelem_ivec_index);

      fts_method_define_varargs(cl, 1, ivec_symbol, getval_set_reference);
    }
  else if(ac >= 2 && fvec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, getelem_fvec);
      fts_method_define_varargs(cl, 0, fts_s_int, getelem_fvec_index);
      fts_method_define_varargs(cl, 0, fts_s_float, getelem_fvec_index);

      fts_method_define_varargs(cl, 1, fvec_symbol, getval_set_reference);
    }
  else if(ac >= 1 && mat_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_mat_t), 3, 1, 0);
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_mat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
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
  if(ac == 1 && value_atom_is(at))
    {
      fts_class_init(cl, sizeof(getval_t), 2, 0, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getval_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
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
  if(ac >= 1 && vec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 3, 0, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
	  
      fts_method_define_varargs(cl, 0, fts_s_anything, setelem_vec);

      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);

      fts_method_define_varargs(cl, 2, vec_symbol, getval_set_reference);
    }
  else if(ac >= 1 && ivec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 3, 0, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
	  
      fts_method_define_varargs(cl, 0, fts_s_int, setval_ivec);
      fts_method_define_varargs(cl, 0, fts_s_float, setval_ivec);

      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);

      fts_method_define_varargs(cl, 2, ivec_symbol, getval_set_reference);
    }
  else if(ac >= 1 && fvec_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_vec_t), 3, 0, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
	  
      fts_method_define_varargs(cl, 0, fts_s_int, setval_fvec);
      fts_method_define_varargs(cl, 0, fts_s_float, setval_fvec);

      fts_method_define_varargs(cl, 1, fts_s_int, setelem_set_i);
      fts_method_define_varargs(cl, 1, fts_s_float, setelem_set_i);

      fts_method_define_varargs(cl, 2, fvec_symbol, getval_set_reference);
    }
  else if(ac >= 1 && mat_atom_is(at + ac - 1))
    {
      fts_class_init(cl, sizeof(getelem_mat_t), 4, 0, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getelem_mat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getval_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_anything, setelem_mat);
      
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

/**********************************************************
 *
 *  getmess / putmess
 *
 */

typedef struct getmess
{
  fts_object_t o;
  dict_t *dict;
  fts_atom_t key;
} getmess_t;

static void
getmess_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  if(this->dict)
    fts_object_release((fts_object_t *)this->dict);
  
  this->dict = (dict_t *)fts_get_object(at);
  fts_object_refer((fts_object_t *)this->dict);
}

static void
getmess_set_key(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  this->key = at[0];
}

static void
getmess_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;
  fts_atom_t a;
  
  dict_recall(this->dict, &this->key, &a);
  fts_outlet_atom(o, 0, &a);
}

static void
getmess_set_key_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_set_key(o, 0, 0, 1, at);
  getmess_output(o, 0, 0, 0, 0);
}

static void
putmess_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  dict_store_atoms(this->dict, &this->key, ac, at);
}

static void
putmess_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  if(ac == 1 && s == fts_get_selector(at))
    dict_store(this->dict, &this->key, at);
  else
    {
      fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_metaclass, 0, 0);
      fts_atom_t a;
      
      fts_message_set(mess, s, ac, at);
      fts_set_object(&a, (fts_object_t *)mess);
      dict_store(this->dict, &this->key, &a);
    }
}

static void
getmess_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  this->dict = 0;
  fts_set_int(&this->key, 0);
  
  if(ac > 1)
    {
      if(fts_is_int(at) || fts_is_symbol(at))
	{
	  this->key = at[0];

	  /* skip key */
	  ac--;
	  at++;
	}
      else
	fts_object_set_error(o, "Wrong arguments");
    }

  if(ac > 0 && dict_atom_is( at))
    getmess_set_reference(o, 0, 0, 1, at);
  else
    fts_object_set_error(o, "First argument of dict required");
}

static void
getmess_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getmess_t *this = (getmess_t *)o;

  fts_object_release((fts_object_t *)this->dict);
}

static fts_status_t
getmess_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(getmess_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getmess_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getmess_delete);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, getmess_output);
  fts_method_define_varargs(cl, 0, fts_s_int, getmess_set_key_and_output);
  fts_method_define_varargs(cl, 0, fts_s_symbol, getmess_set_key_and_output);
  
  fts_method_define_varargs(cl, 1, dict_symbol, getmess_set_reference);

  return fts_Success;
}

static fts_status_t
putmess_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(getmess_t), 3, 0, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getmess_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getmess_delete);
  
  fts_method_define_varargs(cl, 0, fts_s_int, putmess_atoms);
  fts_method_define_varargs(cl, 0, fts_s_float, putmess_atoms);
  fts_method_define_varargs(cl, 0, fts_s_symbol, putmess_atoms);
  fts_method_define_varargs(cl, 0, fts_s_list, putmess_atoms);
  fts_method_define_varargs(cl, 0, fts_s_anything, putmess_anything);
  
  fts_method_define_varargs(cl, 1, fts_s_int, getmess_set_key);
  fts_method_define_varargs(cl, 1, fts_s_symbol, getmess_set_key);

  fts_method_define_varargs(cl, 2, dict_symbol, getmess_set_reference);

  return fts_Success;
}

void
getval_config(void)
{
  fts_metaclass_install(fts_new_symbol("getval"), getval_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("setval"), setval_instantiate, fts_arg_type_equiv);

  fts_metaclass_install(fts_new_symbol("getelem"), getelem_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("setelem"), setelem_instantiate, fts_arg_type_equiv);

  fts_class_install(fts_new_symbol("getmess"), getmess_instantiate);
  fts_class_install(fts_new_symbol("putmess"), putmess_instantiate);
}
