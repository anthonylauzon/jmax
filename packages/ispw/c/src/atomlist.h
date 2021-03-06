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

#ifndef _ATOMLIST_H_
#define _ATOMLIST_H_

#include "ispw.h"

#define FTS_ATOM_LIST_CELL_MAX_ATOMS 16

typedef struct fts_atom_list_cell
{
  int n;
  fts_atom_t atoms[FTS_ATOM_LIST_CELL_MAX_ATOMS];
  struct fts_atom_list_cell *next;
} fts_atom_list_cell_t;

struct fts_atom_list
{
  fts_object_t ob;

  fts_symbol_t name;	       /* list name */
  int size;
  fts_atom_list_cell_t *head;
  fts_atom_list_cell_t *tail;
};

typedef struct fts_atom_list fts_atom_list_t;

#define fts_atom_list_get_size(l)    ((l)->size)

ISPW_API fts_class_t *atomlist_type;
ISPW_API fts_symbol_t sym_atomlist_set_name;
ISPW_API fts_symbol_t sym_atomlist_update;

ISPW_API void fts_atom_list_clear( fts_atom_list_t *list);
ISPW_API void fts_atom_list_append( fts_atom_list_t *list, int ac, const fts_atom_t *atom);
ISPW_API void fts_atom_list_set( fts_atom_list_t *list, int ac, const fts_atom_t *atom);
ISPW_API void fts_atom_list_get_atoms( fts_atom_list_t *list, fts_atom_t *a);
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

ISPW_API void fts_atom_list_dump(fts_atom_list_t *list, fts_dumper_t *dump, fts_object_t *target);

/* Generic find support */



#endif
