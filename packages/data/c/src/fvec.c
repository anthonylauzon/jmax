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

/* fvec is now fmat! here is the code for matrix slices fcol and frow */

fts_class_t *frow_class = NULL;
fts_class_t *fcol_class = NULL;

fts_symbol_t frow_symbol = NULL;
fts_symbol_t fcol_symbol = NULL;




/********************************************************************
 *
 *  utilities
 *
 */

/* copy matrix row or col reference to an fvec */
void
fslice_copy_to_fmat(fslice_t *org, fmat_t *copy)
{
  if (fslice_check_index(org))
  {
    fmat_t *orgmat = org->fmat;
    float  *orgptr = fslice_get_ptr(org);
    int step = fslice_get_stride(org);
    int size = fslice_get_size(org);
    int i;

    fmat_reshape(copy, size, 1);

    for (i = 0; i < size; i++)
    {
      copy->values[i] = *orgptr;
      orgptr += step;
    }

    copy->onset = orgmat->onset;
    copy->domain = orgmat->domain;
    copy->sr = orgmat->sr;
    /* don't copy format from orgmat, since it's set to vector by fmat_reshape */
  }
  else
  {
    /* slice index out of range: set copy empty */
    fmat_reshape(copy, 0, 0);
  }
}


static void
fslice_post_function(fts_object_t *o, fts_bytestream_t *stream)
{
  fslice_t *self = (fslice_t *)o;
  int index = fslice_get_index(self);
  int m = fslice_get_m(self);
  int n = fslice_get_n(self);
  
  if(self->type == fslice_column)
    fts_spost(stream, "<fcol %d (%dx%d)>", index, m, n);
  else
    fts_spost(stream, "<frow %d (%dx%d)>", index, m, n);
}

static void
fslice_array_function(fts_object_t *o, fts_array_t *array)
{
  fslice_t *self = (fslice_t *)o;
  float *values = fslice_get_ptr(self);
  int size = fslice_get_size(self);
  int stride = fslice_get_stride(self);
  int onset = fts_array_get_size(array);
  fts_atom_t *atoms;
  int i, j;
  
  fts_array_set_size(array, onset + size);
  atoms = fts_array_get_atoms(array) + onset;
  
  for(i=0, j=0; i<size; i++, j+=stride)
    fts_set_float(atoms + i, values[j]);
}



/********************************************************************
 *
 *  check & errors
 *
 */

void
fslice_error_index(fslice_t *slice, fslice_t *op, const char *prefix)
{
  if(!fslice_check_index(slice))
  {
    char *str = (slice->type == fslice_row)? "row": "column";
    
    fts_object_error((fts_object_t *)slice, "%s: referencing %s %d of matrix %d x %d", prefix, str, 
                     frow_get_index(slice), fmat_get_m(slice->fmat), fmat_get_n(slice->fmat));
  }

  if(op != NULL && !fslice_check_index(op))
  {
    char *str = (op->type == fslice_row)? "row": "column";
    
    fts_object_error((fts_object_t *)slice, "%s: argument references %s %d of matrix %d x %d", prefix, str,
                     frow_get_index(op), fmat_get_m(op->fmat), fmat_get_n(op->fmat));
  }
}




/******************************************************************************
 *
 *  envelopes
 *
 */

static void
fslice_lookup_fmat_or_slice(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  if(fslice_check_index(self))
  {
    float *ptr = fslice_get_ptr(self);
    int size = fslice_get_size(self);
    int stride = fslice_get_stride(self);
    fts_object_t *obj = fts_get_object(at);
    float *env;
    int env_size, env_stride;
    int i;
    
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
  else
    fslice_error_index(self, NULL, "lookup");
}

static void
fslice_lookup_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    float *ptr = fslice_get_ptr(self);
    int size = fslice_get_size(self);
    int stride = fslice_get_stride(self);
    bpf_t *bpf = (bpf_t *)fts_get_object(at);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      ptr[i] = bpf_get_interpolated(bpf, ptr[i]);
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "lookup");
}
    
