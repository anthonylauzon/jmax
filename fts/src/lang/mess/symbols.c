/*
   Symbols support; include the symbol table, and the predefinitions
   of some symbols needed by the lang subsystem (atoms, and messages).
*/

#include <string.h>

#include "sys.h"
#include "lang/mess.h"


/* Predefined ATOM TYPE symbols */

fts_symbol_t fts_s_void;
fts_symbol_t fts_s_float;
fts_symbol_t fts_s_int;
fts_symbol_t fts_s_number;
fts_symbol_t fts_s_ptr;
fts_symbol_t fts_s_string;
fts_symbol_t fts_s_symbol;
fts_symbol_t fts_s_object;
fts_symbol_t fts_s_true;
fts_symbol_t fts_s_false;

/* Predefined symbols for the mess system */

fts_symbol_t fts_s_init;
fts_symbol_t fts_s_delete;
fts_symbol_t fts_s_replace;
fts_symbol_t fts_s_value;
fts_symbol_t fts_s_name;
fts_symbol_t fts_s_ninlets;
fts_symbol_t fts_s_noutlets;
fts_symbol_t fts_s_bang;
fts_symbol_t fts_s_list;
fts_symbol_t fts_s_set;
fts_symbol_t fts_s_anything;


static void 
fts_predefine_symbols(void)
{
  fts_s_void   = fts_new_symbol("void");
  fts_s_float  = fts_new_symbol("float");
  fts_s_int    = fts_new_symbol("int");
  fts_s_number = fts_new_symbol("number");
  fts_s_ptr    = fts_new_symbol("ptr");
  fts_s_string = fts_new_symbol("string");
  fts_s_symbol = fts_new_symbol("symbol");
  fts_s_object = fts_new_symbol("object");
  fts_s_true   = fts_new_symbol("true");
  fts_s_false  = fts_new_symbol("false");

  fts_s_init 	          = fts_new_symbol("$init");
  fts_s_delete	          = fts_new_symbol("$delete");
  fts_s_replace	          = fts_new_symbol("$replace");
  fts_s_value             = fts_new_symbol("value");
  fts_s_name              = fts_new_symbol("name");
  fts_s_ninlets           = fts_new_symbol("ins");
  fts_s_noutlets          = fts_new_symbol("outs");
  fts_s_bang              = fts_new_symbol("bang");
  fts_s_list              = fts_new_symbol("list");
  fts_s_set               = fts_new_symbol("set");
  fts_s_anything          = fts_new_symbol("anything");
}


/******************************************************************************/
/*                                                                            */
/*             SYMBOLS: utilities and handling                                */
/*                                                                            */
/******************************************************************************/

#define SYMTABSIZE 511		/* Initial Size of the symbol table  */

fts_heap_t symbol_heap;

static struct _symbol_table
{
  /* the hash table */

  fts_symbol_t symbol_hash_table[SYMTABSIZE];
} symbol_table;


void
fts_symbols_init(void)
{
  int i;

  for (i = 0; i < SYMTABSIZE; i++)
    symbol_table.symbol_hash_table[i] = 0;

  fts_heap_init(&symbol_heap, sizeof(struct fts_symbol_descr), 32);

  fts_predefine_symbols();
}


/******************* symbols *******************/


static unsigned int
symbol_hash(const char *s)
{
  unsigned int hval = 0xffffffff;

  while (*s)
    {
      hval *= *(s++);		/* should use a better hash function */
      hval >>= 4;
    }

  return (hval % SYMTABSIZE);
}

/* 
   Pubblic  functions.

   fts_new_symbol: intern a string, and generate the corresponding symbol.
       It assume the string passed as argument is a constant and persistent string.

   fts_new_symbol_copy: as the previous one, but copy the string in a malloc area.
*/

fts_symbol_t 
fts_new_symbol(const char *name)
{
  struct fts_symbol_descr *sp;		/* use the structure, not the type, so it is not 'const' */
  unsigned int hash;

  /* precompute the hash value  */

  hash = symbol_hash(name);

  /* First Look for the symbol in the hash table */

  sp = (struct fts_symbol_descr *) symbol_table.symbol_hash_table[hash];

  for (; sp; sp = sp->next_in_table)
    if (! strcmp(name, sp->name))
      return sp;

  /* Second, the symbol do not exist already: 
     and make a new one */

  sp = (struct fts_symbol_descr *) fts_heap_alloc(&symbol_heap);
  sp->name = name;

  /* third, add the new symbol in the hash table */

  sp->next_in_table = (struct fts_symbol_descr *) symbol_table.symbol_hash_table[hash];
  symbol_table.symbol_hash_table[hash] = sp;

  return sp;
}


fts_symbol_t 
fts_new_symbol_copy(const char *name)
{
  char *s;
  struct fts_symbol_descr *sp;		/* use the structure, not the type, so it is not 'const' */
  unsigned int hash;

  /* precompute the hash value  */

  hash = symbol_hash(name);

  /* First Look for the symbol in the hash table */

  sp = (struct fts_symbol_descr *) symbol_table.symbol_hash_table[hash];

  for (; sp; sp = sp->next_in_table)
    if (! strcmp(name, sp->name))
      return sp;

  /* Second, the symbol do not exist already: copy
     the string, and make a symbol one */

  s = fts_malloc(strlen(name)+1);
  strcpy(s, name);

  sp = (struct fts_symbol_descr *) fts_heap_alloc(&symbol_heap);

  sp->name = s;

  /* third, add the new symbol in the has table */

  sp->next_in_table = (struct fts_symbol_descr *) symbol_table.symbol_hash_table[hash];
  symbol_table.symbol_hash_table[hash] = sp;

  return sp;
}





