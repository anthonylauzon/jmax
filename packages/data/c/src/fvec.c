/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <fts/fts.h>
#include <fts/packages/utils/utils.h>
#include <fts/packages/data/data.h>
#include "tabeditor.h"

#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>

#ifdef WIN32
#define logf log
#define sqrtf sqrt
#define expf exp
#define fabsf fabs
#endif

fts_class_t *fvec_class = NULL;
fts_symbol_t fvec_symbol = NULL;

static fts_symbol_t sym_col = NULL;
static fts_symbol_t sym_row = NULL;
static fts_symbol_t sym_diag = NULL;
static fts_symbol_t sym_unwrap = NULL;
static fts_symbol_t sym_vec = NULL;
static fts_symbol_t sym_refer = NULL;

static fts_symbol_t fvec_type_names[fvec_n_types];

static int
fvec_get_type_from_symbol(fts_symbol_t sym)
{
  if(sym == sym_col)
    return fvec_type_column;
  else if(sym == sym_row)
    return fvec_type_row;
  else if(sym == sym_diag)
    return fvec_type_diagonal;
  else if(sym == sym_unwrap)
    return fvec_type_unwrap;
  else
    return fvec_type_column;
}

/********************************************************************
 *
 *  easy creators
 *
 */

fvec_t *
fvec_create_column(fmat_t *fmat)
{
  fvec_t *fvec = (fvec_t *)fts_object_create(fvec_class, 0, 0);

  fvec->fmat = fmat;
  fts_object_refer((fts_object_t *)fmat);
  fvec->type = fvec_type_column;
  
  return fvec;
}

fvec_t *
fvec_create_row(fmat_t *fmat)
{
  fvec_t *fvec = (fvec_t *)fts_object_create(fvec_class, 0, 0);
  
  fvec->fmat = fmat;
  fts_object_refer((fts_object_t *)fmat);  
  fvec->type = fvec_type_column;
  
  return fvec;
}

fvec_t *
fvec_create_diagonal(fmat_t *fmat)
{
  fvec_t *fvec = (fvec_t *)fts_object_create(fvec_class, 0, 0);
  
  fvec->fmat = fmat;
  fts_object_refer((fts_object_t *)fmat);  
  fvec->type = fvec_type_diagonal;
  
  return fvec;
}

fvec_t *
fvec_create_unwrap(fmat_t *fmat)
{
  fvec_t *fvec = (fvec_t *)fts_object_create(fvec_class, 0, 0);
  
  fvec->fmat = fmat;
  fts_object_refer((fts_object_t *)fmat);  
  fvec->type = fvec_type_unwrap;
  
  return fvec;
}

fvec_t *
fvec_create_vector(int size)
{
  fvec_t *fvec = (fvec_t *)fts_object_create(fvec_class, 0, 0);
  
  fvec->fmat = fmat_create(size, 1);
  fts_object_refer((fts_object_t *) fvec->fmat);  
  fvec->type = fvec_type_vector;
  
  return fvec;
}

/********************************************************************
 *
 *  utilities
 *
 */

void
fvec_set_dimensions(fvec_t *fvec, int ac, const fts_atom_t *at)
{
  switch(ac)
  {
    default :
    case 3:
      if(fts_is_number(at + 2))
      {
        int size = fts_get_number_int(at + 2);
        
        if(size > 0)
          fvec->size = size;
        else
          fvec->size = 0;
      }
    case 2:
      if(fts_is_number(at + 1))
      {
        int onset = fts_get_number_int(at + 1);
        
        if(onset > 0)
          fvec->onset = onset;
        else
          fvec->onset = 0;
      }
    case 1:
      if(fts_is_number(at))
      {
        int index = fts_get_number_int(at);
        
        if(index > 0)
          fvec->index = index;
        else
          fvec->index = 0;
      }
    case 0:
      break;
  }
}

