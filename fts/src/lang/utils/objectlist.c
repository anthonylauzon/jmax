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

#include <assert.h>
#include <fts/lang.h>

static fts_heap_t *cell_heap;

void fts_object_list_init( fts_object_list_t *list)
{
  list->head = 0;

  if ( !cell_heap)
    cell_heap = fts_heap_new( sizeof( struct _fts_object_list_cell));
}

void fts_object_list_destroy( fts_object_list_t *list)
{
  fts_object_list_cell_t *p, *next;

  for( p = list->head; p; p = next)
    {
      next = p->next;

      fts_heap_free( p, cell_heap);
    }

  list->head = 0;
}

void fts_object_list_insert( fts_object_list_t *list, fts_object_t *object)
{
  fts_object_list_cell_t *p;

  p = (fts_object_list_cell_t *) fts_heap_alloc( cell_heap);
  assert( p);

  p->object = object;
  p->next = list->head;

  list->head = p;
}

void fts_object_list_remove( fts_object_list_t *list, fts_object_t *object)
{
  fts_object_list_cell_t **p, *c;

  for (p = &list->head; *p; p = &(*p)->next)
    {
      if ((*p)->object == object)
	{
	  c = *p;
	  *p = c->next;
	  fts_heap_free( c, cell_heap);

	  return;
	}
    }
}
