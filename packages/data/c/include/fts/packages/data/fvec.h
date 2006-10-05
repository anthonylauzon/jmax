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


#ifndef _DATA_FVEC_H_
#define _DATA_FVEC_H_


/** 
 * @defgroup fvec fvec: fmat slice ref
 * @ingroup  data
 * @ingroup fts_classes 
 *
 *  an fvec is a reference to a colum, row or diagonal of an fmat
 */

/**
 * @typedef enum fvec_type_t
 * @brief enum of fvec types
 * @ingroup fvec
 */
typedef enum { fvec_type_column, /**< column type */ 
               fvec_type_row, /**< row type */
               fvec_type_diagonal, /**< diagonal type */
	             fvec_type_unwrap, /**< unwrap type */
               fvec_type_vector, /**< vector type */
               fvec_n_types /**< vector type */
             } fvec_type_t;

/**
 * @typedef struct _fvec_ fvec_t
 * @brief reference to a colum, row or diagonal of an fmat
 * @ingroup fvec
 */
/**
 * @struct _fvec_
 * @brief fvec struct
 * @ingroup fvec
 */
typedef struct _fvec_
{
  fts_object_t o; /**< fts_object ... */
  fvec_type_t  type; /**< type ... */
  fmat_t *fmat; /**< pointer to fmat */
  int index; /**< index of column (col) or index of row (row)  or row onset (diag) */
  int onset; /**< row onset (col) or column onset (row) or column onset (diag) */
  int size; /**< number of elements  */

  int opened; /**< non zero if editor open */
  fts_object_t *editor; /**< fvec editor */
} fvec_t;

#ifdef AVOID_MACROS
/** 
 * @fn fmat_t *fvec_get_fmat(fvec_t *fvec)
 * @brief get associated fmat
 * @param fvec the fvec
 * @return fmat ....
 * @ingroup fvec
 */
fmat_t *fvec_get_fmat(fvec_t *fvec)
/** 
 * @fn fvec_type_t fvec_get_type(fvec_t *fvec)
 * @brief get associated fvec type
 * @param fvec the fvec
 * @return fvec_type_t
 * @ingroup fvec
 */
fvec_type_t fvec_get_type(fvec_t *fvec);
/** 
 * @fn int fvec_get_index(fvec_t *fvec)
 * @brief get index of column (col) or index of row (row)  or row onset (diag)
 * @param fvec the fvec
 * @return the index
 * @ingroup fvec
 */
int fvec_get_index(fvec_t *fvec);
/** 
 * @fn int fvec_get_onset(fvec_t *fvec)
 * @brief get row onset (col) or column onset (row) or column onset (diag)
 * @param fvec the fvec
 * @return the onset
 * @ingroup fvec
 */
int fvec_get_onset(fvec_t *fvec);
/** 
 * @fn int fvec_get_raw_size(fvec_t *fvec)
 * @brief get number of elements
 * @param fvec the fvec
 * @return the size
 * @ingroup fvec
 */
int fvec_get_raw_size(fvec_t *fvec);
/** 
 * @fn void fvec_set_fmat(fvec_t *fvec, fmat_t *fmat)
 * @brief set associated fmat
 * @param fvec the fvec
 * @param fmat new fmat
 * @ingroup fvec
 */
void fvec_set_fmat(fvec_t *fvec, fmat_t *fmat);
/** 
 * @fn void fvec_set_type(fvec_t *fvec, fvec_type_t *type)
 * @brief set fvec type
 * @param fvec the fvec
 * @param type new type
 * @ingroup fvec
*/
void fvec_set_type(fvec_t *fvec, fvec_type_t *type);
/** 
 * @fn void fvec_set_index(fvec_t *fvec, int index)
 * @brief set fvec index
 * @param fvec the fvec
 * @param index new index
 * @ingroup fvec
 */
void fvec_set_index(fvec_t *fvec, int index)
/** 
* @fn int fvec_set_onset(fvec_t *fvec, int onset)
* @brief set fvec onset
* @param fvec the fvec
* @param onset new onset
* @ingroup fvec
*/
void fvec_set_onset(fvec_t *fvec, int onset);
/** 
 * @fn void fvec_set_size(fvec_t *fvec, int size)
 * @brief set fvec size
 * @param fvec the fvec
 * @param size new size
 * @ingroup fvec
 */
void fvec_set_size(fvec_t *fvec, int size);

/* editor */
/** 
 * @fn void fvec_set_editor_open(fvec_t *fvec)
 * @brief set fvec editor as opened
 * @param fvec the fvec
 * @ingroup fvec
 */
void fvec_set_editor_open(fvec_t *fvec);
/** 
* @fn void fvec_set_editor_close(fvec_t *fvec)
* @brief set fvec editor as closed
* @param fvec the fvec
* @ingroup fvec
*/
void fvec_set_editor_close(fvec_t *fvec);
/** 
* @fn int fvec_editor_is_open(fvec_t *fvec)
* @brief tell if fvec editor is open
* @param fvec the fvec
* @return 1 if true, 0 if false
* @ingroup fvec
*/
int fvec_editor_is_open(fvec_t *fvec);
#else
#define fvec_get_fmat(f) ((f)->fmat)
#define fvec_get_type(f) ((f)->type)
#define fvec_get_index(f) ((f)->index)
#define fvec_get_onset(f) ((f)->onset)
#define fvec_get_raw_size(f) ((f)->size)

#define fvec_set_fmat(f, x) do{fts_object_release((fts_object_t *)(f)->fmat); (f)->fmat = (x); fts_object_refer((fts_object_t *)(x)); } while(0)
#define fvec_set_type(f, x) ((f)->type = (x))
#define fvec_set_index(f, x) ((f)->index = (x))
#define fvec_set_onset(f, x) ((f)->onset = (x))
#define fvec_set_size(f, x) ((f)->size = (x))

