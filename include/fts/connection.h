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


#ifndef _FTS_CONNECTION_H_
#define _FTS_CONNNECTION_H_

typedef enum fts_connection_type
{
  fts_c_invalid = 0, /* from error object or type missmatch */
  fts_c_anything = 1, /* message which is not one of the following */
  fts_c_atom = 2, /* single atom (value) */
  fts_c_object = 3, /* objects */
  fts_c_signal = 4 /* signal connection */
} fts_connection_type_t;

struct fts_connection
{
  fts_object_t *src;
  int woutlet;

  fts_object_t *dst;
  int winlet;

  int id; /* the connection ID, when defined */
 
  fts_symbol_t symb; /* the message cache: the symbol: if null, means anything ! */
  fts_method_t  mth; /* the message  cache: the method */

  fts_connection_type_t type; /* user visible connection cathogeries */

  fts_connection_t *next_same_dst;
  fts_connection_t *next_same_src;
};

#endif
