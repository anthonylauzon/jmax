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
 */
#include <fts/fts.h>

/*
 *  Globals
 */
static fts_heap_t *list_heap = 0;

/*
 *  List structure
 */
struct _fts_list_t {
  fts_list_t* next;
  fts_atom_t data;
};

fts_list_t *fts_list_last(fts_list_t *list);


/***********************************************************************
 *
 * Initialization
 *
 */
void 
fts_kernel_list_init(void)
{
  list_heap = fts_heap_new(sizeof(fts_list_t));
}


/***********************************************************************
 *
 * List
 *
 */

fts_list_t*
fts_list_append(fts_list_t *list, const fts_atom_t *data)
{
  fts_list_t *node, *last;

  node = (fts_list_t*) fts_heap_alloc(list_heap);
  node->next = NULL;
  node->data = *data;

  if (list == NULL) {
    return node;
  }

  last = fts_list_last(list);
  last->next = node;
  
  return list;
}

fts_list_t* 
fts_list_prepend(fts_list_t *list, const fts_atom_t *data)
{
  fts_list_t *node;

  node = (fts_list_t*) fts_heap_alloc(list_heap);
  node->next = list;
  node->data = *data;
  return node;
}

fts_list_t* 
fts_list_remove(fts_list_t *list, const fts_atom_t *data)
{
  fts_list_t *tmp;
  fts_list_t *prev;

  prev = NULL;
  tmp = list;

  while (tmp) {
    if (fts_atom_equals(&tmp->data, data)) {
      if (prev) {
	prev->next = tmp->next;
      }
      if (list == tmp) {
	list = list->next;
      }

      tmp->next = NULL;
      fts_list_delete(tmp);

      break;
    }

    prev = tmp;
    tmp = tmp->next;
  }

  return list;
}

fts_list_t* 
fts_list_next(fts_list_t *list)
{
  return (list)? list->next : NULL;
}

fts_atom_t* 
fts_list_get(fts_list_t *list)
{
  return (list)? &list->data : NULL;
}

void 
fts_list_set(fts_list_t *list, const fts_atom_t *data)
{
  if (list) {
    list->data = *data;
  }
}

fts_list_t* 
fts_list_get_nth(fts_list_t *list, int n)
{
 while ((n-- > 0) && list) {
    list = list->next;
  }
  return list;
}

void 
fts_list_delete(fts_list_t *list)
{
  fts_list_t* tmp;

  while (list) {
    tmp = list->next;
    fts_heap_free(list, list_heap);
    list = tmp;
  }
}

fts_list_t* 
fts_list_last(fts_list_t *list)
{
 if (list) {
    while (list->next) {
      list = list->next;
    }
  }

  return list;
}

/***********************************************
 *
 *  List iterator
 */

static void list_iterator_next(fts_iterator_t *i, fts_atom_t *a);
static int list_iterator_has_more(fts_iterator_t *i);

void 
fts_list_get_values( const fts_list_t *list, fts_iterator_t *i)
{
  i->has_more = list_iterator_has_more;
  i->next = list_iterator_next;
  i->data = (void *)list;
}

static void 
list_iterator_next(fts_iterator_t *i, fts_atom_t *a)
{
  fts_list_t* list = (fts_list_t*) i->data;
  *a = list->data;
  i->data = list->next;
}

static int 
list_iterator_has_more(fts_iterator_t *i)
{
  return i->data != NULL;
}
