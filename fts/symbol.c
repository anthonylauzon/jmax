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

#include <fts/fts.h>

static fts_hashtable_t symbol_table;

fts_symbol_t fts_new_symbol( const char *s)
{
  fts_atom_t k, v;
  const char *p;

  /* Lookup symbol in the hash table */
  fts_set_string( &k, (char *)s);
  if ( fts_hashtable_get( &symbol_table, &k, &v))
    {
      return (fts_symbol_t)fts_get_pointer( &v);
    }

  /* Symbol do not exist: copy the string */
  p = strcpy( fts_malloc( strlen(s)+1 ), s);

  /* Add the new symbol in the hash table */
  fts_set_string( &k, (char *)p);
  fts_set_pointer( &v, (char *)p);
  fts_hashtable_put( &symbol_table, &k, &v);

  return (fts_symbol_t)p;
}

/***********************************************************************
 *
 * Predefined symbols
 *
 */

#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) fts_symbol_t V;
#include <fts/predefsymbols.h>

static void fts_predefine_symbols(void)
{
#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) V = fts_new_symbol(S);
#include <fts/predefsymbols.h>
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_symbol_init(void)
{
  fts_hashtable_init( &symbol_table, FTS_HASHTABLE_BIG);

  fts_predefine_symbols();
}
