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


fts_symbol_t fmat_symbol = 0;
fts_class_t *fmat_type = 0;

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

static fmat_format_t *
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

static void
fmat_format_add_column(fmat_format_t *format, fts_symbol_t label)
{
  format->columns[format->n_columns++] = label;
}

static fmat_format_t *
fmat_format_get_by_name(fts_symbol_t name)
{
  fts_atom_t k, a;
  
  fts_set_symbol(&k, name);  
  if(fts_hashtable_get(&fmat_format_hash, &k, &a) == 0)
    return (fmat_format_t *)fts_get_pointer(&a);

  return NULL;
}

/* correct fmat format (if needed) */
static void
fmat_adapt_format(fmat_t *self)
{
  int n = self->n;
  
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

#define HEAD_ROWS 2 /* extra points for interpolation at start of vector */
#define TAIL_ROWS 2 /* extra points for interpolation at end of vector */
#define HEAD_TAIL_COLS 2 /* interpolation head and tail for 1 or 2 column vectors only */
#define HEAD_POINTS (HEAD_TAIL_COLS * HEAD_ROWS)
#define TAIL_POINTS (HEAD_TAIL_COLS * TAIL_ROWS)

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
    {
      self->values = (float *)fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
      
      /* zero head */
      /* for(i=0; i<HEAD_POINTS; i++)
      self->values[i] = 0.0; */
      self->values[0] = self->values[1] = self->values[2] = self->values[3] = 0.0;
    }
    else
      self->values = (float *)fts_realloc(self->values - HEAD_POINTS, (size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
        
    self->values += HEAD_POINTS;
    self->alloc = size;
  }
  
  /* zero tail */
  /* for(i=0; i<TAIL_POINTS; i++) self->values[size + i] = 0.0; */
  self->values[size + 0] = self->values[size + 1] = self->values[size + 2] = self->values[size + 3] = 0.0;
  
  self->m = m;
  self->n = n;
  
  fmat_adapt_format(self);
}

void
fmat_set_m(fmat_t *self, int m)
{
  int size = m * self->n;
  int i;
  
  if(m > self->m)
  {
    if(self->values == NULL)
    {
      self->values = (float *)fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));

      /* zero head */
      /* for(i=0; i<HEAD_POINTS; i++) self->values[i] = 0.0; */
      self->values[0] = self->values[1] = self->values[2] = self->values[3] = 0.0;    
    }
    else
      self->values = (float *)fts_realloc(self->values - HEAD_POINTS, (size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
    
    self->values += HEAD_POINTS;
    self->alloc = size;
  }
  
  /* zero new rows at end (if any) */
  for(i=self->m*self->n; i<size; i++)
    self->values[i] = 0.0;
    
  /* zero tail */
  /* for(i=0; i<TAIL_POINTS; i++) self->values[size + i] = 0.0; */
  self->values[size + 0] = self->values[size + 1] = self->values[size + 2] = self->values[size + 3] = 0.0;    
  
  self->m = m;
}

void
fmat_set_n(fmat_t *self, int n)
{
  int m = self->m;
  int old_n = self->n;
  
  if(n <= 0)
    n = 1;
  
  if(n != old_n)
  {
    int size = m * n;
    int i, j;
  
    if(n > old_n)
    {
      if(self->values == NULL)
      {
        self->values = (float *)fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
        
        /* zero head */
        /* for(i=0; i<HEAD_POINTS; i++) self->values[i] = 0.0; */
        self->values[0] = self->values[1] = self->values[2] = self->values[3] = 0.0;    
      }
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
    
    /* zero tail */
    /* for(i=0; i<TAIL_POINTS; i++) self->values[size + i] = 0.0; */
    self->values[size + 0] = self->values[size + 1] = self->values[size + 2] = self->values[size + 3] = 0.0;    
    self->n = n;
    
    fmat_adapt_format(self);
  }
}

void
fmat_set_size(fmat_t *self, int m, int n)
{
  int old_m = self->m;
  int old_n = self->n;
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
      {
        self->values = (float *)fts_malloc((size + HEAD_POINTS + TAIL_POINTS) * sizeof(float));
        
        /* zero head */
        /* for(i=0; i<HEAD_POINTS; i++) self->values[i] = 0.0; */
        self->values[0] = self->values[1] = self->values[2] = self->values[3] = 0.0;    
      }
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
    
    /* zero tail */
    /* for(i=0; i<TAIL_POINTS; i++) self->values[size + i] = 0.0; */
    self->values[size + 0] = self->values[size + 1] = self->values[size + 2] = self->values[size + 3] = 0.0;    
    
    self->m = m;
    self->n = n;
    
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
}

static void
fmat_copy_function(const fts_atom_t *from, fts_atom_t *to)
{
  fmat_copy((fmat_t *)fts_get_object(from), (fmat_t *)fts_get_object(to));
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

  fmat_reshape(fmat, alloc, 1);
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

  fmat_set_const(fmat, 0.0);

  while(fts_atom_file_read(file, &a, &c))
  {
    m = i + 1;

    /* first row determines # of columns */
    if(i == 0)
      n = j + 1;

    if(m * n > fmat->alloc)
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

/********************************************************************
*
*   user methods
*
*/

static void
fmat_set_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
  }
}

static void
fmat_set_row_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 1 && fts_is_number(at))
  {
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int i = fts_get_number_int(at);
    
    /* skip index argument */
    ac--;
    at++;
    
    /* clip to # of cloumns */
    if(ac > n)
      ac = n;
    
    if(i >= 0 && i < m)
      fmat_set_from_atoms(self, i * n, 1, ac, at);
  }
}

