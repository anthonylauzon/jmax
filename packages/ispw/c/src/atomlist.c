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

#include "atomlist.h"
#include <ftsconfig.h>

#if HAVE_ALLOCA_H
#include <alloca.h>
#else
#include <stdlib.h>
#endif

/* Heaps */

static fts_heap_t *atom_list_cell_heap;
static fts_heap_t *atom_list_iterator_heap;

fts_metaclass_t *atomlist_type = 0;
fts_symbol_t sym_setValues = 0;
fts_symbol_t sym_setName = 0;
fts_symbol_t sym_atomlist_set = 0;  
fts_symbol_t sym_atomlist_set_name = 0;  
fts_symbol_t sym_atomlist_update = 0;  

/********************************************************************************/
/*                                                                              */
/*                           Atom List Cell                                     */
/*                                                                              */
/********************************************************************************/

#define FTS_ATOM_LIST_CELL_MAX_ATOMS 16

typedef struct fts_atom_list_cell
{
  int n;
  fts_atom_t atoms[FTS_ATOM_LIST_CELL_MAX_ATOMS];
  struct fts_atom_list_cell *next;
} fts_atom_list_cell_t;


static fts_atom_list_cell_t *fts_atom_list_cell_new( void )
{
  fts_atom_list_cell_t *cell;

  cell = (fts_atom_list_cell_t *) fts_heap_alloc(atom_list_cell_heap);

  if (cell)
    {
      int i;

      cell->n = 0;

      for( i = 0; i < FTS_ATOM_LIST_CELL_MAX_ATOMS; i++)
	fts_set_void( cell->atoms+i);

      cell->next = 0;
    }

  return cell;
}


static void fts_atom_list_cell_free( fts_atom_list_cell_t *cell)
{
  fts_heap_free((char *) cell, atom_list_cell_heap);
}


/********************************************************************************/
/*                                                                              */
/*                           Atom List                                          */
/*                                                                              */
/********************************************************************************/

struct fts_atom_list
{
  fts_object_t ob;

  fts_symbol_t name;	       /* list name */
  int size;
  fts_atom_list_cell_t *head;
  fts_atom_list_cell_t *tail;
};


static void fts_atom_list_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_list_t *this = (fts_atom_list_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);

  if(name) 
      this->name = name;
  else
      this->name = 0;
  
  this->size = 0;
  this->head = 0;
  this->tail = 0;
}

static void fts_atom_list_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_list_t *this = (fts_atom_list_t *)o;

  fts_atom_list_clear(this);
  fts_free((void *)this);
}

static void fts_atom_list_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_list_t *this = (fts_atom_list_t *)o;
  fts_atom_list_iterator_t *iterator;
  fts_atom_t* a;
  int i =0;

#if HAVE_ALLOCA
  a = alloca((this->size + 1) * sizeof(fts_atom_t));
#else
  a = malloc((this->size + 1) * sizeof(fts_atom_t));
#endif

  if(!fts_object_has_id((fts_object_t *)this))
    fts_client_upload_object((fts_object_t *)this, -1);

  if (this->name)
    {
      fts_set_symbol(a, this->name);
      fts_client_send_message((fts_object_t *)this, sym_setName, 1, a);
    }

  iterator = fts_atom_list_iterator_new(this);

  while (! fts_atom_list_iterator_end(iterator))
    {
      a[i] = *fts_atom_list_iterator_current(iterator);
      fts_atom_list_iterator_next(iterator);
      i++;
    }
  fts_client_send_message((fts_object_t *)this, sym_setValues, i, a);

  fts_atom_list_iterator_free(iterator);

#ifndef HAVE_ALLOCA
  free(a);
#endif
}

/* list manipulation */

void fts_atom_list_clear( fts_atom_list_t *list)
{
  fts_atom_list_cell_t *cell, *next;

  for( cell = list->head; cell; cell = next)
    {
      next = cell->next;
      fts_atom_list_cell_free( cell);
    }

  list->head = 0;
  list->tail = 0;
  list->size = 0;
}

