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


/* New version of the objtable.

   This version imply that the object IDs are assigned
   consecutively, starting from 1; any other client behaviour
   will eat enourmous amount of memory, and possibly will
   just kill the machine.

   Only one client is supported.

   The data structure is a direct access table, adaptively resized 
   when needed.

   The 0 id map by definition to the NULL pointer.
   */

#include <fts/sys.h>
#include <fts/lang/mess.h>

#define OBJECT_TABLE_INIT_SIZE (1024)
#define OBJECT_TABLE_GROW  2

static int object_table_size = 0;
static fts_object_t **object_table = 0;

/* skip 0, use even numbers only */

static int object_id_count = 2;

void fts_object_table_register(fts_object_t *obj)
{
  obj->head.id = object_id_count;
  object_id_count += 2;

  fts_object_table_put(obj->head.id, obj);
}

void
fts_object_table_put(int id, fts_object_t *obj)
{
  if (id <= 0)
    return;

  if (object_table_size == 0)
    {
      int i;

      object_table = fts_malloc(OBJECT_TABLE_INIT_SIZE * sizeof(fts_object_t *));
      object_table_size = OBJECT_TABLE_INIT_SIZE;

      for (i = 0; i < object_table_size; i ++)
	object_table[i] = 0;
    }
  else if (id >= object_table_size)
    {
      int i;
      int new_size;
      
      new_size = object_table_size;

      /* grow exponentially the table to avoid thousands of realloc;
	 this scheme produce ten realloc with 4 Mega objects :->
       */

      while (new_size <= id)
	new_size *= OBJECT_TABLE_GROW;

      object_table = fts_realloc(object_table, new_size * sizeof(fts_object_t *));

      for (i = object_table_size; i < new_size; i ++)
	object_table[i] = 0;

      object_table_size = new_size;
    }

  object_table[id] = obj;
}


void
fts_object_table_remove(int id)
{
  if (id < object_table_size)
    object_table[id] = 0;
}


fts_object_t *
fts_object_table_get(int id)
{
  if (id == 0)
    return 0;
  else if (id < object_table_size)
    return object_table[id];
  else
    return 0;
}

/* for  restart */

void
fts_object_table_delete_all(void)
{
  int i;
  
  /* first slot of the table unused */

  for (i = 1; i < object_table_size; i++)
    if (object_table[i])
      fts_object_delete_from_patcher(object_table[i]);
}


#define CONNECTION_TABLE_INIT_SIZE (1024)
#define CONNECTION_TABLE_GROW  2

static int connection_table_size = 0;
static fts_connection_t **connection_table = 0;

/* skip 0, use even numbers only */

static int connection_id_count = 2;

void fts_connection_table_register(fts_connection_t *conn)
{
  conn->id = connection_id_count;
  connection_id_count += 2;

  fts_connection_table_put(conn->id, conn);
}

void
fts_connection_table_put(int id, fts_connection_t *conn)
{
  if (id <= 0)
    return;

  if (connection_table_size == 0)
    {
      int i;

      connection_table = fts_malloc(CONNECTION_TABLE_INIT_SIZE * sizeof(fts_connection_t *));
      connection_table_size = CONNECTION_TABLE_INIT_SIZE;

      for (i = 0; i < connection_table_size; i ++)
	connection_table[i] = 0;
    }
  else if (id >= connection_table_size)
    {
      int i;
      int new_size;
      
      new_size = connection_table_size;

      /* grow exponentially the table to avoid thousands of realloc;
	 this scheme produce ten realloc with 4 Mega connections :->
       */

      while (new_size <= id)
	new_size *= CONNECTION_TABLE_GROW;

      connection_table = fts_realloc(connection_table, new_size * sizeof(fts_connection_t *));

      for (i = connection_table_size; i < new_size; i ++)
	connection_table[i] = 0;

      connection_table_size = new_size;
    }

  connection_table[id] = conn;
}


void
fts_connection_table_remove(int id)
{
  if (id < connection_table_size)
    connection_table[id] = 0;
}


fts_connection_t *
fts_connection_table_get(int id)
{
  if (id == 0)
    return 0;
  else if (id < connection_table_size)
    return connection_table[id];
  else
    return 0;
}