static void
fmat_set_col_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 1 && fts_is_number(at))
  {
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int j = fts_get_number_int(at);
    
    /* skip index argument */
    ac--;
    at++;
    
    /* clip to # of rows */
    if(ac > m)
      ac = m;
    
    if(j >= 0 && j < n)
      fmat_set_from_atoms(self, j, n, ac, at);
  }
}

static void
fmat_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac == 0)
  {
    if(fts_is_number(at))
      fmat_set_const(self, (float)fts_get_number_float(at));
  }
  
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
  
  if(ac == 1 && fts_is_number(at))
  {
    m = fts_get_number_int(at);
    
    if(m >= 0)
      fmat_set_m(self, m);
  }
  else if(ac > 1 && fts_is_number(at) && fts_is_number(at + 1))
  {
    m = fts_get_number_int(at);
    n = fts_get_number_int(at + 1);
    
    if(m >= 0 && n >= 0)
      fmat_set_size(self, m, n);
  }
}

static void
_fmat_set_m(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int m = fts_get_number_int(at);

  if(m >= 0)
    fmat_set_m(self, m);
}

static void
_fmat_set_n(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int n = fts_get_number_int(at);
  
  if(n >= 0)
    fmat_set_n(self, n);
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

/******************************************************************************
 *
 * functions, i.e. methods that return a value but don't change the object
 *
 * todo: to be called in functional syntax, e.g. (.max $myfvec)
 */

static void
fmat_getmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const fvec_t *self = (fvec_t *) o;
  const int size = self->m * self->n;
  const float *p = fmat_get_ptr(self);
  float max = p[0]; /* start with first element */
  int i;
  
  if (size == 0)
    return; /* no output (void) for empty vector */
  
  for (i = 1; i < size; i++)
    if (p[i] > max)
      max = p[i];
  
  fts_return_float(max);
}


static void
fmat_getmin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const fvec_t *self = (fvec_t *) o;
  const int size = self->m * self->n;
  const float *p = fvec_get_ptr(self);
  float min = p[0]; /* start with first element */
  int i;
  
  if (size == 0)
    return; /* no output (void) for empty vector */
  
  for (i = 1; i < size; i++)
    if (p[i] < min)
      min = p[i];
  
  fts_return_float(min);
}

/* copy a slice (a row or column, given in the first arg) from the
   matrix into an fvec (created here or given as second argument) */
static void 
fmat_get_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *) o;
  
  if(ac > 0 && fts_is_number(at))
  {
    int m = fmat_get_m(self); /* num. rows */
    int n = fmat_get_n(self); /* num. columns */
    int i = fts_get_number_int(at); /* indexed row */
    int j;
    
    if(i >= 0 && i < m)	/* check slice index */
    {
      fvec_t * myfvec;
      int myown;
      
      /* if an fvec is given as second arg, write slice there */
      if(ac > 1  &&  fts_is_a(at + 1, fvec_type))
      {
        myfvec = (fvec_t *) fts_get_object(at + 1);
        myown = 0;
      }
      else
      {
        myfvec = (fvec_t *) fts_object_create(fvec_type, 0, NULL);
        myown = 1;
      }
      
      fvec_set_size(myfvec, n);
      
      /* copy values (source elem index is row * n + col) */
      for(j=0; j<n; j++)
        fvec_set_element(myfvec, j, self->values[i * n + j]);
      
      /* if the fvec was given, see if we have to update the editor (same as fvec_fill), and set data dirty */
      if(!myown)
        fvec_changed(myfvec);
      
      /* set return value to fvec */
      fts_return_object((fts_object_t *) myfvec);
    }
  }
}

