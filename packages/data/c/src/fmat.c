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

/** @file fmat.c fmat data object doc
 */

#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include "floatfuns.h"

#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <string.h>


#ifdef WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

#define ABS_MIN -3.40282347e+38F
#define ABS_MAX 3.40282347e+38F

#define LOG_MIN -103.28
#define LOG_ARG_MIN (float)(1.4e-45)

fmat_t *fmat_null = NULL;
fts_class_t *fmat_class = NULL;
fts_symbol_t fmat_symbol = NULL;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_getcol = 0;
static fts_symbol_t sym_getrow = 0;

static fts_symbol_t sym_vec = 0;
static fts_symbol_t sym_real = 0;
static fts_symbol_t sym_rect = 0;
static fts_symbol_t sym_polar = 0;
static fts_symbol_t sym_re = 0;
static fts_symbol_t sym_im = 0;
static fts_symbol_t sym_mag = 0;
static fts_symbol_t sym_arg = 0;

static fts_symbol_t sym_insert_cols = 0;
static fts_symbol_t sym_delete_cols = 0;

int fmat_or_slice_pointer(fts_object_t *obj, float **ptr, int *size, int *stride);

/********************************************************
 *
 *  utility functions
 *
 */

/* create fmat with m rows and n columns */
fmat_t *
fmat_create(int m, int n)
{
  fts_atom_t size[2];
  
  fts_set_int(&size[0], m);
  fts_set_int(&size[1], n);
  return ((fmat_t *) fts_object_create(fmat_class, 2, size));
}


/* value allocation or reallocation wrapper */
static void
fmat_realloc_values(fmat_t *self, size_t size)
{
  float *values = self->values;
  
  self->values  = NULL;
  
  if (values == NULL)
    values = (float *) fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
  else
    values = (float *) fts_realloc(values - HEAD_POINTS, (size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));

#if 0  /* don't do this, rather crash on such an exceptional occasion */
  if (values == NULL)
  { /* out of memory, at least don't make us crash but allocate valid (0, 0) fmat */
    values      = (float *) fts_malloc((HEAD_POINTS + TAIL_POINTS) * sizeof(float));
    size        = 0;
    self->m     = 0;
    self->n     = 0;
  }
#endif
  
  self->values = values + HEAD_POINTS;
  self->alloc  = size;
}


/* change matrix "form", leaving underlying data vector untouched */
void
fmat_reshape(fmat_t *self, int m, int n)
{
  int size;
  
  if(n <= 0)
    n = 1;
  
  size = m * n;
  
  if (size > self->alloc)
    fmat_realloc_values(self, size);
  
  self->m = m;
  self->n = n;
}


/** copy from fmat in with resampling by factor */
void
fmat_resample (fmat_t *self, fmat_t *in, double factor)
{
  int m = fmat_get_m(in);
  int n = fmat_get_n(in);

  if (m > 3)
  {
    double  	 inv = 1.0 / factor;
    float	*ptr;
    float  	*out_ptr;
    int     	 out_m = (int) ceil((double) m * inv);
    int     	 head  = (int) ceil(inv);
    fts_idefix_t idefix;
    fts_idefix_t incr;
    int		 i, j;
      
    /* zero pad for interpolation */
    fmat_set_m(in, m + 2);
    fmat_set_m(in, m);
    ptr = fmat_get_ptr(in);
        
    fmat_reshape(self, out_m, n);
    out_ptr = fmat_get_ptr(self);
      
    fts_idefix_set_float(&incr, factor);
      
    for (j = 0; j < n; j++)
    {
      fts_idefix_set_zero(&idefix);
        
      /* copy first points without interpolation */
      for (i = j; i < head * n; i += n)
      {
	out_ptr[i] = ptr[j];
	fts_idefix_incr(&idefix, incr);
      }
        
      /* interpolate */
      for(; i < out_m * n; i += n)
      {
	fts_cubic_idefix_interpolate_stride(ptr + j, idefix, n, out_ptr + i);
	fts_idefix_incr(&idefix, incr);
      }
    }
  }
  else
    fmat_reshape(self, 0, 0);  
}


void
fmat_set_m(fmat_t *self, int m)
{
  int size = m * fmat_get_n(self);
  int i;
  
  if (m > fmat_get_m(self))
    fmat_realloc_values(self, size);
  
  /* zero new rows at end (if any) */
  for(i=fmat_get_m(self)*fmat_get_n(self); i<size; i++)
    self->values[i] = 0.0;
    
  self->m = m;
  self->domain = 0.0;
}

void
fmat_set_n(fmat_t *self, int n)
{
  int m = fmat_get_m(self);
  int old_n = fmat_get_n(self);
  
  if(n <= 0)
    n = 1;
  
  if(n != old_n)
  {
    int size = m * n;
    int i, j;
  
    if (n > old_n)
    {
      fmat_realloc_values(self, size);

      /* copy values (from last to first row) */
      for(i=m-1; i>=1; i--)
      {
        float *old_row = self->values + i * old_n;
        float *new_row = self->values + i * n;
        
        /* copy old rows */
        for(j=old_n-1; j>=0; j--)
          new_row[j] = old_row[j];
      }
      
      /* complete rows by zeros */
      for(i=0; i<m; i++)
      {
        float *row = self->values + i * n;
        
        /* zero end of new rows */
        for(j=old_n; j<n; j++)
          row[j] = 0.0;
      }
    }
    else /* if(n < old_n) */
    {
      /* copy and shorten rows */
      for(i=1; i<m; i++)
      {
        float *old_row = self->values + i * old_n;
        float *new_row = self->values + i * n;
        
        /* copy beginning of old rows */
        for(j=0; j<n; j++)
          new_row[j] = old_row[j];
      }
    }
    
    self->n = n;
  }
}

/* change matrix size, copying data around */
void
fmat_set_size(fmat_t *self, int m, int n)
{
  int old_m = fmat_get_m(self);
  int old_n = fmat_get_n(self);
  int min_m, min_n;
  
  if(n <= 0)
    n = 1;
  
  if(n == old_n)
    fmat_set_m(self, m);
  else if(m == old_m)
    fmat_set_n(self, n);
  else
  {
    int size, i, j;
    
    size = m * n;

    min_m = (m < old_m)? m: old_m;
    min_n = (n < old_n)? n: old_n;
    
    if (size > self->alloc)
      fmat_realloc_values(self, size);
        
    if(n > old_n)
    {
      /* copy values (from last to first row) */
      for(i=min_m-1; i>=1; i--)
      {
        float *old_row = self->values + i * old_n;
        float *new_row = self->values + i * n;
        
        /* copy old rows */
        for(j=old_n-1; j>=0; j--)
          new_row[j] = old_row[j];
      }
      
      /* complete rows by zeros */
      for(i=0; i<min_m; i++)
      {
        float *row = self->values + i * n;
        
        /* zero end of new rows */
        for(j=old_n; j<n; j++)
          row[j] = 0.0;
      }
    }
    else /* if(n < old_n) */
    {
      /* copy and shorten rows */
      for(i=1; i<min_m; i++)
      {
        float *old_row = self->values + i * old_n;
        float *new_row = self->values + i * n;
        
        /* copy beginning of old rows */
        for(j=0; j<n; j++)
          new_row[j] = old_row[j];
      }
    }

    /* zero new rows at end (if any) */
    for(i=min_m*n; i<size; i++)
      self->values[i] = 0.0;
    
    self->m = m;
    self->n = n;
    self->domain = 0.0;
  }
}


void
fmat_set_const(fmat_t *mat, float c)
{
  float *values;
  int size;
  int stride;
  int i;

  if (fmat_or_slice_pointer((fts_object_t*)mat, &values, &size, &stride))
  {
    size *= stride;     /* size is num. elements in slice! */

    for (i = 0; i < size; i += stride)
      values[i] = c;
  }
}


void
fmat_set_from_atoms(fmat_t *mat, int onset, int step, int ac, const fts_atom_t *at)
{
  int i;

  for(i=0; i<ac; i++)
  {
    if(fts_is_number(at + i))
      mat->values[onset + i * step] = (float)fts_get_number_float(at + i);
    else
      mat->values[onset + i * step] = 0.0f;
  }
}

static void
fmat_set_from_tuples(fmat_t *mat, int ac, const fts_atom_t *at)
{
  int n = fmat_get_n(mat);
  int i, j;

  for(i=0; i<ac; i++)
  {
    if(fts_is_tuple(at + i))
    {
      fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at + i);
      int size = fts_tuple_get_size(tup);

      if(size > n)
        size = n;

      for(j=0; j<size; j++)
      {
        fts_atom_t *a = fts_tuple_get_element(tup, j);

        if(fts_is_number(a))
          mat->values[i * n + j] = (float)fts_get_number_float(a);
        else
          mat->values[i * n + j] = 0.0;
      }
    }
    else
      break;
  }
}

void
fmat_copy(fmat_t *org, fmat_t *copy)
{
  int m = fmat_get_m(org);
  int n = fmat_get_n(org);
  int i;

  fmat_reshape(copy, org->m, org->n);

  for(i=0; i<m*n; i++)
    copy->values[i] = org->values[i];

  copy->onset = org->onset;
  copy->domain = org->domain;
}


static void
fmat_copy_function(const fts_object_t *from, fts_object_t *to)
{
  fmat_t *dest = (fmat_t *)to;
  fmat_copy((fmat_t *)from, dest);
  
  if(fmat_editor_is_open(dest))
    fmat_upload(dest);
}

static int
fmat_equals(const fmat_t *a, const fmat_t *b)
{
  if(fmat_get_m(a) == fmat_get_m(b) && fmat_get_n(a) == fmat_get_n(b))
  {
    int size = fmat_get_m(a) * fmat_get_n(a);
    float *a_ptr = fmat_get_ptr(a);
    float *b_ptr = fmat_get_ptr(b);
    int i;
    
    for(i=0; i<size; i++)
      if(!data_float_equals(a_ptr[i], b_ptr[i]))
        return 0;
    
    return 1;
  }
  
  return 0;
}

static void
fmat_array_function(fts_object_t *o, fts_array_t *array)
{
  fmat_t *self = (fmat_t *)o;
  float *values = fmat_get_ptr(self);
  int size = fmat_get_m(self) * fmat_get_n(self);
  int onset = fts_array_get_size(array);
  fts_atom_t *atoms;
  int i;
  
  fts_array_set_size(array, onset + size);
  atoms = fts_array_get_atoms(array) + onset;
  
  for(i=0; i<size; i++)
    fts_set_float(atoms + i, values[i]);
}

float
fmat_get_max_abs_value_in_range(fmat_t *mat, int a, int b)
{
  int size = fmat_get_m(mat) * fmat_get_n(mat);
  float *ptr = fmat_get_ptr(mat);
  float max = fabsf(ptr[a]);
  int i;
  
  if(b > size)
    b = size;
  
  for (i=a+1; i<b; i++)
  {
    float f = fabsf(ptr[i]);
    
    if(f > max)
      max = f;
  }
  
  return max;
}

