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

#ifndef _FTS_PRIVATE_CONNECTION_H_
#define _FTS_PRIVATE_CONNECTION_H_

extern void fts_connections_init(void);

#define fts_connection_get_id(c) ((c)->id)
#define fts_connection_get_source(c) ((c)->src)
#define fts_connection_get_outlet(c) ((c)->woutlet)
#define fts_connection_get_destination(c) ((c)->dst)
#define fts_connection_get_inlet(c) ((c)->winlet)
#define fts_connection_get_type(c) ((c)->type)

extern void fts_connection_set_type(fts_connection_t *connection, fts_connection_type_t type);

#endif


