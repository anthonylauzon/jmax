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

/********************************************************************************
 *
 *  fmat format
 *
 */
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

/********************************************************************************
 *
 *  fmat
 *
 */
typedef struct
{
  fts_object_t o;
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
DATA_API fts_class_t *fmat_class;
#define fmat_type fmat_class

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

#define fmat_editor_is_open(m) ((m)->opened == 1)
#define fmat_editor_set_open(m) ((m)->opened = 1)
#define fmat_editor_set_close(m) ((m)->opened = 0)

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
DATA_API void fmat_set_from_atoms(fmat_t *mat, int onset, int step, int ac, const fts_atom_t *at);

DATA_API float fmat_get_max_abs_value_in_range(fmat_t *mat, int a, int b);
DATA_API float fmat_get_max_value_in_range(fmat_t *mat, int a, int b);
DATA_API float fmat_get_min_value_in_range(fmat_t *mat, int a, int b);

/********************************************************************************
 *
 *  fvec
 *
 */
typedef fmat_t fvec_t;

DATA_API fts_symbol_t fvec_symbol;
DATA_API fts_class_t *fvec_class;
#define fvec_type fvec_class

/********************************************************************************
 *
 *  fmat slice: fcol, frow
 *
 */
typedef struct
{
  fts_object_t o;
	enum {fslice_column, fslice_row} type;
  fmat_t *fmat; /* pointer to fmat */
  int index; /* index of row or column */
} fslice_t;

#define fslice_init_column(s, m, i) ((s)->type = fslice_column, (s)->fmat = (m), (s)->index = (i))
#define fslice_init_row(s, m, i) ((s)->type = fslice_row, (s)->fmat = (m), (s)->index = (i))

#define fslice_get_index(s) ((s)->index)
#define fslice_check_index(s) (((s)->type == fslice_row)? \
                               ((s)->index < fmat_get_m((s)->fmat)): \
                               ((s)->index < fmat_get_n((s)->fmat)))

#define fslice_get_ptr(s) (((s)->type == fslice_row)? \
                           (fmat_get_ptr((s)->fmat) + (s)->index * fmat_get_n((s)->fmat)): \
                           (fmat_get_ptr((s)->fmat) + (s)->index))
                           
#define fslice_get_stride(s) (((s)->type == fslice_row)? (1): (fmat_get_n((s)->fmat)))
#define fslice_get_size(s) (((s)->type == fslice_row)? (fmat_get_n((s)->fmat)): (fmat_get_m((s)->fmat)))
#define fslice_get_m(s) (((s)->type == fslice_row)? (1): (fmat_get_m((s)->fmat)))
#define fslice_get_n(s) (((s)->type == fslice_row)? (fmat_get_n((s)->fmat)): (1))
                           
#define frow_get_ptr(f) (fmat_get_ptr((f)->fmat) + (f)->index * fmat_get_n((f)->fmat))
#define fcol_get_ptr(f) (fmat_get_ptr((f)->fmat) + (f)->index)

#define frow_get_stride(f) (1)
#define fcol_get_stride(f) (fmat_get_n((f)->fmat))

#define frow_get_n(f) (fmat_get_n((f)->fmat))
#define fcol_get_n(f) (1)

#define frow_get_m(f) (1)
#define fcol_get_m(f) (fmat_get_m((f)->fmat))

#define frow_get_index(f) ((f)->index)
#define fcol_get_index(f) ((f)->index)

#define frow_check_index(f) ((f)->index < fmat_get_m((f)->fmat))
#define fcol_check_index(f) ((f)->index < fmat_get_n((f)->fmat))

#endif