static void
fvec_get_vector(fvec_t *fvec, float **ptr, int *size, int *stride)
{
  fmat_t *fmat = fvec->fmat;
  float *fmat_ptr = fmat_get_ptr(fmat);
  int fmat_m = fmat_get_m(fmat);
  int fmat_n = fmat_get_n(fmat);
  int fvec_index = fvec->index;
  int fvec_onset = fvec->onset;
  int fvec_size = fvec->size;
  
  switch(fvec->type)
  {
    case fvec_type_column:
      
      if(fvec_index >= fmat_n)
        fvec_index = fmat_n - 1;
      
      while(fvec_index < 0)
        fvec_index += fmat_n;
        
      if(fvec_onset > fmat_m)
        fvec_onset = fmat_m;
          
      if(fvec_onset + fvec_size > fmat_m)
        fvec_size = fmat_m - fvec_onset;
            
      *ptr = fmat_ptr + fvec_index + fvec_onset * fmat_n;
      *size = fvec_size;
      *stride = fmat_n;
      break;
      
    case fvec_type_row:
      
      if(fvec_index >= fmat_m)
        fvec_index = fmat_m - 1;
      
      while(fvec_index < 0)
        fvec_index += fmat_m;
        
      if(fvec_onset > fmat_n)
        fvec_onset = fmat_n;
          
      if(fvec_onset + fvec_size > fmat_n)
        fvec_size = fmat_n - fvec_onset;
          
      *ptr = fmat_ptr + fvec_index * fmat_n + fvec_onset;
      *size = fvec_size;
      *stride = 1;
      break;
      
    case fvec_type_diagonal:
      
      if(fvec_index > fmat_m)
        fvec_index = fmat_m;
      
      if(fvec_onset > fmat_n)
        fvec_onset = fmat_n;
        
      if(fvec_index + fvec_size > fmat_m)
        fvec_size = fmat_m - fvec_index;
            
      if(fvec_onset + fvec_size > fmat_n)
        fvec_size = fmat_n - fvec_onset;
          
      *ptr = fmat_ptr + fvec_index * fmat_n + fvec_onset;
      *size = fvec_size;
      *stride = fmat_n + 1;
      break;

    case fvec_type_unwrap:
      
      if(fvec_index > fmat_m)
        fvec_index = fmat_m;
      
      if(fvec_onset > fmat_n)
        fvec_onset = fmat_n;
        
      if(fvec_index * fmat_n + fvec_onset + fvec_size > fmat_m * fmat_n)
        fvec_size = fmat_m * fmat_n - fvec_index * fmat_n - fvec_onset;
          
      *ptr = fmat_ptr + fvec_index * fmat_n + fvec_onset;
      *size = fvec_size;
      *stride = 1;
      break;
      
    case fvec_type_vector:
      
      *ptr = fmat_ptr;
      *size = fvec_size;
      *stride = 1;
      break;
    
    default:
      break;
  }
}

int
fvec_vector(fts_object_t *obj, float **ptr, int *size, int *stride)
{
  if(fts_object_get_class(obj) == fvec_class)
  {
    fvec_get_vector((fvec_t *)obj, ptr, size, stride);
    return 1;
  }

  *ptr = NULL;
  *size = 0;
  *stride = 0;
  return 0;
}

/* copy matrix row or col reference to an fvec */
void
fvec_copy_to_fmat(fvec_t *org, fmat_t *copy)
{
  fmat_t *orgmat = org->fmat;
  float *orgptr;
  int size, stride;
  int i;
  
  fvec_get_vector(org, &orgptr, &size, &stride);
  
  fmat_reshape(copy, size, 1);
  
  for (i = 0; i < size; i++)
  {
    copy->values[i] = *orgptr;
    orgptr += stride;
  }
  
  copy->onset = orgmat->onset;
  copy->domain = orgmat->domain;
  copy->sr = orgmat->sr;
}

static void
fvec_array_function(fts_object_t *o, fts_array_t *array)
{
  fvec_t *self = (fvec_t *)o;
  int onset = fts_array_get_size(array);
  fts_atom_t *atoms;
  float *values;
  int size, stride;
  int i, j;

  fvec_get_vector(self, &values, &size, &stride);  
  
  fts_array_set_size(array, onset + size);
  atoms = fts_array_get_atoms(array) + onset;
  
  for(i=0, j=0; i<size; i++, j+=stride)
    fts_set_float(atoms + i, values[j]);
}

/******************************************************************************
 *
 *  envelopes
 *
 */

