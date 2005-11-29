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


/** Atom matrix.
 *
 *  @file mat.h
 *  @ingroup data
 */


DATA_API fts_class_t *mat_type;
DATA_API fts_symbol_t mat_symbol;

typedef struct
{
  fts_object_t o;
  fts_atom_t *data; /* mat data */
  int m; /* # of rows */
  int n; /* # of columns */
  int alloc; /* current alloc size for lazy allocation */
  int opened; /* non zero if editor open */
} mat_t;

DATA_API void mat_set_size(mat_t *mat, int m, int n);
#define mat_get_m(x) ((x)->m)
#define mat_get_n(x) ((x)->n)

#define mat_get_ptr(x) ((x)->data)

DATA_API void mat_set_element(mat_t *mat, int i, int j, const fts_atom_t *atom);
#define mat_get_element(x, i, j) ((x)->data + (i) * (x)->n + (j))

#define mat_get_row(x, i) ((x)->data + (i) * (x)->n)

DATA_API void mat_set_const(mat_t *mat, const fts_atom_t *atom);
DATA_API void mat_copy(mat_t *org, mat_t *copy);

DATA_API void mat_set_with_onset_from_atoms(mat_t *mat, int offset, int ac, const fts_atom_t *at);
DATA_API void mat_set_from_tuples(mat_t *mat, int ac, const fts_atom_t *at);

DATA_API int mat_read_atomfile_newline(mat_t *mat, fts_symbol_t file_name);
DATA_API int mat_write_atomfile_newline(mat_t *mat, fts_symbol_t file_name);

DATA_API int mat_read_atomfile_separator(mat_t *mat, fts_symbol_t file_name, fts_symbol_t separator, int ac, const fts_atom_t *at);
DATA_API int mat_write_atomfile_separator(mat_t *mat, fts_symbol_t file_name, fts_symbol_t separator);


/* editor interaction */
#define mat_set_editor_open(m) ((m)->opened = 1)
#define mat_set_editor_close(m) ((m)->opened = 0)
#define mat_editor_is_open(m) ((m)->opened)

DATA_API void mat_upload(mat_t *self);

#endif