static void 
fmat_get_column(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *) o;
  
  if (ac > 0  &&  fts_is_number(at))
  {
    int m = fmat_get_m(self); /* num. rows */
    int n = fmat_get_n(self); /* num. columns */
    int j = fts_get_number_int(at);	/* indexed row or col */
    int i;

    if (0 <= j  &&  j < m)	/* check slice index */
    {
      fvec_t * myfvec;
      int myown;
      
      if(ac > 1  &&  fts_is_a(at + 1, fvec_type))
      {
        myfvec = (fvec_t *) fts_get_object(at+1);
        myown  = 0;
      }
      else
      {
        myfvec = (fvec_t *) fts_object_create(fvec_type, 0, NULL);
        myown  = 1;
      }
      
      fvec_set_size(myfvec, m);
      
      for (i=0; i<m; i++)
        fvec_set_element(myfvec, i, self->values[i * n + j]);
      
      /* if the fvec was given, see if we have to update the editor (same as fvec_fill), and set data dirty */
      if (!myown)
        fvec_changed(myfvec);
      
      /* set return value to fvec */
      fts_return_object((fts_object_t *) myfvec);
    }
  }
}

/******************************************************************************
*
*  arithmetics
*
*/

static void
fmat_add_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] += r[i * n + j];
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
}

static void
fmat_sub_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] -= r[i * n + j];
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
}

static void
fmat_mul_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] *= r[i * n + j];
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
}

static void
fmat_div_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
    {
      if(r[i] != 0)
        l[i * n + j] /= r[i * n + j];
      else
        l[i * n + j] = 0;
    }
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
}

static void
fmat_bus_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] = r[i] - l[i * n + j];
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
}

static void
fmat_vid_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
    {
      if(l[i * n + j] != 0)
        l[i * n + j] = r[i * n + j] / l[i * n + j];
      else
        l[i * n + j] = 0;
    }
}

static void
fmat_vid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  float r = (float)fts_get_number_float(at);
  int size = fmat_get_m(self) * fmat_get_n(self);
  float *p = fmat_get_ptr(self);

  if(r != 0.0)
  {
    int i;

    for(i=0; i<size; i++)
      p[i] = r / p[i];
  }
  else
  {
    int i;

    for(i=0; i<size; i++)
      p[i] = 0.0;
  }
}

/**************************************************************************************
*
*  comparison
*
*/

static void
fmat_ee_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] = (float)(l[i * n + j] == r[i * n + j]);
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
}

static void
fmat_ne_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] = (float)(l[i * n + j] != r[i * n + j]);
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
}

static void
fmat_gt_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] = (float)(l[i * n + j] > r[i * n + j]);
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
}

static void
fmat_ge_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] = (float)(l[i * n + j] >= r[i * n + j]);
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
}

static void
fmat_lt_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] = (float)(l[i * n + j] < r[i * n + j]);
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
}

static void
fmat_le_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fmat_t *right = (fmat_t *)fts_get_object(at);
  int self_m = fmat_get_m(self);
  int self_n = fmat_get_n(self);
  int right_m = fmat_get_m(right);
  int right_n = fmat_get_n(right);
  int m = (self_m <= right_m)? self_m: right_m;
  int n = (self_n <= right_n)? self_n: right_n;
  float *l, *r;
  int i, j;

  l = fmat_get_ptr(self);
  r = fmat_get_ptr(right);

  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      l[i * n + j] = (float)(l[i * n + j] <= r[i * n + j]);
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
}

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
}

