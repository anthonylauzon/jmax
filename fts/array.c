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
 */

#include <fts/fts.h>

#define ARRAY_DEFAULT_ALLOC_INCREMENT 64

void fts_array_init(fts_array_t *array, int ac, const fts_atom_t *at)
{
  array->atoms = 0;
  array->size = 0;
  array->alloc = 0;
  array->alloc_increment = ARRAY_DEFAULT_ALLOC_INCREMENT;

  if (ac > 0)
    fts_array_set(array, ac, at);
}

void 
fts_array_destroy(fts_array_t *array)
{
  fts_array_clear(array);
  fts_free(array->atoms);
  array->atoms = 0;
  array->size = 0;
  array->alloc = 0;
}

void 
fts_array_set_size(fts_array_t *array, int new_size)
{
  int i;

  if(new_size > array->alloc)
    {
      while (array->alloc < new_size)
	array->alloc += array->alloc_increment;
    
      array->atoms = fts_realloc(array->atoms, array->alloc * sizeof(fts_atom_t));
      
      /* void newly allocated region */
      for(i=array->size; i < array->alloc; i++)
	fts_set_void(array->atoms + i);
    }
  else
    {
      if(new_size < 0)
	new_size = 0;
  
      /* void region cut off at end */
      for(i=new_size; i<array->size; i++)
	fts_atom_assign( array->atoms + i, fts_null);
    }

  array->size = new_size;  
}

void 
fts_array_clear(fts_array_t *array)
{
  if(array->size)
    {
      int i;

      for(i=0; i<array->size; i++)
	fts_atom_assign(array->atoms + i, fts_null);
      
      array->size = 0;
    }
}

void 
fts_array_set(fts_array_t *array, int ac, const fts_atom_t *at)
{
  int i;
 
  fts_array_set_size(array, ac);

  for(i = 0; i < ac; i++)
    fts_atom_assign(array->atoms + i, at + i);
}

void
fts_array_insert(fts_array_t *array, int index, int ac, const fts_atom_t *at)
{
  int size = array->size;
  int i;

  if(index > size)
    index = size;
  
  fts_array_set_size(array, size + ac);

  /* shift elements towards end */
  for(i=size-1; i>=index; i--)
    array->atoms[ac + i] = array->atoms[i];

  for(i=0; i<ac; i++)
    fts_atom_assign(array->atoms + index + i, at + i);
}

void
fts_array_prepend(fts_array_t *array, int ac, const fts_atom_t *at)
{
  fts_array_insert(array, 0, ac, at);
}

void 
fts_array_append(fts_array_t *array, int ac, const fts_atom_t *at)
{
  fts_array_insert(array, array->size, ac, at);
}

void 
fts_array_append_int(fts_array_t *array, int i)
{
  int size = array->size;
 
  fts_array_set_size(array, size + 1);
  fts_set_int(array->atoms + size, i);
}

void 
fts_array_append_float(fts_array_t *array, float f)
{
  int size = array->size;
 
  fts_array_set_size(array, size + 1);
  fts_set_float(array->atoms + size, f);
}

void 
fts_array_append_symbol(fts_array_t *array, fts_symbol_t s)
{
  int size = array->size;
 
  fts_array_set_size(array, size + 1);
  fts_set_symbol(array->atoms + size, s);
}

void
fts_array_cut(fts_array_t *array, int index, int n)
{
  int size = array->size;
  int i;

  if(index + n > size)
    n = size - index;

  if(n < 0)
    n = 0;

  for(i=0; i<n; i++)
    fts_atom_assign(array->atoms + index + i, array->atoms + n + index + i);
  
  fts_array_set_size(array, size - n);
}

void 
fts_array_copy(fts_array_t *org, fts_array_t *copy)
{
  int size = org->size;
  int i;

  fts_array_clear(copy);
  fts_array_set_size(copy, size);

  /* shift array towards end */
  for(i=0; i<size; i++)
    fts_atom_assign(copy->atoms + i, org->atoms + i);
}

/***********************************************
 * 
 *  array iterator
 *
 */

typedef struct array_iterator
{
  fts_array_t *array;
  int index;
} array_iterator_t;

static fts_heap_t *iterator_heap = NULL;

static int
array_iterator_has_more(fts_iterator_t *iter)
{
  array_iterator_t *aiter = (array_iterator_t *)iter->data;

  if(aiter->index >= aiter->array->size)
    {
      fts_heap_free(iter->data, iterator_heap);
      iter->data = NULL;
      return 0;
    }
  else
    return 1;
}

static void
array_iterator_next(fts_iterator_t *iter, fts_atom_t *a)
{
  array_iterator_t *aiter = (array_iterator_t *)iter->data;

  *a = aiter->array->atoms[aiter->index++];
}

void
fts_array_get_values(fts_array_t *array, fts_iterator_t *iter)
{
  if(iterator_heap == NULL)
    iterator_heap = fts_heap_new(sizeof(array_iterator_t));

  iter->has_more = array_iterator_has_more;
  iter->next = array_iterator_next;
  iter->data = (array_iterator_t *)fts_heap_alloc(iterator_heap);
}

