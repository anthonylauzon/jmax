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

#include <fts/sys.h>
#include <fts/lang/mess.h>

/************************************************
 *
 *  list utils
 *
 */
 
#define LIST_ALLOC_BLOCK 64

void
fts_list_set_size(fts_list_t *list, int size)
{
  int alloc = list->alloc;

  if(size > alloc)
    {
      fts_atom_t *new_atoms;
      int i;

      while(alloc < size)
	alloc += LIST_ALLOC_BLOCK;

      new_atoms = fts_block_alloc(alloc * sizeof(fts_atom_t));

      if(alloc)
	{
	  /* copy old content */
	  for(i=0; i<list->size; i++)
	    new_atoms[i] = list->atoms[i];
	
	  fts_block_free(list->atoms, list->alloc * sizeof(fts_atom_t));
	}

      /* void newly allocated region */
      for(i=list->size; i<alloc; i++)
	fts_set_void(new_atoms + i);
      
      list->atoms = new_atoms;
      list->size = size;
      list->alloc = alloc;
    }
  else
    {
      int i;

      if(size < 0)
	size = 0;

      /* void region cut off at end */
      for(i=size; i<list->size; i++)
	{
	  fts_atom_t *ap = list->atoms + i;
	  fts_atom_void(ap);
	}

      list->size = size;
    }
}

void
fts_list_init(fts_list_t *list, int ac, const fts_atom_t *at)
{
  int i;

  list->atoms = 0;
  list->size = 0;
  list->alloc = 0;

  fts_list_set_size(list, ac);

  for(i=0; i<ac; i++)
    fts_atom_assign(list->atoms + i, at + i);
}

fts_list_t *
fts_list_new(int ac, const fts_atom_t *at)
{
  fts_list_t *list = (fts_list_t *)fts_malloc(sizeof(fts_list_t));

  fts_list_init(list, ac, at);

  return list;
}

void
fts_list_reset(fts_list_t *list)
{
  fts_list_set_size(list, 0);
  fts_block_free(list->atoms, list->alloc * sizeof(fts_atom_t));
}

void 
fts_list_delete(fts_list_t *list)
{
  fts_list_reset(list);
  fts_free((void *)list);
}

void
fts_list_set(fts_list_t *list, int ac, const fts_atom_t *at)
{
  int i;
 
  fts_list_set_size(list, ac);

  for(i=0; i<ac; i++)
    {
      fts_atom_t *ap = list->atoms + i;
      
      fts_atom_assign(ap, at + i);
    }
}

void
fts_list_append(fts_list_t *list, int ac, const fts_atom_t *at)
{
  int size = list->size;
  int i;
 
  fts_list_set_size(list, size + ac);

  for(i=0; i<ac; i++)
    {
      fts_atom_t *ap = list->atoms + size + i;
      
      fts_atom_assign(ap, at + i);
    }
}

void
fts_list_copy(fts_list_t *in, fts_list_t *out)
{
  int i;

  fts_list_set_size(out, in->size);
  
  for(i=0; i<in->size; i++)
    fts_atom_assign(out->atoms + i, in->atoms + i);
    
}
