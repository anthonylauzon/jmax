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
#include <fts/packages/data/data.h>
#include "floatfuns.h"

#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <string.h>

fts_class_t *fmat_class = NULL;
fts_class_t *fvec_class = NULL;

fts_symbol_t fmat_symbol = NULL;
fts_symbol_t fvec_symbol = NULL;

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

/********************************************************
 *
 *  fmat format
 *
 */
static fts_hashtable_t fmat_format_hash;
static fmat_format_t *fmat_formats[FMAT_FORMATS_MAX + 1];
static int fmat_n_formats = 0;

fmat_format_t *fmat_format_vec = 0;
fmat_format_t *fmat_format_rect = 0;
fmat_format_t *fmat_format_polar = 0;
fmat_format_t *fmat_format_real = 0;

fmat_format_t *
fmat_format_register(fts_symbol_t name)
{
  fts_atom_t k, a;
  
  fts_set_symbol(&k, name);  
  if(name != sym_real && fts_hashtable_get(&fmat_format_hash, &k, &a) == 0 && fmat_n_formats < FMAT_FORMATS_MAX)
  {
    fmat_format_t *format = fts_malloc(sizeof(fmat_format_t));
    fmat_formats[fmat_n_formats] = format;

    format->name = name;
    format->index = fmat_n_formats++;
    format->n_columns = 0;
        
    fts_set_pointer(&a, format);
    fts_hashtable_put(&fmat_format_hash, &k, &a);
    
    return format;
  }
  
  return NULL;
}

void
fmat_format_add_column(fmat_format_t *format, fts_symbol_t label)
{
  format->columns[format->n_columns++] = label;
}

fmat_format_t *
fmat_format_get_by_name(fts_symbol_t name)
{
  fts_atom_t k, a;
  
  fts_set_symbol(&k, name);  
  if(fts_hashtable_get(&fmat_format_hash, &k, &a) != 0)
    return (fmat_format_t *)fts_get_pointer(&a);

  return NULL;
}

/* correct fmat format (if needed) */
static void
fmat_adapt_format(fmat_t *self)
{
  int n = fmat_get_n(self);
  
  if(n == 1)
    self->format = fmat_format_vec;
  else 
  {
    int format_n = fmat_format_get_n(self->format);
    
    switch(fmat_format_get_id(self->format))
    {
      case fmat_format_id_vec:
        if(n > 1)
          self->format = fmat_format_real;
        break;        
        
      case fmat_format_id_rect:
      case fmat_format_id_polar:        
        if(n != 2)
          self->format = fmat_format_real;
        break;
        
      default:
        if(n < format_n)
          self->format = fmat_format_real;
        break;
    }    
  }
}

/********************************************************
 *
 *  utility functions
 *
 */

#define HEAD_ROWS 8 /* extra points for interpolation at start of vector */
#define TAIL_ROWS 8 /* extra points for interpolation at end of vector */
#define HEAD_TAIL_COLS 2 /* interpolation head and tail for 1 or 2 column vectors only */
#define HEAD_POINTS (HEAD_TAIL_COLS * HEAD_ROWS)
#define TAIL_POINTS (HEAD_TAIL_COLS * TAIL_ROWS)


/* create fmat with m rows and n columns */
fmat_t *fmat_create(int m, int n)
{
    fts_atom_t size[2];

    fts_set_int(&size[0], m);
    fts_set_int(&size[1], n);
    return ((fmat_t *) fts_object_create(fmat_type, 2, size));
}


