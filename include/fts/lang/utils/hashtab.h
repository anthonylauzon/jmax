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

#ifndef _FTS_HASHTAB_H_
#define _FTS_HASHTAB_H_

typedef struct _hash_cell_t hash_cell_t;

typedef struct _hash_table_t {
  int length;
  int cells_cnt;
  hash_cell_t **table;
} fts_hash_table_t;

FTS_API void fts_hash_table_init( fts_hash_table_t *ht);

FTS_API fts_hash_table_t *fts_hash_table_new( void);

FTS_API void fts_hash_table_destroy( fts_hash_table_t *ht);

FTS_API void fts_hash_table_free( fts_hash_table_t *ht);

/*
  Lookup entry `sym' in hash_table `ht'.
  If found, copy value in `data'.
  Returns:
   0 if entry is not present
   1 if entry is present
*/
FTS_API int fts_hash_table_lookup( const fts_hash_table_t *ht, fts_symbol_t sym, fts_atom_t *data);

/*
  Insert entry `sym' with value `data' in hash_table `ht'.
  Returns:
   0 if entry is already present or insertion failed
   1 if entry is inserted
*/
FTS_API int fts_hash_table_insert( fts_hash_table_t *ht, fts_symbol_t sym, fts_atom_t *data);

FTS_API void fts_hash_table_apply( const fts_hash_table_t *ht, void (*fun)( fts_symbol_t sym, fts_atom_t *data, void *user_data), void *user_data);

FTS_API int fts_hash_table_remove( fts_hash_table_t *ht, fts_symbol_t sym);

FTS_API int fts_hash_table_get_count( const fts_hash_table_t *ht);

typedef struct _fts_hash_table_iterator_t {
  const fts_hash_table_t *ht;
  hash_cell_t *cell;
  int i;
} fts_hash_table_iterator_t;

FTS_API void fts_hash_table_iterator_init( fts_hash_table_iterator_t *iter, const fts_hash_table_t *ht);
FTS_API fts_hash_table_iterator_t *fts_hash_table_iterator_new( const fts_hash_table_t *ht);
FTS_API void fts_hash_table_iterator_next( fts_hash_table_iterator_t *iter);
FTS_API int fts_hash_table_iterator_end( const fts_hash_table_iterator_t *iter);
FTS_API fts_symbol_t fts_hash_table_iterator_current_symbol( const fts_hash_table_iterator_t *iter);
FTS_API fts_atom_t *fts_hash_table_iterator_current_data( const fts_hash_table_iterator_t *iter);

#endif



