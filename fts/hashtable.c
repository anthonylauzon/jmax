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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <fts/fts.h>


#define FTS_HASHTABLE_STANDARD_LOAD_FACTOR  0.75

/* **********************************************************************
 * Hashtable
 */

struct _fts_hashtable_cell_t {
  fts_atom_t key;
  fts_atom_t value;
  struct _fts_hashtable_cell_t *next;
};

typedef struct {
  fts_hashtable_cell_t **table;
  int index;
  int keys;
  fts_hashtable_cell_t *cell;
} fts_hashtable_iterator_t;

static fts_heap_t *cell_heap = 0;
static fts_heap_t *iterator_heap = 0;

static unsigned int hash_int( const fts_atom_t *a)
{
  return (unsigned int)fts_get_int( a);
}

static int equals_int( const fts_atom_t *a, const fts_atom_t *b)
{
  return fts_get_int( a) == fts_get_int( b);
}

static unsigned int hash_ptr( const fts_atom_t *a)
{
  return (unsigned int)fts_get_pointer( a) >> 3;
}

static int equals_ptr( const fts_atom_t *a, const fts_atom_t *b)
{
  return fts_get_pointer( a) == fts_get_pointer( b);
}

static unsigned int hash_string( const fts_atom_t *a)
{
  char *s = fts_get_string( a);
  unsigned int h = *s, i = 0;

  for ( s += 1; *s != '\0'; s++)
    h = (h<<1) + *s;

  return h;
}

static int equals_string( const fts_atom_t *a, const fts_atom_t *b)
{
  return strcmp( fts_get_string( a), fts_get_string( b)) == 0;
}

static unsigned int hash_symbol( const fts_atom_t *a)
{
  return (unsigned int)fts_get_symbol( a) >> 3;
}

static int equals_symbol( const fts_atom_t *a, const fts_atom_t *b)
{
  return fts_get_symbol( a) == fts_get_symbol( b);
}

static void set_key_type( fts_hashtable_t *h, int key_type)
{
  h->key_type = key_type;

  switch (h->key_type) {
  case FTS_HASHTABLE_INT:
    h->hash_function = hash_int;
    h->equals_function = equals_int;
    break;
  case FTS_HASHTABLE_PTR:
    h->hash_function = hash_ptr;
    h->equals_function = equals_ptr;
    break;
  case FTS_HASHTABLE_STRING:
    h->hash_function = hash_string;
    h->equals_function = equals_string;
    break;
  case 0:
  case FTS_HASHTABLE_SYMBOL:
    h->hash_function = hash_symbol;
    h->equals_function = equals_symbol;
    break;
  default:
    fprintf( stderr, "[hashtable] invalid key type (%d)\n", key_type);
    break;
  }
}

static int get_initial_capacity( int initial_capacity)
{
  switch (initial_capacity) {
  case FTS_HASHTABLE_SMALL:
    return 7;
  case FTS_HASHTABLE_MEDIUM:
    return 101;
  case FTS_HASHTABLE_BIG:
    return 1031;
  default:
    return 101;
  }
}

void fts_hashtable_init( fts_hashtable_t *h, int key_type, int initial_capacity)
{
  set_key_type( h, key_type);

  h->length = get_initial_capacity( initial_capacity);
  h->count = 0;
  h->rehash_count = (int)(h->length * FTS_HASHTABLE_STANDARD_LOAD_FACTOR);

  h->table = (fts_hashtable_cell_t **) fts_zalloc( h->length * sizeof( fts_hashtable_cell_t *));
}

void fts_hashtable_clear( fts_hashtable_t *h)
{
  unsigned int i;

  for ( i = 0; i < h->length; i++)
    {
      fts_hashtable_cell_t *c, *next;
      
      for( c = h->table[i]; c ; c = next)
	{
	  next = c->next;
	  fts_heap_free( c, cell_heap);
	}

      h->table[i] = 0;
    }

  h->count = 0;
}

void fts_hashtable_destroy( fts_hashtable_t *h)
{
  fts_hashtable_clear( h);
  fts_free( h->table);
}

static fts_hashtable_cell_t **lookup_cell( const fts_hashtable_t *h, const fts_atom_t *key)
{
  fts_hashtable_cell_t **c;

  c = &h->table[ (*h->hash_function)( key) % h->length];

  while (*c && !(*h->equals_function)( &(*c)->key, key))
    c = &(*c)->next;

  return c;
}

int fts_hashtable_get( const fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value)
{
  fts_hashtable_cell_t **c = lookup_cell( h, key);

  if (*c)
    {
      *value = (*c)->value;
      return 1;
    }

  return 0;
}

static const unsigned int primes_suite[] = {
  7,
  17,
  31,
  67,
  127,
  257,
  521,
  1031,
  2053,
  4099,
  8191,
  16411,
  32771,
  65537,
  131071,
  262147,
  524287,
  1048583,
  2097169,
  4194319,
  8388617,
  16777259,
  33554467,
};

static unsigned int new_length( unsigned int length)
{
  unsigned int i;

  for ( i = 0; i < sizeof (primes_suite) / sizeof (unsigned int); i++)
    if (length < primes_suite[i])
      return primes_suite[i];

  return primes_suite[i-1];
}

