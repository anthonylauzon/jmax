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

#include <fts/fts.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/sigconn.h>

#define BLOCK_SIZE 256

static void
fts_signal_connection_table_grow(fts_signal_connection_table_t *table)
{
  int alloc = table->alloc + BLOCK_SIZE;
  fts_connection_t **old = table->connections;
  fts_connection_t **new = (fts_connection_t **)fts_malloc(sizeof(fts_connection_t *) * alloc);
  int i;
  
  for(i=0; i<table->size; i++)
    new[i] = old[i];

  if(old)
    fts_free(old);

  table->alloc = alloc;
  table->connections = new;
}

void
fts_signal_connection_table_init(fts_signal_connection_table_t *table)
{
  table->connections = 0;
  table->alloc = 0;
  table->size = 0;

  fts_signal_connection_table_grow(table);
}

void
fts_signal_connection_add(fts_signal_connection_table_t *table, fts_connection_t* connection)
{  
  if(fts_connection_get_type(connection) != fts_c_signal && fts_connection_get_type(connection) > fts_c_hidden)
    {
      fts_object_t *source = fts_connection_get_source(connection);
      int n = table->size;
      
      if(n >= table->alloc)
	fts_signal_connection_table_grow(table);
      
      table->connections[n] = connection;
      table->size++;

      fts_connection_set_type(connection, fts_c_signal);
    }
}

void
fts_signal_connection_remove_all(fts_signal_connection_table_t *table)
{  
  fts_connection_t **ptr = table->connections;
  int i;

  for(i=0; i<table->size; i++)
    {
      fts_object_t *source = fts_connection_get_source(ptr[i]);

      fts_connection_set_type(ptr[i], fts_c_anything);
    }

  table->size = 0;
}