static void
fvec_lookup_fmat_or_slice(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_object_t *obj = fts_get_object(at);
  float *ptr;
  int size, stride;
  float *env;
  int env_size, env_stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  fmat_or_slice_vector(obj, &env, &env_size, &env_stride);
  
  for(i=0; i<size*stride; i+=stride)
  {
    double f_index = ptr[i];
    
    if(f_index < 0.0)
      ptr[i] = env[0];
    else 
    {
      double i_index = floor(f_index);
      int index = (int)i_index;
      
      if(index >= env_size - 1)
        ptr[i] = env[(env_size - 1) * env_stride];
      else
      {
        double frac = f_index - i_index;
        double env_0 = env[index * env_stride];
        double env_1 = env[(index + 1) * env_stride];
        
        ptr[i] = (1.0 - frac) * env_0 + frac * env_1;
      }
    }
  }
  
  fts_return_object(o);
}

static void
fvec_lookup_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  bpf_t *bpf = (bpf_t *)fts_get_object(at);
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = bpf_get_interpolated(bpf, ptr[i]);
  
  fts_return_object(o);
}
    
static void
fvec_env_fmat_or_slice(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;  
  fts_object_t *obj = fts_get_object(at);
  float *ptr;
  int size, stride;
  float *env;
  int env_size, env_stride;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  fmat_or_slice_vector(obj, &env, &env_size, &env_stride);
  
  if(env_size == size)
  {
    int i, j;
    
    /* simply multiply */
    for(i=0, j=0; i<size*stride; i+=stride, j+=env_stride)
      ptr[i] *= env[j];
  }
  else
  {
    double incr = (double)env_size / (double)size;
    double f_index = incr;
    int i;
    
    /* apply envelope by linear interpolation */
    for(i=0; i<size*stride; i+=stride)
    {
      double i_index = floor(f_index);
      int index = (int)i_index;
      double frac = f_index - i_index;
      double env_0 = env[index * env_stride];
      double env_1 = env[index * env_stride + env_stride];
      
      ptr[i] *= (1.0 - frac) * env_0 + frac * env_1;
      
      f_index += incr;
    }
  }
  
  fts_return_object(o);
}

static void
fvec_env_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;  
  float *ptr;
  int size, stride;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  if(size > 0)
  {
    bpf_t *bpf = (bpf_t *)fts_get_object(at);
    double time = 0.0;
    double incr = bpf_get_duration(bpf) / (double)size;
    int i;
    
    for(i=0; i<size*stride; i+=stride)
    {
      ptr[i] *= bpf_get_interpolated(bpf, time);
      time += incr;  
    }
  }
  
  fts_return_object(o);
}

static void
fvec_apply_expr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  expr_t *expr = (expr_t *)fts_get_object(at);
  fts_hashtable_t locals;
  fts_atom_t key_self, key_x, value;
  fts_atom_t ret;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  fts_hashtable_init(&locals, FTS_HASHTABLE_SMALL);
  
  fts_set_symbol(&key_self, fts_s_self);
  fts_set_object(&value, self);
  fts_hashtable_put(&locals, &key_self, &value);
  
  fts_set_symbol(&key_x, fts_s_x);
  
  for(i=0; i<size*stride; i+=stride)
  {
    double f = (double)ptr[i];
    
    fts_set_float(&value, f);
    fts_hashtable_put(&locals, &key_x, &value);
    
    expr_evaluate(expr, &locals, ac - 1, at + 1, &ret);
    
    if(fts_is_number(&ret))
      ptr[i] = fts_get_number_float(&ret);
    else
      ptr[i] = 0.0;
  }
  
  fts_hashtable_destroy(&locals);
  
  fts_set_void(fts_get_return_value());
  
  fts_return_object(o);
}

static void
fvec_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i, j;

  fvec_get_vector(self, &ptr, &size, &stride);

  if(ac > size)
    ac = size;
  
  for(i=0, j=0; i<size; i++, j+=stride)
  {
    if(fts_is_number(at))
      ptr[j] += fts_get_number_float(at + i);
  }
  
  fts_return_object(o);
}

static void
fvec_set_from_fmat_or_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_object_t *obj = fts_get_object(at);
  float *ptr;
  int size, stride;
  float *set;
  int set_size, set_stride;
  int i, j;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  fmat_or_slice_vector(obj, &set, &set_size, &set_stride);
  
  if(set_size > size)
    set_size = size;
  
  for(i=0, j=0; i<set_size*stride; i+=stride, j+=set_stride)
    ptr[i] = set[j];
  
  fts_return_object(o);
}

static void
_fvec_set_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fmat_t *fmat = (fmat_t *)fts_get_object(at);
  
  fts_object_release((fts_object_t *)self->fmat);
  self->fmat = fmat;
  fts_object_refer((fts_object_t *)fmat);
  
  fts_return_object(o);
}

