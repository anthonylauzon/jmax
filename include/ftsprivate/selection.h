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


#ifndef _FTS_PRIVATE_SELECTION_H_
#define _FTS_PRIVATE_SELECTION_H_

/* Selection */
typedef struct _fts_selection_t
{
  fts_object_t ob;

  fts_object_t **objects;
  int objects_size;
  int objects_count;

  fts_connection_t **connections;
  int connections_size;
  int connections_count;
} fts_selection_t;

FTS_API int fts_selection_contains_object(fts_selection_t *sel, fts_object_t *o);
FTS_API int fts_selection_connection_ends_selected(fts_selection_t *sel, fts_connection_t *c); 

#endif
