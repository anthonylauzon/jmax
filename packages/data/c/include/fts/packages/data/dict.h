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

#ifndef _MESS_DICT_H_
#define _MESS_DICT_H_

#include <fts/packages/data/data.h>

/** @defgroup dict dict: dictionary associative list
 *  @ingroup  fts_classes
 *  
 *  dictionary associative list
 */

/**
 * @typedef struct dict dict_t
 * @brief dictionary associative list  
 * @ingroup dict
 */
/**
 * @struct dict
 * @brief dictionary associative list struct
 * @ingroup dict
 */
typedef struct dict
{
  fts_object_t o; /**< object ... */
  fts_hashtable_t hash;/**< hashtable ... */
  
  int opened; /**< non zero if editor open */
} dict_t;

/** 
 * @var fts_symbol_t dict_symbol
 * @brief dict_symbol ... 
 * @ingroup dict 
 */
DATA_API fts_symbol_t dict_symbol;
/** 
 * @var fts_class_t *dict_class
 * @brief dict class ... 
 * @ingroup dict 
 */
DATA_API fts_class_t *dict_class;
/**
 * @def dict_type dict_class
 * @brief dict type
 * @ingroup dict
 */
#define dict_type dict_class

#ifdef AVOID_MACROS
/**
 * @fn fts_hashtable_t dict_get_hashtable(dict_t *dict)
 * @brief get dict hashtable
 * @param the dict
 * @return the dict hashtable
 * @ingroup dict
 */
fts_hashtable_t dict_get_hashtable(dict_t *dict);
/**
 * @fn int dict_editor_is_open(dict_t *dict)
 * @brief tell if dict editor is open
 * @param the dict
 * @return 1 if treu, 0 if false
 * @ingroup dict
 */
int dict_editor_is_open(dict_t *dict);
#else
#define dict_get_hashtable(d) (&(d)->hash)
#define dict_editor_is_open(d) ((d)->opened)
#endif
/**
 * @fn void dict_store(dict_t *dict, const fts_atom_t *key, const fts_atom_t *atom)
 * @brief associates one value to given key in dict
 * @param dict the dict
 * @param key the key
 * @param atom the value
 * @ingroup dict
 */
DATA_API void dict_store(dict_t *dict, const fts_atom_t *key, const fts_atom_t *atom);
/**
 * @fn void dict_store_atoms(dict_t *dict, const fts_atom_t *key, int ac, const fts_atom_t *at)
 * @brief associates multiple values to the same key in dict
 * @param dict the dict
 * @param key the key
 * @param ac values count
 * @param at array of values
 * @ingroup dict
 */
DATA_API void dict_store_atoms(dict_t *dict, const fts_atom_t *key, int ac, const fts_atom_t *at);
/**
 * @fn void dict_store_list(dict_t *dict, int ac, const fts_atom_t *at)
 * @brief store list of key-value pairs
 * @param dict the dict
 * @param ac values count
 * @param at array of values (key-value pairs)
 * @ingroup dict
 */
DATA_API void dict_store_list(dict_t *dict, int ac, const fts_atom_t *at);
/**
 * @fn void dict_get(dict_t *dict, const fts_atom_t *key, fts_atom_t *atom)
 * @brief get valu associated to given key
 * @param dict the dict
 * @param key the key
 * @param atom atom to store return value
 * @ingroup dict
 */
DATA_API void dict_get(dict_t *dict, const fts_atom_t *key, fts_atom_t *atom);
/**
 * @fn void dict_remove_all(dict_t *dict)
 * @brief remove all from dict
 * @param dict the dict
 * @ingroup dict
 */
DATA_API void dict_remove_all(dict_t *dict);
/**
 * @fn void dict_upload(dict_t *dict)
 * @brief upload content of dict to client
 * @param dict the dict
 * @ingroup dict
 */
DATA_API void dict_upload(dict_t *dict);

#endif