static void
_fvec_set_fmat_and_dimensions(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  
  if(ac > 0 && fts_is_a(at, fmat_class))
  {
    _fvec_set_fmat(o, 0, NULL, 1, at);
      
    if(ac > 1 && fts_is_symbol(at + 1))
    {
      fts_symbol_t sym = fts_get_symbol(at + 1);
      self->type = fvec_get_type_from_symbol(sym);
    }
    
    if(ac > 2)
      fvec_set_dimensions(self, ac - 2, at + 2);
    
    fts_return_object(o);
  }
}

static void
_fvec_set_col(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  
  self->type = fvec_type_column;
  fvec_set_dimensions(self, ac, at);
  
  fts_return_object(o);
}

static void
_fvec_set_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  
  self->type = fvec_type_row;
  fvec_set_dimensions(self, ac, at);
  
  fts_return_object(o);
}

static void
_fvec_set_diag(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  
  self->type = fvec_type_diagonal;
  fvec_set_dimensions(self, ac, at);
  
  fts_return_object(o);
}

static void
_fvec_set_unwrap(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  
  self->type = fvec_type_unwrap;
  fvec_set_dimensions(self, ac, at);
  
  fts_return_object(o);
}

static void
_fvec_set_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  int size = 0;
  
  if(ac > 0 && fts_is_number(at))
    size = fts_get_number_int(at);
  
  if(size < 0)
    size = 0;
  
  fts_object_release((fts_object_t *)self->fmat);
  self->fmat = fmat_create(size, 1);
  fts_object_refer((fts_object_t *)self->fmat);
  
  self->type = fvec_type_vector;
  
  fts_return_object(o);
}

static void
_fvec_get_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  int size = fvec_get_size(self);
  
  fts_return_int(size);
}

static void
_fvec_set_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  int size = fts_get_number_int(at);
  
  if(size > 0 && fvec_get_type(self) == fvec_type_vector)
    fmat_set_m(self->fmat, size);
  
  fvec_set_size(self, size);
    
  fts_return_int(size);
}

/******************************************************************************
 *
 *  arithmetics
 *
 */

static void
fvec_add_fvec(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_object_t *right = fts_get_object(at);
  float *l, *r;
  int l_size, r_size;
  int l_stride, r_stride;
  int size;
  int i;
  
  fvec_get_vector(self, &l, &l_size, &l_stride);
  fmat_or_slice_vector(right, &r, &r_size, &r_stride);
  
  if(l_size < r_size)
    size = l_size;
  else
    size = r_size;
    
  for(i=0; i<size; i++)
    l[i * l_stride] += r[i * r_stride];
  
  fts_return_object(o);
}

static void
fvec_add_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] += r;
  
  fts_return_object(o);
}

static void
fvec_sub_fvec(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_object_t *right = fts_get_object(at);
  float *l, *r;
  int l_size, r_size;
  int l_stride, r_stride;
  int size;
  int i;
  
  fvec_get_vector(self, &l, &l_size, &l_stride);
  fmat_or_slice_vector(right, &r, &r_size, &r_stride);
  
  if(l_size < r_size)
    size = l_size;
  else
    size = r_size;
  
  for(i=0; i<size; i++)
    l[i * l_stride] -= r[i * r_stride];
  
  fts_return_object(o);
}

static void
fvec_sub_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] -= r;
  
  fts_return_object(o);
}

static void
fvec_mul_fvec(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_object_t *right = fts_get_object(at);
  float *l, *r;
  int l_size, r_size;
  int l_stride, r_stride;
  int size;
  int i;
  
  fvec_get_vector(self, &l, &l_size, &l_stride);
  fmat_or_slice_vector(right, &r, &r_size, &r_stride);
  
  if(l_size < r_size)
    size = l_size;
  else
    size = r_size;
  
  for(i=0; i<size; i++)
    l[i * l_stride] *= r[i * r_stride];
  
  fts_return_object(o);
}

static void
fvec_mul_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] *= r;
  
  fts_return_object(o);
}

static void
fvec_div_fvec(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_object_t *right = fts_get_object(at);
  float *l, *r;
  int l_size, r_size;
  int l_stride, r_stride;
  int size;
  int i;
  
  fvec_get_vector(self, &l, &l_size, &l_stride);
  fmat_or_slice_vector(right, &r, &r_size, &r_stride);
  
  if(l_size < r_size)
    size = l_size;
  else
    size = r_size;
  
  for(i=0; i<size; i++)
    l[i * l_stride] /= r[i * r_stride];
  
  fts_return_object(o);
}

