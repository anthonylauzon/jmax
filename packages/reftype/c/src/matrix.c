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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"
#include "matrix.h"
#include "refdata.h"

fts_type_t matrix_type = 0;
fts_symbol_t matrix_symbol = 0;
reftype_t *matrix_reftype = 0;

static fts_symbol_t sym_comma = 0;

matrix_t *
matrix_new(int m, int n)
{
  matrix_t *mx = fts_malloc(sizeof(matrix_t));
  int size = m * n;

  if(size > 0)
    {
      int i;

      mx->data = fts_malloc(m * n * sizeof(fts_atom_t));
      mx->m = m;
      mx->n = n;
      mx->alloc = size;

      /* set all atoms to void */
      for(i=0; i<size; i++)
	fts_set_void(mx->data + i);
    }
  else
    {
      mx->data = 0;
      mx->m = 0;
      mx->n = 0;    
      mx->alloc = 0;
    }

  refdata_init((refdata_t *)mx, matrix_reftype);

  return mx;
}

void
matrix_delete(matrix_t *mx)
{
  if(mx->m * mx->n)
    fts_free(mx->data);

  fts_free(mx);
}

void
matrix_void(matrix_t *mx)
{
  int i;
  int size = mx->m * mx->n;

  for(i=0; i<size; i++)
    {
      fts_atom_t *ap = mx->data + i;

      if(refdata_atom_is(ap))
	refdata_atom_release(ap);

      fts_set_void(ap); /* void atom */
    }
}

void
matrix_set_size(matrix_t *mx, int m, int n)
{
  int size = m * n;
  
  if(size > mx->alloc)
    {
      int i;
      if(mx->alloc)
	mx->data = fts_realloc(mx->data, size * sizeof(fts_atom_t));
      else
	mx->data = fts_malloc(size * sizeof(fts_atom_t));

      /* set newly allocated region to void */
      for(i=mx->alloc; i<size; i++)
	fts_set_void(mx->data + i);
      
      mx->m = m;
      mx->n = n;
      mx->alloc = size;
    }
  else
    {
      int old_size = mx->m * mx->n;
      int i;

      if(size <= 0)
	m = n = size = 0;

      /* void region cut off */
      for(i=size; i<old_size; i++)
	{
	  fts_atom_t *ap = mx->data + i;

	  if(refdata_atom_is(ap))
	    refdata_atom_release(ap);

	  fts_set_void(ap);
	}
      
      mx->m = m;
      mx->n = n;
    }
}

extern void
matrix_set_element(matrix_t *mx, int i, int j, fts_atom_t value)
{
  fts_atom_t *ap = mx->data + i * mx->n + j;
  
  if(refdata_atom_is(ap))
    refdata_atom_release(ap);

  *ap = value;

  if(refdata_atom_is(&value))
    refdata_atom_refer(&value);
}

extern void
matrix_void_element(matrix_t *mx, int i, int j)
{
  fts_atom_t *ap = mx->data + i * mx->n + j;
  
  if(refdata_atom_is(ap))
    refdata_atom_release(ap);

  fts_set_void(ap);
}

void
matrix_fill(matrix_t *mx, fts_atom_t value)
{
  int size = mx->m * mx->n;
  int refer = refdata_atom_is(&value);

  int i;

  for(i=0; i<size; i++)
    {
      fts_atom_t *ap = mx->data + i;

      if(refdata_atom_is(ap))
	refdata_atom_release(ap);

      *ap = value;
      
      if(refer)
	refdata_atom_refer(&value);
    }
}

void
matrix_set_from_atom_list(matrix_t *mx, int onset, int ac, const fts_atom_t *at)
{
  fts_atom_t *ap = mx->data + onset;
  int m = matrix_get_m(mx);
  int n = matrix_get_n(mx);
  int size = m * n;
  int i;
  
  if(onset + ac > size)
    ac = size - onset;
  
  for(i=0; i<ac; i++)
    {
      if(refdata_atom_is(ap))
	refdata_atom_release(ap);
      
      *ap++ = at[i];
      
      if(refdata_atom_is(at + i))
	refdata_atom_refer(at + i);	
    }
}

void
matrix_set_from_atom_arrays(matrix_t *mx, int ac, const fts_atom_t *at)
{
  int m = matrix_get_m(mx);
  int n = matrix_get_n(mx);
  int i, j;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_atom_array(at + i))
	{
	  fts_atom_array_t *aa = fts_get_atom_array(at + i);
	  int size = fts_atom_array_get_size(aa);

	  if(size > n)
	    size = n;
	  
	  for(j=0; j<size; j++)
	    {
	      fts_atom_t *mxp = mx->data + i * n + j;
	      fts_atom_t *aap = &fts_atom_array_get_element(aa, j);
	    
	      if(refdata_atom_is(mxp))
		refdata_atom_release(mxp);
	      
	      *mxp = *aap;
	      
	      if(refdata_atom_is(aap))
		refdata_atom_refer(aap);	
	    }
	}
      else
	break;
    }
}

/********************************************************
 *
 *  files
 *
 */

#define MATRIX_BLOCK_SIZE 256

static void
matrix_grow(matrix_t *mx, int size)
{
  int alloc = mx->alloc;

  while(size > alloc)
    alloc += MATRIX_BLOCK_SIZE;

  matrix_set_size(mx, alloc, 1);
}

