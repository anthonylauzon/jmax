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

#ifndef _FTS_ITERATOR_H_
#define _FTS_ITERATOR_H_

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
      post( "%d\n", fts_get_int( a));
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
 * @defgroup iterator iterator
 *
 */

/**
 * @name The FTS iterator structure
 *
 * @typedef fts_iterator_t
 *
 * @ingroup iterator
 */

/*@{*/

typedef struct _fts_iterator_t {
  int (*has_more)( void *);
  void (*next)( void *, fts_atom_t *);
  void *data;
} fts_iterator_t;

/*@}*/
       

/**
 * Tests if iterator contains more elements
 *
 * NOTE: once the iterator contains no more elements, its value is undefined.
 * To reuse the same iterator, you must reinitialize if before calling
 * any of its functions.
 *
 * @fn int fts_iterator_has_more( fts_iterator_t *i)
 * @param i the iterator
 * @return 1 if iterator has more elements, 0 otherwise
 */
#define fts_iterator_has_more(I) ((*(I)->has_more)( (I)->data))

/**
 * Advance to next element in iteration
 *
 * @fn int fts_iterator_next( fts_iterator_t *i)
 * @param i the iterator
 */
#define fts_iterator_next(I,A) ((*(I)->next)( (I)->data,A))

#endif