static void
fslice_env_fmat_or_slice(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;  

  if(fslice_check_index(self))
  {
    fts_object_t *obj = fts_get_object(at);
    float *ptr = fslice_get_ptr(self);
    int size = fslice_get_size(self);
    int stride = fslice_get_stride(self);
    float *env;
    int env_size, env_stride;
      
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
      int i, j;
      
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
  else
    fslice_error_index(self, NULL, "env");
}

static void
fslice_env_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;  

  if(fslice_check_index(self))
  {
    bpf_t *bpf = (bpf_t *)fts_get_object(at);
    float *ptr = fslice_get_ptr(self);
    int size = fslice_get_size(self);
    int stride = fslice_get_stride(self);
    double incr = bpf_get_duration(bpf) / (double)size;
    double time = 0.0;
    int i;
    
    for(i=0; i<size*stride; i+=stride)
    {
      ptr[i] *= bpf_get_interpolated(bpf, time);
      time += incr;  
    }
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "env");
}

static void
fslice_apply_expr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    expr_t *expr = (expr_t *)fts_get_object(at);
    int size = fslice_get_size(self);
    int stride = fslice_get_stride(self);
    float *ptr = fslice_get_ptr(self);
    fts_hashtable_t locals;
    fts_atom_t key_self, key_x, value;
    fts_atom_t ret;
    int i;
    
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
  else
    fslice_error_index(self, NULL, "env");
}




/******************************************************************************
 *
 *  arithmetics
 *
 */

static void
fslice_add_fslice(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  fslice_t *right = (fslice_t *)fts_get_object(at);
  
  if(fslice_check_index(self) && fslice_check_index(right))
  {
    float *l = fslice_get_ptr(self);
    float *r = fslice_get_ptr(right);
    int l_stride = fslice_get_stride(self);
    int r_stride = fslice_get_stride(right);
    int size = fslice_get_size(self);
    int i;
    
    if(size > fslice_get_size(right))
      size = fslice_get_size(right);
    
    for(i=0; i<size; i++)
      l[i * l_stride] += r[i * r_stride];
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, right, "add");
}

static void
fslice_add_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  if(fslice_check_index(self))
  {
    float *l = fslice_get_ptr(self);
    float r = (float)fts_get_number_float(at);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      l[i] += r;
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "add");
}

static void
fslice_sub_fslice(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  fslice_t *right = (fslice_t *)fts_get_object(at);
  
  if(fslice_check_index(self) && fslice_check_index(right))
  {
    float *l = fslice_get_ptr(self);
    float *r = fslice_get_ptr(right);
    int l_stride = fslice_get_stride(self);
    int r_stride = fslice_get_stride(right);
    int size = fslice_get_size(self);
    int i;
    
    if(size > fslice_get_size(right))
      size = fslice_get_size(right);
    
    for(i=0; i<size; i++)
      l[i * l_stride] -= r[i * r_stride];
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, right, "sub");
}

static void
fslice_sub_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  if(fslice_check_index(self))
  {
    float *l = fslice_get_ptr(self);
    float r = (float)fts_get_number_float(at);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      l[i] -= r;
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "sub");
}

static void
fslice_mul_fslice(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  fslice_t *right = (fslice_t *)fts_get_object(at);
  
  if(fslice_check_index(self) && fslice_check_index(right))
  {
    float *l = fslice_get_ptr(self);
    float *r = fslice_get_ptr(right);
    int l_stride = fslice_get_stride(self);
    int r_stride = fslice_get_stride(right);
    int size = fslice_get_size(self);
    int i;
    
    if(size > fslice_get_size(right))
      size = fslice_get_size(right);
    
    for(i=0; i<size; i++)
      l[i * l_stride] *= r[i * r_stride];
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, right, "mul");
}

static void
fslice_mul_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  if(fslice_check_index(self))
  {
    float *l = fslice_get_ptr(self);
    float r = (float)fts_get_number_float(at);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      l[i] *= r;
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "mul");
}

static void
fslice_div_fslice(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  fslice_t *right = (fslice_t *)fts_get_object(at);
  
  if(fslice_check_index(self) && fslice_check_index(right))
  {
    float *l = fslice_get_ptr(self);
    float *r = fslice_get_ptr(right);
    int l_stride = fslice_get_stride(self);
    int r_stride = fslice_get_stride(right);
    int size = fslice_get_size(self);
    int i;
    
    if(size > fslice_get_size(right))
      size = fslice_get_size(right);
    
    for(i=0; i<size; i++)
      l[i * l_stride] /= r[i * r_stride];
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, right, "div");
}

