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

#ifndef _FTS_HASHTAB_H_
#define _FTS_HASHTAB_H_

/*****************************************************************************/
/*                                                                           */
/* Hash tables                                                               */
/*                                                                           */
/*****************************************************************************/

/*
  Structure: fts_hash_table_t
  Description:
    A hash table used to store and retrieve atoms by key.

    It uses separate chaining for collision handling (the table is
    an array of lists).
*/
typedef struct _hash_cell_t hash_cell_t;

typedef struct _hash_table_t {
  /* The length of the hash table */
  int length;
  /* The table itself */
  hash_cell_t **table;
} fts_hash_table_t;


/*
  Function: fts_hash_table_init
  Description:
    Initializes a hash table
  Arguments:
    [out] table   a pointer to the hash table
  Returns:
    none
*/
extern void fts_hash_table_init( fts_hash_table_t *ht);

/*
  Function: fts_hash_table_destroy
  Description:
    De-initializes a hash table
    Mainly frees the memory internally allocated
  Arguments:
    [out] table   a pointer to the hash table
  Returns:
    none
*/
extern void fts_hash_table_destroy( fts_hash_table_t *ht);

/*
  Function: fts_hash_table_get
  Description:
    Lookup entry `key' in hash_table `table'.
    If found, copy value in `value'.
  Arguments:
    [in] table   a pointer to the hash table
    [in] key     the key (a symbol)
    [out] value  a pointer to an atom where the retrieved value will be copied
  Returns:
    1 if entry is present, 0 if not
*/
extern int fts_hash_table_get( const fts_hash_table_t *table, fts_symbol_t key, fts_atom_t *value);

/*
  Function: fts_hash_table_put
  Description:
    Insert entry `key' in hash_table `table'.
  Arguments:
    [in] table   a pointer to the hash table
    [in] key     the key (a symbol)
    [in] value   the value to be inserted in table
  Returns:
    none
*/
extern void fts_hash_table_put( fts_hash_table_t *table, fts_symbol_t key, fts_atom_t *value);

extern int fts_hash_table_remove( fts_hash_table_t *ht, fts_symbol_t sym);


/*****************************************************************************/
/*                                                                           */
/* Hash table iterators                                                      */
/*                                                                           */
/*****************************************************************************/

/*
  Structure: fts_hash_table_iterator_t
  Description:
    A hash table iterator.

    This is a temporary structure used to iterate other all the 
    (key, value) contained in a hash table, a la Java "Enumeration"

    The following code is an example of use of a fts_hash_table_iterator_t:

      fts_hash_table_iterator_init( &iter, table);

      while( !fts_hash_table_iterator_end( &iter))
        {
          void *p = fts_get_ptr( fts_hash_table_iterator_current_value( &iter));

	  fts_free( p);

	  fts_hash_table_iterator_next( &iter);
	}
*/

typedef struct _fts_hash_table_iterator_t {
  const fts_hash_table_t *ht;
  hash_cell_t *cell;
  int i;
} fts_hash_table_iterator_t;

/*
  Function: fts_hash_table_iterator_init
  Description:
    Initializes a hash table iterator
  Arguments:
    [out] iter   a pointer to the iterator to initialize
    [in] table   a pointer to the hash table on which to iterate
  Returns:
    none
*/
extern void fts_hash_table_iterator_init( fts_hash_table_iterator_t *iter, const fts_hash_table_t *table);

/*
  Function: fts_hash_table_iterator_next
  Description:
    Advance to the next ( key, value)
  Arguments:
    [out] iter   a pointer to the iterator
  Returns:
    none
*/
extern void fts_hash_table_iterator_next( fts_hash_table_iterator_t *iter);

/*
  Function: fts_hash_table_iterator_end
  Description:
    Tests if the iterator has reached end of hash table
  Arguments:
    [in] iter   a pointer to the iterator
  Returns:
    1 if iterator is at end of hash table, 0 if not
*/
extern int fts_hash_table_iterator_end( const fts_hash_table_iterator_t *iter);

/*
  Function: fts_hash_table_iterator_current_key
  Description:
    Access to the current key in the iterator
  Arguments:
    [in] iter   a pointer to the iterator
  Returns:
    the current key (a fts_symbol_t)
*/
extern fts_symbol_t fts_hash_table_iterator_current_key( const fts_hash_table_iterator_t *iter);

/*
  Function: fts_hash_table_iterator_current_value
  Description:
    Access to the current value in the iterator
  Arguments:
    [in] iter   a pointer to the iterator
  Returns:
    the current value (a pointer to a fts_atom_t)
*/
extern fts_atom_t *fts_hash_table_iterator_current_value( const fts_hash_table_iterator_t *iter);

#endif



