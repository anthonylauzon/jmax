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


/** Float matrix data class.
 *
 *  @file fmat.h
 *  @ingroup data
 */


/*****************************************************************************/
/** @defgroup fmat fmat: float matrix
 *  @ingroup  data
 *  
 *  float matrices and vectors
 *
 *  @{
 */

/** fmat struct */
typedef struct
{
  fts_object_t o;
  float *values;
  int m;
  int n;
  int alloc;
  double sr; /* sample rate */
  double onset; /* fractional onset */
  double domain; /* fractional size (rows) */
  int opened;
} fmat_t;

#define HEAD_ROWS 8 /* extra points for interpolation at start of vector */
#define TAIL_ROWS 8 /* extra points for interpolation at end of vector */
#define HEAD_TAIL_COLS 2 /* interpolation head and tail for 1 or 2 column vectors only */
#define HEAD_POINTS (HEAD_TAIL_COLS * HEAD_ROWS)
#define TAIL_POINTS (HEAD_TAIL_COLS * TAIL_ROWS)

#define fmat_get_mem(m) ((m)->values - HEAD_POINTS)

/** */
DATA_API fts_symbol_t fmat_symbol;

/** */
DATA_API fts_class_t *fmat_class;
#define fmat_type fmat_class

/** Get number of rows of matrix \p x.
 *
 *  @fn int fmat_get_m(fmat_t *x)
 */
#define fmat_get_m(x) ((x)->m)

/** Get number of columns of matrix \p x.
 *
 *  @fn int fmat_get_n(fmat_t *x)
 */
#define fmat_get_n(x) ((x)->n)

#define fmat_editor_is_open(m) ((m)->opened == 1)
#define fmat_set_editor_open(m) ((m)->opened = 1)
#define fmat_set_editor_close(m) ((m)->opened = 0)

/** create fmat with m rows and n columns. */
DATA_API fmat_t *fmat_create(int m, int n);

/** change matrix "form", leaving underlying data vector untouched. */
DATA_API void fmat_reshape(fmat_t *fmat, int m, int n);

/** change matrix size, COPYING.LIB data around */
DATA_API void fmat_set_size(fmat_t *fmat, int m, int n);
DATA_API void fmat_set_m(fmat_t *fmat, int m);
DATA_API void fmat_set_n(fmat_t *fmat, int n);

/** get pointer to raw float data vector.
 *
 *  @fn float *fmat_get_ptr(fmat_t *x)
 */
#define fmat_get_ptr(m) ((m)->values)

/** get the matrix element at row \p i and column \p j.
 *
 *  @fn float *fmat_get_element(fmat_t *x, int i, int j)
 */
#define fmat_get_element(m, i, j) ((m)->values[(i) * (m)->n + (j)])

/** set the matrix element at row \p i and column \p j to \p val.
 *
 *  @fn float *fmat_set_element(fmat_t *x, int i, int j, float val)
 */
#define fmat_set_element(m, i, j, x) ((m)->values[(i) * (m)->n + (j)] = (x))

#define fmat_set_sr(fm, f) ((fm)->sr = (f))
#define fmat_get_sr(fm) ((fm)->sr)

#define fmat_set_onset(fm, f) ((fm)->onset = (f))
#define fmat_get_onset(fm) ((fm)->onset)

#define fmat_set_domain(fm, f) ((fm)->domain = (f))
#define fmat_get_domain(fm) (((fm)->domain > 0.0)? ((fm)->domain): ((double)(fm)->m))

/** check class and get dimensions for fmat, fcol or frow vector */
DATA_API int fmat_or_slice_vector(fts_object_t *obj, float **ptr, int *size, int *stride);

/** */
DATA_API void fmat_set_const(fmat_t *mat, float c);

/** */
DATA_API void fmat_copy(fmat_t *orig, fmat_t *copy);

/** */
DATA_API void fmat_set_from_atoms(fmat_t *mat, int onset, int step, int ac, const fts_atom_t *at);

DATA_API float fmat_get_max_abs_value_in_range(fmat_t *mat, int a, int b);
DATA_API float fmat_get_max_value_in_range(fmat_t *mat, int a, int b);
DATA_API float fmat_get_min_value_in_range(fmat_t *mat, int a, int b);

DATA_API void fmat_upload(fmat_t *mat);
/** @} end of group fmat */




/** @name fvec fvec backwards compatibility
 *
 * fvec is an alias/macro to fmat, with the constraint of having 
 * only one column.
 *
 * @{
 */

typedef fmat_t fvec_t;

DATA_API fts_symbol_t fvec_symbol;
DATA_API fts_class_t *fvec_class;
#define fvec_type fmat_class

/** @} end of group fvec */


#endif
