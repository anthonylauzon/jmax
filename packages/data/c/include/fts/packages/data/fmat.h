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

#define FMAT_FORMATS_MAX 256

enum fmat_format_id_enum {
  fmat_format_id_vec, 
  fmat_format_id_rect, 
  fmat_format_id_polar,
  fmat_format_id_real = FMAT_FORMATS_MAX
};

typedef struct
{
  fts_symbol_t name;
  enum fmat_format_id_enum index;
  int n_columns;
  fts_symbol_t columns[16];
} fmat_format_t;

DATA_API fmat_format_t *fmat_format_vec;
DATA_API fmat_format_t *fmat_format_rect;
DATA_API fmat_format_t *fmat_format_polar;
DATA_API fmat_format_t *fmat_format_real;

#define fmat_format_get_name(f) ((f)->name)
#define fmat_format_get_id(f) ((f)->index)
#define fmat_format_get_n(f) ((f)->n_columns)
#define fmat_format_get_column_name(f, i) ((f)->column[i])

typedef struct
{
  data_object_t o;
  float *values;
  int m;
  int n;
  int alloc;
  double sr; /* sample rate */
  double onset; /* fractional onset */
  fmat_format_t *format;
  int opened;
  fts_object_t *editor;
} fmat_t;

/**
* @ingroup fmat
 */
DATA_API fts_symbol_t fmat_symbol;

/**
* @ingroup fmat
 */
DATA_API fts_class_t *fmat_type;



/** Get number of rows of matrix x.
*
* @fn int fmat_get_m(fmat_t *x)
* @ingroup fmat
*/
#define fmat_get_m(x) ((x)->m)

/** Get number of columns of matrix x.
*
* @fn int fmat_get_n(fmat_t *x)
* @ingroup fmat
*/
#define fmat_get_n(x) ((x)->n)

#define fmat_get_format(x) ((x)->format)
#define fmat_set_format(x, f) ((x)->format = (f))

/**
* @ingroup fmat
 */
DATA_API void fmat_reshape(fmat_t *fmat, int m, int n);
DATA_API void fmat_set_size(fmat_t *fmat, int m, int n);
DATA_API void fmat_set_m(fmat_t *fmat, int m);
DATA_API void fmat_set_n(fmat_t *fmat, int n);

/**
* @fn float *fmat_get_ptr(fmat_t *x)
 * @ingroup fmat
 */
#define fmat_get_ptr(m) ((m)->values)


/**
* @fn float *fmat_get_element(fmat_t *x, int i, int j)
 * @ingroup fmat
 */
#define fmat_get_element(m, i, j) ((m)->values[(i) * (m)->n + (j)])

/**
* @fn float *fmat_set_element(fmat_t *x, int i, int j, float val)
 * @ingroup fmat
 */
#define fmat_set_element(m, i, j, x) ((m)->values[(i) * (m)->n + (j)] = (x))

#define fmat_set_sr(fm, f) ((fm)->sr = (f))
#define fmat_get_sr(fm) ((fm)->sr)

#define fmat_set_onset(fm, f) ((fm)->onset = (f))
#define fmat_get_onset(fm) ((fm)->onset)

/**
* @ingroup fmat
 */
DATA_API void fmat_set_const(fmat_t *mat, float c);

/**
* @ingroup fmat
 */
DATA_API void fmat_copy(fmat_t *orig, fmat_t *copy);

/**
* @ingroup fmat
 */
DATA_API void fmat_set_with_onset_from_atoms(fmat_t *mat, int offset, int ac, const fts_atom_t *at);

#endif
