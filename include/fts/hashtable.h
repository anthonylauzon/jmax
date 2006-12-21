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
 * Hashtable
 *
 * The FTS hashtable maps keys to values, where both keys and values are FTS atoms.
 *
 * FTS hashtable performs automatic rehashing when the ration between number of inserted keys and the
 * capacity is greater than a "load factor" (typically 0.75).
 *
 * Initial capacity of the hashtable can be choosen among 3 constants: small, medium and big.
 * To construct a big hashtable, it is more efficient to create the hashtable with an initial
 * big capacity; this will avoid a lot of intermediate automatic rehashing.
 *
 * @defgroup hashtable FTS hashtable
 * @ingroup fts_utils
 */

typedef struct _fts_hashtable_cell_t fts_hashtable_cell_t;

/**
 * The FTS hashtable.
 *
 * @typedef fts_hashtable_t
 *
 * @ingroup hashtable
 */

/**
 * @typedef struct _fts_hashtable_ fts_hashtable_t
 * @brief fts hashtable  
 * @ingroup hashtable
 */
/**
 * @struct _fts_hashtable_
 * @brief fts_hashtable struct
 * @ingroup hashtable
 */
typedef struct _fts_hashtable_ 
{
  unsigned int length; /**< length ... */
  int count; /**< count ... */
  int rehash_count; /**< rehash_count */
  fts_hashtable_cell_t **table;/**< table ...*/
} fts_hashtable_t;


/**
 * @def FTS_HASHTABLE_SMALL 
 * @brief fts hashtable small size
 * @ingroup hashtable
 */
#define FTS_HASHTABLE_SMALL 1
/**
 * @def FTS_HASHTABLE_MEDIUM 
 * @brief fts hashtable medium int
 * @ingroup hashtable
 */
#define FTS_HASHTABLE_MEDIUM 2
/**
 * @def FTS_HASHTABLE_BIG 
 * @brief fts hashtable big int
 * @ingroup hashtable
 */
#define FTS_HASHTABLE_BIG 3

#ifdef AVOID_MACROS
/**
 * @fn int fts_hashtable_get_size(fts_hashtable_t *h)
 * @brief get size of given hashtable
 * @param ht hashtable
 * @return hashtable size
 * @ingroup hashtable
 */
int fts_hashtable_get_size(fts_hashtable_t *ht);
#else
#define fts_hashtable_get_size(h) ((h)->count)
#endif

/**
 * Initializes an hashtable
 *
 * @fn void fts_hashtable_init( fts_hashtable_t *h, int initial_capacity)
 * @brief hashtable init function
 * @param h the hashtable
 * @param initial_capacity the initial capacity of the hashtable. Can be one of: 
 * FTS_HASHTABLE_SMALL, FTS_HASHTABLE_MEDIUM, FTS_HASHTABLE_BIG 
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_init( fts_hashtable_t *h, int initial_capacity);

/**
 * Deinitializes a hashtable.
 *
 * @fn void fts_hashtable_destroy( fts_hashtable_t *h)
 * @brief hashtable deinit
 * @param h the hashtable
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_destroy( fts_hashtable_t *h);

/**
 * Allocates and initializes a hashtable
 *
 * @fn fts_hashtable_t *fts_hashtable_new( int initial_capacity)
 * @brief allocates and initializes a hashtable
 * @param initial_capacity the initial capacity of the hashtable. Can be one of: 
 * FTS_HASHTABLE_SMALL, FTS_HASHTABLE_MEDIUM, FTS_HASHTABLE_BIG 
 * @return the allocated hashtable
 * @ingroup hashtable
 */
FTS_API fts_hashtable_t *fts_hashtable_new( int initial_capacity);

/**
 * Frees a hashtable that was obtained by fts_hashtable_new().
 *
 * @fn void fts_hashtable_free( fts_hashtable_t *h)
 * @brief frees a hashtable
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
 * @brief clear hashtable's content
 * @param h the hashtable
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_clear( fts_hashtable_t *h);

/**
 * Retrieve value mapped to specified key.
 *
 * @fn int fts_hashtable_get( const fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value)
 * @brief get value by key
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
 * @brief insert value with key
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
 * @brief remove given key
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
 * @fn void fts_hashtable_get_keys( const fts_hashtable_t *h, fts_iterator_t *i)
 * @brief get iterator on keys
 * @param h the hashtable
 * @param i the iterator
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_get_keys( const fts_hashtable_t *h, fts_iterator_t *i);

/**
 * Returns an iterator to enumerate the values contained in the hashtable
 *
 * @fn void fts_hashtable_get_values( const fts_hashtable_t *h, fts_iterator_t *i)
 * @brief get iterator on values
 * @param h the hashtable
 * @param i the iterator
 * @ingroup hashtable
 */
FTS_API void fts_hashtable_get_values( const fts_hashtable_t *h, fts_iterator_t *i);

FTS_API void fts_shared_set_table(fts_hashtable_t *set);
FTS_API fts_hashtable_t *fts_shared_get_table(void);
FTS_API void *fts_shared_get(fts_symbol_t name);
FTS_API void fts_shared_set(fts_symbol_t name, void *pointer);
