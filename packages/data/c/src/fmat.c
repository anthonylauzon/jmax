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
 * Based on Max/ISPW by Miller Puckette.
 *
 */

#include <fts/fts.h>
#include "fmat.h"

fts_symbol_t fmat_symbol = 0;
fts_metaclass_t *fmat_type = 0;

static fts_symbol_t sym_text = 0;

/********************************************************
 *
 *  utility functions
 *
 */

void
fmat_set_size(fmat_t *mat, int m, int n)
{
  int size = m * n;
  
  if(size > mat->alloc)
    {
      mat->values = fts_realloc(mat->values, size * sizeof(float));

      mat->m = m;
      mat->n = n;
      mat->alloc = size;
    }
  else
    {
      int old_size = mat->m * mat->n;
      int i;

      if(size <= 0)
	m = n = size = 0;

      /* zero region cut off */
      for(i=size; i<old_size; i++)
	mat->values[i] = 0.0;
    }
      
  mat->m = m;
  mat->n = n;
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
fmat_set_with_onset_from_atoms(fmat_t *mat, int offset, int ac, const fts_atom_t *at)
{
  int size = mat->m * mat->n;
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	mat->values[i + offset] = fts_get_number_float(at + i);
      else
	mat->values[i + offset] = 0.0f;
    }
}

void
fmat_set_from_tuples(fmat_t *mat, int ac, const fts_atom_t *at)
{
  int m = fmat_get_m(mat);
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
		mat->values[i * n + j] = fts_get_number_float(a);
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

  fmat_set_size(copy, org->m, org->n);

  for(i=0; i<m*n; i++)
    copy->values[i] = org->values[i];
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

  fmat_set_size(fmat, alloc, 1);
}

int 
fmat_read_atom_file_newline(fmat_t *fmat, fts_symbol_t file_name)
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

  fmat_zero(fmat);

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
	    fmat->values[i * n + j] = fts_get_number_float(&a);
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
fmat_write_atom_file_newline(fmat_t *fmat, fts_symbol_t file_name)
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

int 
fmat_read_atom_file_separator(fmat_t *fmat, fts_symbol_t file_name, fts_symbol_t separator, int ac, const fts_atom_t *at)
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

  if(!separator)
    separator = fts_s_comma;

  fmat_zero(fmat);

  while(fts_atom_file_read(file, &a, &c))
    {
      int skip = 0;
      int k;

      /* filter atoms */
      for(k=0; k<ac; k++)
	{
	  /* ooops! */
	  if(fts_atom_same_type(&a, &at[k]) && a.value.fts_int == at[k].value.fts_int)
	    {
	      skip = 1;
	      break;
	    }
	}

      if(!skip)
	{
	  m = i + 1;
	  
	  if(fts_get_symbol(&a) == separator)
	    {
	      /* reset to beginning of next row */
	      i++;
	      j = 0;
	    }
	  else if(i == 0)
	    {
	      /* first row determines # of columns */    
	      n = j + 1;
	      
	      if(n > fmat->alloc)
		fmat_grow(fmat, n);

	      if(fts_is_number(&a))
		fmat->values[i * n + j] = fts_get_number_float(&a);
	      else
		fmat->values[i * n + j] = 0.0;

	      j++;
	    }
	  else if(j < n)
	    {
	      if(m * n > fmat->alloc)
		fmat_grow(fmat, m * n);
	      
	      if(fts_is_number(&a))
		fmat->values[i * n + j] = fts_get_number_float(&a);
	      else
		fmat->values[i * n + j] = 0.0;

	      j++;
	    }
	}
    }

  /* maybe empty rest of last line */
  if(j > 0)
    {
      i++;
      j = 0;
    }

  if(n > 0)
    {
      fmat->m = m;
      fmat->n = n;
    }
  else
    {
      fmat->m = 0;
      fmat->n = 0;      
    }
  
  fts_atom_file_close(file);

  return(m * n);
}

int
fmat_write_atom_file_separator(fmat_t *fmat, fts_symbol_t file_name, fts_symbol_t separator)
{
  fts_atom_file_t *file;
  int m = fmat->m;
  int n = fmat->n;
  fts_atom_t sep;
  int i, j;

  file = fts_atom_file_open(file_name, "w");

  if(!file)
    return -1;

  fts_set_symbol(&sep, separator);

  /* write the content of the fmat */
  for(i=0; i<m; i++)     
    {
      float *row = fmat->values + i * n;
      fts_atom_t a;

      for(j=0; j<n; j++)	
	{
	  fts_set_float(&a, row[j]);
	  fts_atom_file_write(file, &a, ' ');
	}

      fts_atom_file_write(file, &sep, '\n');
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
fmat_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_object(o, 0, o);
}

static void
fmat_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  fmat_set_const(this, 0.0f);
}

