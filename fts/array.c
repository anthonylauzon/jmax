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

void fts_array_init( fts_array_t *array, int ac, const fts_atom_t *at)
{
  array->atoms = 0;
  array->size = 0;
  array->alloc = 0;
  array->alloc_increment = ARRAY_DEFAULT_ALLOC_INCREMENT;

  if (ac > 0)
    fts_array_set( array, ac, at);
}

void fts_array_destroy( fts_array_t *array)
{
  fts_array_clear( array);
  fts_free( array->atoms);
}

void fts_array_clear( fts_array_t *array)
{
  fts_array_set_size( array, 0);
}

void fts_array_set_size( fts_array_t *array, int new_size)
{
  int i;

  if ( new_size > array->alloc)
    {
      while ( array->alloc < new_size)
	array->alloc += array->alloc_increment;

      array->atoms = fts_realloc( array->atoms, array->alloc * sizeof(fts_atom_t));

      /* void newly allocated region */
      for( i = array->size; i < array->alloc; i++)
	fts_set_void( array->atoms + i);
    }
  else
    {
      if( new_size < 0)
	new_size = 0;

      /* void region cut off at end */
      /* if new_size >= array->size, this will do nothing */
      for( i = new_size; i < array->size; i++)
	fts_atom_void( array->atoms + i);
    }

  array->size = new_size;
}

void fts_array_set( fts_array_t *array, int ac, const fts_atom_t *at)
{
  int i;
 
  fts_array_set_size( array, ac);

  for( i = 0; i < ac; i++)
    fts_atom_assign( array->atoms + i, at + i);
}

void fts_array_append( fts_array_t *array, int ac, const fts_atom_t *at)
{
  int old_size = array->size;
  int i;
 
  fts_array_set_size( array, old_size + ac);

  for( i = 0; i < ac; i++)
    {
      fts_atom_t *ap = array->atoms + old_size + i;
      
      fts_atom_assign( ap, at + i);
    }
}