/* change matrix "form", leaving underlying data vector untouched */
void
fmat_reshape(fmat_t *self, int m, int n)
{
  int size;
  
  if(n <= 0)
    n = 1;
  
  size = m * n;
  
  if(size > self->alloc)
  {
    if(self->values == NULL)
      self->values = (float *)fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
    else
      self->values = (float *)fts_realloc(self->values - HEAD_POINTS, (size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
        
    self->values += HEAD_POINTS;
    self->alloc = size;
  }
  
  self->m = m;
  self->n = n;
  
  fmat_adapt_format(self);
}

void
fmat_set_m(fmat_t *self, int m)
{
  int size = m * fmat_get_n(self);
  int i;
  
  if(m > fmat_get_m(self))
  {
    if(self->values == NULL)
      self->values = (float *)fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
    else
      self->values = (float *)fts_realloc(self->values - HEAD_POINTS, (size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
    
    self->values += HEAD_POINTS;
    self->alloc = size;
  }
  
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
  
    if(n > old_n)
    {
      if(self->values == NULL)
        self->values = (float *)fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
      else
        self->values = (float *)fts_realloc(self->values - HEAD_POINTS, (size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
      
      self->values += HEAD_POINTS;
      self->alloc = size;

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
    
    fmat_adapt_format(self);
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
    
    if(size > self->alloc)
    {
      if(self->values == NULL)
        self->values = (float *)fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
      else
        self->values = (float *)fts_realloc(self->values - HEAD_POINTS, (size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
        
      self->values += HEAD_POINTS;
      self->alloc = size;
    }
        
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
    
    fmat_adapt_format(self);
  }
}

void
fmat_set_const(fmat_t *mat, float c)
{
  float *values = mat->values;
  int size = mat->m * mat->n;
  int i;

  for(i=0; i<size; i++)
    values[i] = c;
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

void
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
  copy->sr = org->sr;
  copy->format = org->format;
}


static void
fmat_copy_function(const fts_atom_t *from, fts_atom_t *to)
{
  fmat_copy((fmat_t *)fts_get_object(from), (fmat_t *)fts_get_object(to));
}

static int
fmat_equals_function(const fts_atom_t *a, const fts_atom_t *b)
{
  fmat_t *o = (fmat_t *)fts_get_object(a);
  fmat_t *p = (fmat_t *)fts_get_object(b);
  
  if(fmat_get_m(o) == fmat_get_m(p) && fmat_get_n(o) == fmat_get_n(p))
  {
    int size = fmat_get_m(o) * fmat_get_n(o);
    float *o_ptr = fmat_get_ptr(o);
    float *p_ptr = fmat_get_ptr(p);
    int i;
    
    for(i=0; i<size; i++)
      if(!data_float_equals(o_ptr[i], p_ptr[i]))
        return 0;
    
    return 1;
  }
  
  return 0;
}


static void
fmat_post_function(fts_object_t *o, fts_bytestream_t *stream)
{
  fmat_t *self = (fmat_t *)o;
  fmat_format_t *format = fmat_get_format(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int size = n * m;
  
  if(size == 0)
    fts_spost(stream, "<fmat>");
  else
    fts_spost(stream, "<fmat %d x %d of %s>", m, n, fts_symbol_name(fmat_format_get_name(format)));
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




/********************************************************
 *
 *  files
 *
 */

#define FMAT_BLOCK_SIZE 256

static void
fmat_grow(fmat_t *fmat, int size)
{
  int alloc = fmat->alloc;

  while(size > alloc)
    alloc += FMAT_BLOCK_SIZE;

  fmat_reshape(fmat, 1, alloc);
}

int
fmat_read_atom_file(fmat_t *fmat, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(file_name, "r");
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;

  while(fts_atom_file_read(file, &a, &c))
  {
    m = i + 1;

    /* first row determines # of columns */
    if(i == 0)
      n = j + 1;

    fmat_grow(fmat, m * n);
    
    if(j < n)
    {
      if(fts_is_number(&a))
        fmat->values[i * n + j] = (float)fts_get_number_float(&a);
      else
        fmat->values[i * n + j] = 0.0;

      j++;

      if(c == '\n')
      {
        for(; j<n; j++)
          fmat->values[i * n + j] = 0.0;
      
        /* reset to beginning of next row */
        i++;
        j = 0;
      }
    }
    else if(c == '\n')
    {
      /* reset to beginning of next row */
      i++;
      j = 0;
    }
  }

  /* maybe empty rest of last line */
  if(j > 0)
  {
    i++;
    j = 0;
  }

  fmat->m = m;
  fmat->n = n;

  fts_atom_file_close(file);

  return(m * n);
}

int
fmat_write_atom_file(fmat_t *fmat, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int m = fmat->m;
  int n = fmat->n;
  int i, j;

  file = fts_atom_file_open(file_name, "w");

  if(!file)
    return -1;

  /* write the content of the fmat */
  for(i=0; i<m; i++)
  {
    float *row = fmat->values + i * n;
    fts_atom_t a;

    for(j=0; j<n-1; j++)
    {
      fts_set_float(&a, row[j]);
      fts_atom_file_write(file, &a, ' ');
    }

    fts_set_float(&a, row[n - 1]);
    fts_atom_file_write(file, &a, '\n');
  }

  fts_atom_file_close(file);

  return(m * n);
}

static int
fmat_import_audiofile(fmat_t *mat, fts_symbol_t file_name)
{
  fts_audiofile_t *sf = fts_audiofile_open_read(file_name);
  int size = 0;
  
  if(fts_audiofile_is_valid(sf))
  {
    float *ptr;
    
    size = fts_audiofile_get_num_frames(sf);
    fmat_reshape(mat, size, 1);
    ptr = fmat_get_ptr(mat);
    
    size = fts_audiofile_read(sf, &ptr, 1, size);
    fmat_reshape(mat, size, 1);
    
    fts_audiofile_close(sf);
    
    if(size <= 0)
    {
      fts_object_error((fts_object_t *)mat, "cannot load from soundfile \"%s\"", fts_symbol_name(file_name));
      size = 0;
    }
  }
  else
  {
    fts_object_error((fts_object_t *)mat, "cannot open file \"%s\"", fts_symbol_name(file_name));
    fts_audiofile_close(sf);
  }
  
  return size;
}

static int
fmat_export_audiofile(fmat_t *mat, fts_symbol_t file_name)
{
  int size = 0;
  
  if(fmat_get_n(mat) == 1)
  {
    fts_audiofile_t *sf = fts_audiofile_open_write(file_name, 1, (int)fts_dsp_get_sample_rate(), fts_s_int16);
    int mat_size = fmat_get_m(mat);
    
    if( fts_audiofile_is_valid(sf))
    {
      float *ptr = fmat_get_ptr(mat);
      
      size = fts_audiofile_write(sf, &ptr, 1, mat_size);
      
      fts_audiofile_close(sf);
    }
  }
  else
    fts_post("*** fmat export of multi channel audio files not yet implemented ***");
  
  return size;
}




/********************************************************************
 *
 *  check & errors
 *
 */

int
fmat_or_slice_vector(fts_object_t *obj, float **ptr, int *size, int *stride)
{
  fts_class_t *cl = fts_object_get_class(obj);

  if(cl == fmat_class)
  {
    fmat_t *fmat = (fmat_t *)obj;
    
    *ptr = fmat_get_ptr(fmat);
    *size = fmat_get_m(fmat);
    *stride = fmat_get_n(fmat);
    
    return 1;
  }
  else if(cl == fcol_class)
  {
    fslice_t *fcol = (fslice_t *)obj;
    int index = fcol->index;
    fmat_t *fmat = fcol->fmat;
    float *fmat_ptr = fmat_get_ptr(fmat);
    int fmat_m = fmat_get_m(fmat);
    int fmat_n = fmat_get_n(fmat);

    if(index > fmat_n)
      index = fmat_n;

    *ptr = fmat_ptr + index;
    *size = fmat_m;
    *stride = fmat_n;
  
    return 1; 
  }
  else if(cl == frow_class)
  {
    fslice_t *frow = (fslice_t *)obj;
    int index = frow->index;
    fmat_t *fmat = frow->fmat;
    float *fmat_ptr = fmat_get_ptr(fmat);
    int fmat_m = fmat_get_m(fmat);
    int fmat_n = fmat_get_n(fmat);

    if(index > fmat_m)
      index = fmat_m;

    *ptr = fmat_ptr + index * fmat_n;
    *size = fmat_n;
    *stride = 1;
  
    return 1; 
  }
  
  *ptr = NULL;

  return 0;
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
fmat_error_format_and_dimensions(fmat_t *fmat, fmat_t *op, const char *prefix)
{
  fmat_format_t *format = fmat_get_format(fmat);
  int m = fmat_get_m(fmat);
  int n = fmat_get_n(fmat);
  fmat_format_t *op_format = fmat_get_format(op);
  int op_m = fmat_get_m(op);
  int op_n = fmat_get_n(op);
  
  fts_object_error((fts_object_t *)fmat, "%s: %s argument %d x%d doesn't match %s matrix %d x %d", prefix, 
                   fts_symbol_name(fmat_format_get_name(op_format)), op_m, op_n, 
                   fts_symbol_name(fmat_format_get_name(format)), m, n);
}




/********************************************************************
 *
 *   user methods
 *
 */
static void
fmat_set_from_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  fmat_copy((fmat_t *)fts_get_object(at), self);
  
  fts_return_object(o);
}

static void
fmat_set_from_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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

  fts_return_object(o);
}

static void
fmat_set_from_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
  {
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int i = fts_get_number_int(at);
    int j = fts_get_number_int(at + 1);
    int size = m * n;
    int onset = i * n + j;
    
    ac -= 2;
    at += 2;

    if(onset + ac > size)
      ac = size - onset;
    
    if(i >= 0 && i < m && j >= 0 && j < n)
      fmat_set_from_atoms(self, onset, 1, ac, at);

    fts_return_object(o);
  }
}

static void
fmat_set_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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

    if(row >= 0 && row < m)
    {
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
  
  fts_return_object(o);
}

static void
fmat_set_col(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
    
    if(col >= 0 && col < n)
    {
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
  
  fts_return_object(o);
}

static void
fmat_fill_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 0 && fts_is_number(at))
    fmat_set_const(self, (float)fts_get_number_float(at));

  fts_return_object(o);
}

static void
fmat_fill_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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

      fts_set_void(fts_get_return_value());
    }
  }
  
  fts_return_object(o);
}

static void
fmat_zero(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *ptr = fmat_get_ptr(self);
  int onset = 0;
  int range = size;
  int i;
  
  if(ac > 0 && fts_is_number(at))
    onset = fts_get_number_int(at);

  if(ac > 1 && fts_is_number(at + 1))
    range = fts_get_number_int(at + 1);

  if(onset + range > size)
    range = size - onset;
  
  for(i=onset; i<range+onset; i++)
    ptr[i] = 0.0;
  
  fts_return_object(o);
}

static void
_fmat_get_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_atom_t a[2];
  fts_atom_t t;
  
  fts_set_int(a + 0, fmat_get_m(self));
  fts_set_int(a + 1, fmat_get_n(self));
  fts_set_object(&t, fts_object_create(fts_tuple_class, 2, a));
  
  fts_return(&t);
}

static void
_fmat_get_m(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_atom_t a;
  
  fts_set_int(&a, fmat_get_m(self));
  
  fts_return(&a);
}

static void
_fmat_get_n(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_atom_t a;
  
  fts_set_int(&a, fmat_get_n(self));
  
  fts_return(&a);
}

static void
_fmat_set_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int m = 0;
  int n = 0;
  
  if(ac > 1 && fts_is_number(at) && fts_is_number(at + 1))
  {
    m = fts_get_number_int(at);
    n = fts_get_number_int(at + 1);
    
    if(m >= 0 && n >= 0)
      fmat_set_size(self, m, n);
  }
  
  fts_return_object(o);
}

static void
_fmat_set_m(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int m = fts_get_number_int(at);

  if(m >= 0)
    fmat_set_m(self, m);
  
  fts_return_object(o);
}

static void
_fmat_set_n(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int n = fts_get_number_int(at);
  
  if(n >= 0)
    fmat_set_n(self, n);
  
  fts_return_object(o);
}

/* called by get element message */
static void
_fmat_get_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int i = 0;
  int j = 0;
  
  if(ac > 0 && fts_is_number(at))
    i = fts_get_number_int(at);

  if(ac > 1 && fts_is_number(at + 1))
    j = fts_get_number_int(at  + 1);

  if(i >= 0 && i < fmat_get_m(self) && j >= 0 && j < fmat_get_n(self))
    fts_return_float(fmat_get_element(self, i, j));
}

static void
_fmat_get_interpolated(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  double ret = 0.0;
  
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
      ret = (1.0 - i_frac) * ptr[i * n + j] + i_frac * ptr[(i + 1) * n + j];
      
      if(j_frac != 0.0)
      {
        double g = (1.0 - i_frac) * ptr[i * n + j + 1] + i_frac * ptr[(i + 1) * n + j + 1];
        
        ret = (1.0 - j_frac) * ret + j_frac * g;
      }
    }
    else if(j_frac != 0.0)
      ret = (1.0 - j_frac) * ptr[i * n + j] + j_frac * ptr[i * n + j + 1];
    else
      ret = ptr[i * n + j];
  }
  
  fts_return_float(ret);
}


