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

#ifndef _FTS_OBJECTSET_H_
#define _FTS_OBJECTSET_H_


/**
 * Objectset
 *
 * The FTS objectset is a fts_object_t container.
 *
 * Current implementation uses a hashtable where key is the object.
 *
 * @defgroup objectset objectset
 */

/**
 * @name The FTS objectset structure
 */
/*@{*/
#include <fts/fts.h>

/**
 * The FTS objectset.
 *
 * @typedef fts_objectset_t
 *
 * @ingroup objectset
 */

typedef struct _fts_object_set_t 
{
  fts_object_t ob;

  fts_hashtable_t hashtable;
} fts_objectset_t;

/*@}*/

extern fts_symbol_t objectset_symbol;
extern fts_symbol_t sym_objectset_add;
extern fts_symbol_t sym_objectset_remove;
extern fts_symbol_t sym_objectset_clear;
extern fts_symbol_t sym_objectset_get_objects;

/**
 * Initializes an objectset
 *
 * @fn fts_objectset_init( fts_objectset_t *set)
 * @param set the objectset
 * @ingroup objectset
 */
/*FTS_API void fts_objectset_init( fts_objectset_t *set);*/

/**
 * Clears objectset's content
 *
 * @fn void fts_objectset_clear( fts_objectset_t *set);
 * @param set the objectset
 * @ingroup objectset
 */
/*FTS_API void fts_objectset_clear( fts_objectset_t *set);*/

/**
 * Deinitializes an objectset
 *
 * @fn void fts_objectset_destroy( fts_objectset_t *set)
 * @param set the objectset
 * @ingroup objectset
 */
/*FTS_API void fts_objectset_destroy( fts_objectset_t *set);*/

/**
 * Add an object to the objectset.<BR>
 * This function has no effect if object is already in the objectset.
 *
 * @fn void fts_objectset_add( fts_objectset_t *set, fts_object_t *object);
 * @param set the objectset
 * @param object the object to add
 * @ingroup objectset
 */
/*FTS_API void fts_objectset_add( fts_objectset_t *set, fts_object_t *object);*/

/**
 * Remove an object from the objectset.
 *
 * @fn void fts_objectset_remove( fts_objectset_t *set, fts_object_t *object)
 * @param set the objectset
 * @param object the object to add
 * @ingroup objectset
 */
/*FTS_API void fts_objectset_remove( fts_objectset_t *set, fts_object_t *object);*/

/* ??? */
/*  FTS_API int fts_objectset_contains( fts_objectset_t *set, fts_object_t *object); */


/**
 * Returns an iterator to enumerate the objects contained in the objectset
 *
 * @fn void fts_objectset_get_objects( const fts_objectset_t *set, fts_iterator_t *i)
 * @param set the objectset
 * @param i the iterator
 * @ingroup objectset
 */
/*FTS_API void fts_objectset_get_objects( const fts_objectset_t *set, fts_iterator_t *i);*/

#endif
