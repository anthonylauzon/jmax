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

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

fts_heap_t *hash_cell_heap = 0;

struct _hash_cell_t {
  fts_symbol_t key;
  fts_atom_t value;
  struct _hash_cell_t *next;
};

#define HASH_TABLE_DEFAULT_LENGTH 11

static unsigned int hash( const fts_hash_table_t *ht, fts_symbol_t key)
{
  return ((unsigned int)key) % ht->length;
}

void fts_hash_table_init( fts_hash_table_t *ht)
{
  ht->length = HASH_TABLE_DEFAULT_LENGTH;
  ht->table = (hash_cell_t **) fts_zalloc( ht->length * sizeof( hash_cell_t *));
}

void fts_hash_table_destroy( fts_hash_table_t *ht)
{
  int i;
  hash_cell_t *c, *next;

  for ( i = 0; i < ht->length; i++)
    for( c = ht->table[i]; c ; c = next)
      {
	next = c->next;
	fts_heap_free((char *)c, hash_cell_heap);
      }

  if (ht->table)
    fts_free( ht->table);
  ht->table = 0;
  ht->length = 0;
}


int fts_hash_table_get( const fts_hash_table_t *table, fts_symbol_t key, fts_atom_t *value)
{
  hash_cell_t *first;
  int h;
  
  h = hash( table, key);
  for( first = table->table[ h]; first ; first = first->next)
    if ( first->key == key)
      {
	*value = first->value;
	return 1;
      }
  return 0;
}


int fts_hash_table_remove( fts_hash_table_t *table, fts_symbol_t key)
{
  hash_cell_t *item, *prev = 0;
  int h;
  
  h = hash( table, key);
  for( item = table->table[h]; item ; item = item->next)
    {
      if ( item->key == key)
	{
	  if (prev)
	    prev->next = item->next;
	  else
	    table->table[h] = item->next;

	  fts_heap_free((char *)item, hash_cell_heap);
	  return 1;
	}
      prev = item;
    }
  return 0;
}

void fts_hash_table_put( fts_hash_table_t *table, fts_symbol_t key, fts_atom_t *value)
{
  hash_cell_t **insert, *new;
  int h;

  h = hash( table, key);
  for( insert = table->table + h; (*insert); insert = &((*insert)->next) )
    if ( (*insert)->key == key)
      {
	(*insert)->value = *value;
	return;
      }

  if (! hash_cell_heap)
    hash_cell_heap = fts_heap_new(sizeof( hash_cell_t));

  new = (hash_cell_t *)fts_heap_zalloc(hash_cell_heap);
  if ( !new)
    return;
  new->key = key;
  new->value = *value;
  new->next = 0;

  *insert = new;
}


static void fts_hash_table_iterator_skip_null( fts_hash_table_iterator_t *iter, int i)
{
  const fts_hash_table_t *ht;

  ht = iter->ht;
  for ( ; i < ht->length && ! ht->table[i]; i++)
    ;
  iter->i = i;
  if ( i >= ht->length)
    iter->cell = 0;
  else
    iter->cell = ht->table[i];
}

void fts_hash_table_iterator_init( fts_hash_table_iterator_t *iter, const fts_hash_table_t *ht)
{
  iter->ht = ht;
  fts_hash_table_iterator_skip_null( iter, 0);
}


void fts_hash_table_iterator_next( fts_hash_table_iterator_t *iter)
{
  iter->cell = iter->cell->next;
  if ( !iter->cell)
    fts_hash_table_iterator_skip_null( iter, iter->i+1);
}

int fts_hash_table_iterator_end( const fts_hash_table_iterator_t *iter)
{
  return iter->cell == 0 && iter->i >= iter->ht->length;
}

fts_symbol_t fts_hash_table_iterator_current_key( const fts_hash_table_iterator_t *iter)
{
  return iter->cell->key;
}

fts_atom_t *fts_hash_table_iterator_current_value( const fts_hash_table_iterator_t *iter)
{
  return &(iter->cell->value);
}