float
fmat_get_max_value_in_range(fmat_t *mat, int a, int b)
{
  int size = fmat_get_m(mat) * fmat_get_n(mat);
  float *ptr = fmat_get_ptr(mat);
  float max = ptr[a];
  int i;
  
  if(b > size)
    b = size;
  
  for (i=a+1; i<b; i++)
  {
    if(ptr[i] > max)
      max = ptr[i];
  }
  
  return max;
}

float
fmat_get_min_value_in_range(fmat_t *mat, int a, int b)
{
  int size = fmat_get_m(mat) * fmat_get_n(mat);
  float *ptr = fmat_get_ptr(mat);
  float min = ptr[a];
  int i;
  
  if(b > size)
    b = size;
  
  for (i=a+1; i<b; i++)
  {
    if(ptr[i] < min)
      min = ptr[i];
  }
  
  return min;
}

/********************************************************************
 *
 *  check & errors
 *
 */

/** return parameters to iterate over one vector of an fmat or over an fvec */
int
fmat_or_slice_vector(fts_object_t *obj, float **ptr, int *size, int *stride)
{
  fts_class_t *cl = fts_object_get_class(obj);

  if(cl == fmat_class)
  {
    fmat_t *fmat = (fmat_t *)obj;
    
    /* return first column */
    *ptr = fmat_get_ptr(fmat);
    *size = fmat_get_m(fmat);
    *stride = fmat_get_n(fmat);
    
    return 1;
  }
  
  return fvec_vector(obj, ptr, size, stride);
}


/** return parameters to iterate over all elements of fmat or fvec */
int
fmat_or_slice_pointer(fts_object_t *obj, float **ptr, int *size, int *stride)
{
  fts_class_t *cl = fts_object_get_class(obj);

  if (cl == fmat_class)
  {
    fmat_t *fmat = (fmat_t *) obj;
    
    *ptr = fmat_get_ptr(fmat);
    *size = fmat_get_m(fmat) * fmat_get_n(fmat);
    *stride = 1;
    
    return 1;
  }
  else
    return fvec_vector(obj, ptr, size, stride);
}



static void
fmat_error_dimensions(fmat_t *fmat, fmat_t *op, const char *prefix)
{
  int m = fmat_get_m(fmat);
  int n = fmat_get_n(fmat);
  int op_m = fmat_get_m(op);
  int op_n = fmat_get_n(op);
  
  fts_object_error((fts_object_t *)fmat, "%s: argument %d x %d doesn't match matrix dimensions %d x %d", prefix, op_m, op_n, m, n);
}

static void
fmat_error_complex(fmat_t *fmat, const char *prefix)
{
  int m = fmat_get_m(fmat);
  int n = fmat_get_n(fmat);
  
  fts_object_error((fts_object_t *)fmat, "%s: matrix %d x %d cannot be handled as complex", prefix, m, n);
}

/********************************************************************
*
*   upload methods
*
*/
#define FMAT_CLIENT_BLOCK_SIZE 128

static void 
fmat_upload_size(fmat_t *self)
{
  fts_atom_t a[2];
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  fts_set_int(a, m);
  fts_set_int(a+1, n);
  fts_client_send_message((fts_object_t *)self, fts_s_size, 2, a);
}

static void 
fmat_upload_from_index(fmat_t *self, int row_id, int col_id, int size)
{
  fts_atom_t a[FMAT_CLIENT_BLOCK_SIZE];  
  int n_cols = fmat_get_n(self);
  int sent = 0;
  int data_size = size;
  int ms = row_id;
  int ns = col_id;
  int start_id = (ms*n_cols + ns);
  
  fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 0, 0);
  
  while( data_size > 0)
  {
    int i = 0;
    int n = (data_size > FMAT_CLIENT_BLOCK_SIZE-2)? FMAT_CLIENT_BLOCK_SIZE-2: data_size;
    
    /* starting row and column index */
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
fmat_upload_data(fmat_t *self)
{  
  fmat_upload_from_index(self, 0, 0, fmat_get_m(self) * fmat_get_n(self));
}

void
fmat_upload(fmat_t *self)
{
  fmat_upload_size(self);
  fmat_upload_data(self);
}

/********************************************************************
 ********************************************************************
 ********************************************************************
 ***
 ***   U S E R   M E T H O D S
 ***
 ********************************************************************
 ********************************************************************
 ********************************************************************/


/** @defgroup fmatobj fmat object messages
 *  @ingroup  dataobj
 *
 *  Messages that can be sent to or invoked on an fmat object.
 *
 *  @{
 */


/** set from fmat.
 *
 * @fn set(fmat matrix);
 *
 * set dimension and values from given fmat
 *
 * @param matrix fmat to be copied
 */
static fts_method_status_t
fmat_set_from_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  fmat_copy((fmat_t *)fts_get_object(at), self);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}


