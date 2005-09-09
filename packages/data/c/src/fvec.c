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

#define ABS_MIN -3.40282347e+38F
#define ABS_MAX 3.40282347e+38F

fts_class_t *fvec_class = NULL;
fts_symbol_t fvec_symbol = NULL;

static fts_symbol_t sym_col = NULL;
static fts_symbol_t sym_row = NULL;
static fts_symbol_t sym_diag = NULL;
static fts_symbol_t sym_unwrap = NULL;
static fts_symbol_t sym_vec = NULL;
static fts_symbol_t sym_refer = NULL;

static fts_symbol_t fvec_type_names[fvec_n_types];


void fvec_config(void);


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
  fvec->type = fvec_type_row;
  
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
 *   upload methods
 *
 */

#if 0   /* copied from fmat.c */

#define FVEC_CLIENT_BLOCK_SIZE 128

static void 
fvec_upload_size(fvec_t *self)
{
  fts_atom_t a[2];
  int m = fvec_get_m(self);
  int n = fvec_get_n(self);
  
  fts_set_int(a, m);
  fts_set_int(a+1, n);
  fts_client_send_message((fts_object_t *)self, fts_s_size, 2, a);
}

static void 
fvec_upload_from_index(fvec_t *self, int row_id, int col_id, int size)
{
  fts_atom_t a[FVEC_CLIENT_BLOCK_SIZE];  
  int n_cols = fvec_get_n(self);
  int sent = 0;
  int data_size = size;
  int ms = row_id;
  int ns = col_id;
  int start_id = (ms*n_cols + ns);
  
  fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 0, 0);
  
  while( data_size > 0)
  {
    int i = 0;
    int n = (data_size > FVEC_CLIENT_BLOCK_SIZE-2)? FVEC_CLIENT_BLOCK_SIZE-2: data_size;
    
    /* starting row and column index */
    if( sent)
    {
      ms = sent/n_cols;
      ns = sent - ms*n_cols;
    }
    fts_set_int(a, ms);
    fts_set_int(a+1, ns);
    
    for(i=0; i < n ; i++)
      fts_set_float(&a[2+i], self->values[start_id  + sent + i]);      
    
    fts_client_send_message((fts_object_t *)self, fts_s_set, n+2, a);
    
    sent += n;
    data_size -= n;
  }
  
  fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
}

static void 
fvec_upload_data(fvec_t *self)
{  
  fvec_upload_from_index(self, 0, 0, fvec_get_m(self) * fvec_get_n(self));
}

static void
fvec_upload(fvec_t *self)
{
  fvec_upload_size(self);
  fvec_upload_data(self);
}

#else

static void
fvec_upload (fvec_t *self)
{
  fts_post("UPLOAD DUMMY does nothing!\n");
}

#endif




/*********************************************************
 *
 *  editor
 *
 */

static void *fvec_editor = NULL;


static void
fvec_editor_callback (fts_object_t *o, void *e)
{
  fvec_t *self = (fvec_t *) o;
  fvec_upload(self); 
}

static fts_method_status_t
fvec_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *) o;
  
  fvec_set_editor_open(self);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  
  fts_object_add_listener(o, fvec_editor, fvec_editor_callback);
  
  fvec_upload(self);
  
  return fts_ok;
}

static fts_method_status_t
fvec_destroy_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *) o;
  
  fvec_set_editor_close(self);
  fts_object_remove_listener(o, fvec_editor);
  
  return fts_ok;
}

static fts_method_status_t
fvec_close_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *) o;
  
  if (fvec_editor_is_open(self))
  {
    fvec_set_editor_close(self);
    fts_client_send_message(o, fts_s_closeEditor, 0, 0);  
    fts_object_remove_listener(o, fvec_editor);
  }
  
  return fts_ok;
}



/********************************************************************
 *
 *  utilities
 *
 */

/* if another object changed our data, do the necessary stuff */
static void fvec_changed(fvec_t *this)
{
    if (this->editor)
        tabeditor_send((tabeditor_t *) this->editor);

    /* ??? no longer in fmat.  data_object_set_dirty((fts_object_t *) this); */
}


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
        fvec->index = fts_get_number_int(at);
    case 0:
      break;
  }
}


