/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _ATOMLIST_H_
#define _ATOMLIST_H_

struct fts_atom_list;
typedef struct fts_atom_list fts_atom_list_t;

extern fts_atom_list_t *fts_atom_list_new( void);
extern void fts_atom_list_free( fts_atom_list_t *list);

extern void fts_atom_list_clear( fts_atom_list_t *list);
extern void fts_atom_list_append( fts_atom_list_t *list, int ac, const fts_atom_t *atom);
extern void fts_atom_list_set( fts_atom_list_t *list, int ac, const fts_atom_t *atom);
extern int  fts_atom_list_is_subsequence( fts_atom_list_t *list, int ac, const fts_atom_t *at);

/* Iterators */

struct fts_atom_list_iterator;
typedef struct fts_atom_list_iterator fts_atom_list_iterator_t;

extern fts_atom_list_iterator_t *fts_atom_list_iterator_new( const fts_atom_list_t *list);
extern void fts_atom_list_iterator_free(fts_atom_list_iterator_t *iter);
extern void fts_atom_list_iterator_copy(fts_atom_list_iterator_t *iter, fts_atom_list_iterator_t *other);
extern void fts_atom_list_iterator_init( fts_atom_list_iterator_t *iter, const fts_atom_list_t *list);
extern void fts_atom_list_iterator_next( fts_atom_list_iterator_t *iter);
extern int fts_atom_list_iterator_end( const fts_atom_list_iterator_t *iter);
extern fts_atom_t *fts_atom_list_iterator_current( const fts_atom_list_iterator_t *iter);

/* Save to bmax */

extern void fts_atom_list_save_bmax(fts_atom_list_t *list, fts_bmax_file_t *f, fts_object_t *target);

/* Generic find support */



#endif