static fts_method_status_t
fmat_set_from_fvec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fts_object_t *obj = fts_get_object(at);
  int vec_size, vec_stride;
  float *vec;
  float *ptr;
  int i, j;
  
  fmat_or_slice_vector(obj, &vec, &vec_size, &vec_stride);
  fmat_reshape(self, vec_size, 1);
  ptr = fmat_get_ptr(self);
  
  for(i=0, j=0; i<vec_size; i++, j+=vec_stride)
    ptr[i] = vec[j];
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_set_from_bpf(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  bpf_t *bpf = (bpf_t *)fts_get_object(at);
  int size = bpf_get_size(bpf);
  float *ptr;
  int i, j;
  
  fmat_reshape(self, size, 2);
  ptr = fmat_get_ptr(self);
  
  for(i=0, j=0; i<size; i++, j+=2)
  {
    ptr[j] = bpf_get_time(bpf, i);
    ptr[j + 1] = bpf_get_value(bpf, i);
  }
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_set_from_ivec(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  ivec_t *ivec = (ivec_t *)fts_get_object(at);
  int *iptr = ivec_get_ptr(ivec);
  int size = ivec_get_size(ivec);
  float *ptr;
  int i;
  
  fmat_reshape(self, size, 1);
  
  ptr = fmat_get_ptr(self);
  
  for(i=0; i<size; i++)
    ptr[i] = (float)iptr[i];

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_set_from_list(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
  {
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int i = fts_get_number_int(at);
    int j = fts_get_number_int(at + 1);
    int size = m * n;
    int onset;
    
    ac -= 2;
    at += 2;

    if(i > m)
      i = m;
    
    if(j > n)
      j = n;

    while(i < 0)
      i += m;
    
    while(j < 0)
      j += n;

    onset = i * n + j;
    if(onset + ac > size)
      ac = size - onset;
        
    fmat_set_from_atoms(self, onset, 1, ac, at);

    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  
  return fts_ok;
}

static fts_method_status_t
fmat_set_row(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 1 && fts_is_number(at))
  {
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int row = fts_get_number_int(at);
    
    /* skip index argument */
    ac--;
    at++;

    if(row >= 0)
    {
      if(row >= m)
      {
        fmat_set_m(self, row + 1);
        m = row + 1;
      }
      
      if(fts_is_number(at))
      {
        /* clip to # of cloumns */
        if(ac > n)
          ac = n;
        
        fmat_set_from_atoms(self, row * n, 1, ac, at);
      }
      else if(fts_is_object(at))
      {
        fts_object_t *obj = fts_get_object(at);
        int vec_size, vec_stride;
        float *vec;
        
        if(fmat_or_slice_vector(obj, &vec, &vec_size, &vec_stride))
        {
          float *ptr = fmat_get_ptr(self) + row * n;
          int i, j;
                    
          if(vec_size > n)
            vec_size = n;
          
          for(i=0, j=0; i<vec_size; i++, j+=vec_stride)
            ptr[i] = vec[j];
        }
      }    
    }
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_set_col(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 1 && fts_is_number(at))
  {
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int col = fts_get_number_int(at);
        
    /* skip index argument */
    ac--;
    at++;
    
    if(col >= 0)
    {
      if(col >= n)
      {
        fmat_set_n(self, col + 1);
        n = col + 1;
      }
      
      if(fts_is_number(at))
      {
        /* clip to # of rows */
        if(ac > m)
          ac = m;
        
        fmat_set_from_atoms(self, col, n, ac, at);
      }
      else if(fts_is_object(at))
      {
        fts_object_t *obj = fts_get_object(at);
        int vec_size, vec_stride;
        float *vec;
        
        if(fmat_or_slice_vector(obj, &vec, &vec_size, &vec_stride))
        {
          float *ptr = fmat_get_ptr(self) + col;
          int i, j;
          
          if(vec_size > m)
            vec_size = m;
          
          for(i=0, j=0; i<vec_size*n; i+=n, j+=vec_stride)
            ptr[i] = vec[j];
        }
      }
    }
  }
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

/* used by fvec, too! */
fts_method_status_t
fmat_fill_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 0 && fts_is_number(at))
    fmat_set_const(self, (float) fts_get_number_float(at));

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_fill_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  if(ac > 0)
  {
    if(fts_is_number(at))
    {
      if(ac > m * n)
        ac = m * n;

      for(i=0; i<ac; i++)
      {
        if(fts_is_number(at + i))
          ptr[i] = (float)fts_get_number_float(at + i);
        else
          ptr[i] = 0.0;
      }
      
      for(; i<m*n; i++)
        ptr[i] = ptr[i % ac];
    }
    else if(fts_is_a(at, expr_class))
    {
      expr_t *expr = (expr_t *)fts_get_object(at);
      int m = fmat_get_m(self);
      int n = fmat_get_n(self);
      float *ptr = fmat_get_ptr(self);
      fts_hashtable_t locals;
      fts_atom_t key_self, key_row, key_col, value;
      fts_atom_t ret;
      int i, j;
      
      fts_hashtable_init(&locals, FTS_HASHTABLE_SMALL);
      
      fts_set_symbol(&key_self, fts_s_self);
      fts_set_object(&value, self);
      fts_hashtable_put(&locals, &key_self, &value);
      
      fts_set_symbol(&key_row, fts_s_row);
      fts_set_symbol(&key_col, fts_s_col);

      for(i=0; i<m; i++)
      {
        fts_set_int(&value, i);
        fts_hashtable_put(&locals, &key_row, &value);
        
        for(j=0; j<n; j++)
        {
          fts_set_int(&value, j);
          fts_hashtable_put(&locals, &key_col, &value);

          expr_evaluate(expr, &locals, ac - 1, at + 1, &ret);
          
          if(fts_is_number(&ret))
            ptr[i * n + j] = fts_get_number_float(&ret);
          else
            ptr[i * n + j] = 0.0;
        }
      }
      
      fts_hashtable_destroy(&locals);
    }
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_fill_zero(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr = fmat_get_ptr(self);
  int onset = 0;
  int range = m;
  int i;
  
  if(ac > 0 && fts_is_number(at))
    onset = fts_get_number_int(at);
  
  if(ac > 1 && fts_is_number(at + 1))
    range = fts_get_number_int(at + 1);
  
  if(onset + range > m)
    range = m - onset;
  
  for(i=onset*n; i<(onset+range)*n; i++)
    ptr[i] = 0.0;
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_fill_random(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *values = self->values;
  int size = self->m * self->n;
  double lower, upper;
  int i;
  
  switch(ac)
  {
    case 0:
      lower = 0.0;
      upper = 1.0;
      break;
      
    case 1:
      lower = 0.0;
      upper = 0.0;
      
      if(fts_is_number(at))
        upper = fts_get_number_float(at);
      break;
      
    default:
      lower = 0.0;
      upper = 0.0;
      
      if(fts_is_number(at))
        lower = fts_get_number_float(at);

      if(fts_is_number(at + 1))
        upper = fts_get_number_float(at + 1);
      break;
  }
  
  for(i=0; i<size; i++)
    values[i] = (float)fts_random_range(lower, upper);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
_fmat_get_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fts_atom_t a[2];
  
  fts_set_int(a + 0, fmat_get_m(self));
  fts_set_int(a + 1, fmat_get_n(self));
  fts_set_object(ret, fts_object_create(fts_tuple_class, 2, a));
  
  return fts_ok;
}

static fts_method_status_t
_fmat_get_m(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fts_atom_t a;
  
  fts_set_int(&a, fmat_get_m(self));
  
  *ret = a;
  
  return fts_ok;
}

static fts_method_status_t
_fmat_get_n(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fts_atom_t a;
  
  fts_set_int(&a, fmat_get_n(self));
  
  *ret = a;
  
  return fts_ok;
}

static fts_method_status_t
_fmat_set_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = 0;
  int n = 0;
  
  if(ac > 1 && fts_is_number(at) && fts_is_number(at + 1))
  {
    m = fts_get_number_int(at);
    n = fts_get_number_int(at + 1);
    
    if(m >= 0 && n >= 0)
    {
      fmat_set_size(self, m, n);
      
      fts_object_changed(o);
      fts_set_object(ret, o);
    }
  }  
  
  return fts_ok;
}

static fts_method_status_t
_fmat_set_m(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fts_get_number_int(at);

  if(m >= 0)
  {    
    fmat_set_m(self, m);
  
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  
  return fts_ok;
}

static fts_method_status_t
_fmat_set_n(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int n = fts_get_number_int(at);
  
  if(n >= 0)
  {
    fmat_set_n(self, n);
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  
  return fts_ok;
}

/* called by get element message */
static fts_method_status_t
_fmat_get_element(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i = 0;
  int j = 0;
  
  if (m == 0  ||  n == 0)
    fts_set_float(ret, 0);        /* empty matrix: no error, just return 0 */
  else
  {
#if USE_LINEAR_INDEXING
    switch (ac)
    {
      default:
      case 2:
        if (fts_is_number(at))
	  i = fts_get_number_int(at);
        
        if (fts_is_number(at + 1))
	  j = fts_get_number_int(at  + 1);
	
	while (i < 0)
	  i += m;
  
	while (j < 0)
	  j += n;
  
	if (i >= m)
	  i = m - 1;
  
	if (j >= n)
	  j = n - 1;
  
  	fts_set_float(ret, fmat_get_element(self, i, j));
      break;
      
      case 1:  /* linear indexing of unrolled matrix */
        if (fts_is_number(at))
	  j = fts_get_number_int(at);

 	while (j < 0)
	  j += n * m;
  
	if (j >= n * m)
	  j = n * m - 1;
	
	fts_set_float(ret, fmat_get_element(self, 0, j));
      break;
      
      case 0:
        fts_return_float(fmat_get_element(self, 0, 0));
      break;
    }
#else
    if (ac > 0  &&  fts_is_number(at))
      i = fts_get_number_int(at);

    if (ac > 1  &&  fts_is_number(at + 1))
      j = fts_get_number_int(at  + 1);
  
    while (i < 0)
      i += m;
  
    while (j < 0)
      j += n;
  
    if (i >= m)
      i = m - 1;
  
    if (j >= n)
      j = n - 1;
  
    fts_set_float(ret, fmat_get_element(self, i, j));
#endif
  }
  
  return fts_ok;
}

static fts_method_status_t
_fmat_get_interpolated(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  double r = 0.0;
  
  if(ac > 0)
  {
    int i = 0;
    int j = 0;
    double i_frac = 0.0;
    double j_frac = 0.0;
    
    if(fts_is_int(at))
    {
      i = fts_get_int(at);
      
      if(i < 0)
        i = 0;
      else if(i >= m)
        i = m - 1;
    }
    else if(fts_is_float(at))
    {
      double i_arg = fts_get_float(at);
      
      if(i_arg > 0.0)
      {
        i = (int)i_arg;
        
        if(i >= m - 1)
          i = m - 1;
        else
          i_frac = i_arg - floor(i_arg);
      }
    }
    
    if(ac > 1 && fts_is_number(at + 1))
    {
      if(fts_is_int(at + 1))
      {
        j = fts_get_int(at + 1);
        
        if(j < 0)
          j = 0;
        else if(j >= m)
          j = m - 1;
      }
      else if(fts_is_float(at + 1))
      {
        double j_arg = fts_get_float(at + 1);
        
        if(j_arg > 0.0)
        {
          j = (int)j_arg;
          
          if(j >= m - 1)
            j = m - 1;
          else
            j_frac = j_arg - floor(j_arg);
        }
      }
    }
    
    if(i_frac != 0.0)
    {
      r = (1.0 - i_frac) * ptr[i * n + j] + i_frac * ptr[(i + 1) * n + j];
      
      if(j_frac != 0.0)
      {
        double g = (1.0 - i_frac) * ptr[i * n + j + 1] + i_frac * ptr[(i + 1) * n + j + 1];
        
        r = (1.0 - j_frac) * r + j_frac * g;
      }
    }
    else if(j_frac != 0.0)
      r = (1.0 - j_frac) * ptr[i * n + j] + j_frac * ptr[i * n + j + 1];
    else
      r = ptr[i * n + j];
  }
  
  fts_set_float(ret, r);
  
  return fts_ok;
}


static fts_method_status_t
fmat_get_tuple(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);
  fts_atom_t *atoms;
  int i;

  fts_tuple_set_size(tuple, m * n);
  atoms = fts_tuple_get_atoms(tuple);
  
  for(i=0; i<m*n; i++)
    fts_set_float(atoms + i, ptr[i]);

  fts_set_object(ret, (fts_object_t *)tuple);
  
  return fts_ok;
}

static fts_method_status_t
fmat_pick_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int onset = 0;
  int size = 0;
  fmat_t *src;
  float *src_ptr;
  int src_m, src_n;
  float *ptr;
  int i;

  switch(ac)
  {
    default:
    case 3:
      if(fts_is_number(at + 2))
      {
        size = fts_get_number_int(at + 2);
        
        if(size < 0)
          size = 0;
      }
    case 2:
      if(ac > 1 && fts_is_number(at + 1))
      {
        onset = fts_get_number_int(at + 1);
        
        if(onset < 0)
          onset = 0;
      }
    case 1:
      if(fts_is_object(at))
      {
        src = (fmat_t *)fts_get_object(at);
        src_m = fmat_get_m(src);
        src_n = fmat_get_n(src);
        src_ptr = fmat_get_ptr(src) + onset * src_n;
        break;
      }
    case 0:
      return fts_ok;  
  }

  if(onset + size > src_m)
    size = src_m - onset;

  fmat_reshape(self, size, src_n);
  ptr = fmat_get_ptr(self);
  
  for(i=0; i<size; i++)
  {
    int j;
    
    for(j=0; j<src_n; j++)
      ptr[j] = src_ptr[j];
    
    ptr += src_n;
  }
  
  fts_set_object(ret, o);
  
  return fts_ok;
}



/* fvec constructors: get vector object that references fmat */

static fts_method_status_t
fmat_get_col(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  fvec_t *fvec = fvec_create(self, fvec_type_column, ac, at);
  
  fts_set_object(ret, (fts_object_t *)fvec);
  
  return fts_ok;
}


static fts_method_status_t
fmat_get_row(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fvec_t *fvec = fvec_create(self, fvec_type_row, ac, at);

  fts_set_object(ret, (fts_object_t *)fvec);
  
  return fts_ok;
}


static fts_method_status_t
fmat_get_diag(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fvec_t *fvec = fvec_create(self, fvec_type_diagonal, ac, at);
  
  fts_set_object(ret, (fts_object_t *)fvec);
  
  return fts_ok;
}


/** append a row of atoms, augment m, clip row to n 
* 
* @method append
* @param  atoms   row of atoms to append, will be clipped to width of matrix
*/
static fts_method_status_t
fmat_append_row_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr;
  
  /* clip to row */
  if (ac > n)
    ac = n;
  
  /* add space, append data */
  fmat_reshape(self, m + 1, n);
  ptr = fmat_get_ptr(self) + m * n;
  
  if(ac > 0)
  {
    int i;
    
    for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
        ptr[i] = fts_get_number_float(at + i);
      else
        ptr[i] = 0.0;
    }
    for(i=ac; i<n; i++)
      ptr[i] = 0.0;
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_append_row_slice(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr;
  fts_object_t *append_fmat = fts_get_object(at);
  float *append_ptr;
  int append_size, append_stride;
  int i, j;
  
  fmat_or_slice_vector(append_fmat, &append_ptr, &append_size, &append_stride);
  
  if(append_size > n)
    append_size = n;
  
  fmat_reshape(self, m + 1, n);
  ptr = fmat_get_ptr(self) + m * n;
  
  for(i=0, j=0; i<append_size; i++, j+=append_stride)
    ptr[i] = append_ptr[j];
  
  for(; i<n; i++)
    ptr[i] = 0.0;    
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_append_row_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr;
  fmat_t *append_fmat = (fmat_t *)fts_get_object(at);
  int append_m = fmat_get_m(append_fmat);
  int append_n = fmat_get_n(append_fmat);
  float *append_ptr = fmat_get_ptr(append_fmat);
  int i;
  
  if(append_n > n)
    n = append_n;
  
  fmat_reshape(self, m + append_m, n);
  ptr = fmat_get_ptr(self) + m * n;
  
  for(i=0; i<append_m; i++)
  {
    int j;
    
    for(j=0; j<append_n; j++)
      ptr[j] = append_ptr[j];
    
    for(; j<n; j++)
      ptr[j] = 0.0;
    
    append_ptr += append_n;
    ptr += n;
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}


/** insert @p num rows of zeros at row @p pos
 *  may insert num rows behind last row m --> append num rows
 *
 * @method insert
 * @param  int: pos    index of row where to insert @p num empty rows, default 0
 * @param  int: num    number of rows to insert, default 1
 * 
 * TODO: give data, clip rows to n, you can use mat_set_row_elements for now
 * @method insert
 * @param  int: pos    index of row where to insert
 * @param  tuples: atoms  list of tuples of rows of atoms to append, 
 *                     will be clipped to width of matrix
 */
static fts_method_status_t
fmat_insert_rows(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  float *newptr;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int pos = 0;        // row position at which to insert 
  int numrows = 1;    // number of rows to insert
  int num, tomove, i;

  /* method returns this object in any case */
  fts_set_object(ret, o);

  if (ac > 0  &&  fts_is_number(at))
    pos = fts_get_number_int(at);

  if (pos < 0)        pos = 0;
  else if (pos > m)   pos = m;

  if (ac > 1  &&  fts_is_number(at+1))
    numrows = fts_get_number_int(at+1) ;
  
  if (numrows <= 0)
    return fts_ok; 

  fmat_set_size(self, m + numrows, n);

  // move rows
  newptr = fmat_get_ptr(self) + n * pos;
  num    = n * numrows;         // atoms to insert 
  tomove = n * (m - pos);       // atoms to move 

  if (pos < m)   
    memmove(newptr + num, newptr, tomove * sizeof(float));

  for (i = 0; i < num; i++)
    self->values[n*pos + i] = 0.0;

  fts_object_changed(o);
  
  return fts_ok;
}


/** insert @p num columns of zeros at row @p pos
*  may insert num rows behind last row m --> append num rows
*
* @method insert
* @param  int: pos    index of row where to insert @p num empty rows, default 0
* @param  int: num    number of rows to insert, default 1
* 
* TODO: give data, clip rows to n, you can use mat_set_row_elements for now
* @method insert
* @param  int: pos    index of row where to insert
* @param  tuples: atoms  list of tuples of rows of atoms to append, 
*                      will be clipped to width of matrix
*/
static fts_method_status_t
fmat_insert_columns(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int pos = 0;        // col position at which to insert
  int numcols = 1;    // number of columns to insert
  int tomove, i, j, start, new_n;
  
  /* method returns this object in any case */
  fts_set_object(ret, o);

  if (ac > 0  &&  fts_is_number(at))
    pos = fts_get_number_int(at);
  
  if(pos < 0)   pos = 0;
  else if(pos > n) pos = n;
  
  if (ac > 1  &&  fts_is_number(at+1))
    numcols = fts_get_number_int(at+1) ;
  
  if(numcols <= 0)
    return fts_ok; 
  
  fmat_set_size(self, m, n + numcols);
  new_n = n+numcols;

  start = (m-1)*new_n + pos;
  tomove = new_n-pos-numcols;
  
  for(i = 0; i < m; i++)
  {
    for(j=tomove-1; j >= 0; j--)
      self->values[start+j+numcols] = self->values[start+j];
    for(j = 0; j < numcols; j++)
      self->values[start + j] = 0.0;
    start = start - new_n;
  }
  
  fts_object_changed(o);
  
  return fts_ok;
}


static fts_method_status_t
fmat_delete_columns(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int pos = 0;        
  int numcols = 1;      // number of columns to delete
  int tomove, i, j, start;
  
  /* method returns this object in any case */
  fts_set_object(ret, o);

  if (ac > 0  &&  fts_is_number(at))
    pos = fts_get_number_int(at);
  
  if(pos < 0)   pos = 0;
  else if(pos > n) pos = n;
  
  if (ac > 1  &&  fts_is_number(at+1))
    numcols = fts_get_number_int(at+1) ;
  
  if(numcols <= 0)
    return fts_ok; 
  
  start = pos + numcols;
  tomove = n-pos-numcols;
  
  for(i = 0; i < m; i++)
  {
    for(j=0; j < tomove; j++)
      self->values[start-numcols+j] = self->values[start+j];
    start = start + n;
  }
  
  fmat_set_size(self, m, n - numcols);
  
  fts_object_changed(o);
  
  return fts_ok;
}


/** delete @p num rows of atoms 
 * 
 * @method delete
 * @param  int: pos    index of row where to delete @p num rows, default 0
 * @param  int: num    number of rows to delete, default 1
 */
static fts_method_status_t
fmat_delete_rows(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  float  *killptr;
  int     m = fmat_get_m(self);
  int     n = fmat_get_n(self);
  int     pos = 0;
  int     numrows = 1;
  int     num, tomove;

  /* method returns this object in any case */
  fts_set_object(ret, o);

  if (ac > 0  &&  fts_is_number(at))
    pos = fts_get_number_int(at);
    
  if (pos <  0)  
    pos = 0;
  else if (pos >= m)
    return fts_ok;    

  if (ac > 1  &&  fts_is_number(at+1))
    numrows = fts_get_number_int(at+1);

  if (numrows <= 0)
    return fts_ok;
  else if (numrows >  m - pos)
    numrows = m - pos;
  
  killptr = fmat_get_ptr(self) + n * pos;
  num     = n * numrows;        // number of elements to delete
  tomove  = n * (m - pos);      // number of elements to move 

  memmove(killptr, killptr + num, tomove * sizeof(float));
  self->m -= numrows; 

  fts_object_changed(o);
  
  return fts_ok;
}


/******************************************************************************
 *
 *  functions, i.e. methods that return a value but don't change the object
 *
 */

static fts_method_status_t
fmat_get_min(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  const fmat_t *self = (fmat_t *) o;
  const int size = fmat_get_m(self) * fmat_get_n(self);
  
  if(size > 0)
  {
    const float *p = fmat_get_ptr(self);
    float min = p[0];
    int i;

    for (i = 1; i < size; i++)
      if (p[i] < min)
        min = p[i];
    
    fts_set_float(ret, min);
  }
  
  return fts_ok;
}

static fts_method_status_t
fmat_get_max(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  const fmat_t *self = (fmat_t *) o;
  const int size = fmat_get_m(self) * fmat_get_n(self);

  if(size > 0)
  {
    const float *p = fmat_get_ptr(self);
    float max = p[0]; /* start with first element */
    int i;
    
    for (i = 1; i < size; i++)
      if (p[i] > max)
        max = p[i];
    
    fts_set_float(ret, max);
  }
  
  return fts_ok;
}

static fts_method_status_t
fmat_get_absmax(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  const fmat_t *self = (fmat_t *) o;
  const int size = fmat_get_m(self) * fmat_get_n(self);

  if(size > 0)
  {
    const float *p = fmat_get_ptr(self);
    float max = p[0]; /* start with first element */
    int i;
    
    for (i = 1; i < size; i++)
      if (fabsf(p[i]) > max)
        max = fabsf(p[i]);
    
    fts_set_float(ret, max);
  }
  
  return fts_ok;
}

static fts_method_status_t
fmat_get_sum(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  const fmat_t *self = (fmat_t *) o;
  const int size = fmat_get_m(self) * fmat_get_n(self);
  const float *p = fmat_get_ptr(self);
  double sum = 0.0;
  int i;
  
  for (i=0; i<size; i++)
    sum += p[i];
  
  fts_set_float(ret, sum);
  
  return fts_ok;
}


static fts_method_status_t
fmat_get_prod (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  const fmat_t *self = (fmat_t *) o;
  const int     size = fmat_get_m(self) * fmat_get_n(self);
  const float  *p    = fmat_get_ptr(self);
  double        prod = 1.0;
  int           i;
  
  for (i = 0; i < size; i++)
    prod *= p[i];
  
  fts_set_float(ret, prod);
  
  return fts_ok;
}


static fts_method_status_t
fmat_get_mean(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  const fmat_t *self = (fmat_t *) o;
  const int size = fmat_get_m(self) * fmat_get_n(self);
  
  if(size > 0)
  {
    const float *p = fmat_get_ptr(self);
    double sum = 0.0;
    int i;
    
    for (i=0; i<size; i++)
      sum += p[i];
    
    fts_set_float(ret, sum / (double)size);
  }
  else
    fts_set_float(ret, 0.0);
  
  return fts_ok;
}

static fts_method_status_t
fmat_get_zc(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  float *p = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);

  if(m * n > 0)
  {
    int zc = 0;
    int i, j;

    for(j=0; j<n; j++)
    {
      float prev = p[j];
      
      for(i=n; i<m*n; i+=n)
      {
        float f = p[i + j];
        
        if(f != 0.0)
        {
          zc += (prev * f) < 0.0;
          prev = f;
        }
      }
    }
    
    fts_set_int(ret, zc);
  }
  
  return fts_ok;
}

/******************************************************************************
 *
 *  element arithmetics
 *
 */
static fts_method_status_t
fmat_elem_add(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1) && fts_is_number(at + 2))
  {
    float *ptr = fmat_get_ptr(self);
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int i = fts_get_number_int(at);
    int j = fts_get_number_int(at + 1);
    float op = fts_get_number_float(at + 2);
    
    ac -= 2;
    at += 2;
    
    if(i > m)
      i = m;
    
    if(j > n)
      j = n;
    
    while(i < 0)
      i += m;
    
    while(j < 0)
      j += n;
    
    ptr[i * n + j] += op;
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  
  return fts_ok;
}

/******************************************************************************
 *
 *  real arithmetics
 *
 */

static fts_method_status_t
fmat_add_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);

  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;

    for(i=0; i<size; i++)
      l[i] += r[i];
  
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "add");
  
  return fts_ok;
}

