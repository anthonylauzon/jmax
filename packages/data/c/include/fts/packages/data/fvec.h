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


/** @defgroup fvec fvec: fmat slice ref
 *  @ingroup  data
 *
 *  an fvec is a reference to a colum, row or diagonal of an fmat
 *
 *  @{
 */

typedef enum { fvec_type_column, fvec_type_row, fvec_type_diagonal, 
	       fvec_type_unwrap, fvec_type_vector, fvec_n_types } fvec_type_t;

/** fvec struct */
typedef struct
{
  fts_object_t o;
  fvec_type_t  type;
  fmat_t *fmat; /* pointer to fmat */
  int index; /* index of column (col) or index of row (row)  or row onset (diag) */
  int onset; /* row onset (col) or column onset (row) or column onset (diag) */
  int size; /* number of elements  */

  int opened; /* non zero if editor open */
  fts_object_t *editor;
} fvec_t;

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

/* globals */
DATA_API fts_symbol_t fvec_symbol;
DATA_API fts_class_t *fvec_class;

/** quick create */
DATA_API fvec_t *fvec_create (fmat_t *fmat, fvec_type_t type, int ac, const fts_atom_t *at);
DATA_API fvec_t *fvec_create_vector (int size);
DATA_API fvec_t *fvec_create_column(fmat_t *fmat);
DATA_API fvec_t *fvec_create_row(fmat_t *fmat);

DATA_API int   fvec_get_size (fvec_t *self);
DATA_API fts_symbol_t  fvec_get_type_as_symbol(fvec_t *self);

/** get element, no checks */
DATA_API float fvec_get_element(fvec_t *self, int i);

/** set element, no checks */
DATA_API void  fvec_set_element(fvec_t *self, int i, float value);

DATA_API void fvec_set_dimensions(fvec_t *fvec, int ac, const fts_atom_t *at);

DATA_API void fvec_set_from_atoms(fvec_t *vec, int onset, int ac, const fts_atom_t *at);

/** get fvec access parameters, then use ptr[i * stride] if i < size 
    @return	true if obj is an fvec, 
		false and size = stride = 0 otherwise */
DATA_API int  fvec_vector     (fts_object_t *obj, 
			       /*out*/ float **ptr, int *size, int *stride);

/** like fvec_vector, no checks */
DATA_API void fvec_get_vector (fvec_t *fvec, 
			       /*out*/ float **ptr, int *size, int *stride);

/** copy row or col from matrix reference to an fmat (1 column matrix) */
DATA_API void fvec_copy_to_fmat(fvec_t *org, fmat_t *copy);


/** @} end of group fvec */


#endif