static void
fmat_get_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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

  fts_return_object((fts_object_t *)tuple);
}

static void
fvec_pick_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *source = (fmat_t *)fts_get_object(at);
  int source_size = fvec_get_size(source);
  int size = fvec_get_size(self);
  float *src = fvec_get_ptr(source);
  float *ptr = fvec_get_ptr(self);
  int onset = 0;
  int i;

  if(ac > 1 && fts_is_number(at + 1))
    onset = fts_get_number_int(at + 1);

  if(ac > 2 && fts_is_number(at + 2))
    size = fts_get_number_int(at + 2);

  if(onset + size > source_size)
    size = source_size - onset;

  if(size > 0)
  {
    fvec_set_size(self, size);

    for(i=0; i<size; i++)
      ptr[i] = src[onset + i];
  }
  
  fts_return_object(o);
}


static void 
fmat_get_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_atom_t a[2];
  
  fts_set_object(a, o);
  a[1] = at[0];
  obj = fts_object_create(frow_class, 2, a);
  
  fts_return_object(obj);
}

static void 
fmat_get_col(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_atom_t a[2];
  
  fts_set_object(a, o);
  a[1] = at[0];
  obj = fts_object_create(fcol_class, 2, a);
  
  fts_return_object(obj);
}




/******************************************************************************
 *
 * functions, i.e. methods that return a value but don't change the object
 *
 * todo: to be called in functional syntax, e.g. (.max $myfvec)
 */

static void
fmat_get_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const fmat_t *self = (fmat_t *) o;
  const int size = fmat_get_m(self) * fmat_get_n(self);
  
  if(size > 0)
  {
    const float *p = fvec_get_ptr(self);
    float min = p[0];
    int i;

    for (i = 1; i < size; i++)
      if (p[i] < min)
        min = p[i];
    
    fts_return_float(min);
  }
}

static void
fmat_get_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
    
    fts_return_float(max);
  }
}

static void
fmat_get_absmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
        max = p[i];
    
    fts_return_float(max);
  }
}

static void
fmat_get_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const fmat_t *self = (fmat_t *) o;
  const int size = fmat_get_m(self) * fmat_get_n(self);
  const float *p = fmat_get_ptr(self);
  double sum = 0.0;
  int i;
  
  for (i=0; i<size; i++)
    sum += p[i];
  
  fts_return_float(sum);
}

static void
fmat_get_mean(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
    
    fts_return_float(sum / (double)size);
  }
  else
    fts_return_float(0.0);
}

static void
fmat_get_zc(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
    
    fts_return_int(zc);
  }
}




/******************************************************************************
 *
 *  real arithmetics
 *
 */

static void
fmat_add_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);

  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;

    for(i=0; i<size; i++)
      l[i] += r[i];
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "add");
}

static void
fmat_add_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] += r;
  
  fts_return_object(o);
}

static void
fmat_sub_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] -= r[i];
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "sub");
}

static void
fmat_sub_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] -= r;
  
  fts_return_object(o);
}

static void
fmat_mul_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] *= r[i];
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "mul");
}

static void
fmat_mul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] *= r;
  
  fts_return_object(o);
}

static void
fmat_div_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] /= r[i];
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "div");
}

static void
fmat_div_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] /= r;
  
  fts_return_object(o);
}

static void
fmat_bus_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = r[i] - l[i];
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "bus");
}

static void
fmat_bus_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = r - p[i];
  
  fts_return_object(o);
}

static void
fmat_vid_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = r[i] / l[i];
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "vid");
}

static void
fmat_vid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = r / p[i];
  
  fts_return_object(o);
}




/******************************************************************************
 *
 *  real comparison
 *
 */

static void
fmat_ee_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] == r[i]);
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "ee");
}

static void
fmat_ee_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] == r);
  
  fts_return_object(o);
}

static void
fmat_ne_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] != r[i]);
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "ne");
}

static void
fmat_ne_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] != r);
  
  fts_return_object(o);
}

static void
fmat_gt_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);

  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] > r[i]);
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "gt");
}

static void
fmat_gt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] > r);
  
  fts_return_object(o);
}

static void
fmat_ge_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] >= r[i]);
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "ge");
}

static void
fmat_ge_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] >= r);
  
  fts_return_object(o);
}

static void
fmat_lt_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] < r[i]);
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "lt");
}

static void
fmat_lt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] < r);
  
  fts_return_object(o);
}

static void
fmat_le_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    int size = m * n;
    int i;
    
    for(i=0; i<size; i++)
      l[i] = (float)(l[i] <= r[i]);
  
    fts_return_object(o);
  }
  else
    fmat_error_dimensions(self, right, "le");
}

static void
fmat_le_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);
  int i;

  for(i=0; i<size; i++)
    p[i] = (float)(p[i] <= r);
  
  fts_return_object(o);
}




/******************************************************************************
 *
 *  complex arithmetics
 *
 */