static fts_method_status_t
fmat_add_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] += r;
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_sub_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] -= r[i];
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "sub");
  
  return fts_ok;
}

static fts_method_status_t
fmat_sub_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] -= r;
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_mul_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] *= r[i];
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "mul");
  
  return fts_ok;
}

static fts_method_status_t
fmat_mul_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] *= r;
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_div_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] /= r[i];

    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "div");
  
  return fts_ok;
}

static fts_method_status_t
fmat_div_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] /= r;
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_bus_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = r[i] - l[i];

    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "bus");
  
  return fts_ok;
}

static fts_method_status_t
fmat_bus_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = r - p[i];
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_vid_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = r[i] / l[i];
  
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "vid");
  
  return fts_ok;
}

static fts_method_status_t
fmat_vid_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = r / p[i];
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

/******************************************************************************
 *
 *  real comparison
 *
 */

static fts_method_status_t
fmat_eq_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] == r[i]);
 
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "eq");
  
  return fts_ok;
}

static fts_method_status_t
fmat_eq_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] == r);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_ne_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] != r[i]);
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "ne");
  
  return fts_ok;
}

static fts_method_status_t
fmat_ne_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] != r);

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_gt_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);

  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] > r[i]);
  
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "gt");
  
  return fts_ok;
}

static fts_method_status_t
fmat_gt_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] > r);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_ge_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] >= r[i]);

    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "ge");
  
  return fts_ok;
}

static fts_method_status_t
fmat_ge_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] >= r);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_lt_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] < r[i]);
  
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "lt");
  
  return fts_ok;
}

static fts_method_status_t
fmat_lt_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] < r);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_le_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(m > fmat_get_m(right))
    m = fmat_get_m(right);
  
  if(fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] <= r[i]);
  
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_dimensions(self, right, "le");
  
  return fts_ok;
}

static fts_method_status_t
fmat_le_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] <= r);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}


/******************************************************************************
 *
 *  misc math funs
 *
 */