static void
fvec_div_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] /= r;
  
  fts_return_object(o);
}

static void
fvec_bus_fvec(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_object_t *right = fts_get_object(at);
  float *l, *r;
  int l_size, r_size;
  int l_stride, r_stride;
  int size;
  int i;
  
  fvec_get_vector(self, &l, &l_size, &l_stride);
  fmat_or_slice_vector(right, &r, &r_size, &r_stride);
  
  if(l_size < r_size)
    size = l_size;
  else
    size = r_size;
  
  for(i=0; i<size; i++)
    l[i * l_stride] = r[i * r_stride] - l[i * l_stride];
  
  fts_return_object(o);
}

static void
fvec_bus_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] = r - l[i];
  
  fts_return_object(o);
}

static void
fvec_vid_fvec(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_object_t *right = fts_get_object(at);
  float *l, *r;
  int l_size, r_size;
  int l_stride, r_stride;
  int size;
  int i;
  
  fvec_get_vector(self, &l, &l_size, &l_stride);
  fmat_or_slice_vector(right, &r, &r_size, &r_stride);
  
  if(l_size < r_size)
    size = l_size;
  else
    size = r_size;
  
  for(i=0; i<size; i++)
    l[i * l_stride] = r[i * r_stride] / l[i * l_stride];
  
  fts_return_object(o);
}

static void
fvec_vid_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] = r / l[i];
  
  fts_return_object(o);
}

/******************************************************************************
 *
 *  misc math funs
 *
 */

static void
fvec_abs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = fabsf(ptr[i]);
  
  fts_return_object(o);
}

static void
fvec_logabs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = logf(fabsf(ptr[i]));
  
  fts_return_object(o);
}

static void
fvec_log(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = logf(ptr[i]);
  
  fts_return_object(o);
}

static void
fvec_exp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = expf(ptr[i]);
  
  fts_return_object(o);
}

static void
fvec_sqrabs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    ptr[i] *= ptr[i];
  
  fts_return_object(o);
}

static void
fvec_sqrt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = sqrtf(ptr[i]);
  
  fts_return_object(o);
}

/******************************************************************************
 *
 *  min, max & co
 *
 */

static void
fvec_get_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *) o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float min = p[0];
    int i;
    
    for (i=stride; i<size; i+=stride)
    {
      if (p[i] < min)
        min = p[i];
    }
    
      fts_return_float(min);
  }
}

static void
fvec_get_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float max = p[0]; /* start with first element */
    int i;
    
    for (i=stride; i<size; i+=stride)
    {
      if(p[i] > max)
        max = p[i];
    }
    
    fts_return_float(max);
  }
}

static void
fvec_get_absmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float max = fabsf(p[0]); /* start with first element */
    int i;
    
    for (i=stride; i<size; i+=stride)
    {
      if (fabsf(p[i]) > max)
        max = fabsf(p[i]);
    }
    
    fts_return_float(max);
  }
}

static void
fvec_get_min_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float min = p[0];
    int mini = 0;
    int i, j;
    
    for(i=1, j=stride; i<size; i++, j+=stride)
    {
      if(p[j] < min)
      {
        min = p[j];
        mini = i;
      }
    }
    
    fts_return_int(mini);
  }
}

static void
fvec_get_max_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float max = p[0];
    int maxi = 0;
    int i, j;
    
    for(i=1, j=stride; i<size; i++, j+=stride)
    {
      if (p[j] > max)
      {
        max = p[j];
        maxi = i;
      }
    }   
    
    fts_return_int(maxi);
  }
}

static void
fvec_get_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  double sum = 0.0;
  float *p;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &p, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    sum += p[i];
  
  fts_return_float(sum);
}

static void
fvec_get_mean(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  double sum = 0.0;
  float *p;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &p, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    sum += p[i];
  
  fts_return_float(sum / (double)size);
}

static void
fvec_get_zc(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *) o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float prev = p[0];
    int zc = 0;
    int i;
    
    for(i=stride; i<size*stride; i+=stride)
    {
      float f = p[i];
      
      if(f != 0.0)
      {
        zc += (prev * f) < 0.0;
        prev = f;
      }
    }
  
    fts_return_int(zc);
  }
}

