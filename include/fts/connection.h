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


FTS_API fts_metaclass_t *fts_connection_metaclass;

struct fts_connection
{
  fts_object_t o;

  fts_object_t *src;
  int woutlet;

  fts_object_t *dst;
  int winlet;

  fts_symbol_t selector; /* cache message selector */
  fts_metaclass_t *class; /* message cache class (type, NULL == varargs) */
  fts_method_t method; /* message cache method */

  fts_connection_type_t type;

  fts_connection_t *next_same_dst;
  fts_connection_t *next_same_src;
};


FTS_API fts_connection_t *fts_connection_new(fts_object_t *src, int out, fts_object_t *dst, int in, fts_connection_type_t type);
FTS_API void fts_connection_delete(fts_connection_t *conn);
FTS_API fts_connection_t *fts_connection_get(fts_object_t *src, int out, fts_object_t *dst, int in);

#define fts_is_connection(p) (fts_object_get_metaclass(p) == fts_connection_metaclass)

FTS_API void fts_object_trim_inlets_connections(fts_object_t *obj, int inlets);
FTS_API void fts_object_trim_outlets_connections(fts_object_t *obj, int outlets);


