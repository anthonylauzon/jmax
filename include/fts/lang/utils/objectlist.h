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
 */

#ifndef _FTS_OBJECTLIST_H_
#define _FTS_OBJECTLIST_H_

typedef struct _fts_object_list_cell {
  fts_object_t *object;
  struct _fts_object_list_cell *next;
} fts_object_list_cell_t;

typedef struct {
  fts_object_list_cell_t *head;
} fts_object_list_t;

FTS_API void fts_object_list_init( fts_object_list_t *list);
FTS_API void fts_object_list_destroy( fts_object_list_t *list);

FTS_API void fts_object_list_insert( fts_object_list_t *list, fts_object_t *object);

FTS_API void fts_object_list_remove( fts_object_list_t *list, fts_object_t *object);

#define fts_object_list_is_empty(L) (!(L)->head)
#endif