static void
fmat_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      float constant = fts_get_float(at);
      
      fmat_set_const(this, constant);
    }
}

static void
fmat_set_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int m = fmat_get_m(this);
      int n = fmat_get_n(this);
      int i = fts_get_number_int(at);
      int j = fts_get_number_int(at + 1);

      if(i >= 0 && i < m && j >= 0 && j < n)
	fmat_set_with_onset_from_atoms(this, i * n + j, ac - 2, at + 2);
    }
}

static void
fmat_set_row_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      int m = fmat_get_m(this);
      int n = fmat_get_n(this);
      int i = fts_get_number_int(at);

      ac--;
      at++;
      
      /* clip to row */
      if(ac > n)
	ac = n;

      if(i >= 0 && i < m)
	fmat_set_with_onset_from_atoms(this, i * n, ac, at);
    }
}

static void
fmat_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  int old_size = fmat_get_m(this)* fmat_get_n(this);
  int m = 0;
  int n = 0;
  int i;

  if(ac == 1 && fts_is_number(at))
    {
      m = fts_get_number_int(at);
      n = fmat_get_n(this);
      
      if(m >= 0 && n >= 0)
	fmat_set_size(this, m, n);
    }  
  else if(ac == 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      m = fts_get_number_int(at);
      n = fts_get_number_int(at + 1);
      
      if(m >= 0 && n >= 0)
	fmat_set_size(this, m, n);
    }

  /* set newly allocated region to void */
  for(i=old_size; i<m*n; i++)
    this->values[i] = 0.0;
}

/**************************************************************************************
 *
 *  arithmetics
 *
 */

static void
fmat_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] += r[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] += r;
	}
    }
}

static void
fmat_sub(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] -= r[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] -= r;
	}
    }
}

static void
fmat_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] *= r[i];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] *= r;
	}
    }
}

static void
fmat_div(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      {
		if(r[i] != 0)
		  l[i * n + j] /= r[i];
		else
		  l[i * n + j] = 0;
	      }
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] /= r;
	}
    }
}

static void
fmat_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = r[i] - l[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = r - p[i];
	}
    }
}

static void
fmat_vid(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      {
		if(l[i * n + j] != 0)
		  l[i * n + j] = r[i] / l[i * n + j];
		else
		  l[i * n + j] = 0;
	      }
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	      
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
    }
}

/**************************************************************************************
 *
 *  comparison
 *
 */

static void
fmat_ee(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = l[i * n + j] == r[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] == r;
	}
    }
}

static void
fmat_ne(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = l[i * n + j] != r[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] != r;
	}
    }
}

static void
fmat_gt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = l[i * n + j] > r[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] > r;
	}
    }
}

static void
fmat_ge(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = l[i * n + j] >= r[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] >= r;
	}
    }
}

static void
fmat_lt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = l[i * n + j] < r[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] < r;
	}
    }
}

static void
fmat_le(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = l[i * n + j] <= r[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = p[i] <= r;
	}
    }
}

static void
fmat_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = (r[i * n + j] <= l[i * n + j])? r[i * n + j]: l[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = (r <= p[i])? r: p[i];
	}
    }
}

static void
fmat_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fmat_type))
	{
	  fmat_t *right = fmat_atom_get(at);
	  int this_m = fmat_get_m(this);
	  int this_n = fmat_get_n(this);
	  int right_m = fmat_get_m(right);
	  int right_n = fmat_get_n(right);
	  int m = (this_m <= right_m)? this_m: right_m;
	  int n = (this_n <= right_n)? this_n: right_n;
	  float *l, *r;
	  int i, j;
  
	  l = fmat_get_ptr(this);
	  r = fmat_get_ptr(right);
  
	  for(i=0; i<m; i++)
	    for(j=0; j<n; j++)
	      l[i * n + j] = (r[i * n + j] >= l[i * n + j])? r[i * n + j]: l[i * n + j];
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = fmat_get_m(this) * fmat_get_n(this);
	  float *p = fmat_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    p[i] = (r >= p[i])? r: p[i];
	}
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
  fmat_t *this = (fmat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
      int size = 0;

      if(separator)
	size = fmat_read_atom_file_separator(this, file_name, separator, ac - 3, at + 3);
      else
	size = fmat_read_atom_file_newline(this, file_name);

      if(size <= 0)
	post("fmat: can't import from text file \"%s\"\n", file_name);
    }
  else
    post("fmat: unknown import file format \"%s\"\n", file_format);
}

