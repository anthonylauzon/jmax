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


/**
 * Iterator
 *
 * A FTS iterator is used to enumerate all the elements of a container.
 * It is a generic interface that can be used with hashtables, lists....
 *
 * Example of code using an iterator to enumerate the elements of a hashtable:
 * @code
{
  fts_hashtable_t h;
  fts_iterator_t i;

  fts_hashtable_get_keys( &h, &i);
  while ( fts_iterator_has_more( &i) )
    {
      fts_atom_t a;

      fts_iterator_next( &i, &a);
      fts_post( "%d\n", fts_get_int( a));
    }
}
 * @endcode
 *
 * To enumerate the elements of an atomlist, the only line that will change in
 * previous code is
 *
 * @code
 for ( fts_atomlist_get_atoms( &h, &i); fts_iterator_has_more( &i); )
 * @endcode
 *
 * @defgroup iterator FTS iterator
 * @ingroup fts_utils
 */

/**
 * @typedef fts_iterator_t
 * @brief FTS iterator structure
 * @ingroup iterator
 */
typedef struct _fts_iterator_t fts_iterator_t;

/**
 * @struct _fts_iterator_t
 * @brief the fts array structure
 * @ingroup iterator
 */
struct _fts_iterator_t {
  int (*has_more)( fts_iterator_t *); /**< has_more ... */
  void (*next)( fts_iterator_t *, fts_atom_t *); /**< next ...*/
  void *data; /**< data ... */
};

#ifdef AVOID_MACROS
/**
 * Tests if iterator contains more elements
 *
 * NOTE: once the iterator contains no more elements, its value is undefined.
 * To reuse the same iterator, you must reinitialize if before calling
 * any of its functions.
 *
 * @fn int fts_iterator_has_more(fts_iterator_t *iter)
 * @brief tests if iterator contains more elements
 * @param iter the iterator
 * @return 1 if iterator has more elements, 0 otherwise
 * @ingroup iterator
 */
int fts_iterator_has_more(fts_iterator_t *iter);
/**
 * Store value and advance to next element in iteration
 *
 * @fn void fts_iterator_next(fts_iterator_t *iter, fts_atom_t *a)
 * @brief store value and advance to next element in iteration
 * @param iter the iterator
 * @param a the atom
 * @ingroup iterator
 */
void fts_iterator_next(fts_iterator_t *iter, fts_atom_t *a);
#else
#define fts_iterator_has_more(I) ((*(I)->has_more)( I))
#define fts_iterator_next(I,A) ((*(I)->next)( I, A))
#endif