static void
fmat_cmul_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_format_t *format = fmat_get_format(self);
  int id = fmat_format_get_id(format);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  fmat_t *right = (fmat_t *)fts_get_object(at);
  fmat_format_t *right_format = fmat_get_format(right);
  int right_id = fmat_format_get_id(right_format);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  
  if(right_m == m && id <= fmat_format_id_polar && right_id <= fmat_format_id_polar)
  {
    float *r = fmat_get_ptr(right);
    float *l;
    int i;
    
    switch(id)
    {
      case fmat_format_id_vec:
        switch(right_id)
        {
          case fmat_format_id_vec:
            /* vec * vec */
            l = fmat_get_ptr(self);
            for(i=0; i<m; i++)
            {
              l[i] *= r[i];
            }
              break;
            
          case fmat_format_id_rect:
            /* vec * rect */
            fmat_reshape(self, m, 2);
            l = fmat_get_ptr(self);
            for(i=m-1; i>=0; i--)
            {
              l[2 * i] = l[i] * r[2 * i];
              l[2 * i + 1] = l[i] * r[2 * i + 1];
            }
              break;
            
          case fmat_format_id_polar:
            /* vec * polar */
            fmat_reshape(self, m, 2);
            l = fmat_get_ptr(self);
            for(i=m-1; i>=0; i--)
            {
              l[2 * i] = l[i] * r[2 * i];
              l[2 * i + 1] = r[2 * i + 1];
            }
              break;
            
          default:
            break;
        }    
        break;
        
      case fmat_format_id_rect:
        switch(right_id)
        {
          case fmat_format_id_vec:
            /* rect * vec */
            l = fmat_get_ptr(self);
            for(i=0; i<m; i++)
            {
              l[2 * i] *= r[i];
              l[2 * i + 1] *= r[i];
            }
              break;
            
          case fmat_format_id_rect:
            /* rect * rect */
            l = fmat_get_ptr(self);
            for(i=0; i<2*m; i+=2)
            {
              l[i] = l[i] * r[i] - l[i + 1] * r[i + 1];
              l[i + 1] = l[i] * r[i + 1] + l[i + 1] * r[i];
            }
              break;
            
          case fmat_format_id_polar:
            /* rect * polar */
            l = fmat_get_ptr(self);
            for(i=0; i<2*m; i+=2)
            {
              float r_re = r[i] * cosf(r[i + 1]);
              float r_im = r[i] * sinf(r[i + 1]);
              l[i] = l[i] * r_re - l[i + 1] * r_im;
              l[i + 1] = l[i] * r_im+ l[i + 1] * r_re;
            }
              break;
            
          default:
            break;
        }    
        break;
        
      case fmat_format_id_polar:
        switch(right_id)
        {
          case fmat_format_id_vec:
            /* polar * polar */
            l = fmat_get_ptr(self);
            for(i=0; i<m; i++)
            {
              l[2 * i] *= r[i];
            }
              break;
            
          case fmat_format_id_rect:
            /* polar * rect */
            l = fmat_get_ptr(self);
            for(i=0; i<2*m; i+=2)
            {
              float r_re = r[i];
              float r_im = r[i + 1];
              l[i] *= sqrtf(r_re * r_re + r_im * r_im);
              l[i + 1] += atan2f(r_im, r_re);
            }
              break;
            
          case fmat_format_id_polar:
            /* polar * polar */
            l = fmat_get_ptr(self);
            for(i=0; i<2*m; i+=2)
            {
              l[i] *= r[i];
              l[i + 1] += r[i + 1];
            }
              break;
            
          default:
            break;
        }    
        break;
        
      default:
        break;
    }
    
    fts_return_object(o);
  }
  else
  {
    fts_object_error((fts_object_t *)self, "cmul: can't multiply %s matrix %d x %d with %s matrix %d x %d", 
                     fts_symbol_name(fmat_format_get_name(format)), m, n,
                     fts_symbol_name(fmat_format_get_name(right_format)), right_m, right_n);
  }
}

static void
fmat_cmul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_format_t *format = fmat_get_format(self);
  int id = fmat_format_get_id(format);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(id <= fmat_format_id_polar)
  {
    float r = fts_get_number_float(at);
    float *l = fmat_get_ptr(self);
    int i;
    
    switch(id)
    {
      case fmat_format_id_vec:
        for(i=0; i<m; i++)
          l[i] *= r;              
        break;
            
      case fmat_format_id_rect:
        for(i=0; i<2*m; i++)
          l[2 * i] *= r;
        break;
        
      case fmat_format_id_polar:
        for(i=0; i<2*m; i+=2)
          l[2 * i] *= r;
        break;
        
      default:
        break;
    }
  
    fts_return_object(o);
  }
  else
    fts_object_error((fts_object_t *)self, "cmul: can't multiply %s matrix %d x %d with number", fts_symbol_name(fmat_format_get_name(format)), m, n);
}




/******************************************************************************
 *
 *  vector and matrix multiplication
 *
 */
static void
fmat_get_dot(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  
  if(fmat_get_m(right) == m && fmat_get_n(right) == n && n == 1)
  {
    float *l = fmat_get_ptr(self);
    float *r = fmat_get_ptr(right);
    float sum = 0.0;
    int i;
    
    for(i=0; i<m; i++)
      sum += l[i] * r[i];
    
    fts_return_float(sum);
  }
  else if(n == 1)
    fmat_error_dimensions(self, right, "dot");
  else
    fts_object_error(o, "dot: matrices must be column vectors");
}




/******************************************************************************
 *
 *  mixed real/complex math funs
 *
 */
static void
fmat_abs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i, j;
  
  switch(id)
  {
    case fmat_format_id_vec:
      for(i=0; i<n; i++)
        ptr[i] = fabsf(ptr[i]);
      break;
      
    case fmat_format_id_rect:
      for(i=0, j=0; i<m; i++, j+=2)
      {
        float re = ptr[j];
        float im = ptr[j + 1];
        
        ptr[i] = sqrtf(re * re + im * im);
      }
      
      fmat_reshape(self, m, 1);
      break;
      
    case fmat_format_id_polar:
      for(i=0, j=0; i<m; i++, j+=2)
        ptr[i] = ptr[j];
      
      fmat_reshape(self, m, 1);
      break;
      
    default:
      for(i=0; i<m*n; i++)
        ptr[i] = fabsf(ptr[i]);
      break;
  }
  
  fts_return_object(o);
}

static void
fmat_logabs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i, j;
  
  switch(id)
  {
    case fmat_format_id_vec:
      for(i=0; i<m; i++)
        ptr[i] = logf(fabsf(ptr[i]));
      break;
      
    case fmat_format_id_rect:
      for(i=0, j=0; i<m; i++, j+=2)
      {
        float re = ptr[j];
        float im = ptr[j + 1];
        
        ptr[i] = (float)(0.5 * logf(re * re + im * im));
      }
      
      fmat_reshape(self, m, 1);
      break;
      
    case fmat_format_id_polar:
      for(i=0, j=0; i<m; i++, j+=2)
        ptr[i] = logf(ptr[j]);
      
      fmat_reshape(self, m, 1);
      break;
      
    default:
      for(i=0; i<m*n; i++)
        ptr[i] = logf(fabsf(ptr[i]));
      break;
  }
  
  fts_return_object(o);
}

static void
fmat_log(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  switch(id)
  {
    case fmat_format_id_vec:
      for(i=0; i<m; i++)
        ptr[i] = logf(ptr[i]);
      break;
      
    case fmat_format_id_rect:
      for(i=0; i<2*m; i+=2)
      {
        float re = ptr[i];
        float im = ptr[i + 1];
        
        ptr[i] = (float)(0.5 * log(re * re + im * im));
        ptr[i + 1] = (float)atan2(im, re);
      }
      break;
      
    case fmat_format_id_polar:
      for(i=0; i<2*m; i+=2)
      {
        float re = logf(ptr[i]);
        float im = ptr[i + 1];
        
        ptr[i] = sqrtf(re * re + im * im);
        ptr[i + 1] = (float)atan2(im, re);
      }
      break;
      
    default:
      for(i=0; i<m*n; i++)
        ptr[i] = logf(ptr[i]);
      break;
  }
  
  fts_return_object(o);
}

static void
fmat_exp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  switch(id)
  {
    case fmat_format_id_vec:
      for(i=0; i<m; i++)
        ptr[i] = expf(ptr[i]);
      break;
      
    case fmat_format_id_rect:
      for(i=0; i<2*m; i+=2)
      {
        float mag = expf(ptr[i]);
        float arg = ptr[i + 1];
        
        ptr[i] = mag * cosf(arg);
        ptr[i + 1] = mag * sinf(arg);
      }
      break;
      
    case fmat_format_id_polar:
      for(i=0; i<2*m; i+=2)
      {
        float mag = ptr[i];
        float arg = ptr[i + 1];
        
        ptr[i] = expf(mag * cosf(arg));
        ptr[i + 1] = mag * sinf(arg);
      }
      break;
      
    default:
      for(i=0; i<m*n; i++)
        ptr[i] = expf(ptr[i]);
      break;
  }
  
  fts_return_object(o);
}

