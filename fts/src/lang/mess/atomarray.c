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

#include "sys.h"
#include "lang/mess.h"

/* local */
static void atom_array_realloc(fts_atom_array_t *array, int size)
{
  int i;

  if(size > array->alloc)
    {
      if(array->alloc)
	array->atoms = (fts_atom_t *)fts_realloc((void *)array->atoms, sizeof(fts_atom_t) * size);
      else
	array->atoms = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t) * size);

      array->alloc = size;
    }
  else
    {
      /* if shorter, void atoms at the end (release references) */
      for(i=size; i<array->size; i++)
	{
	  fts_atom_t *atom = array->atoms + i;
	  
	  fts_set_void(atom);
	}
    }
  
  array->size = size;
}

/* new/delete */

fts_atom_array_t *
fts_atom_array_new(int ac, const fts_atom_t *at)
{
  fts_atom_array_t *array;
  int i;

  array = (fts_atom_array_t *)fts_malloc(sizeof(fts_atom_array_t));

  if(ac > 0)
    {
      int i;
      array->atoms = (fts_atom_t *) fts_malloc(ac * sizeof(fts_atom_t));
      array->size = ac;
      
      for(i=0; i<ac; i++)
	fts_set_void(array->atoms + i);
    }
  else
    {
      array->atoms = 0;
      array->size = 0;
    }

  array->alloc = ac;

  /* fill it */
  for(i=0; i<ac; i++)
    array->atoms[i] = at[i];

  return array;
}

void 
fts_atom_array_delete(fts_atom_array_t *array)
{
  fts_atom_array_void(array);

  if (array->atoms)
    fts_free((void *)array->atoms);

  fts_free((void *)array);
}

/* copy & void */
void
fts_atom_array_copy(fts_atom_array_t *in, fts_atom_array_t *out)
{
  int i;
  atom_array_realloc(out, in->size);
  
  for(i=0; i<in->size; i++)
    out->atoms[i] = in->atoms[i];
}

void
fts_atom_array_void(fts_atom_array_t *array)
{
  int i;
  
  for(i=0; i<array->size; i++)
    {
      fts_atom_t *atom = array->atoms + i;
      fts_set_void(atom);
    }
}

/* set the size of the array */
void
fts_atom_array_set_size(fts_atom_array_t *array, int size)
{
  int old_size = array->size;
  int i;

  atom_array_realloc(array, size);

  /* if longer, set new atoms at the end to void */
  for(i=old_size; i<size; i++)
    fts_set_void(array->atoms + i);
}