static void
fmat_convert_rect(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
      fmat_reshape(self, m, 2);
      
      for(i=2*(m-1), j=m-1; i>=0; i-=2, j--)
      {
        ptr[i] = ptr[j]; /* real part */
        ptr[i + 1] = 0.0; /* imaginary part */
      }
      break;
    
    case fmat_format_id_rect:
      break;
    
    case fmat_format_id_polar:
      for(i=0; i<2*m; i+=2)
      {
        float mag = ptr[i];
        float arg = ptr[i + 1];
      
        ptr[i] = mag * cosf(arg); /* real part */
        ptr[i + 1] = mag * sinf(arg); /* imaginary part */
      }
      break;
    
    default:
      for(i=0, j=0; i<2*m; i+=2, j+=n)
      {
        ptr[i] = ptr[j]; /* real part */
        ptr[i + 1] = 0.0; /* imaginary part */
      }
      break;
          
      fmat_reshape(self, m, 2);
      break;
  }
  
  fmat_set_format(self, fmat_format_rect);
}

static void
fmat_convert_polar(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
      fmat_reshape(self, m, 2);
      
      for(i=2*(m-1), j=m-1; i>=0; i-=2, j--)
      {
        ptr[i] = ptr[j];
        ptr[i + 1] = 0.0;
      }
      break;
    
    case fmat_format_id_rect:
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
      for(i=0, j=0; i<2*m; i+=2, j+=n)
      {
        ptr[i] = ptr[j];
        ptr[i + 1] = 0.0;
      }
      break;
          
      fmat_reshape(self, m, 2);
      break;
  }
  
  fmat_set_format(self, fmat_format_rect);
}

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
  
  fmat_set_format(self, fmat_format_rect);
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
      for(i=0; i<n; i++)
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
      for(i=0; i<n; i++)
        ptr[i] = logf(ptr[i]);
      break;
    
    case fmat_format_id_rect:
      for(i=0; i<m; i++)
      {
        float re = ptr[i];
        float im = ptr[i + 1];
      
        ptr[i]     = (float)(0.5 * log(re * re + im * im));
        ptr[i + 1] = (float)atan2(im, re);
      }
      break;
    
    case fmat_format_id_polar:
      for(i=0; i<m; i++)
      {
        float re = logf(ptr[i]);
        float im = ptr[i + 1];
      
        ptr[i]     = sqrtf(re * re + im * im);
        ptr[i + 1] = (float)atan2(im, re);
      }
      break;
    
    default:
      for(i=0; i<m*n; i++)
        ptr[i] = logf(ptr[i]);
      break;
  }
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
      for(i=0; i<n; i++)
        ptr[i] = expf(ptr[i]);
      break;
    
    case fmat_format_id_rect:
      for(i=0; i<m; i++)
      {
        float mag = expf(ptr[i]);
        float arg = ptr[i + 1];
      
        ptr[i] = mag * cosf(arg);
        ptr[i + 1] = mag * sinf(arg);
      }
      break;
    
    case fmat_format_id_polar:
      for(i=0; i<m; i++)
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
}

/**************************************************************************************
*
*  load, save, import, export
*
*/

static void
fmat_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_symbol_t file_name = fts_get_symbol(at);
  
  if(file_name != NULL)
  {
    int size = fmat_read_atom_file(self, file_name);
    
    if(size <= 0)
      fts_post("fmat: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
  }
}

static void
fmat_import_dialog(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
}

static void
fmat_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if(file_name != NULL)
  {
    int size = fmat_write_atom_file(self, file_name);
    
    if(size < 0)
      fts_post("fmat: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
  }
}

static void
fmat_export_dialog(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
}

/********************************************************************
*
*  system functions
*
*/

static void
fmat_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int size = n * m;

  if(size == 0)
    fts_spost(stream, "<fmat>");
  else
    fts_spost(stream, "<fmat %d %d>", m, n);
}

static void
fmat_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int m = fmat_get_m(self);
  int n = fmat_get_n(self);
  int size = m * n;
  int i, j;

  if(size == 0)
    fts_spost(stream, "<empty fmat>\n");
  else
  {
    fts_spost(stream, "<fmat %dx%d>\n", m, n);
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
}

static void
fmat_set_from_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;

  fmat_copy((fmat_t *)fts_get_object(at), self);
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

static void
fmat_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(data_object_is_persistent(o))
  {
    fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
    fts_atom_t a;

    /* save state */
    fmat_dump_state(o, 0, 0, ac, at);

    /* save persistence flag */
    fts_set_int(&a, 1);
    fts_dumper_send(dumper, fts_s_persistence, 1, &a);
  }

  /* save name */
  fts_name_dump_method(o, 0, 0, ac, at);
}