static void
fmat_sqrabs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i, j;
  
  switch(id)
  {
    case fmat_format_id_vec:
      for(i=0; i<m; i++)
        ptr[i] *= ptr[i];
      break;
      
    case fmat_format_id_rect:
      for(i=0, j=0; i<m; i++, j+=2)
      {
        float re = ptr[j];
        float im = ptr[j + 1];
        
        ptr[i] = re * re + im * im;
      }
      
      fmat_reshape(self, m, 1);
      break;
      
    case fmat_format_id_polar:
      for(i=0, j=0; i<m; i++, j+=2)
        ptr[i] = ptr[j] * ptr[j];
      
      fmat_reshape(self, m, 1);
      break;
      
    default:
      for(i=0; i<m*n; i++)
        ptr[i] *= ptr[i];
      break;
  }
  
  fts_return_object(o);
}

static void
fmat_sqrt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int i;
  
  switch(id)
  {
    case fmat_format_id_vec:
      for(i=0; i<m; i++)
        ptr[i] = sqrtf(ptr[i]);
      break;
      
    case fmat_format_id_rect:
      for(i=0; i<m; i+=2)
      {
        float re = ptr[i];
        float im = ptr[i + 1];
        float mag = sqrtf(re * re + im * im);
        float arg = atan2f(im, re);
        
        mag = sqrtf(mag);
        arg *= 0.5;
            
        ptr[i] = mag * cosf(arg);
        ptr[i + 1] = mag * sinf(arg);
      }
      break;
      
    case fmat_format_id_polar:
      for(i=0; i<m; i+=2)
      {
        float mag = ptr[i];
        float arg = ptr[i + 1];
        
        ptr[i] = sqrtf(mag * cosf(arg));
        ptr[i + 1] = 0.5 * mag * sinf(arg);
      }
      break;
      
    default:
      for(i=0; i<m*n; i++)
        ptr[i] = sqrtf(ptr[i]);
      break;
  }
  
  fts_return_object(o);
}

static void
fmat_fft(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  
  switch(id)
  {
    case fmat_format_id_vec:
    {
      /* real FFT */
      int size = fmat_get_m(self);
      int fft_size = fts_get_fft_size(size);
      float *fft_ptr;
      int i;
      
      fmat_reshape(self, fft_size/2, 2);
      
      fft_ptr = fmat_get_ptr(self);
      
      /* zero padding */
      for(i=size; i<fft_size; i++)
        fft_ptr[i] = 0.0;
      
      fts_rfft_inplc(fft_ptr, fft_size);
      
      /* cast fmat to complex rect */
      fmat_set_format(self, fmat_format_rect);
    } 
      break;
      
    case fmat_format_id_rect:
    {
      /* complex FFT */
      int size = fmat_get_m(self);
      unsigned int fft_size = fts_get_fft_size(size);
      complex *fft_ptr;
      unsigned int i;
      
      fmat_reshape(self, fft_size, 2);
      fft_ptr = (complex *)fmat_get_ptr(self);
      
      /* zero padding */      
      for(i=size; i<fft_size; i++)
        fft_ptr[i].re = fft_ptr[i].im = 0.0;
      
      fts_cfft_inplc(fft_ptr, fft_size);
    }
      break;
      
    case fmat_format_id_polar:
    default:
      /* fmat format error */
      break;
  }
  
  fts_return_object(o);
}

static void
fmat_rifft(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  int m = fmat_get_m(self);
  int size = 2 * m;
  unsigned int fft_size = fts_get_fft_size(size);
  float *ptr;
  int i;
  
  fmat_reshape(self, fft_size, 1);
  ptr = fmat_get_ptr(self);

  switch(id)
  {
    case fmat_format_id_vec:
      /* fill real part */
      for(i=m-1; i>=0; i--)
      {
        ptr[2 * i + 1] = 0.0;
        ptr[2 * i] = ptr[i];
      }
      
    case fmat_format_id_rect:
    {
      /* zero padding */      
      for(i=size; i<fft_size; i++)
        ptr[i] = 0.0;
      
      fts_rifft_inplc(ptr, fft_size);
    }
      break;
      
    case fmat_format_id_polar:
    default:
      /* fmat format error */
      break;
  }
  
  fts_return_object(o);
}

static void
fmat_normalize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
      float scale = max;
      
      for(i=0; i<size; i++)
        ptr[i] *= scale;
    }
  }
  
  fts_return_object(o);
}




/********************************************************************
 *
 *  row order operations
 *
 */

static void
fmat_reverse(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
    
    for(j=0; j<n; j++)
    {
      float f = ptr[i];
      
      ptr[j] = rtp[j];
      rtp[j] = f;
    }
    
    ptr += n;
  }
  
  fts_return_object(o);
}

static void
fmat_rotate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
  
  fts_return_object(o);
}

static int 
fmat_element_compare_ascending(const void *left, const void *right)
{
  float l = ((const float *)left)[0];
  float r = ((const float *)right)[0];
  
  return (r < l) - (l < r);
}

static int 
fmat_element_compare_descending(const void *left, const void *right)
{
  float l = ((const float *)left)[0];
  float r = ((const float *)right)[0];
  
  return (l < r) - (r < l);
}

static void
fmat_sort(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self); 
  int col = 0;
  
  if(ac > 0)
    col = fts_get_number_int(at);

  if(col == 0)
    qsort((void *)ptr, m, n * sizeof(float), fmat_element_compare_ascending);
  else if(col > 0 && col < n)
  {
    int i;
    
    for(i=0; i<m*n; i+=n)
    {
      float f = ptr[i];
      
      ptr[i] = ptr[i + col];
      ptr[i + col] = f;
    }
    
    qsort((void *)ptr, m, n * sizeof(float), fmat_element_compare_ascending);

    for(i=0; i<m*n; i+=n)
    {
      float f = ptr[i];
      
      ptr[i] = ptr[i + col];
      ptr[i + col] = f;
    }    
  }
  
  fts_return_object(o);
}

static void
fmat_tros(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self); 
  int col = 0;
  
  if(ac > 0)
    col = fts_get_number_int(at);

  if(col == 0)
    qsort((void *)ptr, m, n * sizeof(float), fmat_element_compare_descending);
  else if(col > 0 && col < n)
  {
    int i;
    
    for(i=0; i<m*n; i+=n)
    {
      float f = ptr[i];
      
      ptr[i] = ptr[i + col];
      ptr[i + col] = f;
    }
    
    qsort((void *)ptr, m, n * sizeof(float), fmat_element_compare_descending);

    for(i=0; i<m*n; i+=n)
    {
      float f = ptr[i];
      
      ptr[i] = ptr[i + col];
      ptr[i + col] = f;
    }    
  }
  
  fts_return_object(o);
}

static void
fmat_scramble(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
  
  fts_return_object(o);
}




/******************************************************************************
 *
 *  envelopes
 *
 */
static void
fmat_fade(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
  
  fts_return_object(o);
}

static void
fmat_lookup_fmat_or_slice(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
  
  fts_return_object(o);
}

static void
fmat_lookup_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  bpf_t *bpf = (bpf_t *)fts_get_object(at);
  int i;
  
  for(i=0; i<m*n; i++)
    ptr[i] = bpf_get_interpolated(bpf, ptr[i]);
  
  fts_return_object(o);
}
    
static void
fmat_env_fmat_or_slice(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
  
  fts_return_object(o);
}

static void
fmat_env_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
  
  fts_return_object(o);
}

static void
fmat_apply_expr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  expr_t *expr = (expr_t *)fts_get_object(at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr = fmat_get_ptr(self);
  fts_hashtable_t locals;
  fts_atom_t key_self, key_x, value;
  fts_atom_t ret;
  int i;
  
  fts_hashtable_init(&locals, FTS_HASHTABLE_SMALL);
  
  fts_set_symbol(&key_self, fts_s_self);
  fts_set_object(&value, self);
  fts_hashtable_put(&locals, &key_self, &value);
  
  fts_set_symbol(&key_x, fts_s_x);

  /* apply expression to each value */  
  for(i=0; i<m*n; i++)
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




/******************************************************************************
 *
 *  format conversion
 *
 */
static void
fmat_convert_vec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  if(self->format != fmat_format_vec)
  {
    float *ptr = fmat_get_ptr(self);
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int i, j;
  
    for(i=0, j=0; i<m; i++, j+=n)
      ptr[i] = ptr[j];
      
    fmat_reshape(self, m, 1);
  }
  
  fts_return_object(o);
}