/****************************************************************************
 *
 *  system mehods
 *
 */

/* called by get element message */
static void
_fvec_get_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *) o;
  float *ptr;
  int size, stride;
  int i = 0;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  if(ac > 0  &&  fts_is_number(at))
    i = fts_get_number_int(at);

  if(i >= size)
    i = size - 1;
  
  while(i < 0)
    i += size;
  
  if (i >= 0  &&  i < size)
    fts_return_float(ptr[i * stride]);
}

static void
fvec_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  
  if(self->type != fvec_type_vector)
  {
    fts_message_t *mess = fts_dumper_message_new(dumper, sym_vec);
    fts_message_append_int(mess, self->size);
    fts_dumper_message_send(dumper, mess);
  }
  else if(self->fmat != fmat_null)
  {
    fts_message_t *mess = fts_dumper_message_new(dumper, sym_refer);
    fts_message_append_object(mess, (fts_object_t *)self->fmat);
    fts_message_append_symbol(mess, fvec_type_names[self->type]);
    fts_message_append_int(mess, self->index);
    fts_message_append_int(mess, self->onset);
    fts_message_append_int(mess, self->size);
    fts_dumper_message_send(dumper, mess);
  }
}

/****************************************************************************
 *
 *  post and print
 *
 */

static void
fvec_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  int index = fvec_get_index(self);
  int onset = fvec_get_onset(self);
  fmat_t *fmat = fvec_get_fmat(self);
  int m = fmat_get_m(fmat);
  int n = fmat_get_n(fmat);
  fts_bytestream_t* stream = fts_get_default_console_stream();
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  if(m * n * size == 0)
  {
    if(self->type == fvec_type_column)
      fts_spost(stream, "<empty fmat column (%d %d %d)>\n", index, onset, size);
    else if(self->type == fvec_type_row)
      fts_spost(stream, "<empty fmat row (%d %d %d)>\n", index, onset, size);
    else if(self->type == fvec_type_diagonal)
      fts_spost(stream, "<empty fmat diagonal (%d %d %d)>\n", index, onset, size);
    else if(self->type == fvec_type_unwrap)
      fts_spost(stream, "<empty fmat unwrap (%d %d %d)>\n", index, onset, size);
    else if(self->type == fvec_type_vector)
      fts_spost(stream, "<empty fvec>\n");
  }
  else
  {
    if(self->type == fvec_type_column)
      fts_spost(stream, "<fvec column vector (%d %d %d) of %d x %d matrix>\n", index, onset, size, m, n);
    else if(self->type == fvec_type_row)
      fts_spost(stream, "<fvec row vector (%d %d %d) of %d x %d matrix>\n", index, onset, size, m, n);
    else if(self->type == fvec_type_diagonal)
      fts_spost(stream, "<fvec diagonal vector (%d %d %d) of %d x %d matrix>\n", index, onset, size, m, n);
    else if(self->type == fvec_type_unwrap)
      fts_spost(stream, "<fvec unwrap vector (%d %d %d) of %d x %d matrix>\n", index, onset, size, m, n);
    else if(self->type == fvec_type_vector)
      fts_spost(stream, "<fvec of %d elements>\n", size);
    
    fts_spost(stream, "{\n");
    
    for(i=0; i<size*stride; i+=stride)
      fts_spost(stream, "  %.7g\n", ptr[i]);
    
    fts_spost(stream, "}\n");
  }
}

/******************************************************************************
 *
 *  class
 *
 */
static void
fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  
  self->fmat = fmat_null;
  self->type = fvec_type_column;
  self->index = 0;
  self->onset = 0;
  self->size = INT_MAX;
  
  if(ac > 0)
  {
    if(fts_is_a(at, fmat_class))
    {
      self->fmat = (fmat_t *)fts_get_object(at);
      
      if(ac > 1 && fts_is_symbol(at + 1))
      {
        fts_symbol_t sym = fts_get_symbol(at + 1);
        self->type = fvec_get_type_from_symbol(sym);
      }
      
      if(ac > 2)
        fvec_set_dimensions(self, ac - 2, at + 2);
    }
    else if(fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      self->fmat = fmat_create(size, 1);
      self->type = fvec_type_vector;
      self->size = size;
      
      if(ac > 1)
        fvec_set(o, 0, NULL, ac - 1, at + 1);
    }    
  }
  
  fts_object_refer((fts_object_t *)self->fmat);
}

