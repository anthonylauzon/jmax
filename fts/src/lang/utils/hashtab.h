/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _HASHTAB_H_
#define _HASHTAB_H_

typedef struct _hash_cell_t hash_cell_t;

typedef struct _hash_table_t {
  int length;
  int cells_cnt;
  hash_cell_t **table;
} fts_hash_table_t;

extern void fts_hash_table_init( fts_hash_table_t *ht);

extern fts_hash_table_t *fts_hash_table_new( void);

extern void fts_hash_table_destroy( fts_hash_table_t *ht);

extern void fts_hash_table_free( fts_hash_table_t *ht);

/*
  Lookup entry `sym' in hash_table `ht'.
  If found, copy value in `data'.
  Returns:
   0 if entry is not present
   1 if entry is present
*/
extern int fts_hash_table_lookup( const fts_hash_table_t *ht, fts_symbol_t sym, fts_atom_t *data);

/*
  Insert entry `sym' with value `data' in hash_table `ht'.
  Returns:
   0 if entry is already present or insertion failed
   1 if entry is inserted
*/
extern int fts_hash_table_insert( fts_hash_table_t *ht, fts_symbol_t sym, fts_atom_t *data);

extern void fts_hash_table_apply( const fts_hash_table_t *ht, void (*fun)( fts_symbol_t sym, fts_atom_t *data, void *user_data), void *user_data);

extern int fts_hash_table_remove( fts_hash_table_t *ht, fts_symbol_t sym);

extern int fts_hash_table_get_count( const fts_hash_table_t *ht);

typedef struct _fts_hash_table_iterator_t {
  const fts_hash_table_t *ht;
  hash_cell_t *cell;
  int i;
} fts_hash_table_iterator_t;

extern void fts_hash_table_iterator_init( fts_hash_table_iterator_t *iter, const fts_hash_table_t *ht);
extern fts_hash_table_iterator_t *fts_hash_table_iterator_new( const fts_hash_table_t *ht);
extern void fts_hash_table_iterator_next( fts_hash_table_iterator_t *iter);
extern int fts_hash_table_iterator_end( const fts_hash_table_iterator_t *iter);
extern fts_symbol_t fts_hash_table_iterator_current_symbol( const fts_hash_table_iterator_t *iter);
extern fts_atom_t *fts_hash_table_iterator_current_data( const fts_hash_table_iterator_t *iter);

#endif