static void
fmat_convert_rect(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr;
  int i, j;
  
  switch(id)
  {
    case fmat_format_id_vec:
      fmat_reshape(self, m, 2);
      ptr = fmat_get_ptr(self);
      for(i=2*(m-1), j=m-1; i>=0; i-=2, j--)
      {
        ptr[i] = ptr[j]; /* real part */
        ptr[i + 1] = 0.0; /* imaginary part */
      }
      break;
    
    case fmat_format_id_rect:
      break;
    
    case fmat_format_id_polar:
      ptr = fmat_get_ptr(self);
      for(i=0; i<2*m; i+=2)
      {
        float mag = ptr[i];
        float arg = ptr[i + 1];
      
        ptr[i] = mag * cosf(arg); /* real part */
        ptr[i + 1] = mag * sinf(arg); /* imaginary part */
      }
      break;
    
    default:
      ptr = fmat_get_ptr(self);
      for(i=0, j=0; i<2*m; i+=2, j+=n)
      {
        ptr[i] = ptr[j]; /* real part */
        ptr[i + 1] = 0.0; /* imaginary part */
      }      
      fmat_reshape(self, m, 2);
      break;          
  }
  
  fmat_set_format(self, fmat_format_rect);
  
  fts_return_object(o);
}

static void
fmat_convert_polar(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  float *ptr;
  int i, j;
  
  switch(id)
  {
    case fmat_format_id_vec:
      fmat_reshape(self, m, 2);
      ptr = fmat_get_ptr(self);
      for(i=2*(m-1), j=m-1; i>=0; i-=2, j--)
      {
        ptr[i] = ptr[j];
        ptr[i + 1] = 0.0;
      }
      break;
    
    case fmat_format_id_rect:
      ptr = fmat_get_ptr(self);
      for(i=0; i<2*m; i+=2)
      {
        float re = ptr[i];
        float im = ptr[i + 1];
      
        ptr[i] = sqrtf(re * re + im * im);
        ptr[i + 1] = atan2f(im, re);
      }
      break;
    
    case fmat_format_id_polar:
      break;
    
    default:
      ptr = fmat_get_ptr(self);
      for(i=0, j=0; i<2*m; i+=2, j+=n)
      {
        ptr[i] = ptr[j];
        ptr[i + 1] = 0.0;
      }
          
      fmat_reshape(self, m, 2);
      break;
  }
  
  fmat_set_format(self, fmat_format_rect);
  
  fts_return_object(o);
}

static void
fmat_convert_real(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int id = fmat_format_get_id(self->format);
  float *ptr = fmat_get_ptr(self);
  int m = fmat_get_m(self);
  int i, j;
  
  switch(id)
  {
    case fmat_format_id_vec:
      break;
      
    case fmat_format_id_rect:
      for(i=0, j=0; i<m; i++, j+=2)
        ptr[i] = ptr[j];          
      fmat_reshape(self, m, 1);
      break;
      
    case fmat_format_id_polar:
      for(i=0, j=0; i<m; i++, j+=2)
        ptr[i] = ptr[j] * cosf(ptr[j + 1]);
      fmat_reshape(self, m, 1);
      break;
      
    default:
      break;
  }
  
  fts_return_object(o);
}




/******************************************************************************
 *
 *  load, save, import, export
 *
 */

static const char str_aiff[3] = "aif";

/* well this works for "aif" and "aiff" and "aifff" and "aaiiiffff" and so on  */
static int
postfix_aiff(fts_symbol_t name)
{
  const char *str = fts_symbol_name(name);
  int n = strlen(str) - 1;
  int i = strlen(str_aiff) - 1;

  while(i >= 0 && str[n] == str_aiff[i])
  {
    while(n >= 0 && str[n] == str_aiff[i])
      n--;
    
    i--;
  }
  
  return (i < 0);
}

static void
fmat_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;

  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t file_name = fts_get_symbol(at);
    int size = 0;
    
    if(postfix_aiff(file_name))
      size = fmat_import_audiofile(self, file_name);
    else
      size = fmat_read_atom_file(self, file_name);
    
    if(size > 0)
      fts_return_object(o);
    else
      fts_object_error(o, "can't import from file \"%s\"", fts_symbol_name(file_name));
  }
}

static void
fmat_import_dialog(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fts_object_open_dialog(o, fts_s_import, fts_new_symbol("import file"));
}

static void
fmat_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t file_name = fts_get_symbol(at);
    int size = 0;
    
    if(postfix_aiff(file_name))
      size = fmat_export_audiofile(self, file_name);
    else
      size = fmat_write_atom_file(self, file_name);
    
    if(size > 0)
      fts_return_object(o);
    else
      fts_object_error(o, "can't export to file \"%s\"", fts_symbol_name(file_name));
  }
}

static void
fmat_export_dialog(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fts_object_save_dialog(o, fts_s_export, fts_new_symbol("export fmat"), fts_project_get_dir(), fts_new_symbol(".aiff"));
}




/*********************************************************
 *
 *  editor
 *
 */

static void 
fmat_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_atom_t a;
  
  if(self->editor == NULL)
  {
    fts_set_object(&a, o);
    /* self->editor = fts_object_create(tabeditor_type, 1, &a); */
    fts_object_refer( self->editor);
  }
  
  if(!fts_object_has_id( self->editor))
  {
    fts_client_register_object( self->editor, fts_object_get_client_id( o));
	  
    fts_set_int(&a, fts_object_get_id( self->editor));
    fts_client_send_message( o, fts_s_editor, 1, &a);
    
    /* fts_send_message( (fts_object_t *)self->editor, fts_s_upload, 0, 0); */
  }
  
  fmat_editor_set_open( self);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
}

static void
fmat_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  fmat_editor_set_close( self);
}

static void 
fmat_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *) o;
  
  if(fmat_editor_is_open(self))
  {
    fmat_editor_set_close(self);
    fts_client_send_message((fts_object_t *)self, fts_s_closeEditor, 0, 0);  
  }
}




/********************************************************************
 *
 *  system functions
 *
 */

static void
fmat_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  fmat_format_t *format = fmat_get_format(self);
  int size = m * n;
  fts_bytestream_t* stream = fts_get_default_console_stream();
  int i, j;
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  if(size == 0)
    fts_spost(stream, "<empty fmat of %s>\n", fts_symbol_name(fmat_format_get_name(format)));
  else
  {
    fts_spost(stream, "<fmat %d x %d of %s>\n", m, n, fts_symbol_name(fmat_format_get_name(format)));
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
  
  fts_return_object(o);
}

static void
fmat_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  float *data = self->values;
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  fts_message_t *mess;
  int i, j;

  /* dump size message */
  mess = fts_dumper_message_new(dumper, fts_s_size);
  fts_message_append_int(mess, m);
  fts_message_append_int(mess, n);
  fts_dumper_message_send(dumper, mess);

  for(i=0; i<m; i++)
  {
    /* new row */
    mess = fts_dumper_message_new(dumper, fts_s_row);
    fts_message_append_int(mess, i);

    for(j=0; j<n; j++)
    {
      float f = data[i * n + j];

      fts_message_append_float(mess, f);
    }
    
    fts_dumper_message_send(dumper, mess);
  }
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
  self->sr = fts_dsp_get_sample_rate();
  self->format = fmat_format_real;
  self->opened = 0;
  self->editor = NULL;  
}

