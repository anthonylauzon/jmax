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
 * Hashtable
 *
 * The FTS hashtable maps keys to values, where both keys and values are FTS atoms.
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

typedef struct _fts_hashtable_cell_t fts_hashtable_cell_t;

/**
 * @name The FTS hashtable structure
 */
/*@{*/

/**
 * The FTS hashtable.
 *
 * @typedef fts_hashtable_t
 *
 * @ingroup hashtable
 */
typedef unsigned int (*hash_function_t)( const fts_atom_t *);
typedef int (*equals_function_t)( const fts_atom_t *, const fts_atom_t *);

typedef struct {
  unsigned int length;
  int count;
  int rehash_count;
  fts_hashtable_cell_t **table;

  hash_function_t hash_function;
  equals_function_t equals_function;

} fts_hashtable_t;

/*@}*/

#define FTS_HASHTABLE_SMALL 1
#define FTS_HASHTABLE_MEDIUM 2
#define FTS_HASHTABLE_BIG 3

#define fts_hashtable_get_size(h) ((h)->count)

/**
 * Initializes a hashtable
 *
 * @fn void fts_hashtable_init( fts_hashtable_t *h, fts_class_t *key_class, int initial_capacity)
 * @param h the hashtable
 * @param key_class the class of the key.<BR>
 *  Must be one of fts_int_class, fts_symbol_class, fts_string_class, fts_pointer_class
 *  If NULL, the key class will be fts_symbol_class
 * @param initial_capacity the initial capacity of the hashtable. Can be one of: 
 * FTS_HASHTABLE_SMALL, FTS_HASHTABLE_MEDIUM, FTS_HASHTABLE_BIG 
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_init( fts_hashtable_t *h, fts_class_t *key_class, int initial_capacity);

/**
 * Deinitializes a hashtable.
 *
 * @fn void fts_hashtable_destroy( fts_hashtable_t *h)
 * @param h the hashtable
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_destroy( fts_hashtable_t *h);

/**
 * Allocates and initializes a hashtable
 *
 * @fn fts_hashtable_t *fts_hashtable_new( fts_class_t *key_class, int initial_capacity)
 * @param key_class the class of the key.<BR>
 *  Must be one of fts_int_class, fts_symbol_class, fts_string_class, fts_pointer_class
 *  If NULL, the key class will be fts_symbol_class
 * @param initial_capacity the initial capacity of the hashtable. Can be one of: 
 * FTS_HASHTABLE_SMALL, FTS_HASHTABLE_MEDIUM, FTS_HASHTABLE_BIG 
 * @return the allocated hashtable
 * @ingroup hashtable
 */
FTS_API fts_hashtable_t *fts_hashtable_new( fts_class_t *key_class, int initial_capacity);

/**
 * Frees a hashtable that was obtained by fts_hashtable_new().
 *
 * @fn void fts_hashtable_free( fts_hashtable_t *h)
 * @param h the hashtable
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_free( fts_hashtable_t *h);

/**
 * Clears hashtable's content.<BR>
 * After calling <CODE>fts_hashtable_clear</CODE>, hashtable will contain no keys, but
 * will keep its allocation state (i.e. its capacity will be its capacity before call).
 *
 * @fn void fts_hashtable_clear( fts_hashtable_t *h)
 * @param h the hashtable
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_clear( fts_hashtable_t *h);

/**
 * Retrieve value mapped to specified key.
 *
 * @fn int fts_hashtable_get( const fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value)
 * @param h the hashtable
 * @param key a pointer to the key to be inserted
 * @param value a pointer for returning the retrieved value
 * @return 1 if key is mapped, 0 if not
 * @ingroup hashtable
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
 * @ingroup hashtable
 */
FTS_API int fts_hashtable_put( fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value);
/**
 * Removes specified key 
 *
 * @fn int fts_hashtable_remove( fts_hashtable_t *h, const fts_atom_t *key)
 * @param h the hashtable
 * @param key a pointer to the key to be removed
 * @return 1 if entry was mapped, 0 if not
 * @ingroup hashtable
 */
FTS_API int fts_hashtable_remove( fts_hashtable_t *h, const fts_atom_t *key);

FTS_API void fts_hashtable_stats( fts_hashtable_t *h);

/**
 * Returns an iterator to enumerate the keys contained in the hashtable
 *
 * @fn void fts_hashtable_get_keys( fts_hashtable_t *h, fts_iterator_t *i)
 * @param h the hashtable
 * @param i the iterator
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_get_keys( const fts_hashtable_t *h, fts_iterator_t *i);

/**
 * Returns an iterator to enumerate the values contained in the hashtable
 *
 * @fn void fts_hashtable_get_values( fts_hashtable_t *h, fts_iterator_t *i)
 * @param h the hashtable
 * @param i the iterator
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_get_values( const fts_hashtable_t *h, fts_iterator_t *i);

FTS_API fts_symbol_t fts_hashtable_get_unused_symbol(fts_hashtable_t *hash, fts_symbol_t name);