static void fts_atom_list_append_one( fts_atom_list_t *list, const fts_atom_t *atom)
{
  fts_atom_list_cell_t *tail;

  if ( !list->tail )
    {
      list->head = fts_atom_list_cell_new();
      list->tail = list->head;
    }
  tail = list->tail;
  if ( tail->n >= FTS_ATOM_LIST_CELL_MAX_ATOMS )
    {
      tail->next = fts_atom_list_cell_new();
      tail = tail->next;
      list->tail = tail;
    }
  tail->atoms[tail->n] = *atom;
  tail->n++;
  list->size++;  
}

void fts_atom_list_append( fts_atom_list_t *list, int ac, const fts_atom_t *atom)
{
  int i;

  for (i = 0; i < ac; i++)
    fts_atom_list_append_one(list, atom + i);
}

void fts_atom_list_set( fts_atom_list_t *list, int ac, const fts_atom_t *atom)
{
  fts_atom_list_clear( list);
  fts_atom_list_append(list, ac, atom);
}

static void fts_atom_list_client_set( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_list_t *this = (fts_atom_list_t *)o;

  fts_atom_list_clear( this);
  fts_atom_list_append(this, ac, at);
}

static void fts_atom_list_set_name( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_list_t *this = (fts_atom_list_t *)o;
  this->name = fts_get_symbol(at);
}


static void fts_atom_list_update( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    fts_atom_list_t *this = (fts_atom_list_t *)o;
    fts_atom_list_iterator_t *iterator;
    fts_atom_t* a; 
    int i =0;

#if HAVE_ALLOCA
  a = alloca(this->size * sizeof(fts_atom_t));
#else
  a = malloc(this->size * sizeof(fts_atom_t));
#endif

    iterator = fts_atom_list_iterator_new(this);

    while (! fts_atom_list_iterator_end(iterator))
      {
	a[i] = *fts_atom_list_iterator_current(iterator);
	fts_atom_list_iterator_next(iterator);
	i++;
      }

    fts_client_send_message((fts_object_t *)this, sym_setValues, i, a);

    fts_atom_list_iterator_free(iterator);

#ifndef HAVE_ALLOCA
    free(a);
#endif
}

/* Check for subsequences */

int
fts_atom_list_is_subsequence( fts_atom_list_t *list, int ac, const fts_atom_t *at)
{
  fts_atom_list_iterator_t *iterator;
  fts_atom_list_iterator_t *ii;

  /* Look for the first key atom in the atom list;
     if found, try to see the rest of the sequence is there
     */

  iterator = fts_atom_list_iterator_new(list);
  ii = fts_atom_list_iterator_new(list);

  while (! fts_atom_list_iterator_end(iterator))
    {
	if (fts_atom_equals(&at[0], fts_atom_list_iterator_current(iterator)))
	{
	  int i;

	  i = 1;
	  fts_atom_list_iterator_copy(ii, iterator);
	  fts_atom_list_iterator_next(ii);

	  while ((i < ac) && (! fts_atom_list_iterator_end(ii)) &&
		 fts_atom_equals(&at[i], fts_atom_list_iterator_current(ii)))
	    {
	      i++;
	      fts_atom_list_iterator_next(ii);
	    }

	  if (i == ac)
	    {
	      /* Found, return 1 */
	      fts_atom_list_iterator_free(iterator);
	      fts_atom_list_iterator_free(ii);
	      return 1;
	    }

	  if (fts_atom_list_iterator_end(ii))
	    {
	      /* Not found because the atom list is finished,
		 uless to continue, return 0 */
	      fts_atom_list_iterator_free(iterator);
	      fts_atom_list_iterator_free(ii);
	      return 0;
	    }

	  /* Not found, continue */
	}

      fts_atom_list_iterator_next(iterator);
    }

  fts_atom_list_iterator_free(iterator);
  fts_atom_list_iterator_free(ii);

  return 0;
}

/********************************************************************************/
/*                                                                              */
/*                           Atom List Iterators                                */
/*                                                                              */
/********************************************************************************/

struct fts_atom_list_iterator
{
  fts_atom_list_cell_t *cell;
  fts_atom_t *atom;
  fts_atom_t *last;
};

