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
 * Symbols:
 *  - symbol table
 *  - predefinitions of some symbols
 */

#include <fts/ftsnew.h>

/* **********************************************************************
 *
 * Symbol table
 *
 */

static fts_hashtable_t symbol_table;
static fts_heap_t *symbol_heap;

static fts_symbol_t symbol_new_aux( const char *name, int copy)
{
  struct fts_symbol_descr *sp;
  fts_atom_t k, v;

  /* Lookup symbol in the hash table */
  fts_set_string( &k, name);
  if ( fts_hashtable_get( &symbol_table, &k, &v))
    return (fts_symbol_t)fts_get_ptr( &v);

  /* Symbol do not exist: copy the string if needed, and make a new symbol descriptor */
  sp = (struct fts_symbol_descr *) fts_heap_alloc(symbol_heap);
  sp->name = (copy) ? strcpy( fts_malloc( strlen(name)+1 ), name) : name;
  sp->operator = -1;
  sp->cache_index = -1;

  /* Add the new symbol descriptor in the hash table */
  fts_set_ptr( &v, sp);
  fts_hashtable_put( &symbol_table, &k, &v);

  return sp;
}

fts_symbol_t fts_new_symbol(const char *name)
{
  return symbol_new_aux( name, 0);
}

fts_symbol_t fts_new_symbol_copy(const char *name)
{
  return symbol_new_aux( name, 1);
}


/* **********************************************************************
 *
 * Predefined symbols
 *
 */

#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) fts_symbol_t V;
#include <fts/lang/mess/predefsymbols.h>

static void fts_predefine_symbols(void)
{
#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) V = fts_new_symbol(S);
#include <fts/lang/mess/predefsymbols.h>
}


/* **********************************************************************
 *
 * Initialization
 *
 */

void fts_symbols_init(void)
{
  symbol_heap = fts_heap_new( sizeof(struct fts_symbol_descr));

  fts_hashtable_init( &symbol_table, fts_t_string, FTS_HASHTABLE_BIG);

  fts_predefine_symbols();
}
