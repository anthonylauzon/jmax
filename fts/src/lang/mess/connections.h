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


#ifndef _CONNECTIONS_H_
#define _CONNECTIONS_H_

/* init function */

extern void fts_connections_init(void);

/* Connections */

extern fts_connection_t *fts_connection_new(int id, fts_object_t *, int woutlet, fts_object_t *, int winlet);
extern void fts_connection_delete(fts_connection_t *conn);

extern void fts_object_move_connections(fts_object_t *old, fts_object_t *new, int doclient);
extern void fts_object_trim_inlets_connections(fts_object_t *obj, int inlets);
extern void fts_object_trim_outlets_connections(fts_object_t *obj, int outlets);

#define fts_connection_get_id(c)     ((c)->id)

/* Debug print */

extern void fprintf_connection(FILE *f, fts_connection_t *conn);


#endif

