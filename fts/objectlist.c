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

static fts_symbol_t sym_objectlist = NULL;
static fts_heap_t *objectlist_cell_heap = NULL;

void 
fts_objectlist_init( fts_objectlist_t *list)
{
  list->head = 0;
}

void
fts_objectlist_destroy( fts_objectlist_t *list)
{
  fts_objectlist_cell_t *p, *next;

  for( p = list->head; p; p = next)
  {
    next = p->next;
    
    fts_heap_free( p, objectlist_cell_heap);
  }
  
  list->head = 0;
}

void
fts_objectlist_insert( fts_objectlist_t *list, fts_object_t *object)
{
  fts_objectlist_cell_t *p;
  
  p = (fts_objectlist_cell_t *) fts_heap_alloc( objectlist_cell_heap);
  
  p->object = object;
  p->next = list->head;
  
  list->head = p;
}

void
fts_objectlist_remove( fts_objectlist_t *list, fts_object_t *object)
{
  fts_objectlist_cell_t **p, *c;
  
  for (p = &list->head; *p; p = &(*p)->next)
  {
    if ((*p)->object == object)
    {
      c = *p;
      *p = c->next;
      fts_heap_free( c, objectlist_cell_heap);
      
      return;
    }
  }
}

/***********************************************************************
*
* initialization
*
*/
FTS_MODULE_INIT(objectlist)
{
  sym_objectlist = fts_new_symbol("objectlist");
  objectlist_cell_heap = fts_shared_get(sym_objectlist);
  
  if(objectlist_cell_heap == NULL)
  {
    objectlist_cell_heap = fts_heap_new(sizeof( fts_objectlist_cell_t));  
    fts_shared_set(sym_objectlist, objectlist_cell_heap);
  }
}
