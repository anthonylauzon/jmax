/*
   Symbols support; include the symbol table, and the predefinitions
   of some symbols needed by the lang subsystem (atoms, and messages).
*/

#include <string.h>

#include "sys.h"
#include "lang/mess.h"

static fts_symbol_t fts_new_builtin_symbol(const char *name);

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
fts_symbol_t fts_s_ninlets;
fts_symbol_t fts_s_noutlets;
fts_symbol_t fts_s_bang;
fts_symbol_t fts_s_list;
fts_symbol_t fts_s_set;
fts_symbol_t fts_s_print;
fts_symbol_t fts_s_clear;
fts_symbol_t fts_s_stop;
fts_symbol_t fts_s_start;
fts_symbol_t fts_s_open;
fts_symbol_t fts_s_close;
fts_symbol_t fts_s_load;
fts_symbol_t fts_s_read;
fts_symbol_t fts_s_write;
fts_symbol_t fts_s_comma;
fts_symbol_t fts_s_semi;
fts_symbol_t fts_s_anything;

/* Predefined symbol for properties */

fts_symbol_t fts_s_value;
fts_symbol_t fts_s_min_value;
fts_symbol_t fts_s_max_value;
fts_symbol_t fts_s_name;
fts_symbol_t fts_s_x;
fts_symbol_t fts_s_wx;
fts_symbol_t fts_s_y;
fts_symbol_t fts_s_wy;
fts_symbol_t fts_s_width;
fts_symbol_t fts_s_ww;
fts_symbol_t fts_s_height;
fts_symbol_t fts_s_wh;
fts_symbol_t fts_s_range;
fts_symbol_t fts_s_font;
fts_symbol_t fts_s_fontSize;
fts_symbol_t fts_s_autorouting;


static void 
fts_predefine_symbols(void)
{
  fts_s_void   = fts_new_builtin_symbol("void");
  fts_s_float  = fts_new_builtin_symbol("float");
  fts_s_int    = fts_new_builtin_symbol("int");
  fts_s_number = fts_new_builtin_symbol("number");
  fts_s_ptr    = fts_new_builtin_symbol("ptr");
  fts_s_string = fts_new_builtin_symbol("string");
  fts_s_symbol = fts_new_builtin_symbol("symbol");
  fts_s_object = fts_new_builtin_symbol("object");
  fts_s_true   = fts_new_builtin_symbol("true");
  fts_s_false  = fts_new_builtin_symbol("false");

  fts_s_init 	          = fts_new_builtin_symbol("$init");
  fts_s_delete	          = fts_new_builtin_symbol("$delete");
  fts_s_replace	          = fts_new_builtin_symbol("$replace");
  fts_s_ninlets           = fts_new_builtin_symbol("ins");
  fts_s_noutlets          = fts_new_builtin_symbol("outs");
  fts_s_bang              = fts_new_builtin_symbol("bang");
  fts_s_list              = fts_new_builtin_symbol("list");
  fts_s_set               = fts_new_builtin_symbol("set");
  fts_s_print             = fts_new_builtin_symbol("print");
  fts_s_clear             = fts_new_builtin_symbol("clear");
  fts_s_stop              = fts_new_builtin_symbol("stop");
  fts_s_start             = fts_new_builtin_symbol("start");
  fts_s_open              = fts_new_builtin_symbol("open");
  fts_s_close             = fts_new_builtin_symbol("close");
  fts_s_load              = fts_new_builtin_symbol("load");
  fts_s_read              = fts_new_builtin_symbol("read");
  fts_s_write             = fts_new_builtin_symbol("write");
  fts_s_anything          = fts_new_builtin_symbol("anything");
  fts_s_comma             = fts_new_builtin_symbol(",");
  fts_s_semi              = fts_new_builtin_symbol(";");

/* Predefined symbol for properties */

  fts_s_value             = fts_new_builtin_symbol("value");
  fts_s_max_value         = fts_new_builtin_symbol("maxValue");
  fts_s_min_value         = fts_new_builtin_symbol("minValue");
  fts_s_name              = fts_new_builtin_symbol("name");
  fts_s_x                 = fts_new_builtin_symbol("x");
  fts_s_wx                = fts_new_builtin_symbol("wx");
  fts_s_y                 = fts_new_builtin_symbol("y");
  fts_s_wy                = fts_new_builtin_symbol("wy");
  fts_s_width             = fts_new_builtin_symbol("w");
  fts_s_ww                = fts_new_builtin_symbol("ww");
  fts_s_height            = fts_new_builtin_symbol("h");
  fts_s_wh                = fts_new_builtin_symbol("wh");
  fts_s_range             = fts_new_builtin_symbol("range");
  fts_s_font              = fts_new_builtin_symbol("font");
  fts_s_fontSize          = fts_new_builtin_symbol("fs");
  fts_s_autorouting       = fts_new_symbol("autorouting");
}


/******************************************************************************/
/*                                                                            */
/*             SYMBOLS: utilities and handling                                */
/*                                                                            */
/******************************************************************************/

#define SYMTABSIZE 511		/* Initial Size of the symbol table  */

#define BUILTIN_SYMBOL_SIZE 32	/* symbols that have and associated index, used in the
				 * protocols and binary format (to reduce symbol table size)
				 */
fts_heap_t symbol_heap;

static struct _symbol_table
{
  /* the hash table */

  fts_symbol_t symbol_hash_table[SYMTABSIZE];

  fts_symbol_t builtin_symbols[BUILTIN_SYMBOL_SIZE];

  int builtin_counter;
} symbol_table;



void
fts_symbols_init(void)
{
  int i;

  for (i = 0; i < SYMTABSIZE; i++)
    symbol_table.symbol_hash_table[i] = 0;

  fts_heap_init(&symbol_heap, sizeof(struct fts_symbol_descr), 32);

  symbol_table.builtin_counter = 0;

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
  sp->index = -1;

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
  sp->index = -1;

  /* third, add the new symbol in the has table */

  sp->next_in_table = (struct fts_symbol_descr *) symbol_table.symbol_hash_table[hash];
  symbol_table.symbol_hash_table[hash] = sp;

  return sp;
}

/* USed here to define the predefined symbols */

static fts_symbol_t 
fts_new_builtin_symbol(const char *name)
{
  struct fts_symbol_descr *s;		/* use the structure, not the type, so it is not 'const' */

  s = (struct fts_symbol_descr *) fts_new_symbol(name);

  if (symbol_table.builtin_counter < BUILTIN_SYMBOL_SIZE)
    {
      s->index = symbol_table.builtin_counter;
      symbol_table.builtin_counter++;
      symbol_table.builtin_symbols[s->index] = s;
    }

  return (fts_symbol_t) s;
}


/* These function are reserved to those protocol/binary format modules */

fts_symbol_t fts_get_builtin_symbol(int idx)
{
  return symbol_table.builtin_symbols[idx];
}

int fts_is_builtin_symbol(fts_symbol_t s)
{
  return s->index != -1;
}


int fts_get_builtin_symbol_index(fts_symbol_t s)
{
  return s->index;
}


