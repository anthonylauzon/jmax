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
 * @ingroup ftm_utils
 */

/**
 * @struct fts_array
 * @brief the fts array structure
 * @ingroup array
 */

struct fts_array
{
  fts_atom_t *atoms; /**< atoms ...*/
  int size; /**<size ...*/
  int alloc_increment; /**< alloc_increment ... */
  int alloc; /**<lloc ...*/
};

/**
 * Initializes an array
 *
 * @fn void fts_array_init( fts_array_t *array, int ac, const fts_atom_t *at)
 * @brief initializes an array
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
 * @brief deinitializes an array
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
 * @brief clears the content of the array
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
 * @brief set the size of the array
 * @param array the array
 * @param new_size the new size
 * @ingroup array
 */
FTS_API void fts_array_set_size( fts_array_t *array, int new_size);

#ifdef AVOID_MACROS

/**
* @fn int fts_array_get_size(fts_array_t *array)
 * @brief get array size
 * @param array pointer to the array
 * @return the size of the array
 * @ingroup fts_array
 *
 * Get the number of elements in an array
 */
FTS_API int fts_array_get_size(fts_array_t *array);

#else

#define fts_array_get_size(array) ((array)->size)

#endif

#ifdef AVOID_MACROS

/**
* @def fts_atom_t *fts_array_get_atoms(fts_array_t *array)
 * @brief get a pointer to the raw array of atoms
 * @param array pointer to the array
 * @return a pointer to the content of the array
 * @ingroup fts_array
 */
FTS_API fts_atom_t *fts_array_get_atoms(fts_array_t *array);

#else

#define fts_array_get_atoms(array) ((array)->atoms)

#endif

/** 
* @fn void fts_array_set_element(fts_array_t *array, int index, const fts_atom_t* at)
* @brief set element at index
* @param array pointer to the array
* @param index the index
* @param at element value
* @ingroup fts_array
*/
FTS_API void fts_array_set_element(fts_array_t *array, int index, const fts_atom_t *at);

#ifdef AVOID_MACROS

/** 
* @fn fts_atom_t *fts_array_get_element( fts_array_t *array, int index)
* @brief get element by index
* @param array the array
* @param index the index
* @return a pointer to the specified element of the array
* @ingroup array
*/
FTS_API fts_atom_t *fts_array_get_element( fts_array_t *array, int index);

#else

#define fts_array_get_element( array, index) ((array)->atoms + (index))

#endif

FTS_API void fts_array_set( fts_array_t *array, int ac, const fts_atom_t *at);

/**
 * Append elements at the end of the array
 * 
 * @fn void fts_array_append( fts_array_t *array, int ac, const fts_atom_t *at)
 * @brief append elements at the end of the array
 * @param array the array
 * @param ac the atoms count
 * @param at atoms to append
 * @ingroup array
 */
FTS_API void fts_array_append( fts_array_t *array, int ac, const fts_atom_t *at);

/** 
 * Append an int element at the end of array
 * 
 * @fn void fts_array_append_int(fts_array_t* array, int i)
 * @brief append int at the end of array
 * @param array the array 
 * @param i the int to append
 * @ingroup array
 */
FTS_API void fts_array_append_int( fts_array_t *array, int i);

/** 
 * Append a float element at the end of array
 * 
 * @fn void fts_array_append_float(fts_array_t* array, float f)
 * @brief append float at the end of array
 * @param array the array
 * @param f the float to append
 * @ingroup array
 */
FTS_API void fts_array_append_float( fts_array_t *array, float f);

/** 
* Append a symbol element at the end of array
* 
* @fn void fts_array_append_symbol(fts_array_t* array, fts_symbol_t s)
* @brief append symbol at the end of array
* @param array the array
* @param s the symbol to append
* @ingroup array
*/
FTS_API void fts_array_append_symbol( fts_array_t *array, fts_symbol_t s);

/** 
* Append an object element at the end of array
* 
* @fn void fts_array_append_symbol(fts_array_t* array, fts_symbol_t s)
* @brief append object at the end of array
* @param array the array
* @param obj the object
* @ingroup array
*/
FTS_API void fts_array_append_object( fts_array_t *array, fts_object_t * obj);

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
 * Prepend elements at the beginning of the array
 * 
 * @fn void fts_array_prepend( fts_array_t *array, int ac, const fts_atom_t *at)
 * @brief prepend elements to the array
 * @param array the array
 * @param ac the atoms count
 * @param at atoms to append
 * @ingroup array
 */
FTS_API void fts_array_prepend( fts_array_t *array, int ac, const fts_atom_t *at);

/**
 * Prepend an int element at the beginning of the array
 * 
 * @fn void fts_array_prepend_int( fts_array_t *array, int i)
 * @brief prepend int to the array
 * @param array the array
 * @param i the int to prepend
 * @ingroup array
 */
FTS_API void fts_array_prepend_int( fts_array_t *array, int i);

/**
 * Prepend a float element at the beginning of the array
 * 
 * @fn void fts_array_prepend_float( fts_array_t *array, float f)
 * @brief prepend float to the array
 * @param array the array
 * @param f the float to prepend
 * @ingroup array
 */
FTS_API void fts_array_prepend_float( fts_array_t *array, float f);

/**
 * Prepend a symbol element at the beginning of the array
 * 
 * @fn void fts_array_prepend_float( fts_array_t *array, fts_symbol_t s)
 * @brief prepend symbol to the array
 * @param array the array
 * @param s the symbol to prepend
 * @ingroup array
 */
FTS_API void fts_array_prepend_symbol( fts_array_t *array, fts_symbol_t s);

/**
 * Insert elements at given index of the array
 *
 * @fn void fts_array_insert( fts_array_t *array, int index, int ac, const fts_atom_t *at)
 * @brief insert elements at given index
 * @param array the array
 * @param index the index
 * @param ac the atoms count
 * @param at atoms to append
 * @ingroup array
 */
FTS_API void fts_array_insert( fts_array_t *array, int index, int ac, const fts_atom_t *at);

/**
 * Cut given number of elements from given index of the array
 *
 * @fn void fts_array_cut( fts_array_t *array, int index, int n)
 * @brief cut elements from array
 * @param array the array
 * @param index the index
 * @param n the atoms count
 * @ingroup array
 */
FTS_API void fts_array_cut( fts_array_t *array, int index, int n);

/**
 * Copy array
 *
 * @fn void fts_array_copy(fts_array_t *org, fts_array_t *copy)
 * @brief copy array content
 * @param org the source
 * @param copy the destination of the copy
 * @ingroup array
 */
FTS_API void fts_array_copy(fts_array_t *org, fts_array_t *copy);

/** 
 * @fn void fts_array_get_values(fts_array_t* array, fts_iterator_t* iter)
 * @brief get an iterator on array elements
 * @param array the array
 * @param iter the iterator
 * @ingroup array
 */
FTS_API void fts_array_get_values(fts_array_t *array, fts_iterator_t *iter);