static void rehash( fts_hashtable_t *h)
{
  unsigned int old_length, i;
  fts_hashtable_cell_t **old_table;

  old_length = h->length;
  h->length = new_length( h->length);
  h->rehash_count = (int)(h->length * FTS_HASHTABLE_STANDARD_LOAD_FACTOR);

  old_table = h->table;
  h->table = (fts_hashtable_cell_t **) fts_zalloc( h->length * sizeof( fts_hashtable_cell_t *));

  for ( i = 0; i < old_length; i++)
    {
      fts_hashtable_cell_t *c, *next;

      for ( c = old_table[i]; c; c = next)
	{
	  int index = (*h->hash_function)( &c->key) % h->length;

	  next = c->next;
	  c->next = h->table[index];
	  h->table[index] = c;
	}
    }

  fts_free( old_table);
}

int fts_hashtable_put( fts_hashtable_t *h, const fts_atom_t *key, fts_atom_t *value)
{
  fts_hashtable_cell_t **c = lookup_cell( h, key);

  if (*c)
    {
      (*c)->value = *value;
      return 1;
    }

  *c = (fts_hashtable_cell_t *)fts_heap_alloc( cell_heap);
  (*c)->key = *key;
  (*c)->value = *value;
  (*c)->next = 0;

  h->count++;
  if ( h->count >= h->rehash_count)
    rehash( h);

  return 0;
}

int fts_hashtable_remove( fts_hashtable_t *h, const fts_atom_t *key)
{
  fts_hashtable_cell_t **c = lookup_cell( h, key);

  if (*c)
    {
      fts_hashtable_cell_t *to_delete = *c;

      *c = (*c)->next;
      fts_heap_free( to_delete, cell_heap);
      h->count--;
      
      return 1;
    }

  return 0;
}

void fts_hashtable_stats( fts_hashtable_t *h)
{
  unsigned int min_keys = 38928392, max_keys = 0, i;

  printf( "Hashtable: length = %d count = %d rehash = %d\n", h->length, h->count, h->rehash_count);

  for ( i = 0; i < h->length; i++)
    {
      unsigned int n_keys = 0;
      fts_hashtable_cell_t *c;

      for ( c = h->table[i]; c; c = c->next)
	n_keys++;

      if (n_keys < min_keys)
	min_keys = n_keys;
      if (n_keys > max_keys)
	max_keys = n_keys;

      printf( "[%d] ", n_keys);

      for ( c = h->table[i]; c; c = c->next)
	{
	  printf( " %s", fts_get_string( &c->key));
	}

      printf( "\n");
    }

  printf( "minimum %d maximum %d\n", min_keys, max_keys);
}

#ifdef DEBUG
void fts_hashtable_fprintf( fts_hashtable_t *h, FILE *f)
{
  int i;

  fprintf( f, "Hashtable: length = %d count = %d rehash = %d\n", h->length, h->count, h->rehash_count);

  for ( i = 0; i < h->length; i++)
    {
      fts_hashtable_cell_t *c;

      if (!h->table[i])
	continue;

      fprintf( f, "[%d] ", i);

      for ( c = h->table[i]; c; c = c->next)
	{
	  fprintf( f, "(");
	  fprintf_atoms( f, 1, &c->key);
	  fprintf( f, ",");
	  fprintf_atoms( f, 1, &c->value);
	  fprintf( f, ") ");
	}

      fprintf( f, "\n");
    }
}
#endif

/* **********************************************************************
 * Iterator
 */

static int hashtable_iterator_has_more( fts_iterator_t *iter)
{
  fts_hashtable_iterator_t *i = (fts_hashtable_iterator_t *) iter->data;

  if (i->cell)
    return 1;

  while ( i->index-- )
    {
      if ( i->table[i->index] )
	{
	  i->cell = i->table[i->index];
	  return 1;
	}
    }

  fts_heap_free( iter->data, iterator_heap);
  
  return 0;
}

static void hashtable_iterator_next( fts_iterator_t *iter, fts_atom_t *a)
{
  fts_hashtable_iterator_t *i = (fts_hashtable_iterator_t *) iter->data;

  if ( !i->cell)
    {
      while( i->index--)
	{
	  if (i->table[i->index])
	    {
	      i->cell = i->table[i->index];
	      break;
	    }
	}
    }

  if (!i->cell)
    return;

  *a = (i->keys) ? i->cell->key : i->cell->value;

  i->cell = i->cell->next;
}

static void hashtable_iterator_get( const fts_hashtable_t *h, fts_iterator_t *i, int keys)
{
  fts_hashtable_iterator_t *hiter;

  hiter = (fts_hashtable_iterator_t *)fts_heap_alloc( iterator_heap);

  hiter->table = h->table;
  hiter->keys = keys;
  hiter->cell = 0;
  hiter->index = h->length;

  i->has_more = hashtable_iterator_has_more;
  i->next = hashtable_iterator_next;
  i->data = hiter;
}

void fts_hashtable_get_keys( const fts_hashtable_t *h, fts_iterator_t *i)
{
  hashtable_iterator_get( h, i, 1);
}

void fts_hashtable_get_values( const fts_hashtable_t *h, fts_iterator_t *i)
{
  hashtable_iterator_get( h, i, 0);
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_hashtable_init( void)
{
  cell_heap = fts_heap_new(sizeof( fts_hashtable_cell_t));
  iterator_heap = fts_heap_new(sizeof( fts_hashtable_iterator_t));
}