static fts_method_status_t
fmat_abs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  for(i=0; i<m*n; i++)
    ptr[i] = fabsf(ptr[i]);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_logabs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  for(i=0; i<m*n; i++)
  {
    float abs = fabsf(ptr[i]);
    
    if(abs > LOG_ARG_MIN)
      ptr[i] = logf(abs);
    else
      ptr[i] = LOG_MIN;
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_log(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  for(i=0; i<m*n; i++)
  {
    float f = ptr[i];
    
    if(f > LOG_ARG_MIN)
      ptr[i] = logf(f);
    else
      ptr[i] = LOG_MIN;
  }

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_exp(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  for(i=0; i<m*n; i++)
    ptr[i] = expf(ptr[i]);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_sqrabs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  for(i=0; i<m*n; i++)
    ptr[i] *= ptr[i];

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_sqrt(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  for(i=0; i<m*n; i++)
    ptr[i] = sqrtf(ptr[i]);

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

/******************************************************************************
*
*  complex conversion
*
*/
static fts_method_status_t
fmat_convert_rect(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr = fmat_get_ptr(self);
  int i;
  
  if(n == 2)
  {
    for(i=0; i<2*m; i+=2)
    {
      float mag = ptr[i];
      float arg = ptr[i + 1];
      
      ptr[i] = mag * cosf(arg); /* real part */
      ptr[i + 1] = mag * sinf(arg); /* imaginary part */
    }
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "rect");
  
  return fts_ok;
}

static fts_method_status_t
fmat_convert_polar(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr = fmat_get_ptr(self);
  int i;
  
  if(n == 2)
  {
    for(i=0; i<2*m; i+=2)
    {
      float re = ptr[i];
      float im = ptr[i + 1];
#ifdef WIN32
      ptr[i] = hypot((double)re, (double)im);
#else
      ptr[i] = hypot(re, im);
#endif
      ptr[i + 1] = atan2f(im, re);
    }
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "polar");
  
  return fts_ok;
}

/******************************************************************************
 *
 *  complex arithmetics and math
 *
 */
static fts_method_status_t
fmat_cmul_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  float *r = fmat_get_ptr(right);
  
  if(m > right_m)
    m = right_m;
  
  if(n == 1)
  {
    if(right_n == 1)
    {
      /* real * real */
      float *l = fmat_get_ptr(self);
      int i;
      
      for(i=0; i<m; i++)
        l[i] *= r[i];
    }
    else if(right_n == 2)
    {
      /* real * complex */
      float *l = fmat_get_ptr(self);
      int i, j;

      fmat_reshape(self, m, 2);
      l = fmat_get_ptr(self);
      
      for(i=m-1, j=(m-1)*2; i>=0; i--, j-=2)
      {
        float re = l[i];
        
        l[j] = re * r[j];
        l[j + 1] = re * r[j + 1];
      }
    }
    else
      fmat_error_complex(right, "cmul");
  }
  else if(n == 2)
  {
    if(right_n == 1)
    {
      /* complex * real */
      float *l = fmat_get_ptr(self);
      int i, j;

      for(i=0, j=0; i<m; i++, j+=2)
      {
        l[j] *= r[i];
        l[j + 1] *= r[i];
      }
    }
    else if(right_n == 2)
    {
      /* complex * complex */
      float *l = fmat_get_ptr(self);
      int i;

      for(i=0; i<m*2; i+=2)
      {
        float re = l[i];
        float im = l[i + 1];
        
        l[i] = re * r[i] - im * r[i + 1];
        l[i + 1] = re * r[i + 1] + im * r[i];
      }
    }
    else
      fmat_error_complex(right, "cmul");
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "cmul");
  
  return fts_ok;
}

static fts_method_status_t
fmat_cmul_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float r = fts_get_number_float(at);
  float *l = fmat_get_ptr(self);
  int i;
  
  if(n <= 2)
  {
    for(i=0; i<m*n; i++)
      l[i] *= r;              
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "cmul");
  
  return fts_ok;
}


static fts_method_status_t
fmat_cabs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);

  if(n == 2)
  {
    int i, j;
    
    for(i=0, j=0; i<m; i++, j+=2)
    {
      float re = ptr[j];
      float im = ptr[j + 1];
      
      ptr[i] = sqrtf(re * re + im * im);
    }
    
    fmat_reshape(self, m, 1);
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "cabs");
  
  return fts_ok;
}

static fts_method_status_t
fmat_csqrabs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(n == 2)
  {
    int i, j;
    
    for(i=0, j=0; i<m; i++, j+=2)
    {
      float re = ptr[j];
      float im = ptr[j + 1];
      
      ptr[i] = re * re + im * im;
    }
    
    fmat_reshape(self, m, 1);
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "csqrabs");
  
  return fts_ok;
}

static fts_method_status_t
fmat_clogabs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(n == 2)
  {
    int i, j;
  
    for(i=0, j=0; i<m; i++, j+=2)
    {
      float re = ptr[j];
      float im = ptr[j + 1];
      float abs2 = re * re + im * im;
      
      if(abs2 > LOG_ARG_MIN)
        ptr[i] = 0.5 * logf(abs2);
      else
        ptr[i] = 0.5 * LOG_MIN;
    }
    
    fmat_reshape(self, m, 1);
  
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "clogabs");
  
  return fts_ok;
}

static fts_method_status_t
fmat_clog(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);

  if(n == 2)
  {
    int i;
    
    for(i=0; i<2*m; i+=2)
    {
      float re = ptr[i];
      float im = ptr[i + 1];
      float abs2 = re * re + im * im;
      
      if(abs2 > LOG_ARG_MIN)
        ptr[i] = 0.5 * logf(abs2);
      else
        ptr[i] = 0.5 * LOG_MIN;

      ptr[i + 1] = (float)atan2(im, re);
    }

    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "clog");
  
  return fts_ok;
}

static fts_method_status_t
fmat_cexp(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(n == 2)
  {
    int i;
  
    for(i=0; i<2*m; i+=2)
    {
      float mag = expf(ptr[i]);
      float arg = ptr[i + 1];
      
      ptr[i] = mag * cosf(arg);
      ptr[i + 1] = mag * sinf(arg);
    }

    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "cexp");
  
  return fts_ok;
}

/******************************************************************************
*
*  FFT
*
*/

static fts_method_status_t
fmat_fft(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);  
  
  if(n == 1)
  {
    /* real FFT */
    int fft_size = fts_get_fft_size(m);
    float *fft_ptr;
    int i;
    
    fmat_reshape(self, fft_size/2 + 1, 2);
    
    fft_ptr = fmat_get_ptr(self);
    
    /* zero padding */
    for(i=m; i<fft_size; i++)
      fft_ptr[i] = 0.0;
    
    fts_rfft_inplc(fft_ptr, fft_size);
    fft_ptr[fft_size] = fft_ptr[1];
    fft_ptr[fft_size + 1] = fft_ptr[1] = 0.0;
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else if(n == 2)
  {
    /* complex FFT */
    unsigned int fft_size = fts_get_fft_size(m);
    complex *fft_ptr;
    unsigned int i;
    
    fmat_reshape(self, fft_size, 2);
    fft_ptr = (complex *)fmat_get_ptr(self);
    
    /* zero padding */      
    for(i=m; i<fft_size; i++)
      fft_ptr[i].re = fft_ptr[i].im = 0.0;
    
    fts_cfft_inplc(fft_ptr, fft_size);
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "fft");
  
  return fts_ok;
}

static fts_method_status_t
fmat_ifft(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(n == 2)
  {
    int fft_size = fts_get_fft_size(m);
    int i;
    
    /* zero padding */      
    for(i=2*m; i<fft_size; i++)
      ptr[i] = 0.0;
    
    fts_cifft_inplc((complex *)ptr, fft_size);
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "ifft");
  
  return fts_ok;
}

static fts_method_status_t
fmat_rifft(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(n == 2)
  {
    int fft_size = fts_get_fft_size(2 * (m - 1));
    int i;
    
    if(m > fft_size / 2)
      ptr[1] = ptr[fft_size];
    else
      ptr[1] = 0.0;
    
    /* zero padding */      
    for(i=2*m; i<fft_size; i++)
      ptr[i] = 0.0;
    
    fts_rifft_inplc(ptr, fft_size);
    
    fmat_reshape(self, fft_size, 1);
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fmat_error_complex(self, "rifft");
  
  return fts_ok;
}

/******************************************************************************
*
*  vector and matrix multiplication
*
*/

static fts_method_status_t
fmat_xmul_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr = fmat_get_ptr(self);
  fmat_t *in = (fmat_t *)fts_get_object(at);
  int in_m = fmat_get_m(in);
  int in_n = fmat_get_n(in);
  float *in_row = fmat_get_ptr(in);
  
  if(in_n == m)
  {
    float *res = alloca(sizeof(float) * in_m * n);
    float *res_row = res;
    int i_row, i_col, i, j;
    
    for(i_row=0; i_row<in_m; i_row++)
    {
      float *col = ptr;
      
      for(i_col=0; i_col<n; i_col++)
      {
        float f = 0.0;
        
        for(i=0, j=0; i<m; i++, j+=n)
          f += in_row[i] * col[j];
        
        res_row[i_col] = f;
        col++;
      }
      
      /* next row */
      in_row += in_n;
      res_row += n;
    }
    
    fmat_set_m(self, in_m);
    
    for(i=0; i<in_m*n; i++)
      ptr[i] = res[i]; 
    
    fts_object_changed(o);
    fts_set_object(ret, o);
  }
  else
    fts_object_error((fts_object_t *)self, "xmul: can't multiply matrix %d x %d with matrix %d x %d", m, n, in_m, in_n);
  
  return fts_ok;
}

static fts_method_status_t
fmat_get_dot(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(n == 1 && fmat_get_n(right) == 1)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    float sum = 0.0;
    int i;
    
    if(m > fmat_get_m(right))
      m = fmat_get_m(right);
    
    for(i=0; i<m; i++)
      sum += l[i] * r[i];
    
    fts_set_float(ret, sum);
  }
  else if(n == 1)
    fmat_error_dimensions(self, right, "dot");
  else
    fts_object_error(o, "dot: matrices must be column vectors");
  
  return fts_ok;
}

/******************************************************************************
*
*  misc
*
*/

