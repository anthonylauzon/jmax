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

#ifndef _DATA_FMAT_H_
#define _DATA_FMAT_H_

#include <fts/packages/data/data.h>

/*****************************************************************************/
/** @defgroup fmat fmat: float matrix
 *  @ingroup  data
 *  @ingroup  fts_classes
 *  
 *  float matrices and vectors
 */

/**
 * @typedef struct _fmat_ fmat_t
 * @brief float matrix 
 * @ingroup fmat
 */
/**
 * @struct _fmat_
 * @brief float matrix struct
 * @ingroup fmat
 */
typedef struct _fmat_
{
  fts_object_t o; /**< fts object ... */
  float *values; /**< values ... */
  int m; /**< lines count */
  int n; /**< columns count */
  int alloc; /**< alloc ... */
  double onset; /**< fractional onset */
  double domain; /**< fractional size (rows) */
  int opened;/**< 1 if editor is opened */
  fts_object_t *editor; /**< fmat_editor ... */
} fmat_t;

/**
 * @def HEAD_ROWS 8
 * @brief extra points for interpolation at start of vector
 * @ingroup fmat
 */
#define HEAD_ROWS 8 /* extra points for interpolation at start of vector */
/**
 * @def TAIL_ROWS 8
 * @brief  extra points for interpolation at end of vector
 * @ingroup fmat
 */
#define TAIL_ROWS 8 /* extra points for interpolation at end of vector */
/**
 * @def HEAD_TAIL_COLS 2
 * @brief interpolation head and tail for 1 or 2 column vectors only
 * @ingroup fmat
 */
#define HEAD_TAIL_COLS 2 /* interpolation head and tail for 1 or 2 column vectors only */
/**
 * @def HEAD_POINTS 
 * @brief ....
 * @ingroup fmat
 */
#define HEAD_POINTS (HEAD_TAIL_COLS * HEAD_ROWS)
/**
 * @def TAIL_POINTS 
 * @brief ....
 * @ingroup fmat
 */
#define TAIL_POINTS (HEAD_TAIL_COLS * TAIL_ROWS)
/**
 * @def fmat_get_mem(m) 
 * @brief ....
 * @param m the fmat
 * @return ...
 * @ingroup fmat
 */
#define fmat_get_mem(m) ((m)->values - HEAD_POINTS)

/** 
 * @var fts_symbol_t fmat_symbol
 * @brief fmat_symbol ... 
 * @ingroup fmat 
 */
DATA_API fts_symbol_t fmat_symbol;

/** 
 * @var fts_class_t *fmat_class
 * @brief fmat_class ... 
 * @ingroup fmat 
 */
DATA_API fts_class_t *fmat_class;
/**
 * @def fmat_type fmat_class
 * @brief fmat_type type
 * @ingroup fmat
 */
#define fmat_type fmat_class

#ifdef AVOID_MACROS
/** 
 * @fn int fmat_get_m(fmat_t *fmat)
 * @brief get number of rows of matrix
 * @param fmat the fmat
 * @return number of rows
 * @ingroup fmat
 */
int fmat_get_m(fmat_t *fmat);
/** 
 * @fn int fmat_get_m(fmat_t *fmat)
 * @brief get number of columns of matrix
 * @param fmat the fmat
 * @return number of columns
 * @ingroup fmat
 */
int fmat_get_n(fmat_t *fmat);
/** 
 * @fn int fmat_editor_is_open(fmat_t *fmat)
 * @brief tell if fmat editor is open
 * @param fmat the fmat
 * @return 1 if true, 0 if false
 * @ingroup fmat
 */
int fmat_editor_is_open(fmat_t *fmat);
/** 
 * @fn void fmat_set_editor_open(fmat_t *fmat)
 * @brief set fmat editor as open
 * @param fmat the fmat
 * @ingroup fmat
 */
void fmat_set_editor_open(fmat_t *fmat);
/** 
 * @fn void fmat_set_editor_close(fmat_t *fmat)
 * @brief set fmat editor as closed
 * @param fmat the fmat
 * @ingroup fmat
 */
