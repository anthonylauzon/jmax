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


/*
   Symbols support; include the symbol table, and the predefinitions
   of some symbols needed by the lang subsystem (atoms, and messages).
*/

#include <string.h>

#include <fts/sys.h>
#include <fts/lang/mess.h>
#include "messP.h"

#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) fts_symbol_t V;
#include <fts/lang/mess/predefsymbols.h>

static void fts_predefine_symbols(void)
{

#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) V = fts_new_symbol(S);
#include <fts/lang/mess/predefsymbols.h>

}


/******************************************************************************/
/*                                                                            */
/*             SYMBOLS: utilities and handling                                */
/*                                                                            */
/******************************************************************************/

#define SYMTABSIZE 511		/* Initial Size of the symbol table  */

static fts_heap_t *symbol_heap;

static fts_symbol_t symbol_hash_table[SYMTABSIZE];

void fts_symbols_init(void)
{
  int i;

  for (i = 0; i < SYMTABSIZE; i++)
    symbol_hash_table[i] = 0;

  symbol_heap = fts_heap_new(sizeof(struct fts_symbol_descr));

  fts_predefine_symbols();
}

static unsigned int symbol_hash(const char *s)
{
  unsigned int hval = 0xffffffff;

  while (*s)
    {
      hval *= *(s++);		/* should use a better hash function */
      hval >>= 4;
    }

  return (hval % SYMTABSIZE);
}

static fts_symbol_t symbol_new_aux( const char *name, int copy)
{
  char *s;
  struct fts_symbol_descr *sp;		/* use the structure, not the type, so it is not 'const' */
  unsigned int hash;

  /* precompute the hash value  */

  hash = symbol_hash(name);

  /* First Look for the symbol in the hash table */

  sp = (struct fts_symbol_descr *) symbol_hash_table[hash];

  for (; sp; sp = sp->next_in_table)
    {
      if (! strcmp(name, sp->name))
	return sp;
    }

  /* Second, the symbol do not exist already: copy
     the string if needed, and make a symbol one */

  sp = (struct fts_symbol_descr *) fts_heap_alloc(symbol_heap);

  sp->name = (copy) ? strcpy( fts_malloc( strlen(name)+1 ), name) : name;
  sp->operator = -1;
  sp->cache_index = -1;

  /* third, add the new symbol in the hash table */

  sp->next_in_table = (struct fts_symbol_descr *) symbol_hash_table[hash];
  symbol_hash_table[hash] = sp;

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

void fprintf_symbol( FILE *file, fts_symbol_t symbol)
{
  struct fts_symbol_descr *sp = (struct fts_symbol_descr *)symbol;

  fprintf( file, "<SYMBOL> %s [%d] ", sp->name, sp->cache_index);
}