static void
fmat_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
      int size = 0;

      if(separator)
	size = fmat_write_atom_file_separator(this, file_name, separator);
      else
	size = fmat_write_atom_file_newline(this, file_name);

      if(size < 0)
	post("fmat: can't export to text file \"%s\"\n", file_name);
    }
  else
    post("fmat: unknown export file format \"%s\"\n", file_format);
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
fmat_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  int m = fmat_get_m(this);
  int n = fmat_get_n(this);
  int i, j;
  
  post("{\n");

  for(i=0; i<m; i++)
    {
      post("  {");

      for(j=0; j<n-1; j++)
	post("%f ", fmat_get_element(this, i, j));

      post("%f}\n", fmat_get_element(this, i, j));
    }

  post("}\n");
}

static void
fmat_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  fmat_t *set = fmat_atom_get(at);

  fmat_copy(set, this);
}

static void 
fmat_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  float *data = this->values;
  int m = fmat_get_m(this);
  int n = fmat_get_n(this);
  int size = m * n;
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
    }
}

/*********************************************************
 *
 *  class
 *
 */
static void
fmat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  
  data_object_set_keep((data_object_t *)o, fts_s_no);

  if(ac == 0)
    fmat_set_size(this, 0, 0);
  else if(ac == 1 && fts_is_int(at))
    fmat_set_size(this, fts_get_int(at), 1);
  else if(ac == 2 && fts_is_int(at) && fts_is_int(at + 1))
    fmat_set_size(this, fts_get_int(at), fts_get_int(at + 1));
  else if(fts_is_tuple(at))
    {
      int m = 0;
      int n = 0;
      int i;
      
      /* check n (longest row) and m */
      for(i=0; i<ac; i++)
	{
	  if(fts_is_tuple(at + i))
	    {
	      fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at + i);
	      int size = fts_tuple_get_size(tup);
	      
	      if(size > n)
		n = size;
	      
	      m++;
	    }
	  else
	    break;
	}
      
      fmat_set_size(this, m, n);
      fmat_set_from_tuples(this, ac, at);

      data_object_set_keep((data_object_t *)o, fts_s_args);
    }
  else
    fts_object_set_error(o, "Wrong arguments");
}

static void
fmat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fmat_t *this = (fmat_t *)o;
  
  fts_free(this->values);
}

static void
fmat_get_fmat(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

static fts_status_t
fmat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fmat_t), 1, 1, 0);
  
  /* init / delete */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fmat_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fmat_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, fmat_print); 

  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, data_object_daemon_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, data_object_daemon_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, fmat_get_fmat);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_instance, fmat_set_from_instance);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, fmat_set_elements);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_row, fmat_set_row_elements);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_size, fmat_size);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, fmat_dump);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, fmat_output);
  
  fts_method_define_varargs(cl, 0, fts_s_clear, fmat_clear);
  fts_method_define_varargs(cl, 0, fts_s_fill, fmat_fill);
  fts_method_define_varargs(cl, 0, fts_s_set, fmat_set_elements);
  fts_method_define_varargs(cl, 0, fts_s_row, fmat_set_row_elements);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("add"), fmat_add);
  fts_method_define_varargs(cl, 0, fts_new_symbol("sub"), fmat_sub);
  fts_method_define_varargs(cl, 0, fts_new_symbol("mul"), fmat_mul);
  fts_method_define_varargs(cl, 0, fts_new_symbol("div"), fmat_div);
  fts_method_define_varargs(cl, 0, fts_new_symbol("bus"), fmat_bus);
  fts_method_define_varargs(cl, 0, fts_new_symbol("vid"), fmat_vid);
  fts_method_define_varargs(cl, 0, fts_new_symbol("ee"), fmat_ee);
  fts_method_define_varargs(cl, 0, fts_new_symbol("ne"), fmat_ne);
  fts_method_define_varargs(cl, 0, fts_new_symbol("gt"), fmat_gt);
  fts_method_define_varargs(cl, 0, fts_new_symbol("ge"), fmat_ge);
  fts_method_define_varargs(cl, 0, fts_new_symbol("lt"), fmat_lt);
  fts_method_define_varargs(cl, 0, fts_new_symbol("le"), fmat_le);
  fts_method_define_varargs(cl, 0, fts_new_symbol("min"), fmat_min);
  fts_method_define_varargs(cl, 0, fts_new_symbol("max"), fmat_max);

  fts_method_define_varargs(cl, 0, fts_s_size, fmat_size);
            
  fts_method_define_varargs(cl, 0, fts_s_import, fmat_import);
  fts_method_define_varargs(cl, 0, fts_s_export, fmat_export);
    
  return fts_Success;
}

void 
fmat_config(void)
{
  sym_text = fts_new_symbol("text");
  fmat_symbol = fts_new_symbol("fmat");

  fmat_type = fts_class_install(fmat_symbol, fmat_instantiate);
}