static int
fmat_equals(const fts_atom_t *a, const fts_atom_t *b)
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

/*********************************************************
 *
 *  class
 *
 */
static void
fmat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;
  int m = 0;
  int n = 1;

  data_object_init(o);

  self->values = NULL;
  self->m = 0;
  self->n = 0;
  self->alloc = -1;
  self->onset = 0.0;
  self->sr = fts_dsp_get_sample_rate();
  self->format = fmat_format_real;
  self->opened = 0;
  self->editor = NULL;
  
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
  
  fmat_reshape(self, m, n);
  
  /* init from arguments */
  if(ac > 2)
  {
    int size = m * n;
    ac -= 2;
    at += 2;
    
    if(ac > size)
      ac = size;
    
    fmat_set_from_atoms(self, 0, 1, ac, at);
  }
}

static void
fmat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *self = (fmat_t *)o;

  if(self->values != NULL)
    fts_free(self->values - HEAD_POINTS);
}

static void
fmat_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fmat_t), fmat_init, fmat_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_persistence, data_object_persistence);
  fts_class_message_varargs(cl, fts_s_update_gui, data_object_update_gui);
  fts_class_message_varargs(cl, fts_s_dump_state, fmat_dump_state);
  fts_class_message_varargs(cl, fts_s_dump, fmat_dump);

  fts_class_message_varargs(cl, fts_s_post, fmat_post);
  fts_class_message_varargs(cl, fts_s_print, fmat_print);

  fts_class_set_equals_function(cl, fmat_equals);
  
  fts_class_message_varargs(cl, fts_s_set_from_instance, fmat_set_from_fmat);

  fts_class_message_varargs(cl, fts_s_set, fmat_set_elements);
  fts_class_message(cl, fts_s_set, cl, fmat_set_from_fmat);
  fts_class_message_varargs(cl, fts_s_row, fmat_set_row_elements);
  fts_class_message_varargs(cl, fts_s_col, fmat_set_col_elements);
  
  fts_class_message_number(cl, fts_s_fill, fmat_fill);

  fts_class_message_void(cl, fts_s_size, _fmat_get_size);
  fts_class_message_varargs(cl, fts_s_size, _fmat_set_size);
  
  fts_class_message_void(cl, fts_new_symbol("rows"), _fmat_get_m);
  fts_class_message_number(cl, fts_new_symbol("rows"), _fmat_set_m);
  
  fts_class_message_void(cl, fts_new_symbol("cols"), _fmat_get_n);
  fts_class_message_number(cl, fts_new_symbol("cols"), _fmat_set_n);
  
  fts_class_message(cl, fts_new_symbol("add"), cl, fmat_add_fmat);
  fts_class_message_number(cl, fts_new_symbol("add"), fmat_add_number);

  fts_class_message(cl, fts_new_symbol("sub"), cl, fmat_sub_fmat);
  fts_class_message_number(cl, fts_new_symbol("sub"), fmat_sub_number);

  fts_class_message(cl, fts_new_symbol("mul"), cl, fmat_mul_fmat);
  fts_class_message_number(cl, fts_new_symbol("mul"), fmat_mul_number);

  fts_class_message(cl, fts_new_symbol("div"), cl, fmat_div_fmat);
  fts_class_message_number(cl, fts_new_symbol("div"), fmat_div_number);

  fts_class_message(cl, fts_new_symbol("bus"), cl, fmat_bus_fmat);
  fts_class_message_number(cl, fts_new_symbol("bus"), fmat_bus_number);

  fts_class_message(cl, fts_new_symbol("vid"), cl, fmat_vid_fmat);
  fts_class_message_number(cl, fts_new_symbol("vid"), fmat_vid_number);

  fts_class_message(cl, fts_new_symbol("ee"), cl, fmat_ee_fmat);
  fts_class_message_number(cl, fts_new_symbol("ee"), fmat_ee_number);

  fts_class_message(cl, fts_new_symbol("ne"), cl, fmat_ne_fmat);
  fts_class_message_number(cl, fts_new_symbol("ne"), fmat_ne_number);

  fts_class_message(cl, fts_new_symbol("gt"), cl, fmat_gt_fmat);
  fts_class_message_number(cl, fts_new_symbol("gt"), fmat_gt_number);

  fts_class_message(cl, fts_new_symbol("ge"), cl, fmat_ge_fmat);
  fts_class_message_number(cl, fts_new_symbol("ge"), fmat_ge_number);

  fts_class_message(cl, fts_new_symbol("lt"), cl, fmat_lt_fmat);
  fts_class_message_number(cl, fts_new_symbol("lt"), fmat_lt_number);

  fts_class_message(cl, fts_new_symbol("le"), cl, fmat_le_fmat);
  fts_class_message_number(cl, fts_new_symbol("le"), fmat_le_number);

  fts_class_message_void(cl, fts_new_symbol("abs"), fmat_abs);
  fts_class_message_void(cl, fts_new_symbol("logabs"), fmat_logabs);
  fts_class_message_void(cl, fts_new_symbol("log"), fmat_log);
  fts_class_message_void(cl, fts_new_symbol("exp"), fmat_exp);

  fts_class_message_void(cl, sym_vec, fmat_convert_vec);
  fts_class_message_void(cl, sym_rect, fmat_convert_rect);
  fts_class_message_void(cl, sym_polar, fmat_convert_polar);
  
  fts_class_message_void(cl, fts_new_symbol("fft"), fmat_fft);

  fts_class_message_symbol(cl, fts_s_import, fmat_import);
  fts_class_message_void(cl, fts_s_import, fmat_import_dialog);
  fts_class_message_symbol(cl, fts_s_export, fmat_export);
  fts_class_message_void(cl, fts_s_export, fmat_export_dialog);

  fts_class_message_varargs(cl, fts_s_get_element, _fmat_get_element);
  fts_class_message_varargs(cl, sym_getrow, fmat_get_row);
  fts_class_message_varargs(cl, sym_getcol, fmat_get_column);

  fts_class_message_void(cl, fts_new_symbol("getmax"), fmat_getmax);
  fts_class_message_void(cl, fts_new_symbol("getmin"), fmat_getmin);

  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);

  fts_class_set_copy_function(cl, fmat_copy_function);


  /*
   * class doc 
   */

  fts_class_doc(cl, fmat_symbol, "[<num: # of rows> [<num: # of columns (default is 1)> [<num: init values> ...]]]", "matrix of floats");
  fts_class_doc(cl, fts_s_set, "<num: row index> <num: column index> [<num:value> ...]" , "set matrix values at given index");
  fts_class_doc(cl, fts_s_set, "<fmat: other>", "set from fmat instance");
  fts_class_doc(cl, fts_s_row, "<num: index> [<num:value> ...]", "set values of given row");
  fts_class_doc(cl, fts_s_col, "<num: index> [<num:value> ...]", "set values of given column");
  fts_class_doc(cl, fts_s_fill, "<num: value>", "fill matrix with given value");
  fts_class_doc(cl, fts_s_size, "[<num: # of rows> [<num: # of columns (default is 1)>]]", "get/set size");
  fts_class_doc(cl, fts_new_symbol("rows"), "[<num: # of rows>]", "get/set # of rows");
  fts_class_doc(cl, fts_new_symbol("columns"), "[<num: # of rows>]", "get/set # of columns");
  
  fts_class_doc(cl, fts_s_get_element, "<num: row index> <num: column index>", "get value at given index");
  fts_class_doc(cl, fts_new_symbol("getmax"), NULL, "get maximum value");
  fts_class_doc(cl, fts_new_symbol("getmin"), NULL, "get minimum value");
  
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
  
  fts_class_doc(cl, fts_s_import, "[<sym: file name]", "import data from file");
  fts_class_doc(cl, fts_s_export, "[<sym: file name]", "export data to file");
}

void
fmat_config(void)
{
  sym_getcol = fts_new_symbol("getcol");
  sym_getrow = fts_new_symbol("getrow");
  sym_text = fts_new_symbol("text");
  fmat_symbol = fts_new_symbol("fmat");
  
  sym_vec = fts_new_symbol("vec");
  sym_real = fts_new_symbol("real");
  sym_rect = fts_new_symbol("rect");
  sym_polar = fts_new_symbol("polar");
  sym_re = fts_new_symbol("re");
  sym_im = fts_new_symbol("im");
  sym_mag = fts_new_symbol("mag");
  sym_arg = fts_new_symbol("arg");

  fmat_type = fts_class_install(fmat_symbol, fmat_instantiate);
  
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
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
