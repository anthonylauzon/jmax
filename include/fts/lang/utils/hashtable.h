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

#ifndef _FTS_HASHTABLE_H_
#define _FTS_HASHTABLE_H_

#include <fts/lang/utils/iterator.h>

/**
 * Hashtable
 *
 * The FTS hashtable maps keys to values, where both keys and values
 * are FTS atoms.
 *
 * <STRONG>NOTE:</STRONG>A FTS hashtable is restricted to contain only keys of the same <B>type</B>.
 *
 * Currently supported key types are: fts_s_int, fts_s_ptr, fts_s_symbol and fts_s_string (i.e. char *)
 *
 * FTS hashtable performs automatic rehashing when the ration between number of inserted keys and the
 * capacity is greater than a "load factor" (typically 0.75).
 *
 * Initial capacity of the hashtable can be choosen among 3 constants: small, medium and big.
 * To construct a big hashtable, it is more efficient to create the hashtable with an initial
 * big capacity; this will avoid a lot of intermediate automatic rehashing.
 *
 * @defgroup hashtable hashtable
 */

typedef struct _fts_hashtable_cell_t {
  fts_atom_t key;
  fts_atom_t value;
  struct _fts_hashtable_cell_t *next;
} fts_hashtable_cell_t;

/**
 * @name The FTS hashtable structure
 */
/*@{*/

typedef unsigned int (*hash_function_t)( const fts_atom_t *);
typedef int (*equals_function_t)( const fts_atom_t *, const fts_atom_t *);

/**
 * The FTS hashtable.
 *
 * @typedef fts_hashtable_t
 *
 * @ingroup hashtable
 */
typedef struct fts_hashtable {
  unsigned int length;
  int count;
  int rehash_count;
  fts_hashtable_cell_t **table;

  fts_type_t key_type;
  hash_function_t hash_function;
  equals_function_t equals_function;

} fts_hashtable_t;

/*@}*/

#define FTS_HASHTABLE_SMALL 1
#define FTS_HASHTABLE_MEDIUM 2
#define FTS_HASHTABLE_BIG 3

/** 
 * @name Hash table handling
 *
 * Functions to initialize, clear and destroy a hash table
 */
/*@{*/

/**
 * Initializes a hashtable
 *
 * @fn void fts_hashtable_init( fts_hashtable_t *h, fts_type_t key_type, int initial_capacity)
 * @param h the hashtable
 * @param key_type the type of the key. If 0, the key type will be `symbol'
 * @param initial_capacity the initial capacity of the hashtable. Can be one of: 
 * FTS_HASHTABLE_SMALL, FTS_HASHTABLE_MEDIUM, FTS_HASHTABLE_BIG 
 *
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_init( fts_hashtable_t *h, fts_type_t key_type, int initial_capacity);

/**
 * Clears hashtable's content.<BR>
 * After calling <CODE>fts_hashtable_clear</CODE>, hashtable will contain no keys, but
 * will keep its allocation state (i.e. its capacity will be its capacity before call).
 *
 * @fn void fts_hashtable_clear( fts_hashtable_t *h)
 * @param h the hashtable
 *
 * @ingroup hashtable
 */
extern void fts_hashtable_clear( fts_hashtable_t *h);

/**
 * Deinitializes a hashtable.
 *
 * @fn void fts_hashtable_destroy( fts_hashtable_t *h)
 * @param h the hashtable
 *
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_destroy( fts_hashtable_t *h);

/*@}*/

/** 
 * @name Using hash tables
 *
 * Insert, retrieve and remove hash table elements
 */
/*@{*/

/**
 * Retrieve value mapped to specified key.
 *
 * @fn int fts_hashtable_get( const fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value)
 * @param h the hashtable
 * @param key a pointer to the key to be inserted
 * @param value a pointer for returning the retrieved value
 * @return 1 if key is mapped, 0 if not
 */
FTS_API int fts_hashtable_get( const fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value);

/**
 * Insert specified key mapped to specified value.
 *
 * @fn int fts_hashtable_put( fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value)
 * @param h the hashtable
 * @param key a pointer to the key to be inserted
 * @param value a pointer to the value to be inserted
 * @return 1 if entry was already mapped, 0 if not
 */
FTS_API int fts_hashtable_put( fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value);

/**
 * Removes specified key 
 *
 * @fn int fts_hashtable_remove( fts_hashtable_t *h, const fts_atom_t *key)
 * @param h the hashtable
 * @param key a pointer to the key to be removed
 * @return 1 if entry was mapped, 0 if not
 */
FTS_API int fts_hashtable_remove( fts_hashtable_t *h, const fts_atom_t *key);

/**
 * Returns an iterator to enumerate the keys contained in the hashtable
 *
 * @fn void fts_hashtable_get_keys( const fts_hashtable_t *h, fts_iterator_t *i)
 * @param h the hashtable
 * @param i the iterator
 */
FTS_API void fts_hashtable_get_keys( const fts_hashtable_t *h, fts_iterator_t *i);

/**
 * Returns an iterator to enumerate the values contained in the hashtable
 *
 * @fn void fts_hashtable_get_values( const fts_hashtable_t *h, fts_iterator_t *i)
 * @param h the hashtable
 * @param i the iterator
 */
FTS_API void fts_hashtable_get_values( const fts_hashtable_t *h, fts_iterator_t *i);

FTS_API void fts_hashtable_stats( fts_hashtable_t *h);

#endif