fts_atom_list_iterator_t *
fts_atom_list_iterator_new( const fts_atom_list_t *list)
{
  fts_atom_list_iterator_t *iter;

  iter = (fts_atom_list_iterator_t *) fts_heap_alloc(atom_list_iterator_heap);

  if (iter)
    fts_atom_list_iterator_init( iter, list);

  return iter;
}

void fts_atom_list_iterator_free(fts_atom_list_iterator_t *iter)
{
  fts_heap_free((char *) iter, atom_list_iterator_heap);
}

void fts_atom_list_iterator_copy(fts_atom_list_iterator_t *iter, fts_atom_list_iterator_t *other)
{
  iter->cell = other->cell;
  iter->atom = other->atom;
  iter->last = other->last;
}

void fts_atom_list_iterator_init( fts_atom_list_iterator_t *iter, const fts_atom_list_t *list)
{
  iter->cell = list->head;

  if (iter->cell)
    {
      iter->atom = iter->cell->atoms;
      iter->last = iter->atom + iter->cell->n - 1;
    }
}

void fts_atom_list_iterator_next( fts_atom_list_iterator_t *iter)
{
  if ( fts_atom_list_iterator_end( iter) )
    return;

  if (iter->atom >= iter->last)
    {
      iter->cell = iter->cell->next;
      if ( iter->cell)
	{
	  iter->atom = iter->cell->atoms;
	  iter->last = iter->atom + iter->cell->n - 1;
	}
    }
  else
    iter->atom++;
}

int fts_atom_list_iterator_end( const fts_atom_list_iterator_t *iter)
{
  return iter->cell == 0;
}

fts_atom_t *fts_atom_list_iterator_current( const fts_atom_list_iterator_t *iter)
{
  return iter->atom;
}

void 
fts_atom_list_dump(fts_atom_list_t *list, fts_dumper_t *dumper, fts_object_t *target)
{
  fts_atom_list_iterator_t *iterator;
  fts_message_t *mess;
  
  /* send clear message */
  fts_dumper_send(dumper, fts_s_clear, 0, 0);

  /* start new append message */
  mess = fts_dumper_message_new(dumper, fts_s_append);

  /* iterate over atom list */
  iterator = fts_atom_list_iterator_new(list);

  while(!fts_atom_list_iterator_end(iterator))
    {
      /* append atom to message */
      fts_message_append(mess, 1, fts_atom_list_iterator_current(iterator));

      if(fts_message_get_ac(mess) >= 256)
	{
	  /* send append message */
	  fts_dumper_message_send(dumper, mess);

	  /* start new append message */
	  mess = fts_dumper_message_new(dumper, fts_s_append);
	}

      /* nest iteration */
      fts_atom_list_iterator_next(iterator);
    }

  /* send rest of atoms */
  if(fts_message_get_ac(mess) > 0)
    fts_dumper_message_send(dumper, mess);

  fts_atom_list_iterator_free(iterator);
}

static fts_status_t
atom_list_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_atom_list_t), 0, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_atom_list_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fts_atom_list_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload, fts_atom_list_upload);

  fts_method_define_varargs(cl, fts_SystemInlet, sym_atomlist_set, fts_atom_list_client_set);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_atomlist_set_name, fts_atom_list_set_name);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_atomlist_update, fts_atom_list_update);

  return fts_Success;
}

/********************************************************************/
/*                                                                  */
/*            init functions on integer vectors                */
/*                                                                  */
/********************************************************************/

void atom_list_config(void)
{
  atom_list_cell_heap = fts_heap_new(sizeof( fts_atom_list_cell_t));
  atom_list_iterator_heap = fts_heap_new(sizeof( fts_atom_list_iterator_t));

  sym_setValues = fts_new_symbol("setValues");
  sym_setName = fts_new_symbol("setName");
  sym_atomlist_set_name = fts_new_symbol("atomlist_set_name");  
  sym_atomlist_set = fts_new_symbol("atomlist_set");  
  sym_atomlist_update = fts_new_symbol("atomlist_update");  

  atomlist_type = fts_class_install(fts_new_symbol("ispw_atomlist"), atom_list_instantiate);
}
