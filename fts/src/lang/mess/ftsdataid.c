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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"

#define DATA_TABLE_INIT_SIZE (1024)
#define DATA_TABLE_GROW  2

static int data_table_size = 0;
static fts_data_t **data_table = 0;

static void grow_table( int id)
{
  int i, old_size;

  old_size = data_table_size;

  while (data_table_size <= id)
    data_table_size *= DATA_TABLE_GROW;

  data_table = realloc( data_table, data_table_size * sizeof(fts_data_t *));

  for (i = old_size; i < data_table_size; i ++)
    data_table[i] = 0;
}

static void init_table(void)
{
  int i;

  data_table_size = DATA_TABLE_INIT_SIZE;
  data_table = malloc( data_table_size * sizeof(fts_data_t *));

  for ( i = 0; i < data_table_size; i++)
    data_table[i] = 0;
}

void fts_data_id_put( int id, fts_data_t *d)
{
  if (id <= 0)
    return;

  if (id >= data_table_size)
    grow_table( id);
    
  data_table[id] = d;
}

/* We should have a way to recycle ids in big deletes */

void fts_data_id_remove( int id, fts_data_t *d)
{
  if ((id <= 0) || (id >= data_table_size))
    return;
    
  data_table[id] = 0;
}

fts_data_t *fts_data_id_get( int id)
{
  if (id <= 0 || id > data_table_size)
    return 0;

  return data_table[id];
}

void fts_data_id_init()
{
  init_table();
}