/** get pointer, size, stride to access data in fmat 
 *  referenced by this fvec
 *
 *  fvec onset and size must be >= 0  (as checked by fvec_set_dimensions)
 */
void
fvec_get_vector(fvec_t *fvec, float **ptr, int *size, int *stride)
{
  /* clip x to be less or equal than upper limit u.
     postcondition:  x <= u */
# define CLIP(x, u)      do { if ((x) > (u))  { (x) = (u); } } while (0)
# define CLIP2(x, l, u)  do { if      ((x) < (l))  { (x) = (l); }       \
                              else if ((x) > (u))  { (x) = (u); } } while (0)

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
    {
      int i, j; /* diagonal start indices */

      if (fvec_index >= 0)
      { /* superdiagonal: like col index */
        CLIP(fvec_onset, fmat_m);
        CLIP(fvec_onset, fmat_n - fvec_index);

        i = fvec_onset;
        j = fvec_index + fvec_onset;
      }
      else
      { /* subdiagonal:   like row index */
        CLIP(fvec_onset, fmat_m + fvec_index);
        CLIP(fvec_onset, fmat_n);

        i = -fvec_index + fvec_onset;
        j = fvec_onset;
      }

      /* clip diagonal end indices to matrix size */
      CLIP(fvec_size, fmat_m - i);
      CLIP(fvec_size, fmat_n - j);

      *ptr    = fmat_ptr + i * fmat_n + j;
      *size   = fvec_size;
      *stride = fmat_n + 1;
    }
    break;

    case fvec_type_unwrap:
      
      /* row onset */
      if(fvec_index < 0)
        fvec_index = 0;
      else if(fvec_index > fmat_m)
        fvec_index = fmat_m;
      
      /* column onset */
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
  
  for(i=0, j=0; i<size*stride; i++, j+=stride)
    fts_set_float(atoms + i, values[j]);
}

/******************************************************************************
 *
 *  envelopes
 *
 */

