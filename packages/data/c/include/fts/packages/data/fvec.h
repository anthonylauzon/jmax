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

/******************************************************************************
*
*  fvec compatibility
*  
*  Note: fvec is now fmat! here is the code for matrix slices fcol and frow
*
*/
#include <fts/packages/data/data.h>

#define fvec_get_size(v) ((v)->m)
#define fvec_set_size(v, s) fmat_set_size((v), (s), 1)

#define fvec_get_ptr(v) ((v)->values)
#define fvec_get_element(m, i) ((m)->values[(i)])
#define fvec_set_element(v, i, x) ((v)->values[i] = (x))

#define fvec_copy(o, c) fmat_copy((o), (c))
#define fvec_set_const(v, c) fmat_set_const((v), (c))
#define fvec_set_with_onset_from_atoms(v, o, n, a) fmat_set_from_atoms((v), (o), 1, (n), (a))


/******************************************************************************
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

DATA_API fts_symbol_t fcol_symbol;
DATA_API fts_symbol_t frow_symbol;
DATA_API fts_class_t *fcol_class;
DATA_API fts_class_t *frow_class;

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

DATA_API void fslice_copy_to_fmat(fslice_t *org, fmat_t *copy);

#endif