static void
fslice_div_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  if(fslice_check_index(self))
  {
    float *l = fslice_get_ptr(self);
    float r = (float)fts_get_number_float(at);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      l[i] /= r;
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "div");
}

static void
fslice_bus_fslice(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  fslice_t *right = (fslice_t *)fts_get_object(at);
  
  if(fslice_check_index(self) && fslice_check_index(right))
  {
    float *l = fslice_get_ptr(self);
    float *r = fslice_get_ptr(right);
    int l_stride = fslice_get_stride(self);
    int r_stride = fslice_get_stride(right);
    int size = fslice_get_size(self);
    int i;
    
    if(size > fslice_get_size(right))
      size = fslice_get_size(right);
    
    for(i=0; i<size; i++)
      l[i * l_stride] = r[i * r_stride] - l[i * l_stride];
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, right, "bus");
}

static void
fslice_bus_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  if(fslice_check_index(self))
  {
    float *l = fslice_get_ptr(self);
    float r = (float)fts_get_number_float(at);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      l[i] = r - l[i];
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "bus");
}

static void
fslice_vid_fslice(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  fslice_t *right = (fslice_t *)fts_get_object(at);
  
  if(fslice_check_index(self) && fslice_check_index(right))
  {
    float *l = fslice_get_ptr(self);
    float *r = fslice_get_ptr(right);
    int l_stride = fslice_get_stride(self);
    int r_stride = fslice_get_stride(right);
    int size = fslice_get_size(self);
    int i;
    
    if(size > fslice_get_size(right))
      size = fslice_get_size(right);
    
    for(i=0; i<size; i++)
      l[i * l_stride] = r[i * r_stride] / l[i * l_stride];
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, right, "vid");
}

static void
fslice_vid_number(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  if(fslice_check_index(self))
  {
    float *l = fslice_get_ptr(self);
    float r = (float)fts_get_number_float(at);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      l[i] = r / l[i];
  
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "vid");
}




/******************************************************************************
 *
 *  misc math funs
 *
 */
static void
fslice_abs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    float *ptr = fslice_get_ptr(self);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      ptr[i] = fabsf(ptr[i]);
    
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "abs");
}

static void
fslice_logabs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    float *ptr = fslice_get_ptr(self);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      ptr[i] = logf(fabsf(ptr[i]));
    
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "logabs");
}

static void
fslice_log(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    float *ptr = fslice_get_ptr(self);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      ptr[i] = logf(ptr[i]);
    
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "log");
}

static void
fslice_exp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    float *ptr = fslice_get_ptr(self);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      ptr[i] = expf(ptr[i]);
    
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "exp");
}

static void
fslice_sqrabs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    float *ptr = fslice_get_ptr(self);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      ptr[i] *= ptr[i];
    
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "sqrabs");
}

static void
fslice_sqrt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    float *ptr = fslice_get_ptr(self);
    int stride = fslice_get_stride(self);
    int size = fslice_get_size(self);
    int i;
    
    for(i=0; i<size*stride; i+=stride)
      ptr[i] = sqrtf(ptr[i]);
    
    fts_return_object(o);
  }
  else
    fslice_error_index(self, NULL, "sqtr");
}




/******************************************************************************
 *
 *  min, max & co
 *
 */
static void
fslice_get_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *) o;
  
  if(fslice_check_index(self))
  {
    const int size = fslice_get_size(self);
    const int stride = fslice_get_stride(self);
    
    if(size > 0)
    {
      const float *p = fslice_get_ptr(self);
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
  else
    fslice_error_index(self, NULL, "max");
}

static void
fslice_get_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    const int size = fslice_get_size(self);
    const int stride = fslice_get_stride(self);
    
    if(size > 0)
    {
      const float *p = fslice_get_ptr(self);
      float max = p[0]; /* start with first element */
      int i;
      
      for (i=stride; i<size; i+=stride)
      {
        if (p[i] > max)
          max = p[i];
      }
      
      fts_return_float(max);
    }
  }
  else
    fslice_error_index(self, NULL, "max");
}

static void
fslice_get_absmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    const int size = fslice_get_size(self);
    const int stride = fslice_get_stride(self);
    
    if(size > 0)
    {
      const float *p = fslice_get_ptr(self);
      float max = fabsf(p[0]); /* start with first element */
      int i;
      
      for (i=stride; i<size; i+=stride)
      {
        if (p[i] > max)
          max = p[i];
      }
      
      fts_return_float(max);
    }
  }
  else
    fslice_error_index(self, NULL, "absmax");
}