static void
fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *self = (fvec_t *)o;
  
  fts_object_release((fts_object_t *)self->fmat);
}

static void
fvec_message(fts_class_t *cl, fts_symbol_t s, fts_method_t slice_method, fts_method_t scalar_method)
{	
	fts_class_message(cl, s, fmat_class, slice_method);
	fts_class_message(cl, s, fvec_class, slice_method);
	fts_class_message_number(cl, s, scalar_method);	  
}

static void
fvec_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fvec_t), fvec_init, fvec_delete);
  
  fts_class_instantiate(fmat_class);
  
  /* standard functions */
  fts_class_message_varargs(cl, fts_s_print, fvec_print);
  fts_class_message_varargs(cl, fts_s_get_element, _fvec_get_element);
  fts_class_message_varargs(cl, fts_s_get, _fvec_get_element);
  
  fts_class_set_array_function(cl, fvec_array_function);
  fts_class_message_varargs(cl, fts_s_dump_state, fvec_dump_state);

  fts_class_message_varargs(cl, fts_new_symbol("set"), fvec_set);
  fts_class_message(cl, fts_new_symbol("set"), fmat_class, fvec_set_from_fmat_or_fvec);
  fts_class_message(cl, fts_new_symbol("set"), fvec_class, fvec_set_from_fmat_or_fvec);

  fts_class_message(cl, sym_refer, fmat_class, _fvec_set_fmat);
  fts_class_message_varargs(cl, sym_refer, _fvec_set_fmat_and_dimensions);
  fts_class_message_varargs(cl, sym_col, _fvec_set_col);
  fts_class_message_varargs(cl, sym_row, _fvec_set_row);
  fts_class_message_varargs(cl, sym_diag, _fvec_set_diag);
  fts_class_message_varargs(cl, sym_unwrap, _fvec_set_unwrap);
  fts_class_message_varargs(cl, sym_vec, _fvec_set_vector);
  
  fts_class_message_void(cl, fts_s_size, _fvec_get_size);
  fts_class_message_number(cl, fts_s_size, _fvec_set_size);

  /* arithmetics */
  fvec_message(cl, fts_new_symbol("add"), fvec_add_fvec, fvec_add_number);
  fvec_message(cl, fts_new_symbol("sub"), fvec_sub_fvec, fvec_sub_number);
  fvec_message(cl, fts_new_symbol("mul"), fvec_mul_fvec, fvec_mul_number);
  fvec_message(cl, fts_new_symbol("div"), fvec_div_fvec, fvec_div_number);
  fvec_message(cl, fts_new_symbol("bus"), fvec_bus_fvec, fvec_bus_number);
  fvec_message(cl, fts_new_symbol("vid"), fvec_vid_fvec, fvec_vid_number);

  fts_class_message_void(cl, fts_new_symbol("abs"), fvec_abs);
  fts_class_message_void(cl, fts_new_symbol("logabs"), fvec_logabs);
  fts_class_message_void(cl, fts_new_symbol("log"), fvec_log);
  fts_class_message_void(cl, fts_new_symbol("exp"), fvec_exp);
  fts_class_message_void(cl, fts_new_symbol("sqrabs"), fvec_sqrabs);
  fts_class_message_void(cl, fts_new_symbol("sqrt"), fvec_sqrt);

  fts_class_message_void(cl, fts_new_symbol("min"), fvec_get_min);
  fts_class_message_void(cl, fts_new_symbol("max"), fvec_get_max);
  fts_class_message_void(cl, fts_new_symbol("mini"), fvec_get_min_index);
  fts_class_message_void(cl, fts_new_symbol("maxi"), fvec_get_max_index);
  fts_class_message_void(cl, fts_new_symbol("absmax"), fvec_get_absmax);
  fts_class_message_void(cl, fts_new_symbol("sum"), fvec_get_sum);
  fts_class_message_void(cl, fts_new_symbol("mean"), fvec_get_mean);
  fts_class_message_void(cl, fts_new_symbol("zc"), fvec_get_zc);

  fts_class_message(cl, fts_new_symbol("lookup"), fmat_class, fvec_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), fvec_class, fvec_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), bpf_type, fvec_lookup_bpf);
  
  fts_class_message(cl, fts_new_symbol("env"), fmat_class, fvec_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), fvec_class, fvec_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), bpf_type, fvec_env_bpf);

  fts_class_message(cl, fts_new_symbol("apply"), expr_class, fvec_apply_expr);

  /*
   * fvec class documentation
   */
  fts_class_doc(cl, fvec_symbol, "<'col'> <num: column index> [<num: row onset> [<num: size>]]", "vector reference to matrix column");
  fts_class_doc(cl, fvec_symbol, "<'row'> <num: row index> [<num: column onset> [<num: size>]]", "vector reference to matrix row");
  fts_class_doc(cl, fvec_symbol, "<'diag'> <num: row onset> [<num: column onset> [<num: size>]]", "vector reference to matrix diagonal");
  fts_class_doc(cl, fvec_symbol, "<'unwrap'> <num: row onset> [<num: column onset> [<num: size>]]", "vector reference to unwrapped matrix");
  fts_class_doc(cl, fvec_symbol, "<num: size>", "vector reference compatible float vector");
  
  fts_class_doc(cl, fts_new_symbol("add"), "<num|fvec: operand>", "add given scalar, fvec (element by element) to current values");
  fts_class_doc(cl, fts_new_symbol("sub"), "<num|fvec: operand>", "substract given scalar, fvec (element by element)");
  fts_class_doc(cl, fts_new_symbol("mul"), "<num|fvec: operand>", "multiply current values by given scalar, fvec (element by element)");
  fts_class_doc(cl, fts_new_symbol("div"), "<num|fvec: operand>", "divide current values by given scalar, fvec (element by element)");
  fts_class_doc(cl, fts_new_symbol("bus"), "<num|fvec: operand>", "subtract current values from given scalar, fvec (element by element)");  
  fts_class_doc(cl, fts_new_symbol("vid"), "<num|fvec: operand>", "divide given scalar, fvec (element by element) by current values");

  fts_class_doc(cl, fts_new_symbol("abs"), NULL, "calulate absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("logabs"), NULL, "calulate logarithm of absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("log"), NULL, "calulate lograrithm of current values");
  fts_class_doc(cl, fts_new_symbol("exp"), NULL, "calulate exponent function of current values");
  fts_class_doc(cl, fts_new_symbol("sqrabs"), NULL, "calulate square of absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("sqrt"), NULL, "calulate square root of absolute values of current values");

  fts_class_doc(cl, fts_new_symbol("min"), NULL, "get minimum value");
  fts_class_doc(cl, fts_new_symbol("mini"), NULL, "get index of minimum value");
  fts_class_doc(cl, fts_new_symbol("max"), NULL, "get maximum value");
  fts_class_doc(cl, fts_new_symbol("maxi"), NULL, "get index of maximum value");
  fts_class_doc(cl, fts_new_symbol("absmax"), NULL, "get maximum absolute value");
  fts_class_doc(cl, fts_new_symbol("sum"), NULL, "get sum of all values");
  fts_class_doc(cl, fts_new_symbol("mean"), NULL, "get mean value of all values");
  fts_class_doc(cl, fts_new_symbol("zc"), NULL, "get number of zerocrossings");  
  
  fts_class_doc(cl, fts_new_symbol("lookup"), "<fmat|fvec|bpf: function>", "apply given function (by linear interpolation)");
  fts_class_doc(cl, fts_new_symbol("env"), "<fmat|fvec|bpf: envelope>", "multiply given envelope");
  fts_class_doc(cl, fts_new_symbol("apply"), "<expr: expression>", "apply expression each value (use $self and $x)");
  
  fts_class_set_super(cl, fvec_class);
}

void 
fvec_config(void)
{
  fvec_symbol = fts_new_symbol("fvec");
  sym_col = fts_new_symbol("col");
  sym_row = fts_new_symbol("row");
  sym_diag = fts_new_symbol("diag");
  sym_unwrap = fts_new_symbol("unwrap");
  sym_vec = fts_new_symbol("vec");
  sym_refer = fts_new_symbol("refer");
  
  fvec_type_names[fvec_type_column] = sym_col;
  fvec_type_names[fvec_type_row] = sym_row;
  fvec_type_names[fvec_type_diagonal] = sym_diag;
  fvec_type_names[fvec_type_unwrap] = sym_unwrap;
  fvec_type_names[fvec_type_vector] = sym_vec;
  
  fvec_class = fts_class_install(fvec_symbol, fvec_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
