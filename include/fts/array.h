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

#ifndef _FTS_ARRAY_H_
#define _FTS_ARRAY_H_

/**
 * Array
 *
 * The FTS array is a growable array of atoms.
 *
 * FTS array contains atoms that can be accessed by index. Its size can grow and shrink
 * to accomodate adding elements or by call to set_size()
 *
 * An array has a allocation_increment: when the array memory increases, it increases by chunk
 * of size allocation_increment. To reduce the amount of reallocation, this allocation_increment 
 * can be made larger before adding a large number of elements.
 *
 * @defgroup array array
 */

/**
 * @name The FTS array structure
 */
/*@{*/

/**
 * The FTS array.
 *
 * @typedef fts_array_t
 *
 * @ingroup array
 */
typedef struct _fts_array_t 
{
  fts_object_t head;
  fts_atom_t *atoms;
  int size;
  int alloc_increment;
  int alloc;
} fts_array_t;

FTS_API fts_class_t *fts_array_class;

/*@}*/

/**
 * Initializes an array
 *
 * @fn void fts_array_init( fts_array_t *array, int ac, const fts_atom_t *at);
 * @param array the array
 * @param ac the atoms count
 * @param at atoms to initialize the content of the array
 * @ingroup array
 */
FTS_API void fts_array_init( fts_array_t *array, int ac, const fts_atom_t *at);

/**
 * Deinitializes an array
 *
 * @fn void fts_array_destroy( fts_array_t *array);
 * @param array the array
 * @ingroup array
 */
FTS_API void fts_array_destroy( fts_array_t *array);

/**
 * Clears the content of the array<BR>
 * After calling <CODE>fts_array_clear</CODE>, array will contain no elements, but
 * will keep its allocation state (i.e. its capacity will be its capacity before call).
 *
 * @fn void fts_array_clear( fts_array_t *array);
 * @param array the array
 * @ingroup array
 */
FTS_API void fts_array_clear( fts_array_t *array);

/**
 * Set the size of the array<BR>
 * If new size is greater that current size, VOID atoms will be added to the end.
 * If new size is smaller, all elements between new_size and current size will be 
 * made VOID.
 * 
 * @fn void fts_array_set_size( fts_array_t *array, int new_size);
 * @param array the array
 * @param new_size the new size
 * @ingroup array
 */
FTS_API void fts_array_set_size( fts_array_t *array, int new_size);

/**
 * Set the content of the array
 * 
 * @fn void fts_array_set( fts_array_t *array, int ac, const fts_atom_t *at)
 * @param array the array
 * @param ac the atoms count
 * @param at atoms to initialize the content of the array
 * @ingroup array
 */
FTS_API void fts_array_set( fts_array_t *array, int ac, const fts_atom_t *at);

/**
 * Append elements at the end of the array
 * 
 * @fn void fts_array_append( fts_array_t *array, int ac, const fts_atom_t *at)
 * @param array the array
 * @param ac the atoms count
 * @param at atoms to append
 * @ingroup array
 */
FTS_API void fts_array_append( fts_array_t *array, int ac, const fts_atom_t *at);

FTS_API void fts_array_append_int( fts_array_t *array, int i);
FTS_API void fts_array_append_float( fts_array_t *array, float f);
FTS_API void fts_array_append_symbol( fts_array_t *array, fts_symbol_t s);

/**
 * Prepend elements at the beginning of the array
 * 
 * @fn void fts_array_prepend( fts_array_t *array, int ac, const fts_atom_t *at)
 * @param array the array
 * @param ac the atoms count
 * @param at atoms to append
 * @ingroup array
 */
FTS_API void fts_array_prepend( fts_array_t *array, int ac, const fts_atom_t *at);
/**
 * Get the content of an array
 * 
 * @fn fts_atom_t *fts_array_get_atoms( fts_array_t *array)
 * @param array the array
 * @return a pointer to the content of the array
 * @ingroup array
 */
#define fts_array_get_atoms( array) ((array)->atoms)

/**
 * Get the number of elements in an array
 * 
 * @fn int fts_array_get_size( fts_array_t *array)
 * @param array the array
 * @return the size of the array
 * @ingroup array
 */
#define fts_array_get_size( array) ((array)->size)

/**
 * Get an element by index
 * 
 * @fn fts_atom_t *fts_array_get_element( fts_array_t *array, int index)
 * @param array the array
 * @param index the index
 * @return a pointer to the specified element of the array
 * @ingroup array
 */
#define fts_array_get_element( array, index) ((array)->atoms + (index))

/**
 * Set an element by index
 * 
 * @fn void fts_array_set_element( fts_array_t *array, int index, fts_atom_t *value)
 * @param array the array
 * @param index the index
 * @param value the new value of the element
 * @ingroup array
 */
#define fts_array_set_element( array, index, value) ((array)->atoms[(index)] = *(value))

/**
 * Check index bounds
 * 
 * @fn fts_array_check_index(array, index)  (((index) >= 0) && ((index) < (array)->size))
 * @param array the array
 * @param index the index
 * @return 1 if index is within bounds, 0 if not
 * @ingroup array
 */
#define fts_array_check_index(array, index)  (((index) >= 0) && ((index) < (array)->size))


#endif
