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


/*
 * The 0 id map by definition to the NULL pointer.
 */

#include <fts/ftsnew.h>
#include <fts/private/objtable.h>

static fts_hashtable_t object_table;
static fts_hashtable_t connection_table;

/* **********************************************************************
 *
 * Object table
 *
 */

/* skip 0, use even numbers only */
static int object_id_count = 2;

void fts_object_table_register(fts_object_t *obj)
{
  obj->head.id = object_id_count;
  object_id_count += 2;

  fts_object_table_put(obj->head.id, obj);
}

void fts_object_table_put(int id, fts_object_t *obj)
{
  fts_atom_t k, v;

  if (id <= 0)
    return;

  fts_set_int( &k, id);
  fts_set_object( &v, obj);
  fts_hashtable_put( &object_table, &k, &v);
}

void fts_object_table_remove( int id)
{
  fts_atom_t k;

  if (id <= 0)
    return;

  fts_set_int( &k, id);
  fts_hashtable_remove( &object_table, &k);
}

fts_object_t *fts_object_table_get(int id)
{
  fts_atom_t k, v;

  if (id == 0)
    return 0;

  fts_set_int( &k, id);
  if (fts_hashtable_get( &object_table, &k, &v))
    return fts_get_object( &v);

  return 0;
}


/* **********************************************************************
 *
 * Connection table
 *
 */

/* skip 0, use even numbers only */
static int connection_id_count = 2;

void fts_connection_table_register( fts_connection_t *conn)
{
  conn->id = connection_id_count;
  connection_id_count += 2;

  fts_connection_table_put(conn->id, conn);
}

void fts_connection_table_put(int id, fts_connection_t *conn)
{
  fts_atom_t k, v;

  if (id <= 0)
    return;

  fts_set_int( &k, id);
  fts_set_connection( &v, conn);
  fts_hashtable_put( &connection_table, &k, &v);
}

void fts_connection_table_remove(int id)
{
  fts_atom_t k;

  if (id <= 0)
    return;

  fts_set_int( &k, id);
  fts_hashtable_remove( &connection_table, &k);
}

fts_connection_t *fts_connection_table_get(int id)
{
  fts_atom_t k, v;

  if (id == 0)
    return 0;

  fts_set_int( &k, id);
  if (fts_hashtable_get( &connection_table, &k, &v))
    return fts_get_connection( &v);

  return 0;
}


/* **********************************************************************
 *
 * Initialization
 *
 */

void fts_objtable_init( void)
{
  fts_hashtable_init( &object_table, fts_t_int, FTS_HASHTABLE_BIG);
  fts_hashtable_init( &connection_table, fts_t_int, FTS_HASHTABLE_BIG);
}
