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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _ATOMMX_H_
#define _ATOMMX_H_

#include "fts.h"

extern fts_symbol_t sym_atom_matrix;

typedef struct
{
  fts_atom_t *data; /* matrix data */
  int m; /* # of rows */
  int n; /* # of columns */
  int alloc; /* current alloc size for lazy allocation */
} atom_matrix_t;


#define atom_matrix_get_m(mx) ((mx)->m)
#define atom_matrix_get_n(mx) ((mx)->n)

#define atom_matrix_get_row(mx, i) ((mx)->data + (i) * (mx)->n)

#define atom_matrix_set_element(mx, i, j, v) ((mx)->data[(i) * (mx)->n + (j)] = (v))
#define atom_matrix_get_element(mx, i, j) ((mx)->data[(i) * (mx)->n + (j)])

extern void atom_matrix_void(atom_matrix_t *mx);
extern atom_matrix_t *atom_matrix_new(int m, int n);
extern void atom_matrix_delete(atom_matrix_t *mx);
extern void atom_matrix_set_size(atom_matrix_t *mx, int m, int n);
extern void atom_matrix_fill(atom_matrix_t *mx, fts_atom_t atom);

extern int atom_matrix_import_ascii_newline(atom_matrix_t *mx, fts_symbol_t file_name);
extern int atom_matrix_export_ascii_newline(atom_matrix_t *mx, fts_symbol_t file_name);

extern int atom_matrix_import_ascii_separator(atom_matrix_t *mx, fts_symbol_t file_name, 
					      fts_symbol_t separator, int ac, fts_atom_t *at);
extern int atom_matrix_export_ascii_separator(atom_matrix_t *mx, fts_symbol_t file_name, fts_symbol_t separator);

#endif
