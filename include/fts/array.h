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
  fts_atom_t *atoms;
  int size;
  int alloc_increment;
  int alloc;
} fts_array_t;

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

/** 
 * Append an int element at the end of array
 * 
 * @fn void fts_array_append_int(fts_array_t* array, int i)
 * @param array the array 
 * @param i the int to append
 * @ingroup array
 */
FTS_API void fts_array_append_int( fts_array_t *array, int i);

/** 
 * Append a float element at the end of array
 * 
 * @fn void fts_array_append_float(fts_array_t* array, float f)
 * @param array the array
 * @param f the float to append
 * @ingroup array
 */
FTS_API void fts_array_append_float( fts_array_t *array, float f);

/** 
 * Append a symbol element at the end of array
 * 
 * @fn void fts_array_append_symbol(fts_array_t* array, fts_symbol_t s)
 * @param array the array
 * @param s the symbol to append
 * @ingroup array
 */
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
 * Insert elements at given index of the array
 *
 * @fn void fts_array_insert( fts_array_t *array, int index, int ac, const fts_atom_t *at)
 * @param array the array
 * @param index the index
 * @param ac the atoms count
 * @param at atoms to append
 * @ingroup array
 */
FTS_API void fts_array_insert( fts_array_t *array, int index, int ac, const fts_atom_t *at);

/**
 * Cut given numer of elements from given index of the array
 *
 * @fn void fts_array_cut( fts_array_t *array, int index, int n)
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
 * @param org the source
 * @param copy the destination of the copy
 * @ingroup array
 */
FTS_API void fts_array_copy(fts_array_t *org, fts_array_t *copy);

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
 * Set an element at index
 * 
 * @fn void fts_array_set_element(fts_array_t* array, int index, const fts_atom_t* at)
 * @param array the array
 * @param index the index
 * @param at element value
 * @ingroup array
 */
FTS_API void fts_array_set_element(fts_array_t *array, int index, const fts_atom_t *at);

/** 
 * Get an iterator on array elements
 * 
 * @fn void fts_array_get_values(fts_array_t* array, fts_iterator_t* iter)
 * @param array the array
 * @param iter the iterator
 * @ingroup array
 */
FTS_API void fts_array_get_values(fts_array_t *array, fts_iterator_t *iter);