static void
fmat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int m = 0;
  int n = 1;

  fmat_initialize(self);
  
  if(ac > 0 && fts_is_number(at))
    m = fts_get_number_int(at);
  
  if(ac > 1)
  {
    if(fts_is_number(at + 1))
      n = fts_get_number_int(at + 1);
    else if(fts_is_symbol(at + 1))
    {
      fts_symbol_t format_name = fts_get_symbol(at + 1);
      fmat_format_t *format = fmat_format_get_by_name(format_name);
      
      if(format != NULL)
      {
        n = format->n_columns;
        self->format = format;
      }
      else
        fts_object_error(o, "unknown fmat format: %s", fts_symbol_name(format_name));
    }
    else
      fts_object_error(o, "bad column argument");
  }
  
  if(ac > 2)
  {
    int size = m * n;
    
    ac -= 2;
    at += 2;
    
    if(ac > size)
      ac = size;
    
    fmat_reshape(self, m, n);  
    fmat_set_from_atoms(self, 0, 1, ac, at);
  }
  else
    fmat_set_size(self, m, n);  
}

static void
fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  fmat_initialize(self);
  
  if(ac == 0)
    fmat_set_size(self, 0, 1);
  else if(ac == 1)
  {
    int size = 0;
    
    if(fts_is_number(at))
    {
      size = fts_get_number_int(at);
     
      if(size < 0)
        size = 0;
    }
    
    fmat_set_size(self, size, 1);
  }  
  else
  {
    fmat_reshape(self, ac, 1);
    fmat_set_from_atoms(self, 0, 1, ac, at);
  }
  
  /* hack: there won't actually be any object of fvec_class */
  o->cl = fmat_class;
}

static void
fmat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;

  if(self->values != NULL)
    fts_free(self->values - HEAD_POINTS);
}




/*********************************************************
 *
 *  class instantiate
 *
 */

static void
fmat_message(fts_class_t *cl, fts_symbol_t s, fts_method_t marix_method, fts_method_t scalar_method)
{
	if(marix_method != NULL)
		fts_class_message(cl, s, fmat_class, marix_method);

	if(scalar_method != NULL)
		fts_class_message_number(cl, s, scalar_method);
}

