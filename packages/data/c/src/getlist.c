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
#include <fts/fts.h>
#include "mat.h"

typedef struct 
{
  fts_object_t o;
  union {
    mat_t *mat;
    fts_object_t *obj;
  } ref;
  fts_atom_t *list; /* ouput buffer */
  int alloc;
} getlist_t;

/************************************************
 *
 *    object
 *
 */
 
static void
getlist_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;
  mat_t *mat = mat_atom_get(at + 1);
  int n = mat_get_n(mat); /* # of cols */

  this->ref.mat = mat;
  
  /* init output list to # of cols */
  if(n > 0)
    {
      this->list = (fts_atom_t *)fts_block_alloc(n * sizeof(fts_atom_t));
      this->alloc = n;
    }
  else
    {
      this->list = 0;
      this->alloc = 0;
    }
}

static void
getlist_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;

  if(this->alloc)
    fts_block_free(this->list, this->alloc);
}

static void
getlist_resize_buffer(getlist_t *this, int size)
{
  if(size > this->alloc)
    {
      fts_block_free(this->list, this->alloc);
      this->list = (fts_atom_t *)fts_block_alloc(size * sizeof(fts_atom_t));
      this->alloc = size;
    }

  this->alloc = size;
}

/************************************************
 *
 *  user methods
 *
 */

static void
getlist_mat_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;
  mat_t *mat = this->ref.mat;
  int m = mat_get_m(mat);
  int n = mat_get_n(mat);
  int i = fts_get_int(at);
  int j;
  
  if(i >= 0 && i < m)
    {
      getlist_resize_buffer(this, n);

      for(j=0; j<n; j++)
	this->list[j] = mat_get_element(mat, i, j);
      
      fts_outlet_send(o, 0, fts_s_list, n, this->list);
    }
}

static void
getlist_mat_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;
  mat_t *mat = mat_atom_get(at);

  fts_object_release(this->ref.obj);
  this->ref.mat = mat;
  fts_object_refer(this->ref.obj);
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
getlist_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(getlist_t), 2, 1, 0); 

  if(ac == 2 && mat_atom_is(at + 1))
    {
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getlist_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getlist_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_int, getlist_mat_row);
      fts_method_define_varargs(cl, 1, mat_type, getlist_mat_set);

      fts_outlet_type_define_varargs(cl, 0, fts_s_list);
  
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
getlist_config(void)
{
  fts_metaclass_install(fts_new_symbol("getlist"), getlist_instantiate, fts_arg_type_equiv);
}
