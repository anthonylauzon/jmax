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
 */

#ifndef _ATOMLIST_H_
#define _ATOMLIST_H_

#include "ispw.h"

struct fts_atom_list;
typedef struct fts_atom_list fts_atom_list_t;

/*fts_atom_list_t *fts_atom_list_new( void);
  void fts_atom_list_free( fts_atom_list_t *list);*/

ISPW_API fts_symbol_t atomlist_symbol;
ISPW_API fts_symbol_t sym_atomlist_set_name;
ISPW_API fts_symbol_t sym_atomlist_update;

ISPW_API void fts_atom_list_clear( fts_atom_list_t *list);
ISPW_API void fts_atom_list_append( fts_atom_list_t *list, int ac, const fts_atom_t *atom);
ISPW_API void fts_atom_list_set( fts_atom_list_t *list, int ac, const fts_atom_t *atom);
ISPW_API int  fts_atom_list_is_subsequence( fts_atom_list_t *list, int ac, const fts_atom_t *at);

/* Iterators */

struct fts_atom_list_iterator;
typedef struct fts_atom_list_iterator fts_atom_list_iterator_t;

ISPW_API fts_atom_list_iterator_t *fts_atom_list_iterator_new( const fts_atom_list_t *list);
ISPW_API void fts_atom_list_iterator_free(fts_atom_list_iterator_t *iter);
ISPW_API void fts_atom_list_iterator_copy(fts_atom_list_iterator_t *iter, fts_atom_list_iterator_t *other);
ISPW_API void fts_atom_list_iterator_init( fts_atom_list_iterator_t *iter, const fts_atom_list_t *list);
ISPW_API void fts_atom_list_iterator_next( fts_atom_list_iterator_t *iter);
ISPW_API int fts_atom_list_iterator_end( const fts_atom_list_iterator_t *iter);
ISPW_API fts_atom_t *fts_atom_list_iterator_current( const fts_atom_list_iterator_t *iter);

/* Save to bmax */

ISPW_API void fts_atom_list_save_bmax(fts_atom_list_t *list, fts_bmax_file_t *f, fts_object_t *target);

/* Generic find support */



#endif
