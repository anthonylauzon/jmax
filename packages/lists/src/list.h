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

#ifndef _LIST_H_
#define _LIST_H_

typedef struct 
{
  fts_atom_t *at;
  int ac;
  int alloc;
} list_t;

extern void list_init(list_t *list);
extern void list_free(list_t *list);
extern void list_set_size(list_t *list, int size);
extern void list_set(list_t *list, int ac, const fts_atom_t *at);

extern void list_raw_resize(list_t *list, int size);
extern void list_raw_set(list_t *list, int offset, int ac, const fts_atom_t *at);

#define list_get_ptr(l) ((l)->at)
#define list_get_size(l) ((l)->ac)

#endif