/* editor */
#define fvec_set_editor_open(v) ((v)->opened = 1)
#define fvec_set_editor_close(v) ((v)->opened = 0)
#define fvec_editor_is_open(v) ((v)->opened)
#endif

/* globals */
/** 
 * @var fts_symbol_t fvec_symbol
 * @brief fvec_symbol ... 
 * @ingroup fvec 
 */
DATA_API fts_symbol_t fvec_symbol;
/** 
 * @var  fts_class_t *fvec_class
 * @brief fvec_class ... 
 * @ingroup fvec 
 */
DATA_API fts_class_t *fvec_class;

/** quick create */
/** 
 * @fn fvec_t *fvec_create (fmat_t *fmat, fvec_type_t type, int ac, const fts_atom_t *at)
 * @brief fvec create function 
 * @param fmat associated fmat
 * @param type fvec type
 * @param ac arguments count
 * @param at arguments
 * @return pointer to created fvec
 * @ingroup fvec 
 */
DATA_API fvec_t *fvec_create(fmat_t *fmat, fvec_type_t type, int ac, const fts_atom_t *at);
/** 
 * @fn fvec_t *fvec_create_vector(int size)
 * @brief create fvec as vector 
 * @param size vector size
 * @return pointer to created fvec
 * @ingroup fvec 
 */
DATA_API fvec_t *fvec_create_vector(int size);
/** 
 * @fn fvec_t *fvec_create_column(fmat_t *fmat)
 * @brief create fvec as column 
 * @param fmat associated fmat
 * @return pointer to created fvec
 * @ingroup fvec 
 */
DATA_API fvec_t *fvec_create_column(fmat_t *fmat);
/** 
 * @fn fvec_t *fvec_create_row(fmat_t *fmat)
 * @brief create fvec as row 
 * @param fmat associated fmat
 * @return pointer to created fvec
 * @ingroup fvec 
 */
DATA_API fvec_t *fvec_create_row(fmat_t *fmat);
/** 
 * @fn int fvec_get_size(fvec_t *self)
 * @brief get fvec size
 * @param fvec the fvec
 * @return fvec size
 * @ingroup fvec 
 */
DATA_API int fvec_get_size(fvec_t *self);
/** 
 * @fn fts_symbol_t fvec_get_type_as_symbol(fvec_t *self)
 * @brief get fvec type as fts_symbol
 * @param fvec the fvec
 * @return type as fts_symbol
 * @ingroup fvec 
 */
DATA_API fts_symbol_t fvec_get_type_as_symbol(fvec_t *self);

/** get element, no checks */
/** 
 * @fn float fvec_get_element(fvec_t *self, int i)
 * @brief get fvec element at given index (with no checks)
 * @param fvec the fvec
 * @param i the index
 * @return element at given index
 * @ingroup fvec 
 */
DATA_API float fvec_get_element(fvec_t *self, int i);

/** set element, no checks */
/** 
 * @fn void fvec_set_element(fvec_t *self, int i, float value)
 * @brief set fvec element at given index to given value (with no checks)
 * @param fvec the fvec
 * @param i the index
 * @param value new value
 * @ingroup fvec 
 */
DATA_API void fvec_set_element(fvec_t *self, int i, float value);
/** 
 * @fn void fvec_set_dimensions(fvec_t *fvec, int ac, const fts_atom_t *at)
 * @brief set fvec index, onset, size from args (when given)
 * @param fvec the fvec
 * @param ac arguments count
 * @param at aguments
 * @ingroup fvec 
 */
DATA_API void fvec_set_dimensions(fvec_t *fvec, int ac, const fts_atom_t *at);
/** 
 * @fn void fvec_set_from_atoms(fvec_t *vec, int onset, int ac, const fts_atom_t *at)
 * @brief set fvec content from atoms
 * @param fvec the fvec
 * @param onset onset ...
 * @param ac arguments count
 * @param at aguments
 * @ingroup fvec 
 */
DATA_API void fvec_set_from_atoms(fvec_t *vec, int onset, int ac, const fts_atom_t *at);

/** 
 * @fn int fvec_vector(fts_object_t *obj, float **ptr, int *size, int *stride)
 * @brief get fvec access parameters, then use ptr[i * stride] if i < size 
 * @param obj ...
 * @param ptr ...
 * @param size ...
 * @param stride ...
 * @return	true if obj is an fvec, false and size = stride = 0 otherwise 
 * 
 * ...................................
 */
DATA_API int fvec_vector(fts_object_t *obj, float **ptr, int *size, int *stride);

/** like fvec_vector, no checks */
/** 
 * @fn void fvec_get_vector(fvec_t *fvec, float **ptr, int *size, int *stride)
 * @brief get fvec access parameters, then use ptr[i * stride] if i < size (with no checks)
 * @param fvec ...
 * @param ptr ...
 * @param size ...
 * @param stride ...
 * 
 * ...................................
 */
DATA_API void fvec_get_vector (fvec_t *fvec, float **ptr, int *size, int *stride);

/** copy row or col from matrix reference to an fmat (1 column matrix) */
/** 
 * @fn void fvec_copy_to_fmat(fvec_t *org, fmat_t *copy)
 * @brief copy row or col from matrix reference to an fmat (1 column matrix)
 * @param org original fvec 
 * @param copy copy fmat
 */
DATA_API void fvec_copy_to_fmat(fvec_t *org, fmat_t *copy);

#endif