static fts_method_status_t
fmat_clip(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float low = ABS_MIN;
  float high = ABS_MAX;
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
  
  for(i=0; i<m*n; i++)
  {
    float f = ptr[i];
    
    if(f > high)
      ptr[i] = high;
    else if(f < low)
      ptr[i] = low;
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_normalize(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int size = fmat_get_m(self) * fmat_get_n(self); 
  float *ptr = fmat_get_ptr(self);
  
  if(size > 0)
  {
    float max = ptr[0];
    int i;
    
    /* find maximum */
    for(i=1; i<size; i++)
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
      
      for(i=0; i<size; i++)
        ptr[i] *= scale;
    }
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

/********************************************************************
 *
 *  row order operations
 *
 */

static fts_method_status_t
fmat_reverse(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self); 
  float *rtp = ptr + m * n;
  int i;
  
  for(i=0; i<m/2; i++)
  {
    int j;
    
    rtp -= n;
    
    /* swap rows ptr (front) and rtp (back) */
    for (j = 0; j < n; j++)
    {
      float f = ptr[j];
      ptr[j]  = rtp[j];
      rtp[j]  = f;
    }
    
    ptr += n;
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_rotate(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self); 
  int n = fmat_get_n(self); 
  int size = m * n; 
  float *ptr = fmat_get_ptr(self);
  
  if(size > 1)
  {
    int shift = n;
    
    if(ac > 0 && fts_is_number(at))
      shift = fts_get_number_int(at) * n;
    
    if(shift == 1)
    {
      float f = ptr[size - 1];
      int i;
      
      for(i=size-2; i>=0; i--)
        ptr[i + 1] = ptr[i];
      
      ptr[0] = f;
    }
    else if(shift == -1)
    {
      float f = ptr[0];
      int i;
      
      for(i=0; i<size-1; i++)
        ptr[i] = ptr[i + 1];
      
      ptr[size - 1] = f;
    }
    else
    {
      float forward;
      int i, j, next, end;
      
      while(shift < 0)
        shift += size;
      
      while(shift >= size)
        shift -= size;
      
      i = 0;
      j = shift;
      end = shift; 
      
      forward = ptr[shift];
      ptr[shift] = ptr[0];
      
      while(i < end)
      {
        next = (j + shift) % size;
        
        if(next != i)
        {
          float swap = ptr[next];
          
          if(next < end)
            end = next;
          
          ptr[next] = forward;
          forward = swap;
          
          j = next;
        }
        else
        {
          ptr[i] = forward;
          
          i++;
          j = i;
          
          forward = ptr[i];
        }
      }
    }
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

int 
_fmat_element_compare_ascending(const void *left, const void *right)
{
  float l = ((const float *)left)[0];
  float r = ((const float *)right)[0];
  
  return (r < l) - (l < r);
}

int 
_fmat_element_compare_descending(const void *left, const void *right)
{
  float l = ((const float *)left)[0];
  float r = ((const float *)right)[0];
  
  return (l < r) - (r < l);
}


void
_fmat_sort (fmat_t *self, int col, int (*compare)(const void *left, const void *right))
{  
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self); 

  if (col == 0)
    qsort((void *)ptr, m, n * sizeof(float), compare);
  else if(col > 0 && col < n)
  {
    int i;
    
    for(i=0; i<m*n; i+=n)
    {
      float f = ptr[i];
      
      ptr[i] = ptr[i + col];
      ptr[i + col] = f;
    }
    
    qsort((void *)ptr, m, n * sizeof(float), compare);

    for(i=0; i<m*n; i+=n)
    {
      float f = ptr[i];
      
      ptr[i] = ptr[i + col];
      ptr[i + col] = f;
    }    
  }
}


static fts_method_status_t
fmat_sort (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  int col = 0;
  
  if (ac > 0)
    col = fts_get_number_int(at);

  _fmat_sort(self, col, _fmat_element_compare_ascending);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}


static fts_method_status_t
fmat_tros (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  int col = 0;
  
  if (ac > 0)
    col = fts_get_number_int(at);

  _fmat_sort(self, col, _fmat_element_compare_descending);
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}


static fts_method_status_t
fmat_scramble(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self); 
  int i;
  
  if(n == 1)
  {
    for(i=0; i<m; i++)
    {
      int random = (int)(((double)m / (double)FTS_RANDOM_RANGE) * (double)fts_random());
      float f = ptr[i];
    
      ptr[i] = ptr[random];
      ptr[random] = f;
    }
  }
  else
  {
    for(i=0; i<m; i++)
    {
      int random = (int)(((double)m / (double)FTS_RANDOM_RANGE) * (double)fts_random());
      float *row = ptr + i * n;
      float *rand_row = ptr + random * n;
      int j;
      
      for(j=0; j<n; j++)
      {
        float f = row[j];
        
        row[j] = rand_row[j];
        rand_row[j] = f;
      }
    }
  }  
    
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

/******************************************************************************
 *
 *  envelopes
 *
 */
static fts_method_status_t
fmat_fade(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self); 
  int fade = 0;
  float f, incr;
  int n_fade;
  int i, j;

  if(ac > 0 && fts_is_number(at))
    fade = fts_get_number_int(at);

  n_fade = fade;
  incr = (float)(1.0 / (float)fade);

  if(n_fade > m / 2)
    n_fade = m / 2;
          
  f = 0.0;
  
  if(n == 1)
  {
    /* apply fade in/out to vector */
    ptr[0] = 0.0;
    
    for(i=1, j=m-1; i<n_fade; i++, j--)
    {
      ptr[i] *= f;
      ptr[j] *= f;
      
      f += incr;
    }
  }
  else
  {
    /* apply fade in/out to each column */
    float *rtp = ptr + m * n;
    
    for(j=0; j<n; j++)
      ptr[j] = 0.0;
    
    for(i=1; i<n_fade; i++)
    {
      rtp -= n;
    
      for(j=0; j<n; j++)
      {
        ptr[j] *= f;
        rtp[j] *= f;
      }
      
      ptr += n;
      f += incr;
    }
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_lookup_fmat_or_slice(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  fts_object_t *obj = fts_get_object(at);
  float *fun;
  int fun_size, fun_stride;
  int i;
  
  fmat_or_slice_vector(obj, &fun, &fun_size, &fun_stride);
      
  for(i=0; i<m*n; i++)
  {
    double f_index = ptr[i];
    
    if(f_index < 0.0)
      ptr[i] = fun[0];
    else 
    {
      double i_index = floor(f_index);
      int index = (int)i_index;
    
      if(index >= fun_size - 1)
        ptr[i] = fun[(fun_size - 1) * fun_stride];
      else
      {
        double frac = f_index - i_index;
        double fun_0 = fun[index * fun_stride];
        double fun_1 = fun[(index + 1) * fun_stride];
  
        ptr[i] = (1.0 - frac) * fun_0 + frac * fun_1;
      }
    }
  }
  
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_lookup_bpf(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  bpf_t *bpf = (bpf_t *)fts_get_object(at);
  int i;
  
  for(i=0; i<m*n; i++)
    ptr[i] = bpf_get_interpolated(bpf, ptr[i]);

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_env_fmat_or_slice(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;  
  fts_object_t *obj = fts_get_object(at);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *env;
  int env_size, env_stride;
    
  fmat_or_slice_vector(obj, &env, &env_size, &env_stride);

  if(env_size == m)
  {
    int i, j, k;
    
    /* simply multiply function vector each column*/
    for(i=0, k=0; i<m*n; i+=n, k+=env_stride)
    {
      double f = env[k];
      
      for(j=0; j<n; j++)
        ptr[i + j] *= f;
    }
  }
  else
  {
    double incr = (double)env_size / (double)m;
    double f_index = incr;
    int i, j;
    
    /* apply envelope function to each column */
    for(j=0; j<n; j++)
      ptr[j] *= env[0];

    for(i=n; i<m*n; i+=n)
    {
      double i_index = floor(f_index);
      int index = (int)i_index;
      double frac = f_index - i_index;
      double env_0 = env[index * env_stride];
      double env_1 = env[index * env_stride + env_stride];
      
      for(j=0; j<n; j++)
        ptr[i + j] *= (1.0 - frac) * env_0 + frac * env_1;
      
      f_index += incr;
    }
  }

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_env_bpf(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;  
  bpf_t *bpf = (bpf_t *)fts_get_object(at);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  double incr = bpf_get_duration(bpf) / (double)m;
  double time = 0.0;
  int i, j;
  
  /* apply bpf envelope to each column */
  for(i=0; i<m*n; i+=n)
  {
    float f = bpf_get_interpolated(bpf, time);
    
    for(j=0; j<n; j++)
      ptr[i + j] *= f;
      
    time += incr;  
  }

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_apply_expr(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  expr_t *expr = (expr_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr = fmat_get_ptr(self);
  fts_hashtable_t locals;
  fts_atom_t key_self, key_x, key_i, value, index;
  fts_atom_t r;
  int i;
  
  fts_hashtable_init(&locals, FTS_HASHTABLE_SMALL);
  
  fts_set_symbol(&key_self, fts_s_self);
  fts_set_object(&value, self);
  fts_hashtable_put(&locals, &key_self, &value);
  
  fts_set_symbol(&key_x, fts_s_x);
  fts_set_symbol(&key_i, fts_s_i);

  /* apply expression to each value */  
  for(i=0; i<m*n; i++)
  {
    double f = (double)ptr[i];
  
    fts_set_int  (&index, i);
    fts_set_float(&value, f);
    fts_hashtable_put(&locals, &key_i, &index);
    fts_hashtable_put(&locals, &key_x, &value);

    expr_evaluate(expr, &locals, ac - 1, at + 1, &r);
    
    if(fts_is_number(&r))
      ptr[i] = fts_get_number_float(&r);
    else
      ptr[i] = 0.0;
  }
  
  fts_hashtable_destroy(&locals);

  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_apply_expr_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0 && fts_is_a(at, expr_class))
    fmat_apply_expr(o, s, ac, at, ret);
      
  return fts_ok;
}


static fts_method_status_t
_fmat_find (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  expr_t *expr = (expr_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr = fmat_get_ptr(self);
  fts_hashtable_t locals;
  fts_atom_t key_self, key_x, key_i, value, index;
  fts_atom_t r;
  int i, j;
  
  fts_hashtable_init(&locals, FTS_HASHTABLE_SMALL);
  
  fts_set_symbol(&key_self, fts_s_self);
  fts_set_symbol(&key_x,    fts_s_x);
  fts_set_symbol(&key_i,    fts_s_i);

  fts_set_object(&value, self);
  fts_hashtable_put(&locals, &key_self, &value);
  
  /* find all indices where expression on value is true */  
  for (i = 0, j = 0; i < m * n; i++)
  {
    double f = (double) ptr[i];
  
    fts_set_int  (&index, i);
    fts_set_float(&value, f);
    fts_hashtable_put(&locals, &key_i, &index);
    fts_hashtable_put(&locals, &key_x, &value);

    expr_evaluate(expr, &locals, ac - 1, at + 1, &r);
    
    if (fts_is_number(&r)  &&  fts_get_number_int(&r) != 0)
      ptr[j++] = i;
  }
  
  fts_hashtable_destroy(&locals);

  /* keep only found linear indices */
  fmat_reshape(self, j, 1);
  fts_object_changed(o);
  fts_set_object(ret, o);
  
  return fts_ok;
}


static fts_method_status_t
_fmat_find_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (ac > 0  &&  fts_is_a(at, expr_class))
    _fmat_find(o, s, ac, at, ret);
      
  return fts_ok;
}




/*********************************************************
 *
 *  editor
 *
 */

static void *fmat_editor = NULL;

static void
fmat_editor_callback(fts_object_t *o, void *e)
{
  fmat_t *self = (fmat_t *)o;

  if(fmat_editor_is_open(self))
    fmat_upload(self);
}

static fts_method_status_t
fmat_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  fmat_set_editor_open( self);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  
  fts_object_add_listener(o, fmat_editor, fmat_editor_callback);
  
  fmat_upload(self);
  
  return fts_ok;
}

static fts_method_status_t
fmat_destroy_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  fmat_set_editor_close( self);
  fts_object_remove_listener(o, fmat_editor);
  
  return fts_ok;
}

static fts_method_status_t
fmat_close_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  
  if(fmat_editor_is_open(self))
  {
    fmat_set_editor_close(self);
    fts_client_send_message(o, fts_s_closeEditor, 0, 0);  
    fts_object_remove_listener(o, fmat_editor);
  }
  
  return fts_ok;
}

/* move track by client request */
static fts_method_status_t
_fmat_upload(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  
  if(fmat_editor_is_open(self))
    fmat_upload(self);
  
  return fts_ok;
}

/********************************************************************
 *
 *  system functions
 *
 */

static fts_method_status_t
fmat_print(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int size = m * n;
  fts_bytestream_t* stream = fts_get_default_console_stream();
  int i, j;
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  if(size == 0)
    fts_spost(stream, "<empty fmat>\n");
  else
  {
    fts_spost(stream, "<fmat %d x %d>\n", m, n);
    fts_spost(stream, "{\n");
    
    for(i=0; i<m; i++)
    {
      fts_spost(stream, "  ");
      
      for(j=0; j<n-1; j++)
        fts_spost(stream, "%.7g ", fmat_get_element(self, i, j));
      
      fts_spost(stream, "%.7g,\n", fmat_get_element(self, i, n-1));
    }
    
    fts_spost(stream, "}\n");
  }
  
  fts_set_object(ret, o);
  
  return fts_ok;
}

static fts_method_status_t
fmat_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  float *data = self->values;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  fts_message_t *mess;
  int i, j;

  /* dump size message */
  mess = fts_dumper_message_get(dumper, fts_s_size);
  fts_message_append_int(mess, m);
  fts_message_append_int(mess, n);
  fts_dumper_message_send(dumper, mess);

  for(i=0; i<m*n; i+=128)
  {
    int n_block = m * n - i;
    
    if(n_block > 128)
      n_block = 128;
    
    mess = fts_dumper_message_get(dumper, fts_s_set);
    fts_message_append_int(mess, i / n);
    fts_message_append_int(mess, i % n);

    for(j=0; j<n_block; j++)
      fts_message_append_float(mess, data[i + j]);
    
    fts_dumper_message_send(dumper, mess);
  }

  return fts_ok;
}




/*********************************************************
 *
 *  class init/delete
 *
 */

static void
fmat_initialize(fmat_t *self)
{
  self->values = NULL;
  self->m = 0;
  self->n = 0;
  self->alloc = -1;
  self->onset = 0.0;
  self->domain = 0.0;
  self->opened = 0;
}


/** fmat constructor.
 *
 * @fn fmat(num nrow, num ncol, num init... );
 *
 * @param nrow (optional) number of rows    (default 0)
 * @param ncol (optional) number of columns (default 1) 
 * @param nrow (optional) initial values    (default zeros)
 */

static fts_method_status_t
fmat_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  int m = 0;
  int n = 1;
    
  fmat_initialize(self);
  
  if(ac > 0 && fts_is_number(at))
    m = fts_get_number_int(at);
  
  if(ac > 1 && fts_is_number(at + 1))
    n = fts_get_number_int(at + 1);
  
  /* remaining args are init values */
  if(ac > 2)
  {
    int size = m * n;
    int i;
    
    ac -= 2;
    at += 2;
    
    if(ac > size)
      ac = size;
    
    fmat_reshape(self, m, n);  
    fmat_set_from_atoms(self, 0, 1, ac, at);

    /* zero remaining elements */
    for (i = ac; i < size; i++)
      self->values[i] = 0.0;
  }
  else
    fmat_set_size(self, m, n);  
  
  return fts_ok;
}

static fts_method_status_t
fmat_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(self->values != NULL)
    fts_free(self->values - HEAD_POINTS);
  
  return fts_ok;
}

/** @} end of fmatmess */






/*********************************************************
 *
 *  class instantiate
 *
 */

static void
fmat_message (fts_class_t *cl, fts_symbol_t s, fts_method_t matrix_method, fts_method_t scalar_method)
{
  if(matrix_method != NULL)
    fts_class_message(cl, s, fmat_class, matrix_method);
  
  if(scalar_method != NULL)
    fts_class_message_number(cl, s, scalar_method);
}

static void
fmat_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fmat_t), fmat_init, fmat_delete);
  
  fts_class_set_copy_function(cl, fmat_copy_function);
  fts_class_set_array_function(cl, fmat_array_function);
  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, fmat_dump_state);
  
  fts_class_message_varargs(cl, fts_s_print, fmat_print);

  fts_class_message_varargs(cl, fts_s_get_element, _fmat_get_element);
  fts_class_message_varargs(cl, fts_s_get, _fmat_get_element);
  fts_class_message_varargs(cl, fts_new_symbol("iget"), _fmat_get_interpolated);
  
  fts_class_message_varargs(cl, fts_s_set, fmat_set_from_list);
  fts_class_message(cl, fts_s_set, cl, fmat_set_from_fmat);
  fts_class_message(cl, fts_s_set, fvec_class, fmat_set_from_fvec);
  fts_class_message(cl, fts_s_set, bpf_class, fmat_set_from_bpf);
  fts_class_message(cl, fts_s_set, ivec_class, fmat_set_from_ivec);

  fts_class_message_varargs(cl, fts_new_symbol("pick"), fmat_pick_fmat);
  
  /* compatible fvec creators */
  fts_class_message_number(cl, fts_s_col, fmat_get_col);
  fts_class_message_number(cl, fts_s_row, fmat_get_row);

  /* compatible col and row set from list */
  fts_class_message_varargs(cl, fts_s_col, fmat_set_col);
  fts_class_message_varargs(cl, fts_s_row, fmat_set_row);

  /* new fvec creators */
  fts_class_message_varargs(cl, fts_new_symbol("colref"), fmat_get_col);
  fts_class_message_varargs(cl, fts_new_symbol("rowref"), fmat_get_row);
  fts_class_message_varargs(cl, fts_new_symbol("diagref"), fmat_get_diag);
  
  /* but these are preferred: */
  fts_class_message_varargs(cl, fts_new_symbol("setrow"), fmat_set_row);
  fts_class_message_varargs(cl, fts_new_symbol("setcol"), fmat_set_col);
  
  fts_class_message_number (cl, fts_s_fill, fmat_fill_number);
  fts_class_message_varargs(cl, fts_s_fill, fmat_fill_varargs);
  fts_class_message_varargs(cl, fts_new_symbol("zero"),   fmat_fill_zero);
  fts_class_message_varargs(cl, fts_new_symbol("random"), fmat_fill_random);
  
  fts_class_message_varargs(cl, fts_s_append, fmat_append_row_varargs);
  fts_class_message(cl, fts_s_append, cl, fmat_append_row_fmat);
  fts_class_message(cl, fts_s_append, fvec_class, fmat_append_row_slice);
  
  fts_class_message_varargs(cl, fts_s_insert, fmat_insert_rows);
  fts_class_message_varargs(cl, sym_insert_cols, fmat_insert_columns);
  fts_class_message_varargs(cl, fts_s_delete, fmat_delete_rows);
  fts_class_message_varargs(cl, sym_delete_cols, fmat_delete_columns);
  
  fts_class_message_void(cl, fts_s_size, _fmat_get_size);
  fts_class_message_varargs(cl, fts_s_size, _fmat_set_size);
  
  fts_class_message_void(cl, fts_s_rows, _fmat_get_m);
  fts_class_message_number(cl, fts_s_rows, _fmat_set_m);

  fts_class_message_void(cl, fts_s_cols, _fmat_get_n);
  fts_class_message_number(cl, fts_s_cols, _fmat_set_n);  

  fts_class_message_void(cl, fts_new_symbol("min"), fmat_get_min);
  fts_class_message_void(cl, fts_new_symbol("max"), fmat_get_max);
  fts_class_message_void(cl, fts_new_symbol("absmax"), fmat_get_absmax);
  fts_class_message_void(cl, fts_new_symbol("sum"), fmat_get_sum);
  fts_class_message_void(cl, fts_new_symbol("mean"), fmat_get_mean);
  fts_class_message_void(cl, fts_new_symbol("zc"), fmat_get_zc);
  
  fts_class_message_varargs(cl, fts_new_symbol("add"), fmat_elem_add);

  fmat_message(cl, fts_new_symbol("add"), fmat_add_fmat, fmat_add_number);
  fmat_message(cl, fts_new_symbol("sub"), fmat_sub_fmat, fmat_sub_number);
  fmat_message(cl, fts_new_symbol("mul"), fmat_mul_fmat, fmat_mul_number);
  fmat_message(cl, fts_new_symbol("div"), fmat_div_fmat, fmat_div_number);
  fmat_message(cl, fts_new_symbol("bus"), fmat_bus_fmat, fmat_bus_number);
  fmat_message(cl, fts_new_symbol("vid"), fmat_vid_fmat, fmat_vid_number);
  fmat_message(cl, fts_new_symbol("ee"), fmat_eq_fmat, fmat_eq_number);
  fmat_message(cl, fts_new_symbol("eq"), fmat_eq_fmat, fmat_eq_number);
  fmat_message(cl, fts_new_symbol("ne"), fmat_ne_fmat, fmat_ne_number);
  fmat_message(cl, fts_new_symbol("gt"), fmat_gt_fmat, fmat_gt_number);
  fmat_message(cl, fts_new_symbol("ge"), fmat_ge_fmat, fmat_ge_number);
  fmat_message(cl, fts_new_symbol("lt"), fmat_lt_fmat, fmat_lt_number);
  fmat_message(cl, fts_new_symbol("le"), fmat_le_fmat, fmat_le_number);

  fts_class_message_void(cl, fts_new_symbol("abs"), fmat_abs);
  fts_class_message_void(cl, fts_new_symbol("logabs"), fmat_logabs);
  fts_class_message_void(cl, fts_new_symbol("log"), fmat_log);
  fts_class_message_void(cl, fts_new_symbol("exp"), fmat_exp);
  fts_class_message_void(cl, fts_new_symbol("sqrabs"), fmat_sqrabs);
  fts_class_message_void(cl, fts_new_symbol("sqrt"), fmat_sqrt);
  
  fts_class_message_void(cl, sym_rect, fmat_convert_rect);
  fts_class_message_void(cl, sym_polar, fmat_convert_polar);

  fmat_message(cl, fts_new_symbol("cmul"), fmat_cmul_fmat, fmat_cmul_number);
  fts_class_message_void(cl, fts_new_symbol("cabs"), fmat_cabs);
  fts_class_message_void(cl, fts_new_symbol("clogabs"), fmat_clogabs);
  fts_class_message_void(cl, fts_new_symbol("clog"), fmat_clog);
  fts_class_message_void(cl, fts_new_symbol("cexp"), fmat_cexp);
  fts_class_message_void(cl, fts_new_symbol("csqrabs"), fmat_csqrabs);

  fts_class_message_void(cl, fts_new_symbol("fft"), fmat_fft);
  fts_class_message_void(cl, fts_new_symbol("ifft"), fmat_ifft);
  fts_class_message_void(cl, fts_new_symbol("rifft"), fmat_rifft);  

  fts_class_message(cl, fts_new_symbol("xmul"), fmat_class, fmat_xmul_fmat);
  fts_class_message(cl, fts_new_symbol("dot"), fmat_class, fmat_get_dot);
  
  fts_class_message_varargs(cl, fts_new_symbol("clip"), fmat_clip);
  fts_class_message_void(cl, fts_new_symbol("normalize"), fmat_normalize);
  fts_class_message_void  (cl, fts_s_sort, fmat_sort);
  fts_class_message_number(cl, fts_s_sort, fmat_sort);
  fts_class_message_void  (cl, fts_s_sortrev, fmat_tros);
  fts_class_message_number(cl, fts_s_sortrev, fmat_tros);
  fts_class_message_void(cl, fts_new_symbol("rotate"), fmat_rotate);
  fts_class_message_number(cl, fts_new_symbol("rotate"), fmat_rotate);
  fts_class_message_void  (cl, fts_s_reverse, fmat_reverse);
  fts_class_message_void(cl, fts_new_symbol("scramble"), fmat_scramble);

  fts_class_message(cl, fts_new_symbol("lookup"), cl, fmat_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), fvec_class, fmat_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), bpf_class, fmat_lookup_bpf);
  
  fts_class_message(cl, fts_new_symbol("env"), cl, fmat_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), fvec_class, fmat_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), bpf_class, fmat_env_bpf);

  fts_class_message(cl, fts_new_symbol("apply"), expr_class, fmat_apply_expr);
  fts_class_message_varargs(cl, fts_new_symbol("apply"), fmat_apply_expr_varargs);
  fts_class_message        (cl, fts_new_symbol("find"), expr_class, _fmat_find);
  fts_class_message_varargs(cl, fts_new_symbol("find"), _fmat_find_varargs);
  
  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_message_varargs(cl, fts_s_openEditor, fmat_open_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, fmat_close_editor); 
  fts_class_message_varargs(cl, fts_s_destroyEditor, fmat_destroy_editor);
  
  fts_class_message_varargs(cl, fts_s_upload, _fmat_upload);
  
  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);

  /* 
   * fmat class documentation 
   */
  
  fts_class_doc(cl, fmat_symbol, "[<num: # of rows> [<num: # of columns (def 1)> [<num: init values> ...]]]", "matrix of floats");
  
  fts_class_doc(cl, fts_s_col, "<num: index>", "get column reference (creates fvec object)");
  fts_class_doc(cl, fts_s_row, "<num: index>", "get row reference (creates fvec object)");
  
  fts_class_doc(cl, fts_s_size, "[<num: # of rows> [<num: # of columns (def 1)>]]", "get/set dimensions");
  fts_class_doc(cl, fts_s_rows, "[<num: # of rows>]", "get/set # of rows");
  fts_class_doc(cl, fts_s_cols, "[<num: # of columns>]", "get/set # of columns");
  fts_class_doc(cl, fts_s_fill, "<num: value>", "fill with given value or pattern of values");
  fts_class_doc(cl, fts_s_fill, "<expr: expression>", "fill with given expression (use $self, $row and $col)");
  fts_class_doc(cl, fts_new_symbol("zero"), "[<num: row index> <num: column index> [<num: # of elements>]]", "zero given number of elements starting from indicated element (row by row)");

  fts_class_doc(cl, fts_s_append, "[<num: value> ...]", "append row with given values");
  fts_class_doc(cl, fts_s_append, "<fvec: row values>", "append row with values from fvec");
  fts_class_doc(cl, fts_s_append, "<fmat: row values>", "append rows with values from fmat");

  fts_class_doc(cl, fts_s_insert,    "[<num: index (def 0)> [<num: number (def 1)>]]", "insert <number> empty rows at given index");
  fts_class_doc(cl, sym_insert_cols, "[<num: index (def 0)> [<num: number (def 1)>]]", "insert <number> empty columns at given index");

  fts_class_doc(cl, fts_s_delete,    "[<num: index (def 0)> [<num: number (def 1)>]]", "delete <number> rows from given index");
  fts_class_doc(cl, sym_delete_cols, "[<num: index (def 0)> [<num: number (def 1)>]]", "delete <number> columns from given index");


  fts_class_doc(cl, fts_s_set, "<fmat: matrix>", "set dimension and values from given fmat");
  fts_class_doc(cl, fts_s_set, "<num: row index> <num: column index> [<num:value> ...]", "set values starting from indicated element (row by row)");
  
  fts_class_doc(cl, fts_new_symbol("setcol"), "<num: index> [<num: value> ...]", "set values of given column from list");
  fts_class_doc(cl, fts_new_symbol("setcol"), "<num: index> <fvec: values>", "set values of given column from fmat or fvec");
  fts_class_doc(cl, fts_new_symbol("setrow"), "<num: index> [<num: value> ...]", "set values of given row from list");
  fts_class_doc(cl, fts_new_symbol("setrow"), "<num: index> <fvec: values>", "set values of given row from fmat or fvec");
  
  fts_class_doc(cl, fts_new_symbol("min"), NULL, "get minimum value");
  fts_class_doc(cl, fts_new_symbol("max"), NULL, "get maximum value");
  fts_class_doc(cl, fts_new_symbol("absmax"), NULL, "get maximum absolute value");
  fts_class_doc(cl, fts_new_symbol("sum"),  NULL, "get sum of all values");
  fts_class_doc(cl, fts_new_symbol("prod"), NULL, "get product of all values");
  fts_class_doc(cl, fts_new_symbol("mean"), NULL, "get mean value of all values");
  fts_class_doc(cl, fts_new_symbol("zc"), NULL, "get number of zerocrossings");
  
  fts_class_doc(cl, fts_s_get, "<num: row index> <num: column index>", "get value at given index");
  fts_class_doc(cl, fts_new_symbol("iget"), "<num: float row index> <num: float column index>", "get (linearly) interpolated value at given float index");
  
  fts_class_doc(cl, fts_new_symbol("add"), "<num|fmat: operand>", "add given scalar or fmat (element by element) to current values");
  fts_class_doc(cl, fts_new_symbol("sub"), "<num|fmat: operand>", "substract given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("mul"), "<num|fmat: operand>", "multiply current values by given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("div"), "<num|fmat: operand>", "divide current values by given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("bus"), "<num|fmat: operand>", "subtract current values from given scalar or fmat (element by element)");  
  fts_class_doc(cl, fts_new_symbol("vid"), "<num|fmat: operand>", "divide given scalar or fmat (element by element) by current values");
  fts_class_doc(cl, fts_new_symbol("eq"), "<num|fmat: operand>", "replace current values by result of == comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("ne"), "<num|fmat: operand>", "replace current values by result of != comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("gt"), "<num|fmat: operand>", "replace current values by result of > comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("ge"), "<num|fmat: operand>", "replace current values by result of >= comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("lt"), "<num|fmat: operand>", "replace current values by result of < comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("le"), "<num|fmat: operand>", "replace current values by result of <= comparison (0 or 1) with given scalar or fmat (element by element)");
  
  fts_class_doc(cl, fts_new_symbol("abs"), NULL, "calulate absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("logabs"), NULL, "calulate logarithm of absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("log"), NULL, "calulate lograrithm of current values");
  fts_class_doc(cl, fts_new_symbol("exp"), NULL, "calulate exponent function of current values");
  fts_class_doc(cl, fts_new_symbol("sqrabs"), NULL, "calulate square of absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("sqrt"), NULL, "calulate square root of absolute values of current values");
  
  fts_class_doc(cl, fts_new_symbol("cmul"), "<num|fmat: operand>", "multiply current values of complex vector by given scalar or complex vector fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("cabs"), NULL, "calulate absolute values of current complex values");
  fts_class_doc(cl, fts_new_symbol("clogabs"), NULL, "calulate logarithm of absolute values of current complex values");
  fts_class_doc(cl, fts_new_symbol("clog"), NULL, "calulate lograrithm of current complex values");
  fts_class_doc(cl, fts_new_symbol("cexp"), NULL, "calulate exponent function of current complex values");
  fts_class_doc(cl, fts_new_symbol("csqrabs"), NULL, "calulate square of absolute values of current complex values");
  fts_class_doc(cl, fts_new_symbol("fft"), NULL, "calulate inplace FFT of real or complex vector");
  fts_class_doc(cl, fts_new_symbol("ifft"), NULL, "calulate inplace inverse FFT of complex vector");
  fts_class_doc(cl, fts_new_symbol("rifft"), NULL, "calulate inplace real inverse FFT of complex vector");
  
  fts_class_doc(cl, fts_new_symbol("xmul"), "<fmat: operand>", "calculate matrix multiplication of operand M with current matrix C so that C' = M x C");
  fts_class_doc(cl, fts_new_symbol("dot"), "<fmat: operand>", "get dot product of column vector with given vector");
  
  fts_class_doc(cl, fts_new_symbol("clip"), "[<lower limit>] <upper limit>", "clip values within given limits");
  fts_class_doc(cl, fts_new_symbol("normalize"), NULL, "normalize to between -1.0 and 1.0");
  fts_class_doc(cl, fts_s_reverse, NULL, "reverse order of rows");
  fts_class_doc(cl, fts_new_symbol("rotate"), "[<num: # of elements (def 1)>]", "rotate by given number of rows");
  fts_class_doc(cl, fts_s_sort, "[<num: index of column>]", "sort rows by ascending values of given column");
  fts_class_doc(cl, fts_s_sortrev, "[<num: index of column>]", "sort rows by descending values of given column");
  fts_class_doc(cl, fts_new_symbol("scramble"), NULL, "scramble rows randomly");

  fts_class_doc(cl, fts_new_symbol("lookup"), "<fmat|fvec|bpf: function>", "apply given function to each value (by linear interpolation)");
  fts_class_doc(cl, fts_new_symbol("env"), "<fmat|fvec|bpf: envelope>", "multiply given envelope function to each column");

  fts_class_doc(cl, fts_new_symbol("apply"), "<expr: expression>", "apply expression each value (use $self and $x)");

  fts_class_doc(cl, sym_rect, NULL, "convert complex polar vector to complex rectangular vector (matrix of 2 columns)");
  fts_class_doc(cl, sym_polar, NULL, "convert complex rectangular vector to complex polar vector (matrix of 2 columns)");
  
  if(fmat_null == NULL)
  {
    fmat_null = (fmat_t *)fts_object_create(fmat_class, 0, 0);
    fts_object_refer((fts_object_t *)fmat_null);
  }
}

FTS_MODULE_INIT(fmat)
{
  fmat_symbol = fts_new_symbol("fmat");

  sym_getcol = fts_new_symbol("getcol");
  sym_getrow = fts_new_symbol("getrow");
  sym_text = fts_new_symbol("text");
  
  sym_vec = fts_new_symbol("vec");
  sym_real = fts_new_symbol("real");
  sym_rect = fts_new_symbol("rect");
  sym_polar = fts_new_symbol("polar");
  sym_re = fts_new_symbol("re");
  sym_im = fts_new_symbol("im");
  sym_mag = fts_new_symbol("mag");
  sym_arg = fts_new_symbol("arg");

  sym_insert_cols = fts_new_symbol("insert_cols");
  sym_delete_cols = fts_new_symbol("delete_cols");

  fmat_class = fts_class_install(fmat_symbol, fmat_instantiate);
  fmat_null = NULL;
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
