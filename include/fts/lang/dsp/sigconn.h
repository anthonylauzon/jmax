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
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#ifndef _FTS_SIGCONN_H_
#define _FTS_SIGCONN_H_

typedef struct _fts_signal_connection_table_
{
  fts_connection_t **connections;
  int size;
  int alloc;
} fts_signal_connection_table_t;

FTS_API void fts_signal_connection_table_init(fts_signal_connection_table_t *table);
FTS_API void fts_signal_connection_add(fts_signal_connection_table_t *table, fts_connection_t* connection);
FTS_API void fts_signal_connection_remove_all(fts_signal_connection_table_t *table);

#endif