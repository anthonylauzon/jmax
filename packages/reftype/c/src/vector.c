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
#include "vector.h"

/* type derived from atom matrix */

fts_type_t vector_type = 0;
fts_symbol_t vector_symbol = 0;
reftype_t *vector_reftype = 0;

vector_t *
vector_new(int size)
{
  vector_t *vec = (vector_t *)matrix_new(size, 1);

  refdata_init((refdata_t *)vec, vector_reftype);

  return vec;
}

/********************************************************
 *
 *  files
 *
 */

#define VECTOR_BLOCK_SIZE 64

static void
vector_grow(vector_t *vec, int size)
{
  int alloc = vec->alloc;

  while(size > alloc)
    alloc += VECTOR_BLOCK_SIZE;

  vector_set_size(vec, alloc);
}

int 
vector_read_atom_file(vector_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int n = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;

  vector_void(vec);

  while(fts_atom_file_read(file, &a, &c))
    {
      if(n >= vec->alloc)
	vector_grow(vec, n);

      vec->data[n] = a;
      n++;
    }

  vector_set_size(vec, n);
  
  fts_atom_file_close(file);

  return (n);
}

int
vector_write_atom_file(vector_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int size = vector_get_size(vec);
  int i;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the vector */
  for(i=0; i<size; i++)     
    {
      fts_atom_t *data = vec->data;

      fts_atom_file_write(file, data + i, '\n');
    }

  fts_atom_file_close(file);

  return (i);
}

/*********************************************************
 *
 *  reference type and expression function
 *
 */

static refdata_t *
vector_constructor_null(int ac, const fts_atom_t *at)
{
  return (refdata_t *)vector_new(0);
}

static refdata_t *
vector_constructor_size(int ac, const fts_atom_t *at)
{
  return (refdata_t *)vector_new(fts_get_int(at));
}

static refdata_t *
vector_constructor_atom_array(int ac, const fts_atom_t *at)
{
  vector_t *vec;
  fts_atom_array_t *aa = fts_get_atom_array(at);
  int size = fts_atom_array_get_size(aa);
  
  vec = vector_new(size);      
  vector_set_from_atom_list(vec, 0, size, fts_atom_array_get_ptr(aa));

  return (refdata_t *)vec;
}

static refdata_t *
vector_constructor_atoms(int ac, const fts_atom_t *at)
{
  vector_t *vec = vector_new(ac);

  vector_set_from_atom_list(vec, 0, ac, at);

  return (refdata_t *)vec;
}

static reftype_constructor_t
vector_dispatcher(int ac, const fts_atom_t *at)
{
  vector_t *vec = 0;

  if(ac == 0)
    return vector_constructor_null;
  else if(ac == 1 && fts_is_int(at))
    return vector_constructor_size;
  else if(ac == 1 && fts_is_atom_array(at))
    return vector_constructor_atom_array;
  else if(ac > 1)
    return vector_constructor_atoms;
  else
    return 0;
}

static void
vector_destructor(refdata_t *vec)
{
  vector_delete((vector_t *)vec);
}

static int
vector_function(int ac, const fts_atom_t *at, fts_atom_t *result)
{
  vector_t *vec = vector_create(ac - 1, at + 1);
  
  if(vec)
    {
      vector_atom_set(result, vec);
      return FTS_EXPRESSION_OK;
    }
  else
    return FTS_EXPRESSION_SYNTAX_ERROR;
}

void
vector_config(void)
{
  vector_symbol = fts_new_symbol("vec");
  vector_type = vector_symbol;

  vector_reftype = reftype_declare(vector_symbol, vector_dispatcher, vector_destructor);

  fts_expression_declare_fun(vector_symbol, vector_function);
}
