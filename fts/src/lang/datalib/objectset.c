/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <assert.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"

#include "runtime.h"

/* Implementation of an object set as FTS Data; note that for the moment
   the implementation is very limited, and include a *readonly* java part.
   */

extern void fts_variable_find_users(fts_patcher_t *scope, fts_symbol_t name, fts_object_set_t *set);

/* Structure definitions */

static fts_data_class_t *fts_object_set_data_class = 0;

typedef struct fts_object_set_cell
{
  fts_object_t *object;
  struct fts_object_set_cell *next;
} fts_object_set_cell_t;

struct fts_object_set
{
  fts_data_t dataobj;
  fts_object_set_cell_t *head;
};

struct fts_object_set_iterator
{
  fts_object_set_cell_t *cell;
};

static fts_heap_t *object_set_cell_heap;
static fts_heap_t *object_set_heap;
static fts_heap_t *object_set_iterator_heap;

/* Remote call codes */

#define REMOTE_CLEAN        1
#define REMOTE_APPEND       2
#define REMOTE_REMOVE_ONE   3
#define REMOTE_FIND         4
#define REMOTE_FIND_ERRORS  5
#define REMOTE_FIND_FRIENDS 6


/********************************************************************/
/*                                                                  */
/*            OBJECT SET                                            */
/*                                                                  */
/********************************************************************/

static fts_object_set_cell_t *
fts_object_set_cell_new(fts_object_t *obj, fts_object_set_cell_t *next)
{
  fts_object_set_cell_t *cell;

  cell = (fts_object_set_cell_t *) fts_heap_alloc(object_set_cell_heap);

  if (cell)
    {
      cell->object = obj;
      cell->next   = next;
    }

  return cell;
}


static void
fts_object_set_cell_free( fts_object_set_cell_t *cell)
{
  fts_heap_free((char *) cell, object_set_cell_heap);
}



fts_object_set_t *
fts_object_set_new( void)
{
  fts_object_set_t *set;

  set = (fts_object_set_t *) fts_heap_alloc(object_set_heap);
  assert( set != 0);
  set->head = 0;
  fts_data_init((fts_data_t *)set, fts_object_set_data_class);

  return set;
}


void
fts_object_set_delete(fts_object_set_t *set)
{
  fts_object_set_cell_t *cell, *next;

  fts_data_delete((fts_data_t *) set);

  for( cell = set->head; cell; cell = next)
    {
      next = cell->next;
      fts_object_set_cell_free( cell);
    }

  fts_heap_free((char *)set, object_set_heap);
}


/* set manipulation */

int fts_object_set_have_member(fts_object_set_t *set, fts_object_t *object)
{
  fts_object_set_cell_t *p;

  for (p = set->head; p; p = p->next)
    if (p->object == object)
      return 1;
  
  return 0;
}

void fts_object_set_add( fts_object_set_t *set, fts_object_t *object)
{
  if (fts_object_set_have_member(set, object))
    return;
  else
    {
      if (fts_data_is_exported((fts_data_t *) set))
	{
	  fts_atom_t a;

	  if (object->id == FTS_NO_ID)
	    fts_client_upload_object(object);

	  fts_set_object(&a, object);
	  fts_data_remote_call((fts_data_t *) set, REMOTE_APPEND, 1, &a);
	}
	    
      set->head = fts_object_set_cell_new(object, set->head);
    }
}


void fts_object_set_remove(fts_object_set_t *set, fts_object_t *object)
{
  fts_object_set_cell_t **p;	/* indirect precusor */

  for (p = &(set->head); *p; p = &((*p)->next))
    if ((* p)->object == object)
      {
	fts_object_set_cell_t *cell;

	cell = (*p);
	(*p) = cell->next;
	fts_object_set_cell_free(cell);

	/* remote remove */

	if (fts_data_is_exported((fts_data_t *) set))
	  {
	    fts_atom_t a;

	    fts_set_object(&a, object);
	    fts_data_remote_call((fts_data_t *) set, REMOTE_REMOVE_ONE, 1, &a);
	  }
	    
	return;
      }
}

void fts_object_set_remove_all(fts_object_set_t *set)
{
  fts_object_set_cell_t *p;

  if (fts_data_is_exported((fts_data_t *) set))
    fts_data_remote_call((fts_data_t *) set, REMOTE_CLEAN, 0, 0);

  p = set->head;
  while (p)
    {
      fts_object_set_cell_t *cell;

      cell = p;
      p = p->next;
      fts_object_set_cell_free(cell);
    }
  set->head = 0;
}

void fts_object_set_send_message(fts_object_set_t *set, int winlet, fts_symbol_t sel,
				 int ac, const fts_atom_t *av)
{
  fts_object_set_cell_t *p;

  for (p = set->head; p; p = p->next)
    fts_send_message(p->object, winlet, sel, ac, av);
}


/********************************************************************/
/*                                                                  */
/*            OBJECT SET ITERATOR                                   */
/*                                                                  */
/********************************************************************/