static fts_method_status_t
fvec_lookup_fmat_or_slice(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_lookup_bpf(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  bpf_t *bpf = (bpf_t *)fts_get_object(at);
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = bpf_get_interpolated(bpf, ptr[i]);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}
    
static fts_method_status_t
fvec_env_fmat_or_slice(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_env_bpf(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_apply_expr(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  expr_t *expr = (expr_t *)fts_get_object(at);
  fts_hashtable_t locals;
  fts_atom_t key_self, key_x, value;
  fts_atom_t r;
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
    
    expr_evaluate(expr, &locals, ac - 1, at + 1, &r);
    
    if(fts_is_number(&r))
      ptr[i] = fts_get_number_float(&r);
    else
      ptr[i] = 0.0;
  }
  
  fts_hashtable_destroy(&locals);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

/* get element, no checks */
float
fvec_get_element (fvec_t *self, int i)
{
  float *ptr;
  int    size, stride;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  return ptr[i * stride];
}


/* set element, no checks */
void
fvec_set_element (fvec_t *self, int i, float value)
{
  float *ptr;
  int    size, stride;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  ptr[i * stride] = value;
}


/** set values in fvec
 *  @fn fvec_object set(int offset, number values...)
 */
static fts_method_status_t
fvec_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 1  &&  fts_is_number(at))
  {
    fvec_t *self = (fvec_t *)o;
    float  *ptr;
    int     size, stride;
    int     onset = fts_get_number_int(at);
    int     i, j;
    
    ac -= 1;
    at += 1;

    fvec_get_vector(self, &ptr, &size, &stride);

    if (onset + ac > size)
      ac = size - onset;
  
    for (i = 0, j = onset; i < ac; i++, j += stride) 
    {
      if (fts_is_number(at + i))
        ptr[j] = fts_get_number_float(at + i);
    }
  
    fts_object_changed(o);
  }

  fts_set_object(ret, o);
  
  return fts_ok;
}


static fts_method_status_t
fvec_set_from_fmat_or_fvec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_fvec_set_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  fmat_t *fmat = (fmat_t *)fts_get_object(at);
  
  fts_object_release((fts_object_t *)self->fmat);
  self->fmat = fmat;
  fts_object_refer((fts_object_t *)fmat);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_fvec_set_fmat_and_dimensions(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  
  if(ac > 0 && fts_is_a(at, fmat_class))
  {
    _fvec_set_fmat(o, NULL, 1, at, fts_nix);
      
    if(ac > 1 && fts_is_symbol(at + 1))
    {
      fts_symbol_t sym = fts_get_symbol(at + 1);
      self->type = fvec_get_type_from_symbol(sym);
    }
    
    if(ac > 2)
      fvec_set_dimensions(self, ac - 2, at + 2);
    
    fts_set_object(ret, o);
  }
  
  return fts_ok;
}

static fts_method_status_t
_fvec_set_col(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  
  self->type = fvec_type_column;
  fvec_set_dimensions(self, ac, at);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_fvec_set_row(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  
  self->type = fvec_type_row;
  fvec_set_dimensions(self, ac, at);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_fvec_set_diag(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  
  self->type = fvec_type_diagonal;
  fvec_set_dimensions(self, ac, at);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_fvec_set_unwrap(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  
  self->type = fvec_type_unwrap;
  fvec_set_dimensions(self, ac, at);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_fvec_set_vector(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  self->size = size;
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_fvec_get_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  int size = fvec_get_size(self);
  
  fts_set_int(ret, size);
  
  return fts_ok;
}

static fts_method_status_t
_fvec_set_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  int size = fts_get_number_int(at);
  
  if(size > 0 && fvec_get_type(self) == fvec_type_vector)
    fmat_set_m(self->fmat, size);
  
  fvec_set_size(self, size);
    
  fts_set_int(ret, size);
  
  return fts_ok;
}

/******************************************************************************
 *
 *  arithmetics
 *
 */

static fts_method_status_t
fvec_add_fvec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_add_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] += r;
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_sub_fvec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_sub_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] -= r;
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_mul_fvec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_mul_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] *= r;
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_div_fvec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_div_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] /= r;
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_bus_fvec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_bus_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] = r - l[i];
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_vid_fvec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_vid_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float r = (float)fts_get_number_float(at);
  float *l;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &l, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    l[i] = r / l[i];
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

/******************************************************************************
 *
 *  misc math funs
 *
 */
static fts_method_status_t
fvec_clip(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float low = ABS_MIN;
  float high = ABS_MAX;
  float *ptr;
  int size, stride;
  int i;
  
  if(ac > 0 && fts_is_number(at))
  {
    if(ac > 1 && fts_is_number(at + 1))
    {
      low = fts_get_number_float(at);
      high = fts_get_number_float(at + 1);
    }
    else
      high = fts_get_number_float(at);
  }
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
  {
    float f = ptr[i];
    
    if(f > high)
      f = high;
    else if(f < low)
      f = low;
  }
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_normalize(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  if(size > 0)
  {
    float max = ptr[0];
    int i;
    
    /* find maximum */
    for(i=0; i<size*stride; i+=stride)
    {
      float f = ptr[i];
      
      if(f > max)
        max = f;
      else if(f < -max)
        max = -f;
    }
  
    if(max != 0.0)
    {
      float scale = 1.0 / max;
      
      for(i=0; i<size*stride; i+=stride)
        ptr[i] *= scale;
    }
  }

  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_abs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = fabsf(ptr[i]);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_logabs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = logf(fabsf(ptr[i]));
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_log(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = logf(ptr[i]);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_exp(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = expf(ptr[i]);
  
  fts_set_object(ret, o);
  

  return fts_ok;
}

static fts_method_status_t
fvec_sqrabs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    ptr[i] *= ptr[i];
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fvec_sqrt(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *ptr;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &ptr, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    ptr[i] = sqrtf(ptr[i]);
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

/******************************************************************************
 *
 *  min, max & co
 *
 */

static fts_method_status_t
fvec_get_min(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *) o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float min = p[0];
    int i;
    
    for (i=stride; i<size*stride; i+=stride)
    {
      if (p[i] < min)
        min = p[i];
    }
    
      fts_set_float(ret, min);
  }
  
  return fts_ok;
}

static fts_method_status_t
fvec_get_max(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float max = p[0]; /* start with first element */
    int i;
    
    for (i=stride; i<size*stride; i+=stride)
    {
      if(p[i] > max)
        max = p[i];
    }
    
    fts_set_float(ret, max);
  }
  
  return fts_ok;
}

static fts_method_status_t
fvec_get_absmax(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  float *p;
  int size, stride;
  
  fvec_get_vector(self, &p, &size, &stride);

  if(size > 0)
  {
    float max = fabsf(p[0]); /* start with first element */
    int i;
    
    for (i=stride; i<size*stride; i+=stride)
    {
      if (fabsf(p[i]) > max)
        max = fabsf(p[i]);
    }
    
    fts_set_float(ret, max);
  }
  
  return fts_ok;
}

static fts_method_status_t
fvec_get_min_index(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
    
    for(i=1, j=stride; i<size*stride; i++, j+=stride)
    {
      if(p[j] < min)
      {
        min = p[j];
        mini = i;
      }
    }
    
    fts_set_int(ret, mini);
  }
  
  return fts_ok;
}