static void
fslice_get_min_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    const int size = fslice_get_size(self);
    const int stride = fslice_get_stride(self);
  
    if(size > 0)
    {
      const float *p = fslice_get_ptr(self);
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
  else
    fslice_error_index(self, NULL, "mini");
}

static void
fslice_get_max_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    const int size = fslice_get_size(self);
    const int stride = fslice_get_stride(self);
  
    if(size > 0)
    {
      const float *p = fslice_get_ptr(self);
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
  else
    fslice_error_index(self, NULL, "maxi");
}

static void
fslice_get_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    const float *p = fslice_get_ptr(self);
    const int size = fslice_get_size(self);
    const int stride = fslice_get_stride(self);
    double sum = 0.0;
    int i;
    
    for(i=0; i<size*stride; i+=stride)
        sum += p[i];
    
    fts_return_float(sum);
  }
  else
    fslice_error_index(self, NULL, "sum");
}

static void
fslice_get_mean(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;

  if(fslice_check_index(self))
  {
    const float *p = fslice_get_ptr(self);
    const int size = fslice_get_size(self);
    const int stride = fslice_get_stride(self);
    double sum = 0.0;
    int i;
    
    for(i=0; i<size*stride; i+=stride)
        sum += p[i];
    
    fts_return_float(sum / (double)size);
  }
  else
    fslice_error_index(self, NULL, "mean");
}

static void
fslice_get_zc(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *) o;
  const float *p = fslice_get_ptr(self);
  const int size = fslice_get_size(self);
  const int stride = fslice_get_stride(self);
    
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
 *  post and print
 *
 */
static void
fslice_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  float *ptr = fslice_get_ptr(self);
  int index = fslice_get_index(self);
  int size = fslice_get_size(self);
  int stride = fslice_get_stride(self);
  int m = fslice_get_m(self);
  int n = fslice_get_n(self);
  fts_bytestream_t* stream = fts_get_default_console_stream();
  int i;
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  if(m * n == 0)
  {
    if(self->type == fslice_column)
      fts_spost(stream, "<fcol reference to column %d of empty fmat>\n", index);
    else
      fts_spost(stream, "<frow reference to row %d of empty fmat>\n", index);
  }
  else
  {
    if(self->type == fslice_column)
      fts_spost(stream, "<fcol reference to column %d of fmat %dx%d>\n", index, m, n);
    else
      fts_spost(stream, "<frow reference to row %d of fmat %dx%d>\n", index, m, n);
      
    fts_spost(stream, "{\n");

    for(i=0; i<size; i++)
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
fslice_init(fts_object_t *o, int type, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  self->type = type;
  self->fmat = NULL;
  self->index = 0;
  
  if(ac > 0 && fts_is_a(at, fmat_class))
  {
    self->fmat = (fmat_t *)fts_get_object(at);
    fts_object_refer((fts_object_t *)self->fmat);
  }
  else
    fts_object_error(o, "fmat argument required");
  
  if(ac > 1 && fts_is_number(at + 1))
  {
    int index = fts_get_number_int(at + 1);
    
    if(index > 0)
      self->index = index;
  }
}

static void
fcol_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_init(o, fslice_column, ac, at);
}

static void
frow_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_init(o, fslice_row, ac, at);
}

static void
fslice_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fslice_t *self = (fslice_t *)o;
  
  if(self->fmat != NULL)
    fts_object_release(self->fmat);
}

static void
fslice_message(fts_class_t *cl, fts_symbol_t s, fts_method_t slice_method, fts_method_t scalar_method)
{	
	fts_class_message(cl, s, fcol_class, slice_method);
	fts_class_message(cl, s, frow_class, slice_method);	  
	fts_class_message_number(cl, s, scalar_method);	  
}

