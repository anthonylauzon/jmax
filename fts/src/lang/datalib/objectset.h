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

#ifndef _OBJECTSET_H_
#define _OBJECTSET_H_


struct fts_object_set;
typedef struct fts_object_set fts_object_set_t;

extern fts_object_set_t *fts_object_set_new(void);
extern void fts_object_set_delete(fts_object_set_t *set);

extern void fts_object_set_add(fts_object_set_t *set, fts_object_t *object);
extern void fts_object_set_remove(fts_object_set_t *set, fts_object_t *object);
extern void fts_object_set_remove_all(fts_object_set_t *set);

extern int fts_object_set_have_member(fts_object_set_t *set, fts_object_t *object);

extern void fts_object_set_send_message(fts_object_set_t *set, int winlet, fts_symbol_t sel,
					int ac, const fts_atom_t *av);

/* Iterators */

struct fts_object_set_iterator;
typedef struct fts_object_set_iterator fts_object_set_iterator_t;

extern void fts_object_set_iterator_init( fts_object_set_iterator_t *iter, const fts_object_set_t *set);
extern fts_object_set_iterator_t *fts_object_set_iterator_new( const fts_object_set_t *set);
extern void fts_object_set_iterator_free(fts_object_set_iterator_t *iter);

extern void fts_object_set_iterator_next( fts_object_set_iterator_t *iter);
extern int fts_object_set_iterator_end( const fts_object_set_iterator_t *iter);
extern fts_object_t *fts_object_set_iterator_current( const fts_object_set_iterator_t *iter);


#endif