static fts_method_status_t
fvec_get_max_index(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
    
    for(i=1, j=stride; i<size*stride; i++, j+=stride)
    {
      if (p[j] > max)
      {
        max = p[j];
        maxi = i;
      }
    }   
    
    fts_set_int(ret, maxi);
  }
  
  return fts_ok;
}


static fts_method_status_t
fvec_sort (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *) o;

  _fmat_sort(self->fmat, 0, _fmat_element_compare_ascending);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}


static fts_method_status_t
fvec_sortrev (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;

  _fmat_sort(self->fmat, 0, _fmat_element_compare_descending);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}


static fts_method_status_t
fvec_get_sum(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  double sum = 0.0;
  float *p;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &p, &size, &stride);
  
  for(i=0; i<size*stride; i+=stride)
    sum += p[i];
  
  fts_set_float(ret, sum);
  
  return fts_ok;
}


static fts_method_status_t
fvec_get_prod (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *) o;
  double  prod = 0.0;
  float  *p;
  int     size, stride;
  int     i;
  
  fvec_get_vector(self, &p, &size, &stride);
  
  for (i = 0; i < size * stride; i += stride)
    prod *= p[i];
  
  fts_set_float(ret, prod);
  
  return fts_ok;
}


static fts_method_status_t
fvec_get_mean(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  double sum = 0.0;
  float *p;
  int size, stride;
  int i;
  
  fvec_get_vector(self, &p, &size, &stride);
    
  for(i=0; i<size*stride; i+=stride)
    sum += p[i];
  
  fts_set_float(ret, sum / (double)size);
  
  return fts_ok;
}


static fts_method_status_t
fvec_get_variance (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  double  sum = 0.0, sum2 = 0.0;
  double  mean, var;
  float  *p;
  int     size, stride;
  int     i;
  
  fvec_get_vector(self, &p, &size, &stride);
    
  for (i = 0; i < size * stride; i += stride)
  {
    sum  += p[i];
    sum2 += p[i] * p[i];
  }

  mean = sum  / (double) size;
  var  = sum2 / (double) size - mean * mean;

  fts_set_float(ret, var);
  
  return fts_ok;
}


static fts_method_status_t
fvec_get_zc(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
    fts_set_int(ret, zc);
  }
  
  return fts_ok;
}

/****************************************************************************
 *
 *  system mehods
 *
 */

/* called by get element message */
static fts_method_status_t
_fvec_get_element(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *) o;
  float *ptr;
  int size, stride;
  int i = 0;
  
  fvec_get_vector(self, &ptr, &size, &stride);

  if (ptr  &&  size == 0)
    fts_set_float(ret, 0);        /* empty matrix: no error, just return 0 */
  else
  {
    if(ac > 0  &&  fts_is_number(at))
      i = fts_get_number_int(at);

    if(i >= size)
      i = size - 1;
  
    while (i < 0)
      i += size;
  
    fts_set_float(ret, ptr[i * stride]);
  }

  return fts_ok;
}