static void
fslice_instantiate(fts_class_t *cl)
{
  fts_class_message_varargs(cl, fts_s_print, fslice_print);
  
  fts_class_set_post_function(cl, fslice_post_function);
  fts_class_set_array_function(cl, fslice_array_function);

  fslice_message(cl, fts_new_symbol("add"), fslice_add_fslice, fslice_add_number);
  fslice_message(cl, fts_new_symbol("sub"), fslice_sub_fslice, fslice_sub_number);
  fslice_message(cl, fts_new_symbol("mul"), fslice_mul_fslice, fslice_mul_number);
  fslice_message(cl, fts_new_symbol("div"), fslice_div_fslice, fslice_div_number);
  fslice_message(cl, fts_new_symbol("bus"), fslice_bus_fslice, fslice_bus_number);
  fslice_message(cl, fts_new_symbol("vid"), fslice_vid_fslice, fslice_vid_number);

  fts_class_message_void(cl, fts_new_symbol("abs"), fslice_abs);
  fts_class_message_void(cl, fts_new_symbol("logabs"), fslice_logabs);
  fts_class_message_void(cl, fts_new_symbol("log"), fslice_log);
  fts_class_message_void(cl, fts_new_symbol("exp"), fslice_exp);
  fts_class_message_void(cl, fts_new_symbol("sqrabs"), fslice_sqrabs);
  fts_class_message_void(cl, fts_new_symbol("sqrt"), fslice_sqrt);

  fts_class_message_void(cl, fts_new_symbol("min"), fslice_get_min);
  fts_class_message_void(cl, fts_new_symbol("max"), fslice_get_max);
  fts_class_message_void(cl, fts_new_symbol("mini"), fslice_get_min_index);
  fts_class_message_void(cl, fts_new_symbol("maxi"), fslice_get_max_index);
  fts_class_message_void(cl, fts_new_symbol("absmax"), fslice_get_absmax);
  fts_class_message_void(cl, fts_new_symbol("sum"), fslice_get_sum);
  fts_class_message_void(cl, fts_new_symbol("mean"), fslice_get_mean);
  fts_class_message_void(cl, fts_new_symbol("zc"), fslice_get_zc);

  fts_class_message(cl, fts_new_symbol("lookup"), fmat_class, fslice_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), frow_class, fslice_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), fcol_class, fslice_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), bpf_type, fslice_lookup_bpf);
  
  fts_class_message(cl, fts_new_symbol("env"), fmat_class, fslice_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), frow_class, fslice_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), fcol_class, fslice_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), bpf_type, fslice_env_bpf);

  fts_class_message(cl, fts_new_symbol("apply"), expr_class, fslice_apply_expr);


  /*
   * fcol/frow class documentation
   */  

  fts_class_doc(cl, fts_new_symbol("add"), "<num|fcol|frow: operand>", "add given scalar, fcol or frow (element by element) to current values");
  fts_class_doc(cl, fts_new_symbol("sub"), "<num|fcol|frow: operand>", "substract given scalar, fcol or frow (element by element)");
  fts_class_doc(cl, fts_new_symbol("mul"), "<num|fcol|frow: operand>", "multiply current values by given scalar, fcol or frow (element by element)");
  fts_class_doc(cl, fts_new_symbol("div"), "<num|fcol|frow: operand>", "divide current values by given scalar, fcol or frow (element by element)");
  fts_class_doc(cl, fts_new_symbol("bus"), "<num|fcol|frow: operand>", "subtract current values from given scalar, fcol or frow (element by element)");  
  fts_class_doc(cl, fts_new_symbol("vid"), "<num|fcol|frow: operand>", "divide given scalar, fcol or frow (element by element) by current values");

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
  
  fts_class_doc(cl, fts_new_symbol("lookup"), "<fmat|fcol|frow|bpf: function>", "apply given function (by linear interpolation)");
  fts_class_doc(cl, fts_new_symbol("env"), "<fmat|fcol|frow|bpf: envelope>", "multiply given envelope");
  fts_class_doc(cl, fts_new_symbol("apply"), "<expr: expression>", "apply expression each value (use $self and $x)");
}

static void
fcol_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fslice_t), fcol_init, fslice_delete);

  fts_class_doc(cl, fcol_symbol, "<num: column index>", "reference to matrix column");
  fslice_instantiate(cl);
}

static void
frow_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fslice_t), frow_init, fslice_delete);
  
  fts_class_doc(cl, frow_symbol, "<num: row index>", "reference to matrix row");
  fslice_instantiate(cl);
}

void 
fvec_config(void)
{
  frow_symbol = fts_new_symbol("frow");
  fcol_symbol = fts_new_symbol("fcol");

  frow_class = fts_class_install(frow_symbol, frow_instantiate);
  fcol_class = fts_class_install(fcol_symbol, fcol_instantiate);
}