int 
matrix_read_atom_file_newline(matrix_t *mx, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;

  matrix_void(mx);

  while(fts_atom_file_read(file, &a, &c))
    {
      m = i + 1;

      /* first row determines # of columns */    
      if(i == 0)
	n = j + 1;
      
      if(m * n > mx->alloc)
	matrix_grow(mx, m * n);

      if(j < n)
	{
	  mx->data[i * n + j] = a;
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

  mx->m = m;
  mx->n = n;
  
  fts_atom_file_close(file);

  return(m * n);
}

int
matrix_write_atom_file_newline(matrix_t *mx, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int m = mx->m;
  int n = mx->n;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the matrix */
  for(i=0; i<m; i++)     
    {
      fts_atom_t *row = mx->data + i * n;

      for(j=0; j<n-1; j++)	
	fts_atom_file_write(file, row + j, ' ');

      fts_atom_file_write(file, row + n - 1, '\n');
    }

  fts_atom_file_close(file);
  return(m * n);
}

int 
matrix_read_atom_file_separator(matrix_t *mx, fts_symbol_t file_name, fts_symbol_t separator, int ac, const fts_atom_t *at)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;

  if(!separator)
    separator = sym_comma;

  matrix_void(mx);

  while(fts_atom_file_read(file, &a, &c))
    {
      int skip = 0;
      int k;

      /* filter atoms */
      for(k=0; k<ac; k++)
	{
	  /* ooops! */
	  if(a.type == at[k].type && a.value.fts_int == at[k].value.fts_int)
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
	      
	      if(n > mx->alloc)
		matrix_grow(mx, n);
	      
	      mx->data[i * n + j] = a;
	      j++;
	    }
	  else if(j < n)
	    {
	      if(m * n > mx->alloc)
		matrix_grow(mx, m * n);
	      
	      mx->data[i * n + j] = a;
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
      mx->m = m;
      mx->n = n;
    }
  else
    {
      mx->m = 0;
      mx->n = 0;      
    }
  
  fts_atom_file_close(file);

  return(m * n);
}

int
matrix_write_atom_file_separator(matrix_t *mx, fts_symbol_t file_name, fts_symbol_t separator)
{
  fts_atom_file_t *file;
  int m = mx->m;
  int n = mx->n;
  fts_atom_t sep;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  fts_set_symbol(&sep, separator);

  /* write the content of the matrix */
  for(i=0; i<m; i++)     
    {
      fts_atom_t *row = mx->data + i * n;

      for(j=0; j<n; j++)	
	fts_atom_file_write(file, row + j, ' ');

      fts_atom_file_write(file, &sep, '\n');
    }

  fts_atom_file_close(file);

  return(m * n);
}

/*********************************************************
 *
 *  reference type and expression function
 *
 */

static refdata_t *
matrix_constructor_null(int ac, const fts_atom_t *at)
{
  return (refdata_t *)matrix_new(0, 0);
}

static refdata_t *
matrix_constructor_m(int ac, const fts_atom_t *at)
{
  return (refdata_t *)matrix_new(fts_get_int(at), 1);
}

static refdata_t *
matrix_constructor_m_n(int ac, const fts_atom_t *at)
{
  return (refdata_t *)matrix_new(fts_get_int(at), fts_get_int(at + 1));
}

static refdata_t *
matrix_constructor_atom_arrays(int ac, const fts_atom_t *at)
{
  matrix_t *mx;
  int m = 0;
  int n = 0;
  int i;
  
  /* check n (longest row) and m */
  for(i=0; i<ac; i++)
    {
      if(fts_is_atom_array(at + i))
	{
	  fts_atom_array_t *aa = fts_get_atom_array(at + i);
	  int size = fts_atom_array_get_size(aa);
	  
	  if(size > n)
	    n = size;
	  
	  m++;
	}
      else
	break;
    }
  
  mx = matrix_new(m, n);
  matrix_set_from_atom_arrays(mx, ac, at);
  
  return (refdata_t *)mx;
}

static reftype_constructor_t
matrix_dispatcher(int ac, const fts_atom_t *at)
{
  matrix_t *mx = 0;

  if(ac == 0)
    return matrix_constructor_null;
  if(ac == 1 && fts_is_int(at))
    return matrix_constructor_m;
  else if(ac == 2 && fts_is_int(at) && fts_is_int(at + 1))
    return matrix_constructor_m_n;
  else if(fts_is_atom_array(at))
    return matrix_constructor_atom_arrays;
  else
    return 0;
}

static void
matrix_destructor(refdata_t *mx)
{
  matrix_delete((matrix_t *)mx);
}

static int
matrix_function(int ac, const fts_atom_t *at, fts_atom_t *result)
{
  matrix_t *mat = matrix_create(ac - 1, at + 1);
  
  if(mat)
    {
      matrix_atom_set(result, mat);
      return FTS_EXPRESSION_OK;
    }
  else
    return FTS_EXPRESSION_SYNTAX_ERROR;
}

void
matrix_config(void)
{
  sym_comma = fts_new_symbol(",");

  matrix_symbol = fts_new_symbol("mat");
  matrix_type = matrix_symbol;

  matrix_reftype = reftype_declare(matrix_symbol, matrix_dispatcher, matrix_destructor);

  fts_expression_declare_fun(matrix_symbol, matrix_function);
}