static fts_method_status_t
_fvec_get_matrix (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *) o;

  fts_set_object(ret, self->fmat);
}


static fts_method_status_t
fvec_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  
  if(self->type != fvec_type_vector)
  {
    fts_message_t *mess = fts_dumper_message_get(dumper, sym_refer);
    fts_message_append_object(mess, (fts_object_t *)self->fmat);
    fts_message_append_symbol(mess, fvec_type_names[self->type]);
    fts_message_append_int(mess, self->index);
    fts_message_append_int(mess, self->onset);
    fts_message_append_int(mess, self->size);
    fts_dumper_message_send(dumper, mess);
  }
  else if(self->fmat != fmat_null)
  {
    fts_message_t *mess = fts_dumper_message_get(dumper, sym_vec);
    fts_message_append_int(mess, self->size);
    fts_dumper_message_send(dumper, mess);
    
    mess = fts_dumper_message_get(dumper, fts_s_set);
    fts_message_append_object(mess, (fts_object_t *)self->fmat);
    fts_dumper_message_send(dumper, mess);
  }
  
  return fts_ok;
}




/****************************************************************************
 *
 *  post and print
 *
 */

static fts_method_status_t
fvec_print(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
  
  return fts_ok;
}

/******************************************************************************
 *
 *  class
 *
 */
static fts_method_status_t
fvec_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  
  self->fmat = fmat_null;
  self->type = fvec_type_column;
  self->index = 0;
  self->onset = 0;
  self->size = INT_MAX >> 2;

  self->editor = 0;
  
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
      {
	fts_set_int((fts_atom_t *)at, 0);
        fvec_set(o, NULL, ac, at, fts_nix);
      }
    }    
  }
  else
  { /* no init args given: create empty column vector with own fmat */
    self->fmat = fmat_create(0, 1);
    self->type = fvec_type_vector;
    self->size = 0;
  }
  
  fts_object_refer((fts_object_t *)self->fmat);
  
  return fts_ok;
}

