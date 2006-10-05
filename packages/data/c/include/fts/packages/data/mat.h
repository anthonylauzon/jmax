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

#ifndef _DATA_MAT_H_
#define _DATA_MAT_H_

#include <fts/fts.h>
#include <fts/packages/data/data.h>

/** 
 * @defgroup mat mat: atom matrix
 * @ingroup  fts_classes
 *  
 *  atom matrix
 */

/** 
 * @var fts_class_t *mat_class
 * @brief mat_class ... 
 * @ingroup mat 
 */
DATA_API fts_class_t *mat_class;
/** 
 * @var fts_symbol_t mat_symbol
 * @brief mat_symbol ... 
 * @ingroup mat 
 */
DATA_API fts_symbol_t mat_symbol;
/** 
 * @def  mat_type mat_class
 * @brief mat_type ... 
 * @ingroup mat 
 */
#define mat_type mat_class

/**
 * @typedef struct _mat_ mat_t
 * @brief atom matrix
 * @ingroup mat
 */
/**
 * @struct _mat_
 * @brief mat struct
 * @ingroup mat
 */
typedef struct _mat_
{
  fts_object_t o; /**< fts_object ...*/
  fts_atom_t *data; /**< mat data */
  int m; /**< # of rows */
  int n; /**< # of columns */
  int alloc; /**< current alloc size for lazy allocation */
  int opened; /**< non zero if editor open */
} mat_t;

/** 
 * @fn void mat_set_size(mat_t *mat, int m, int n)
 * @brief set size of mat
 * @param mat the mat
 * @param m new rows count
 * @param n new columns count
 * @ingroup mat
 */
DATA_API void mat_set_size(mat_t *mat, int m, int n);
#ifdef AVOID_MACROS
/** 
 * @fn int mat_get_m(mat_t *mat)
 * @brief get rows count
 * @param mat the mat
 * @return rows count
 * @ingroup mat
 */
int mat_get_m(mat_t *mat);
/** 
 * @fn int mat_get_n(mat_t *mat)
 * @brief get columns count
 * @param mat the mat
 * @return columns count
 * @ingroup mat
 */
int mat_get_n(mat_t *mat);
/** 
 * @fn fts_atom_t *mat_get_ptr(mat_t *mat)
 * @brief get mat data
 * @param mat the mat
 * @return mat data
 * @ingroup mat
 */
fts_atom_t *mat_get_ptr(mat_t *mat);
#else
#define mat_get_m(x) ((x)->m)
#define mat_get_n(x) ((x)->n)
#define mat_get_ptr(x) ((x)->data)
#endif

/** 
 * @fn void mat_set_element(mat_t *mat, int i, int j, const fts_atom_t *atom)
 * @brief set mat element at given indexs to given value
 * @param mat the mat
 * @param i row index
 * @param j column index
 * @param atom new value
 * @ingroup mat
 */
DATA_API void mat_set_element(mat_t *mat, int i, int j, const fts_atom_t *atom);

#ifdef AVOID_MACROS
/** 
 * @fn fts_atom_t *mat_get_element(mat_t *mat, int i, int j)
 * @brief get mat element at given indexs
 * @param mat the mat
 * @param i row index
 * @param j column index
 * @return value at given indexs
 * @ingroup mat
 */
fts_atom_t *mat_get_element(mat_t *mat, int i, int j);
/** 
 * @fn fts_atom_t *mat_get_row(mat_t *mat, int i)
 * @brief get mat row
 * @param mat the mat
 * @param i row index
 * @return values of given row
 * @ingroup mat
 */
fts_atom_t *mat_get_row(mat_t *mat, int i);
#else
#define mat_get_element(x, i, j) ((x)->data + (i) * (x)->n + (j))
#define mat_get_row(x, i) ((x)->data + (i) * (x)->n)
#endif

/** 
 * @fn void mat_set_const(mat_t *mat, const fts_atom_t *atom)
 * @brief set mat content to const value
 * @param mat the mat
 * @param atom the const value
 * @ingroup mat
 */
DATA_API void mat_set_const(mat_t *mat, const fts_atom_t *atom);
/** 
 * @fn void mat_copy(mat_t *org, mat_t *copy)
 * @brief copy content org mat to copy mat
 * @param org original mat
 * @param copy copy mat
 * @ingroup mat
 */
DATA_API void mat_copy(mat_t *org, mat_t *copy);
/** 
 * @fn void mat_set_with_onset_from_atoms(mat_t *mat, int onset, int ac, const fts_atom_t *at)
 * @brief set mat content from atoms with given onset
 * @param mat the mat
 * @param onset the onset
 * @param ac arguments count
 * @param at arguments
 * @ingroup mat
 */
DATA_API void mat_set_with_onset_from_atoms(mat_t *mat, int onset, int ac, const fts_atom_t *at);
/** 
 * @fn void mat_set_from_tuples(mat_t *mat, int ac, const fts_atom_t *at)
 * @brief set mat content from tuples
 * @param mat the mat
 * @param ac arguments count
 * @param at arguments
 * @ingroup mat
 */
DATA_API void mat_set_from_tuples(mat_t *mat, int ac, const fts_atom_t *at);

/* editor interaction */
#ifdef AVOID_MACROS
/** 
 * @fn void mat_set_editor_open(mat_t *mat)
 * @brief set mat editor as opened
 * @param mat the mat
 * @ingroup mat
 */
void mat_set_editor_open(mat_t *mat);
/** 
 * @fn void mat_set_editor_close(mat_t *mat)
 * @brief set mat editor as closed
 * @param mat the mat
 * @ingroup mat
 */
void mat_set_editor_close(mat_t *mat);
/** 
 * @fn int mat_editor_is_open(mat_t *mat)
 * @brief tell if mat editor is open
 * @param mat the mat
 * @return 1 if true, 0 if false
 * @ingroup mat
 */
int mat_editor_is_open(mat_t *mat);
#else
#define mat_set_editor_open(m) ((m)->opened = 1)
#define mat_set_editor_close(m) ((m)->opened = 0)
#define mat_editor_is_open(m) ((m)->opened)
#endif
/** 
 * @fn void mat_upload(mat_t *mat)
 * @brief upload mat content to client
 * @param mat the mat
 * @ingroup mat
 */
DATA_API void mat_upload(mat_t *mat);

#endif