/* Iterators */


fts_object_set_iterator_t *
fts_object_set_iterator_new(const fts_object_set_t *set)
{
  fts_object_set_iterator_t *iter;

  iter = (fts_object_set_iterator_t *) fts_heap_alloc(object_set_iterator_heap);

  if (iter)
    iter->cell = set->head;

  return iter;
}


void
fts_object_set_iterator_free(fts_object_set_iterator_t *iter)
{
  fts_heap_free((char *) iter, object_set_iterator_heap);
}


void 
fts_object_set_iterator_next(fts_object_set_iterator_t *iter)
{
  if (iter->cell == 0)
    return;

  iter->cell = iter->cell->next;
}


int 
fts_object_set_iterator_end(const fts_object_set_iterator_t *iter)
{
  return iter->cell == 0;
}


fts_object_t *
fts_object_set_iterator_current(const fts_object_set_iterator_t *iter)
{
  return iter->cell->object;
}

/********************************************************************/
/*                                                                  */
/*            FTS_DATA functions on Object sets                     */
/*                                                                  */
/********************************************************************/

/* utilities */

static void fts_object_set_upload_objects(fts_object_set_t *set)
{
  fts_object_set_cell_t *p;

  for (p = set->head; p; p = p->next)
    if (p->object->id == FTS_NO_ID)
      fts_client_upload_object(p->object);
}


/* Just a very limited version for now */


static fts_data_t *fts_object_set_remote_constructor(int ac, const fts_atom_t *at)
{
  return (fts_data_t *) fts_object_set_new();
}


static void fts_object_set_remote_destructor(fts_data_t *d)
{
  fts_object_set_t *this = (fts_object_set_t *)d;

  fts_object_set_delete(this);
}


/*
 * The export function
 */


static void fts_object_set_export_fun(fts_data_t *d)
{
  fts_object_set_t *this = (fts_object_set_t *)d;
  fts_object_set_cell_t *p;

  fts_object_set_upload_objects(this);

  fts_data_remote_call(d, REMOTE_CLEAN, 0, 0);

  fts_data_start_remote_call(d, REMOTE_APPEND);

  for (p = this->head; p; p = p->next)
    fts_client_mess_add_object(p->object);

  fts_data_end_remote_call();
}

static void fts_object_set_find( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_object_set_t *this = (fts_object_set_t *)d;
  fts_object_t *scope = fts_get_object(at);
  fts_atom_t a[256];
  int i;
  
  fts_object_set_remove_all(this);

  fts_set_data(&a[0], (fts_data_t *) this);

  for (i = 1; (i < ac) && (i < 256); i++)
    a[i] = at[i];

  fts_send_message(scope, fts_SystemInlet, fts_s_find, ac, a);
}

static void fts_object_set_find_errors( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_object_set_t *this = (fts_object_set_t *)d;
  fts_object_t *scope = fts_get_object(at);
  fts_atom_t a[1];

  fts_object_set_remove_all(this);

  fts_set_data(&a[0], (fts_data_t *) this);
  fts_send_message(scope, fts_SystemInlet, fts_s_find_errors, 1, a);
}

static void fts_object_set_find_friends( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_object_set_t *this = (fts_object_set_t *)d;
  fts_object_t *target = fts_get_object(at);

  fts_object_set_remove_all(this);

  if (fts_object_get_variable(target))
    {
      fts_variable_find_users(target->patcher, fts_object_get_variable(target), this);
    }
  else
    {
      fts_atom_t a[1];

      fts_set_data(&a[0], (fts_data_t *) this);
      fts_send_message(target, fts_SystemInlet, fts_s_find_friends, 1, a);
    }
}


/********************************************************************/
/*                                                                  */
/*            INIT_DATA functions on integer vectors                */
/*                                                                  */
/********************************************************************/

void fts_object_set_config(void)
{
  object_set_cell_heap = fts_heap_new(sizeof(fts_object_set_cell_t));
  object_set_heap = fts_heap_new(sizeof( fts_object_set_t));
  object_set_iterator_heap = fts_heap_new(sizeof( fts_object_set_iterator_t));

  fts_object_set_data_class = fts_data_class_new( fts_new_symbol( "object_set_data"));
  fts_data_class_define_export_function(fts_object_set_data_class, fts_object_set_export_fun);

  fts_data_class_define_remote_constructor(fts_object_set_data_class, fts_object_set_remote_constructor);
  fts_data_class_define_remote_destructor(fts_object_set_data_class, fts_object_set_remote_destructor);

  fts_data_class_define_function(fts_object_set_data_class, REMOTE_FIND, fts_object_set_find);
  fts_data_class_define_function(fts_object_set_data_class, REMOTE_FIND_ERRORS, fts_object_set_find_errors);
  fts_data_class_define_function(fts_object_set_data_class, REMOTE_FIND_FRIENDS, fts_object_set_find_friends);
}
