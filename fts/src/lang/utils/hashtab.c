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
  fts_symbol_t symbol;
  fts_atom_t user_data;
  struct _hash_cell_t *next;
};

#define HASH_TABLE_DEFAULT_LENGTH 11

static unsigned int hash( const fts_hash_table_t *ht, fts_symbol_t sym)
{
  return ((unsigned int)sym) % ht->length;
}

void fts_hash_table_init( fts_hash_table_t *ht)
{
  ht->length = HASH_TABLE_DEFAULT_LENGTH;
  ht->cells_cnt = 0;
  ht->table = (hash_cell_t **) fts_zalloc( ht->length * sizeof( hash_cell_t *));
}

fts_hash_table_t *fts_hash_table_new( void)
{
  fts_hash_table_t *ht;

  ht = (fts_hash_table_t *) fts_zalloc( sizeof( fts_hash_table_t));

  if ( ht)
    fts_hash_table_init( ht);
  return ht;
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


void fts_hash_table_free( fts_hash_table_t *ht)
{
  fts_hash_table_destroy( ht);
  fts_free( ht);
}


int fts_hash_table_get_count( const fts_hash_table_t *ht)
{
  return ht->cells_cnt;
}


int fts_hash_table_lookup( const fts_hash_table_t *ht, fts_symbol_t sym, fts_atom_t *data)
{
  hash_cell_t *first;
  int h;
  
  h = hash( ht, sym);
  for( first = ht->table[ h]; first ; first = first->next)
    if ( first->symbol == sym)
      {
	*data = first->user_data;
	return 1;
      }
  return 0;
}


int fts_hash_table_remove( fts_hash_table_t *ht, fts_symbol_t sym)
{
  hash_cell_t *item, *prev = 0;
  int h;
  
  h = hash( ht, sym);
  for( item = ht->table[h]; item ; item = item->next)
    {
      if ( item->symbol == sym)
	{
	  if (prev)
	    prev->next = item->next;
	  else
	    ht->table[h] = item->next;

	  fts_heap_free((char *)item, hash_cell_heap);
	  ht->cells_cnt--;
	  return 1;
	}
      prev = item;
    }
  return 0;
}

int fts_hash_table_insert( fts_hash_table_t *ht, fts_symbol_t sym, fts_atom_t *data)
{
  hash_cell_t **insert, *new;
  int h;

  h = hash( ht, sym);
  for( insert = ht->table + h; (*insert); insert = &((*insert)->next) )
    if ( (*insert)->symbol == sym)
      return 0;

  if (! hash_cell_heap)
    hash_cell_heap = fts_heap_new(sizeof( hash_cell_t));

  new = (hash_cell_t *)fts_heap_zalloc(hash_cell_heap);
  if ( !new)
    return 0;
  new->symbol = sym;
  new->user_data = *data;
  new->next = 0;
  *insert = new;

  ht->cells_cnt++;
  return 1;
}


void fts_hash_table_apply( const fts_hash_table_t *ht, void (*fun)( fts_symbol_t sym, fts_atom_t *data, void *user_data),  void *user_data)
{
  hash_cell_t *cell;
  int i;

  for ( i=0; i<ht->length; i++)
    for ( cell = ht->table[i]; cell; cell = cell->next)
      (*fun) ( cell->symbol, &(cell->user_data), user_data);
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


fts_hash_table_iterator_t *fts_hash_table_iterator_new( const fts_hash_table_t *ht)
{
  fts_hash_table_iterator_t *iter;

  iter = (fts_hash_table_iterator_t *) fts_malloc( sizeof( fts_hash_table_iterator_t ));

  if (iter)
    fts_hash_table_iterator_init( iter, ht);

  return iter;
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

fts_symbol_t fts_hash_table_iterator_current_symbol( const fts_hash_table_iterator_t *iter)
{
  return iter->cell->symbol;
}

fts_atom_t *fts_hash_table_iterator_current_data( const fts_hash_table_iterator_t *iter)
{
  return &(iter->cell->user_data);
}