static void
fmat_instantiate(fts_class_t *cl)
{
  if(cl == fmat_class)
    fts_class_init(cl, sizeof(fmat_t), fmat_init, fmat_delete);
  else
    fts_class_init(cl, sizeof(fmat_t), fvec_init, fmat_delete);    
  
  fts_class_set_copy_function(cl, fmat_copy_function);
  fts_class_set_equals_function(cl, fmat_equals_function);
  fts_class_set_post_function(cl, fmat_post_function);
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
  fts_class_message(cl, fts_s_set, ivec_type, fmat_set_from_ivec);

  fts_class_message_number(cl, fts_s_row, fmat_get_row);
  fts_class_message_varargs(cl, fts_s_row, fmat_set_row);

  fts_class_message_number(cl, fts_s_col, fmat_get_col);
  fts_class_message_varargs(cl, fts_s_col, fmat_set_col);
  
  fts_class_message_number(cl, fts_s_fill, fmat_fill_number);
  fts_class_message_varargs(cl, fts_s_fill, fmat_fill_varargs);
  fts_class_message_varargs(cl, fts_new_symbol("zero"), fmat_zero);
  
  fts_class_message_void(cl, fts_s_size, _fmat_get_size);
  fts_class_message_varargs(cl, fts_s_size, _fmat_set_size);
  
  fts_class_message_void(cl, fts_new_symbol("rows"), _fmat_get_m);
  fts_class_message_number(cl, fts_new_symbol("rows"), _fmat_set_m);

  fts_class_message_void(cl, fts_new_symbol("cols"), _fmat_get_n);
  fts_class_message_number(cl, fts_new_symbol("cols"), _fmat_set_n);  

  fts_class_message_void(cl, fts_new_symbol("min"), fmat_get_min);
  fts_class_message_void(cl, fts_new_symbol("max"), fmat_get_max);
  fts_class_message_void(cl, fts_new_symbol("absmax"), fmat_get_absmax);
  fts_class_message_void(cl, fts_new_symbol("sum"), fmat_get_sum);
  fts_class_message_void(cl, fts_new_symbol("mean"), fmat_get_mean);
  fts_class_message_void(cl, fts_new_symbol("zc"), fmat_get_zc);
  
  fmat_message(cl, fts_new_symbol("add"), fmat_add_fmat, fmat_add_number);
  fmat_message(cl, fts_new_symbol("sub"), fmat_sub_fmat, fmat_sub_number);
  fmat_message(cl, fts_new_symbol("mul"), fmat_mul_fmat, fmat_mul_number);
  fmat_message(cl, fts_new_symbol("div"), fmat_div_fmat, fmat_div_number);
  fmat_message(cl, fts_new_symbol("bus"), fmat_bus_fmat, fmat_bus_number);
  fmat_message(cl, fts_new_symbol("vid"), fmat_vid_fmat, fmat_vid_number);
  fmat_message(cl, fts_new_symbol("ee"), fmat_ee_fmat, fmat_ee_number);
  fmat_message(cl, fts_new_symbol("ne"), fmat_ne_fmat, fmat_ne_number);
  fmat_message(cl, fts_new_symbol("gt"), fmat_gt_fmat, fmat_gt_number);
  fmat_message(cl, fts_new_symbol("ge"), fmat_ge_fmat, fmat_ge_number);
  fmat_message(cl, fts_new_symbol("lt"), fmat_lt_fmat, fmat_lt_number);
  fmat_message(cl, fts_new_symbol("le"), fmat_le_fmat, fmat_le_number);

  fmat_message(cl, fts_new_symbol("cmul"), fmat_cmul_fmat, fmat_cmul_number);
  fts_class_message(cl, fts_new_symbol("dot"), fmat_class, fmat_get_dot);
  
  fts_class_message_void(cl, fts_new_symbol("abs"), fmat_abs);
  fts_class_message_void(cl, fts_new_symbol("logabs"), fmat_logabs);
  fts_class_message_void(cl, fts_new_symbol("log"), fmat_log);
  fts_class_message_void(cl, fts_new_symbol("exp"), fmat_exp);
  fts_class_message_void(cl, fts_new_symbol("sqrabs"), fmat_sqrabs);
  fts_class_message_void(cl, fts_new_symbol("sqrt"), fmat_sqrt);
  
  fts_class_message_void(cl, fts_new_symbol("fft"), fmat_fft);
  fts_class_message_void(cl, fts_new_symbol("rifft"), fmat_rifft);  
  
  fts_class_message_void(cl, fts_new_symbol("normalize"), fmat_normalize);
  
  fts_class_message_void(cl, fts_new_symbol("sort"), fmat_sort);
  fts_class_message_number(cl, fts_new_symbol("sort"), fmat_sort);
  fts_class_message_void(cl, fts_new_symbol("tros"), fmat_tros);
  fts_class_message_number(cl, fts_new_symbol("tros"), fmat_tros);
  fts_class_message_void(cl, fts_new_symbol("rotate"), fmat_rotate);
  fts_class_message_number(cl, fts_new_symbol("rotate"), fmat_rotate);
  fts_class_message_void(cl, fts_new_symbol("reverse"), fmat_reverse);
  fts_class_message_void(cl, fts_new_symbol("scramble"), fmat_scramble);

  fts_class_message(cl, fts_new_symbol("lookup"), cl, fmat_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), fcol_class, fmat_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), frow_class, fmat_lookup_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("lookup"), bpf_type, fmat_lookup_bpf);
  
  fts_class_message(cl, fts_new_symbol("env"), cl, fmat_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), fcol_class, fmat_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), frow_class, fmat_env_fmat_or_slice);
  fts_class_message(cl, fts_new_symbol("env"), bpf_type, fmat_env_bpf);

  fts_class_message(cl, fts_new_symbol("apply"), expr_class, fmat_apply_expr);

  fts_class_message_void(cl, sym_vec, fmat_convert_vec);
  fts_class_message_void(cl, sym_rect, fmat_convert_rect);
  fts_class_message_void(cl, sym_polar, fmat_convert_polar);
  fts_class_message_void(cl, sym_real, fmat_convert_real);
  
  fts_class_message_symbol(cl, fts_s_import, fmat_import);
  fts_class_message_void(cl, fts_s_import, fmat_import_dialog);
  fts_class_message_symbol(cl, fts_s_export, fmat_export);
  fts_class_message_void(cl, fts_s_export, fmat_export_dialog);
  
  fts_class_inlet_bang(cl, 0, data_object_output);
  
  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);
  

  /* 
   * fmat class documentation 
   */
  
  fts_class_doc(cl, fmat_symbol, "[<num: # of rows> [<num: # of columns (def 1)> [<num: init values> ...]]]", "matrix of floats");
  
  fts_class_doc(cl, fts_s_set, "<fmat: matrix>", "set dimension and values from given fmat");
  fts_class_doc(cl, fts_s_set, "<num: row index> <num: column index> [<num:value> ...]" , "set values starting from indicated element (row by row)");
  
  fts_class_doc(cl, fts_s_row, "<num: index>", "get row reference (creates frow object)");
  fts_class_doc(cl, fts_s_row, "<num: index> [<num:value> ...]", "set values of given row from list");
  fts_class_doc(cl, fts_s_row, "<num: index> <frow|fcol: row values>", "set values of given row from frow or fcol");
  
  fts_class_doc(cl, fts_s_col, "<num: index>", "get column reference (creates fcol object)");
  fts_class_doc(cl, fts_s_col, "<num: index> [<num:value> ...]", "set values of given column from list");
  fts_class_doc(cl, fts_s_col, "<num: index> <frow|fcol: col values>", "set values of given column from frow or fcol");
  
  fts_class_doc(cl, fts_s_fill, "<num: value>", "fill with given value or pattern of values");
  fts_class_doc(cl, fts_s_fill, "<expr: expression>", "fill with given expression (use $self, $row and $col)");
  fts_class_doc(cl, fts_new_symbol("zero"), "[<num: row index> <num: column index> [<num: # of elements>]]", "zero given number of elements starting from indicated element (row by row)");
  fts_class_doc(cl, fts_s_size, "[<num: # of rows> [<num: # of columns (def 1)>]]", "get/set dimensions");
  fts_class_doc(cl, fts_new_symbol("rows"), "[<num: # of rows>]", "get/set # of rows");
  fts_class_doc(cl, fts_new_symbol("columns"), "[<num: # of rows>]", "get/set # of columns");
  
  fts_class_doc(cl, fts_new_symbol("min"), NULL, "get minimum value");
  fts_class_doc(cl, fts_new_symbol("max"), NULL, "get maximum value");
  fts_class_doc(cl, fts_new_symbol("absmax"), NULL, "get maximum absolute value");
  fts_class_doc(cl, fts_new_symbol("sum"), NULL, "get sum of all values");
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
  fts_class_doc(cl, fts_new_symbol("ee"), "<num|fmat: operand>", "replace current values by result of == comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("ne"), "<num|fmat: operand>", "replace current values by result of != comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("gt"), "<num|fmat: operand>", "replace current values by result of > comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("ge"), "<num|fmat: operand>", "replace current values by result of >= comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("lt"), "<num|fmat: operand>", "replace current values by result of < comparison (0 or 1) with given scalar or fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("le"), "<num|fmat: operand>", "replace current values by result of <= comparison (0 or 1) with given scalar or fmat (element by element)");
  
  fts_class_doc(cl, fts_new_symbol("cmul"), "<num|fmat: operand>", "multiply current values of complex vector (rect or polar format) by given scalar or complex vector fmat (element by element)");
  fts_class_doc(cl, fts_new_symbol("dot"), "<fmat: operand>", "get dot product of column vector with given vector");

  fts_class_doc(cl, fts_new_symbol("abs"), NULL, "calulate absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("logabs"), NULL, "calulate logarithm of absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("log"), NULL, "calulate lograrithm of current values");
  fts_class_doc(cl, fts_new_symbol("exp"), NULL, "calulate exponent function of current values");
  fts_class_doc(cl, fts_new_symbol("sqrabs"), NULL, "calulate square of absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("sqrt"), NULL, "calulate square root of absolute values of current values");
  fts_class_doc(cl, fts_new_symbol("fft"), NULL, "calulate inplace FFT of real or complex vector (vec or rect format)");
  fts_class_doc(cl, fts_new_symbol("rifft"), NULL, "calulate inplace real IFFT of complex vector (rect format)");

  fts_class_doc(cl, fts_new_symbol("normalize"), NULL, "normalize to between -1.0 and 1.0");
  fts_class_doc(cl, fts_new_symbol("reverse"), NULL, "reverse order of rows");
  fts_class_doc(cl, fts_new_symbol("rotate"), "[<num: # of elements (def 1)>]", "rotate by given number of rows");
  fts_class_doc(cl, fts_new_symbol("sort"), "[<num: index of column>]", "sort rows by ascending values of given column");
  fts_class_doc(cl, fts_new_symbol("tros"), "[<num: index of column>]", "sort rows by descending values of given column");
  fts_class_doc(cl, fts_new_symbol("scramble"), NULL, "scramble rows randomly");

  fts_class_doc(cl, fts_new_symbol("lookup"), "<fmat|fcol|frow|bpf: function>", "apply given function to each value (by linear interpolation)");
  fts_class_doc(cl, fts_new_symbol("env"), "<fmat|fcol|frow|bpf: envelope>", "multiply given envelope function to each column");

  fts_class_doc(cl, fts_new_symbol("apply"), "<expr: expression>", "apply expression each value (use $self and $x)");

  fts_class_doc(cl, sym_vec, NULL, "convert matrix to vector (vec format)");
  fts_class_doc(cl, sym_rect, NULL, "convert matrix to rectangular complex vector (rect format)");
  fts_class_doc(cl, sym_polar, NULL, "convert matrix to polar complex vector (polar format)");
  fts_class_doc(cl, sym_real, NULL, "convert complex vectors to real column vectors (of real part)");

  fts_class_doc(cl, fts_s_import, "[<sym: file name]", "import data from file");
  fts_class_doc(cl, fts_s_export, "[<sym: file name]", "export data to file");
}

void
fmat_config(void)
{
  fmat_symbol = fts_new_symbol("fmat");
  fvec_symbol = fts_new_symbol("fvec");

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

  fmat_class = fts_class_install(fmat_symbol, fmat_instantiate);
  fvec_class = fts_class_install(fvec_symbol, fmat_instantiate);

  fts_hashtable_init(&fmat_format_hash, FTS_HASHTABLE_SMALL);

  /* init fmat format table */
  fmat_format_vec = fmat_format_register(sym_vec);
  
  fmat_format_rect = fmat_format_register(sym_rect);
  fmat_format_add_column(fmat_format_rect, sym_re);
  fmat_format_add_column(fmat_format_rect, sym_im);
  
  fmat_format_polar = fmat_format_register(sym_polar);
  fmat_format_add_column(fmat_format_polar, sym_mag);
  fmat_format_add_column(fmat_format_polar, sym_arg);
  
  /* fmat default format */
  fmat_format_real = fts_malloc(sizeof(fmat_format_t));
  fmat_format_real->name = sym_real;
  fmat_format_real->index = FMAT_FORMATS_MAX;
  fmat_format_real->n_columns = 0;
  
  /*fmat_null = (fmat_t *)fts_object_create(fmat_class, 0, 0);*/
  /*fts_object_refer((fts_object_t *)fmat_null);*/
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
