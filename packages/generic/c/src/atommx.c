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
#include "atommx.h"

fts_symbol_t sym_atom_matrix = 0;

void
atom_matrix_void(atom_matrix_t *mx)
{
  int i;
  int size = mx->m * mx->n;

  for(i=0; i<size; i++)
    fts_set_void(mx->data + i);
}

atom_matrix_t *
atom_matrix_new(int m, int n)
{
  atom_matrix_t *mx = fts_malloc(sizeof(atom_matrix_t));
  int size = m * n;

  if(size > 0)
    {
      mx->data = fts_malloc(m * n * sizeof(fts_atom_t));
      mx->m = m;
      mx->n = n;
      mx->alloc = size;

      atom_matrix_void(mx);
    }
  else
    {
      mx->data = 0;
      mx->m = 0;
      mx->n = 0;    
      mx->alloc = 0;
    }

  return mx;
}

void
atom_matrix_delete(atom_matrix_t *mx)
{
  if(mx->m * mx->n)
    fts_free(mx->data);

  fts_free(mx);
}

void
atom_matrix_set_size(atom_matrix_t *mx, int m, int n)
{
  int size = m * n;
  
  if(size > mx->alloc)
    {
      if(mx->alloc)
	mx->data = fts_realloc(mx->data, m * n * sizeof(fts_atom_t));
      else
	mx->data = fts_malloc(m * n * sizeof(fts_atom_t));
    }
  else if(size > 0)
    {
      mx->m = m;
      mx->n = n;
    }
  else
    {
      mx->m = 0;
      mx->n = 0;
    }
}

#define ATOM_MATRIX_BLOCK_SIZE 256

static void
atom_matrix_grow(atom_matrix_t *mx, int size)
{
  int alloc = mx->alloc;

  if(alloc)
    {
      while(size > alloc)
	alloc += ATOM_MATRIX_BLOCK_SIZE;

      mx->data = fts_realloc(mx->data, alloc * sizeof(fts_atom_t));
      mx->alloc = alloc;
    }
  else
    mx->data = fts_malloc(ATOM_MATRIX_BLOCK_SIZE * sizeof(fts_atom_t));
}

void
atom_matrix_fill(atom_matrix_t *mx, fts_atom_t atom)
{
  int i;
  int size = mx->m * mx->n;

  for(i=0; i<size; i++)
    mx->data[i] = atom;
}

/********************************************************
 *
 *  files
 *
 */

int 
atom_matrix_import_ascii_newline(atom_matrix_t *mx, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  fts_atom_t *data = mx->data;
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;

  if(!file)
    {
      post("mat: can't open file to read: %s\n", fts_symbol_name(file_name));
      return(-1);
    }

  while(fts_atom_file_read(file, &a, &c))
    {
      m = i + 1;

      /* first row determines # of columns */    
      if(i == 0)
	n = j + 1;
      
      if(m * n > mx->alloc)
	atom_matrix_grow(mx, m * n);

      if(j < n)
	{
	  data[i * n + j] = a;
	  j++;
	  
	  if(c == '\n')
	    {
	      /* void end of row */
	      for(; j<n; j++)
		fts_set_void(data + i * n + j);
	      
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
      for(; j<n; j++)
	fts_set_void(data + n * i + j);
      
      i++;
      j = 0;
    }

  atom_matrix_set_size(mx, m, n);
  
  fts_atom_file_close(file);

  return(m * n);
}

int
atom_matrix_export_ascii_newline(atom_matrix_t *mx, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int m = mx->m;
  int n = mx->n;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    {
      post("mat: can't open file to write: %s\n", fts_symbol_name(file_name));
      return(-1);
    }

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
atom_matrix_import_ascii_separator(atom_matrix_t *mx, fts_symbol_t file_name, fts_symbol_t separator, int ac, fts_atom_t *at)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  fts_atom_t *data = mx->data;
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;

  if(!file)
    {
      post("mat: can't open file to read: %s\n", fts_symbol_name(file_name));
      return(-1);
    }

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
	      /* maybe void end of row */
	      for(; j<n; j++)
		fts_set_void(data + i * n + j);
	      
	      /* reset to beginning of next row */
	      i++;
	      j = 0;
	    }
	  else if(i == 0)
	    {
	      /* first row determines # of columns */    
	      n = j + 1;
	      
	      if(n > mx->alloc)
		atom_matrix_grow(mx, n);
	      
	      data[i * n + j] = a;
	      j++;
	    }
	  else if(j < n)
	    {
	      if(m * n > mx->alloc)
		atom_matrix_grow(mx, m * n);
	      
	      data[i * n + j] = a;
	      j++;
	    }
	}
    }

  /* maybe empty rest of last line */
  if(j > 0)
    {
      for(; j<n; j++)
	fts_set_void(data + n * i + j);
      
      i++;
      j = 0;
    }

  atom_matrix_set_size(mx, m, n);
  
  fts_atom_file_close(file);

  return(m * n);
}

int
atom_matrix_export_ascii_separator(atom_matrix_t *mx, fts_symbol_t file_name, fts_symbol_t separator)
{
  fts_atom_file_t *file;
  int m = mx->m;
  int n = mx->n;
  fts_atom_t sep;
  int i, j;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    {
      post("mat: can't open file to write: %s\n", fts_symbol_name(file_name));
      return(-1);
    }

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

void
atom_matrix_config(void)
{
  sym_atom_matrix = fts_new_symbol("atom_matrix");
}