void fmat_set_editor_close(fmat_t *fmat);
#else
#define fmat_get_m(x) ((x)->m)
#define fmat_get_n(x) ((x)->n)
#define fmat_editor_is_open(m) ((m)->opened == 1)
#define fmat_set_editor_open(m) ((m)->opened = 1)
#define fmat_set_editor_close(m) ((m)->opened = 0)
#endif

/** 
 * @fn fmat_t *fmat_create(int m, int n)
 * @brief create fmat with m rows and n columns
 * @param m number of rows
 * @param n number of columns
 * @return created fmat 
 * @ingroup fmat
 */
DATA_API fmat_t *fmat_create(int m, int n);
/** 
 * @fn void fmat_reshape(fmat_t *fmat, int m, int n)
 * @brief change matrix "form", leaving underlying data vector untouched
 * @param fmat the fmat 
 * @param m number of rows
 * @param n number of columns
 * @ingroup fmat
 */
DATA_API void fmat_reshape(fmat_t *fmat, int m, int n);
/** 
 * @fn void fmat_resample (fmat_t *fmat, fmat_t *in, double factor)
 * @brief copy from fmat in with resampling by factor
 * @param fmat the fmat 
 * @param in fmat from where copy
 * @param factor resampling factor
 * @ingroup fmat
 */
DATA_API void fmat_resample (fmat_t *fmat, fmat_t *in, double factor);
/** 
 * @fn void fmat_set_size(fmat_t *fmat, int m, int n)
 * @brief change matrix size, COPYING.LIB data around (???)
 * @param fmat the fmat 
 * @param  m number of rows
 * @param n number of columns
 * @ingroup fmat
 */
DATA_API void fmat_set_size(fmat_t *fmat, int m, int n);
/** 
 * @fn void fmat_set_m(fmat_t *fmat, int m)
 * @brief change matrix number of rows
 * @param fmat the fmat 
 * @param  m new number of rows
 * @ingroup fmat
 */
DATA_API void fmat_set_m(fmat_t *fmat, int m);
/** 
 * @fn void fmat_set_n(fmat_t *fmat, int n)
 * @brief change matrix number of columns
 * @param fmat the fmat 
 * @param  n new number of columns
 * @ingroup fmat
 */
DATA_API void fmat_set_n(fmat_t *fmat, int n);

#ifdef AVOID_MACROS
/** 
 * @fn float *fmat_get_ptr(fmat_t *fmat)
 * @brief get pointer to raw float data vector
 * @param fmat the fmat
 * @return pointer to raw float data vector
 * @ingroup fmat
 */
float *fmat_get_ptr(fmat_t *fmat);
/**
 * @fn float *fmat_get_element(fmat_t *fmat, int i, int j)
 * @brief get the matrix element at given row and column
 * @param fmat the fmat
 * @param i the row
 * @param j the column
 * @return element at row i and coulmn j
 * @ingroup fmat
 */
float *fmat_get_element(fmat_t *fmat, int i, int j);
/**
 * @fn float *fmat_set_element(fmat_t *fmat, int i, int j, float val)
 * @brief set matrix element at given row and column to given value
 * @param fmat the fmat
 * @param i the row
 * @param j the column
 * @param val new value
 * @return new element
 * @ingroup fmat
 */
float *fmat_set_element(fmat_t *fmat, int i, int j, float val);
/**
 * @fn void fmat_set_onset(fmat_t *fmat, double onset)
 * @brief set matrix onset
 * @param fmat the fmat
 * @param onset the onset
 * @ingroup fmat
 */
void fmat_set_onset(fmat_t *fmat, double onset);
/**
 * @fn double fmat_get_onset(fmat_t *fmat)
 * @brief get matrix onset
 * @param fmat the fmat
 * @return the matrix onset
 * @ingroup fmat
 */
double fmat_get_onset(fmat_t *fmat);
/**
 * @fn void fmat_set_domain(fmat_t *fmat, doube domain)
 * @brief set matrix domain
 * @param fmat the fmat
 * @param the matrix domain
 * @ingroup fmat
 */
void fmat_set_domain(fmat_t *fmat, double domain);
/**
 * @fn double fmat_get_domain(fmat_t *fmat)
 * @brief get matrix domain
 * @param fmat the fmat
 * @return the matrix domain
 * @ingroup fmat
 */
