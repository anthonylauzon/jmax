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
#include "matrix.h"

typedef struct 
{
  fts_object_t o;
  matrix_t *mx; /* referenced matrix */
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
  matrix_t *mx = matrix_atom_get(at + 1);
  int n = matrix_get_n(mx); /* # of cols */

  this->mx = mx;
  
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
getlist_matrix_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;
  matrix_t *mx = this->mx;
  int m = matrix_get_m(mx);
  int n = matrix_get_n(mx);
  int i = fts_get_int(at);
  int j;
  
  if(i >= 0 && i < m)
    {
      getlist_resize_buffer(this, n);

      for(j=0; j<n; j++)
	this->list[j] = matrix_get_element(mx, i, j);
      
      fts_outlet_send(o, 0, fts_s_list, n, this->list);
    }
}

static void
getlist_matrix_col(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;
  matrix_t *mx = this->mx;
  int m = matrix_get_m(mx);
  int n = matrix_get_n(mx);
  int i;
  int j = fts_get_int(at);
  
  if(j >= 0 && j < n)
    {
      getlist_resize_buffer(this, n);

      for(i=0; i<m; i++)
	this->list[i] = matrix_get_element(mx, i, j);
      
      fts_outlet_send(o, 0, fts_s_list, m, this->list);
    }
}

static void
getlist_matrix_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;
  matrix_t *mx = matrix_atom_get(at);

  matrix_release(this->mx);  
  this->mx = mx;
  matrix_refer(mx);
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
getlist_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(getlist_t), 2, 1, 0); 

  if(ac == 2 && matrix_atom_is(at + 1))
    {
      /* define the system methods */
      a[0] = fts_s_symbol;
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getlist_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getlist_delete);
      
      /* user methods */
      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, getlist_matrix_row, 1, a);
      fts_method_define(cl, 0, fts_new_symbol("row"), getlist_matrix_row, 1, a);
      fts_method_define(cl, 0, fts_new_symbol("col"), getlist_matrix_col, 1, a);

      fts_method_define_varargs(cl, 1, matrix_type, getlist_matrix_set);

      /* type the outlet */
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




