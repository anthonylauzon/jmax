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

#include <fts/fts.h>
#include "list.h"

/************************************************
 *
 *  list utils
 *
 */
 
#define LIST_ALLOC_BLOCK 64

void
list_init(list_t *list)
{
  int i;
  
  list->at = fts_block_alloc(LIST_ALLOC_BLOCK * sizeof(fts_atom_t));
  list->ac = 0;
  list->alloc = LIST_ALLOC_BLOCK;
  
  /* set all atoms to void */
  for(i=0; i<LIST_ALLOC_BLOCK; i++)
    fts_set_void(list->at + i);
}

void
list_free(list_t *list)
{
  if(list->alloc)
    fts_block_alloc(list->alloc * sizeof(fts_atom_t));
}

void
list_set_size(list_t *list, int size)
{
  int alloc = list->alloc;

  if(size > alloc)
    {
      int i;

      if(alloc)
	fts_block_free(list->at, alloc * sizeof(fts_atom_t));

      while(alloc < size)
	alloc += LIST_ALLOC_BLOCK;

      list->at = fts_block_alloc(alloc * sizeof(fts_atom_t));

      /* set newly allocated region to void */
      for(i=list->alloc; i<size; i++)
	fts_set_void(list->at + i);
      
      list->alloc = alloc;
    }
  else
    {
      int i;

      if(size <= 0)
	size = 0;

      /* void region cut off at end */
      for(i=size; i<list->ac; i++)
	{
	  fts_atom_t *ap = list->at + i;

	  if(fts_is_object(ap))
	    fts_release(ap);
	  
	  fts_set_void(ap);
	}
    }

  list->ac = size;
}

void
list_set(list_t *list, int ac, const fts_atom_t *at)
{
  int i;
 
  list_set_size(list, ac);

  for(i=0; i<ac; i++)
    {
      fts_atom_t *ap = list->at + i;
      
      if(fts_is_object(ap))
	fts_release(ap);
      
      *ap = at[i];
      
      if(fts_is_object(at + i))
	fts_refer(at + i);	
    }
}

void
list_raw_resize(list_t *list, int size)
{
  int alloc = list->alloc;

  if(size > alloc)
    {
      if(alloc)
	fts_block_free(list->at, list->alloc);

      while(alloc < size)
	alloc += LIST_ALLOC_BLOCK;

      list->at = (fts_atom_t *)fts_block_alloc(alloc * sizeof(fts_atom_t));
      list->alloc = alloc;
    }

  list->ac = size;
}

void 
list_raw_set(list_t *list, int offset, int ac, const fts_atom_t *at)
{
  fts_atom_t *ap = list->at + offset;
  int i;
 
  for(i=0; i<ac; i++)
    ap[i] = at[i];
}


/************************************************
 *
 *    object
 *
 */
 
typedef struct 
{
  fts_object_t o;
  list_t list;
} list_obj_t;

static void
list_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  list_init(&this->list);
  list_set(&this->list, ac - 1, at + 1);
}

static void
list_obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  list_free(&this->list);
}

/************************************************
 *
 *  user methods
 *
 */

static void
list_obj_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  fts_outlet_send(o, 0, fts_s_list, list_get_size(&this->list), list_get_ptr(&this->list));
}

static void
list_obj_store_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  list_set(&this->list, ac, at);
}

static void
list_obj_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  list_set(&this->list, ac, at);
  fts_outlet_send(o, 0, fts_s_list, list_get_size(&this->list), list_get_ptr(&this->list));
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
list_obj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */
  fts_class_init(cl, sizeof(list_obj_t), 2, 1, 0); 

  /* define the system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, list_obj_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, list_obj_delete);

  /* user methods */
  fts_method_define_varargs(cl, 0, fts_s_bang, list_obj_bang);

  fts_method_define_varargs(cl, 0, fts_s_list, list_obj_list);
  fts_method_define_varargs(cl, 1, fts_s_list, list_obj_store_list);

  /* type the outlet */
  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
list_config(void)
{
  fts_class_install(fts_new_symbol("list"), list_obj_instantiate);
  fts_class_alias(fts_new_symbol("l"), fts_new_symbol("list"));
}