double fmat_get_domain(fmat_t *fmat);
#else
#define fmat_get_ptr(m) ((m)->values)
#define fmat_get_element(m, i, j) ((m)->values[(i) * (m)->n + (j)])
#define fmat_set_element(m, i, j, x) ((m)->values[(i) * (m)->n + (j)] = (x))
#define fmat_set_onset(fm, f) ((fm)->onset = (f))
#define fmat_get_onset(fm) ((fm)->onset)
#define fmat_set_domain(fm, f) ((fm)->domain = (f))
#define fmat_get_domain(fm) (((fm)->domain > 0.0)? ((fm)->domain): ((double)(fm)->m))
#endif

/**
 * @fn int fmat_or_slice_vector(fts_object_t *obj, float **ptr, int *size, int *stride)
 * @brief check class and get dimensions for fmat, fvec vector; returns 1 for fvec, 2 for fmat
 * @param obj the object (fmat or fvec)
 * @param ptr ...
 * @param size ...
 * @param stride ...
 * @return 1 for fvec, 2 for fmat
 * @ingroup fmat
 */
DATA_API int fmat_or_slice_vector(fts_object_t *obj, float **ptr, int *size, int *stride);
/**
 * @fn int fmat_or_slice_column(fts_object_t *obj, float **ptr, int *size, int *stride)
 * @brief check class and get parameters to iterate over first column of fmat or fvec; returns 1 for fvec, 2 for fmat
 * @param obj the object (fmat or fvec)
 * @param ptr ...
 * @param size ...
 * @param stride ...
 * @return 1 for fvec, 2 for fmat
 * @ingroup fmat
 */
DATA_API int fmat_or_slice_column(fts_object_t *obj, float **ptr, int *size, int *stride);
/**
 * @fn void fmat_set_const(fmat_t *mat, float c)
 * @brief set fmat or fvec to constant value
 * @param mat the object (fmat or fvec)
 * @param c the const value
 * @ingroup fmat
 */
DATA_API void fmat_set_const(fmat_t *mat, float c);
/**
 * @fn void fmat_copy(fmat_t *orig, fmat_t *copy)
 * @brief copy orig fmat content to copy fmat
 * @param orig the original fmat 
 * @param copy the copy fmat
 * @ingroup fmat
 */
DATA_API void fmat_copy(fmat_t *orig, fmat_t *copy);
/**
 * @fn void fmat_set_from_atoms(fmat_t *fmat, int onset, int step, int ac, const fts_atom_t *at)
 * @brief set fmat content from atoms 
 * @param fmat the fmat
 * @param onset ...
 * @param step ...
 * @param ac atoms count
 * @param at atoms array
 * @ingroup fmat
 */
DATA_API void fmat_set_from_atoms(fmat_t *fmat, int onset, int step, int ac, const fts_atom_t *at);
/**
 * @fn float fmat_get_max_abs_value_in_range(fmat_t *fmat, int a, int b)
 * @brief get max absolute value in range
 * @param fmat the fmat
 * @param a ...
 * @param b ...
 * @return max absolute value in range
 * @ingroup fmat
 */
DATA_API float fmat_get_max_abs_value_in_range(fmat_t *fmat, int a, int b);
/**
 * @fn float fmat_get_max_value_in_range(fmat_t *fmat, int a, int b)
 * @brief get max value in range
 * @param fmat the fmat
 * @param a ...
 * @param b ...
 * @return max value in range
 * @ingroup fmat
 */
DATA_API float fmat_get_max_value_in_range(fmat_t *fmat, int a, int b);
/**
 * @fn float fmat_get_min_value_in_range(fmat_t *fmat, int a, int b)
 * @brief get min value in range
 * @param fmat the fmat
 * @param a ...
 * @param b ...
 * @return min value in range
 * @ingroup fmat
 */
DATA_API float fmat_get_min_value_in_range(fmat_t *fmat, int a, int b);

/* sort function */
int  _fmat_element_compare_ascending  (const void *left, const void *right);
int  _fmat_element_compare_descending (const void *left, const void *right);
void _fmat_sort (fmat_t *self, int col, int (*compare)(const void *, const void *));

/** 
 * @var fmat_t *fmat_null
 * @brief fmat_null ... 
 * @ingroup fmat 
 */
DATA_API fmat_t *fmat_null;

#endif
