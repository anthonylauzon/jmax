/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"
#include "runtime.h"		/* @@@ */
/* Heaps */

static fts_heap_t *atom_list_cell_heap;
static fts_heap_t *atom_list_heap;
static fts_heap_t *atom_list_iterator_heap;

/* Remote call codes */

#define ATOM_LIST_UPDATE 1
#define ATOM_LIST_SET    2

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

static fts_data_class_t *fts_atom_list_data_class = 0;

struct fts_atom_list
{
  fts_data_t dataobj;
  fts_atom_list_cell_t *head;
  fts_atom_list_cell_t *tail;
};


fts_atom_list_t *fts_atom_list_new( void)
{
  fts_atom_list_t *list;

  list = (fts_atom_list_t *) fts_heap_alloc(atom_list_heap);

  if (list)
    {
      list->head = 0;
      list->tail = 0;
    }

  fts_data_init((fts_data_t *) list, fts_atom_list_data_class);

  return list;
}

void fts_atom_list_free( fts_atom_list_t *list)
{
  fts_data_delete((fts_data_t *) list);
  fts_atom_list_clear( list);
  fts_heap_free((char *)list, atom_list_heap);
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
      if (fts_atom_are_equals(&at[0], fts_atom_list_iterator_current(iterator)))
	{
	  int i;

	  i = 1;
	  fts_atom_list_iterator_copy(ii, iterator);
	  fts_atom_list_iterator_next(ii);

	  while ((i < ac) && (! fts_atom_list_iterator_end(ii)) &&
		 fts_atom_are_equals(&at[i], fts_atom_list_iterator_current(ii)))
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

/********************************************************************************/
/*                                                                              */
/*                     Saving Atom Lists to Bmax files                          */
/*                                                                              */
/********************************************************************************/

/* this function save an atom list to a bmax file as a set of messages
   to an object (passed as argument).
   The first message is "clear" to clean the atom list,
   the others are a number of append, each of max 256 elements.
   Note that the there is provision here for the loading; the messages
   must be understood natively by the object saving the atom list (within
   its save_bmax method); in the future, a better support for data will
   be implemented.
   */

void fts_atom_list_save_bmax(fts_atom_list_t *list, fts_bmax_file_t *f, fts_object_t *target)
{
  fts_atom_list_iterator_t *iterator;
  fts_atom_t av[256];
  int ac = 0;

  /* Code a "clear" message without arguments */

  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_clear, 0);

  iterator = fts_atom_list_iterator_new(list);

  while (! fts_atom_list_iterator_end(iterator))
    {
      av[ac++] = *fts_atom_list_iterator_current(iterator);
      fts_atom_list_iterator_next(iterator);

      if (ac == 256)
	{
	  /* Code a push of all the values */

	  fts_bmax_code_push_atoms(f, ac, av);

	  /* Code an "append" message for 256 values */

	  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_append, ac);

	  /* Code a pop of all the values  */

	  fts_bmax_code_pop_args(f, ac);

	  /* Put the arg counter to zero */

	  ac = 0;
	}
    }

  if (ac != 0)
    {
      /* Code a push of all the values */

      fts_bmax_code_push_atoms(f, ac, av);

      /* Code an "append" message for the values left */
      
      fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_append, ac);

      /* Code a pop of all the values  */

      fts_bmax_code_pop_args(f, ac);
    }

  fts_atom_list_iterator_free(iterator);
}


/********************************************************************/
/*                                                                  */
/*            FTS_DATA functions on atom list                       */
/*                                                                  */
/********************************************************************/

/*
 * The export function
 */


static void fts_atom_list_export_fun(fts_data_t *d)
{
  fts_atom_list_iterator_t *iterator;
  fts_atom_list_t *this = (fts_atom_list_t *)d;

  iterator = fts_atom_list_iterator_new(this);

  fts_data_start_remote_call(d, ATOM_LIST_SET);

  while (! fts_atom_list_iterator_end(iterator))
    {
      fts_client_mess_add_atoms(1, fts_atom_list_iterator_current(iterator));
      fts_atom_list_iterator_next(iterator);
    }

  fts_data_end_remote_call();

  fts_atom_list_iterator_free(iterator);
}

/*
 * The remote set function.
 */

static void fts_atom_list_remote_set( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_atom_list_t *this = (fts_atom_list_t *)d;

  fts_atom_list_clear(this);
  fts_atom_list_set(this, ac, at);
}

static void fts_atom_list_remote_update( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_atom_list_iterator_t *iterator;
  fts_atom_list_t *this = (fts_atom_list_t *)d;

  iterator = fts_atom_list_iterator_new(this);

  fts_data_start_remote_call(d, ATOM_LIST_SET);

  while (! fts_atom_list_iterator_end(iterator))
    {
      fts_client_mess_add_atoms(1, fts_atom_list_iterator_current(iterator));
      fts_atom_list_iterator_next(iterator);
    }

  fts_data_end_remote_call();

  fts_atom_list_iterator_free(iterator);
}

/********************************************************************/
/*                                                                  */
/*            INIT_DATA functions on integer vectors                */
/*                                                                  */
/********************************************************************/

void fts_atom_list_config(void)
{
  atom_list_cell_heap     = fts_heap_new(sizeof( fts_atom_list_cell_t));
  atom_list_heap          = fts_heap_new(sizeof( fts_atom_list_t));
  atom_list_iterator_heap = fts_heap_new(sizeof( fts_atom_list_iterator_t));

  fts_atom_list_data_class = fts_data_class_new( fts_new_symbol( "atom_list_data"));
  fts_data_class_define_export_function(fts_atom_list_data_class, fts_atom_list_export_fun);
  fts_data_class_define_function(fts_atom_list_data_class, ATOM_LIST_SET, fts_atom_list_remote_set);
  fts_data_class_define_function(fts_atom_list_data_class, ATOM_LIST_UPDATE, fts_atom_list_remote_update);
}