static fts_method_status_t
fvec_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fvec_t *self = (fvec_t *)o;
  
  fts_object_release((fts_object_t *)self->fmat);
  
  return fts_ok;
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
  fts_class_set_array_function(cl, fvec_array_function);

  /* standard methods for naming, persistence, and dump */
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, fvec_dump_state);

  /* graphical editor */
  fts_class_message_varargs(cl, fts_s_openEditor,    fvec_open_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor,   fvec_close_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, fvec_destroy_editor);

  /* access methods */
  fts_class_message_varargs(cl, fts_s_print, fvec_print);
  fts_class_message_varargs(cl, fts_s_get_element, _fvec_get_element);
  fts_class_message_varargs(cl, fts_s_get, _fvec_get_element);
  fts_class_message_void   (cl, fts_s_get, _fvec_get_matrix);

  fts_class_message_varargs(cl, fts_s_set, fvec_set);
  fts_class_message(cl, fts_s_set, fmat_class, fvec_set_from_fmat_or_fvec);
  fts_class_message(cl, fts_s_set, fvec_class, fvec_set_from_fmat_or_fvec);

  fts_class_message(cl, sym_refer, fmat_class, _fvec_set_fmat);
  fts_class_message_varargs(cl, sym_refer, _fvec_set_fmat_and_dimensions);
  fts_class_message_varargs(cl, sym_col, _fvec_set_col);
  fts_class_message_varargs(cl, sym_row, _fvec_set_row);
  fts_class_message_varargs(cl, sym_diag, _fvec_set_diag);
  fts_class_message_varargs(cl, sym_unwrap, _fvec_set_unwrap);
  fts_class_message_varargs(cl, sym_vec, _fvec_set_vector);
  
  fts_class_message_void(cl, fts_s_size, _fvec_get_size);
  fts_class_message_number(cl, fts_s_size, _fvec_set_size);

  /* arithmetics (on fvec or fmat!) */
  fvec_message(cl, fts_new_symbol("add"), fvec_add_fvec, fvec_add_number);
  fvec_message(cl, fts_new_symbol("sub"), fvec_sub_fvec, fvec_sub_number);
  fvec_message(cl, fts_new_symbol("mul"), fvec_mul_fvec, fvec_mul_number);
  fvec_message(cl, fts_new_symbol("div"), fvec_div_fvec, fvec_div_number);
  fvec_message(cl, fts_new_symbol("bus"), fvec_bus_fvec, fvec_bus_number);
  fvec_message(cl, fts_new_symbol("vid"), fvec_vid_fvec, fvec_vid_number);

  fts_class_message_varargs(cl, fts_new_symbol("clip"), fvec_clip);
  fts_class_message_void(cl, fts_new_symbol("normalize"), fvec_normalize);

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
  fts_class_message_void(cl, fts_new_symbol("prod"), fvec_get_prod);
  fts_class_message_void(cl, fts_new_symbol("mean"), fvec_get_mean);
  fts_class_message_void(cl, fts_new_symbol("variance"), fvec_get_variance);
  fts_class_message_void(cl, fts_new_symbol("zc"), fvec_get_zc);

  fts_class_message_void(cl, fts_s_sort,    fvec_sort);
  fts_class_message_void(cl, fts_s_sortrev, fvec_sortrev);

  /* fmat methods that work on fvec, too: */
  fts_class_message_number(cl, fts_s_fill, fmat_fill_number);

  fts_class_message(cl, fts_new_symbol("lookup"), fmat_class, fvec_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), fvec_class, fvec_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), bpf_type, fvec_lookup_bpf);
  
  fts_class_message(cl, fts_new_symbol("env"), fmat_class, fvec_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), fvec_class, fvec_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), bpf_type, fvec_env_bpf);

  fts_class_message(cl, fts_new_symbol("apply"), expr_class, fvec_apply_expr);

  /* let's have standard in/outlets */
  fts_class_inlet_bang(cl, 0, data_object_output);
  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);
  


  /*
   * fvec class documentation
   */
  fts_class_doc(cl, fvec_symbol, "<'col'> <num: column index> [<num: row onset> [<num: size>]]", "vector reference to matrix column");
  fts_class_doc(cl, fvec_symbol, "<'row'> <num: row index> [<num: column onset> [<num: size>]]", "vector reference to matrix row");
  fts_class_doc(cl, fvec_symbol, "<'diag'> <num: row onset> [<num: column onset> [<num: size>]]", "vector reference to matrix diagonal");
  fts_class_doc(cl, fvec_symbol, "<'unwrap'> <num: row onset> [<num: column onset> [<num: size>]]", "vector reference to unwrapped matrix");
  fts_class_doc(cl, fvec_symbol, "<num: size>", "vector reference compatible float vector");

  fts_class_doc(cl, fts_s_size, NULL, "get size");

  fts_class_doc(cl, fts_new_symbol("min"), NULL, "get minimum value");
  fts_class_doc(cl, fts_new_symbol("mini"), NULL, "get index of minimum value");
  fts_class_doc(cl, fts_new_symbol("max"), NULL, "get maximum value");
  fts_class_doc(cl, fts_new_symbol("maxi"), NULL, "get index of maximum value");
  fts_class_doc(cl, fts_new_symbol("absmax"), NULL, "get maximum absolute value");
  fts_class_doc(cl, fts_new_symbol("sum"),  NULL, "get sum of all values");
  fts_class_doc(cl, fts_new_symbol("prod"), NULL, "get product of all values");
  fts_class_doc(cl, fts_new_symbol("mean"), NULL, "get mean value of all values");
  fts_class_doc(cl, fts_new_symbol("variance"), NULL, "get variance of all values");
  fts_class_doc(cl, fts_new_symbol("zc"), NULL, "get number of zerocrossings");  
  
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

  fts_class_doc(cl, fts_new_symbol("clip"), "[<lower limit>] <upper limit>", "clip values within given limits");
  fts_class_doc(cl, fts_new_symbol("normalize"), NULL, "normalize to between -1.0 and 1.0");

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
