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


/**
 * Lists
 *
 * The API for FTS lists.
 *
 * @code
{
  fts_list_t *list = NULL;
  fts_list_t *p;
  fts_atom_t a, *ap;
  int i;

  for (i = 0; i < 10; i++) {
    fts_set_int(&a, i);
    list = fts_list_append(list, &a);
    post("appending %d\n", i);
  }

  p = list;
  while (p) {
    ap = fts_list_get( p);
    p = fts_list_next( p);
    post("reading %d\n", fts_get_int(ap));
  }

  p = fts_list_get_nth(list, 5);
  post("5th element = %d\n", fts_get_int(fts_list_get(p)));

  for (i = 0; i < 5; i++) {
    fts_set_int(&a, i);
    list = fts_list_remove(list, &a);
    post("removing %d\n", i);
  }
  
  fts_list_delete(list);
}
 * @endcode
 *
 * @defgroup list list
 */

typedef struct _fts_list_t fts_list_t;

#define fts_list_init(l) ((l) = NULL)

/**
 * Appends a new node with the given data to the end of the list. The
 * functions returns the list. If the argument "list" is NULL, a new
 * list is allocated and returned.
 *
 * @fn fts_list_t *fts_list_append(fts_list_t *list, const fts_atom_t *data)
 * @param list the list 
 * @param data the data of the new node 
 * @return the new list
 * @ingroup list */
FTS_API fts_list_t *fts_list_append(fts_list_t *list, const fts_atom_t *data);

/**
 * Prepends a new node with the given data to the beginning of the
 * list. The functions returns the list. If the argument "list" is
 * NULL, a new list is allocated and returned.
 *
 * @fn fts_list_t *fts_list_prepend(fts_list_t *list, const fts_atom_t *data)
 * @param list the list 
 * @param data the data of the new node 
 * @return the new list
 * @ingroup list */
FTS_API fts_list_t *fts_list_prepend(fts_list_t *list, const fts_atom_t *data);

/**
 * Removes a new node from the list. Returns the list with the data
 * removed.  The functions returns the list or NULL if the list has no
 * more nodes.
 *
 * @fn fts_list_t *fts_list_remove(fts_list_t *list, const fts_atom_t *data)
 * @param list the list 
 * @param data the data to be removed 
 * @return the new list
 * @ingroup list */
FTS_API fts_list_t *fts_list_remove(fts_list_t *list, const fts_atom_t *data);

/**
 * Returns the next node of the list or NULL when the end of the list
 * is reached.
 *
 * @fn fts_list_t *fts_list_next(fts_list_t *list)
 * @param list the list 
 * @return the next node
 * @ingroup list */
FTS_API fts_list_t *fts_list_next(fts_list_t *list);

/**
 * Returns the data associated with the first node of the list.
 *
 * @fn fts_atom_t *fts_list_get(fts_list_t *list);
 * @param list the list 
 * @return a pointer to the atom contained in first node
 * @ingroup list */
FTS_API fts_atom_t *fts_list_get(fts_list_t *list);

/**
 * Sets the data associated with the first node of the list.
 *
 * @fn void fts_list_set(fts_list_t *list, const fts_atom_t *data)
 * @param list the list 
 * @param data the new data of the node 
 * @ingroup list */
FTS_API void fts_list_set(fts_list_t *list, const fts_atom_t *data);

/**
 * Returns the n-th node of the list or NULL if the length of the list
 * is smaller than n.
 *
 * @fn fts_list_t *fts_list_get_nth(fts_list_t *list, int n)
 * @param list the list 
 * @param n the node number
 * @return the n-th node
 * @ingroup list */
FTS_API fts_list_t *fts_list_get_nth(fts_list_t *list, int n);

/**
 * Deletes the list and all its nodes.
 *
 * @fn void fts_list_delete(fts_list_t *list)
 * @param list the list 
 * @ingroup list */
FTS_API void fts_list_delete( fts_list_t *list);

/***********************************************
 *
 *  List iterator
 */

/**
 * Returns a new iterator object for this list. 
 *
 * @fn void fts_list_get_values( const fts_list_t *list, fts_iterator_t *i )
 * @param list the list to be iterated
 * @param i the iterator
 * @ingroup list */
FTS_API void fts_list_get_values( const fts_list_t *list, fts_iterator_t *i );


