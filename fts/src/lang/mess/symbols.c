/*
   Symbols support; include the symbol table, and the predefinitions
   of some symbols needed by the lang subsystem (atoms, and messages).
*/

#include <string.h>

#include "sys.h"
#include "lang/mess.h"

static fts_symbol_t fts_new_builtin_symbol(const char *name, int code);

/* Predefined ATOM TYPE symbols */

fts_symbol_t fts_s_void;
fts_symbol_t fts_s_float;
fts_symbol_t fts_s_int;
fts_symbol_t fts_s_number;
fts_symbol_t fts_s_ptr;
fts_symbol_t fts_s_string;
fts_symbol_t fts_s_symbol;
fts_symbol_t fts_s_object;
fts_symbol_t fts_s_connection;
fts_symbol_t fts_s_true;
fts_symbol_t fts_s_false;

/* Predefined symbols for the mess system */

fts_symbol_t fts_s_init;
fts_symbol_t fts_s_delete;
fts_symbol_t fts_s_ninlets;
fts_symbol_t fts_s_noutlets;
fts_symbol_t fts_s_bang;
fts_symbol_t fts_s_list;
fts_symbol_t fts_s_set;
fts_symbol_t fts_s_append;
fts_symbol_t fts_s_print;
fts_symbol_t fts_s_clear;
fts_symbol_t fts_s_stop;
fts_symbol_t fts_s_restore;
fts_symbol_t fts_s_start;
fts_symbol_t fts_s_open;
fts_symbol_t fts_s_upload;
fts_symbol_t fts_s_close;
fts_symbol_t fts_s_load;
fts_symbol_t fts_s_read;
fts_symbol_t fts_s_write;
fts_symbol_t fts_s_save_bmax;
fts_symbol_t fts_s_comma;
fts_symbol_t fts_s_quote;
fts_symbol_t fts_s_dollar;
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
fts_symbol_t fts_s_error;

/* Symbols related to builtin classes */

fts_symbol_t fts_s_patcher;
fts_symbol_t fts_s_inlet;
fts_symbol_t fts_s_outlet;
fts_symbol_t fts_s_qlist;
fts_symbol_t fts_s_table;
fts_symbol_t fts_s_explode;

/* Expression operators */

fts_symbol_t fts_s_plus;
fts_symbol_t fts_s_minus;
fts_symbol_t fts_s_times;
fts_symbol_t fts_s_div;
fts_symbol_t fts_s_open_par;
fts_symbol_t fts_s_closed_par;

static void
fts_predefine_symbols(void)
{
  /* In ANY case, do not change the association symbol value in the following;
     just add new pairs, in any order, but do not change existing values;
     this code are part of the binary format, and of the client protocol; you
     would simply screw up compatibility with existing files;
     You cannot also delete symbols, just add them.
     */

  fts_s_void       = fts_new_builtin_symbol("void",   0);
  fts_s_float      = fts_new_builtin_symbol("float",  1);
  fts_s_int        = fts_new_builtin_symbol("int",    2);
  fts_s_number     = fts_new_builtin_symbol("number", 3);
  fts_s_ptr        = fts_new_builtin_symbol("ptr",    4);
  fts_s_string     = fts_new_builtin_symbol("string", 5);
  fts_s_symbol     = fts_new_builtin_symbol("symbol", 6);
  fts_s_object     = fts_new_builtin_symbol("object", 7);
  fts_s_connection = fts_new_builtin_symbol("connection", 8);
  fts_s_true       = fts_new_builtin_symbol("true",   9);
  fts_s_false      = fts_new_builtin_symbol("false",  10);

  fts_s_init 	   = fts_new_builtin_symbol("$init",   11);
  fts_s_delete	   = fts_new_builtin_symbol("$delete", 12);
  fts_s_ninlets    = fts_new_builtin_symbol("ins",     13);
  fts_s_noutlets   = fts_new_builtin_symbol("outs",    14);
  fts_s_bang       = fts_new_builtin_symbol("bang",    15);
  fts_s_list       = fts_new_builtin_symbol("list",    16);
  fts_s_set        = fts_new_builtin_symbol("set",     17);
  fts_s_append     = fts_new_builtin_symbol("append",  18);
  fts_s_print      = fts_new_builtin_symbol("print",   19);
  fts_s_clear      = fts_new_builtin_symbol("clear",   20);
  fts_s_stop       = fts_new_builtin_symbol("stop",    21);
  fts_s_start      = fts_new_builtin_symbol("start",   22);
  fts_s_restore    = fts_new_builtin_symbol("restore", 23);
  fts_s_open       = fts_new_builtin_symbol("open",    24);
  fts_s_upload     = fts_new_builtin_symbol("upload",  57);
  fts_s_close      = fts_new_builtin_symbol("close",   25);
  fts_s_load       = fts_new_builtin_symbol("load",    26);
  fts_s_read       = fts_new_builtin_symbol("read",    27);
  fts_s_write      = fts_new_builtin_symbol("write",   28);
  fts_s_save_bmax  = fts_new_builtin_symbol("save_bmax", 29);
  fts_s_anything   = fts_new_builtin_symbol("anything", 30);
  fts_s_comma      = fts_new_builtin_symbol(",",       31);
  fts_s_quote      = fts_new_builtin_symbol("'",       32);
  fts_s_dollar     = fts_new_builtin_symbol("$",       33);
  fts_s_semi       = fts_new_builtin_symbol(";",       34);

  /* Predefined symbol for properties */

  fts_s_value      = fts_new_builtin_symbol("value",    35);
  fts_s_max_value  = fts_new_builtin_symbol("maxValue", 36);
  fts_s_min_value  = fts_new_builtin_symbol("minValue", 37);
  fts_s_name       = fts_new_builtin_symbol("name",     38);
  fts_s_x          = fts_new_builtin_symbol("x",        39);
  fts_s_wx         = fts_new_builtin_symbol("wx",       40);
  fts_s_y          = fts_new_builtin_symbol("y",        41);
  fts_s_wy         = fts_new_builtin_symbol("wy",       42);
  fts_s_width      = fts_new_builtin_symbol("w",        43);
  fts_s_ww         = fts_new_builtin_symbol("ww",       44);
  fts_s_height     = fts_new_builtin_symbol("h",        45);
  fts_s_wh         = fts_new_builtin_symbol("wh",       46);
  fts_s_range      = fts_new_builtin_symbol("range",    47);
  fts_s_font       = fts_new_builtin_symbol("font",     48);
  fts_s_fontSize   = fts_new_builtin_symbol("fs",       49);
  fts_s_error      = fts_new_builtin_symbol("error",    56);

  /* Symbols related to builtin classes */

  fts_s_patcher    = fts_new_builtin_symbol("patcher", 50);
  fts_s_inlet      = fts_new_builtin_symbol("inlet",   51);
  fts_s_outlet     = fts_new_builtin_symbol("outlet",  52);
  fts_s_qlist      = fts_new_builtin_symbol("qlist",   53);
  fts_s_table      = fts_new_builtin_symbol("table",   54);
  fts_s_explode    = fts_new_builtin_symbol("explode", 55);

  /* Espression operators */

  fts_s_plus       = fts_new_builtin_symbol("+", 58);
  fts_s_minus      = fts_new_builtin_symbol("-", 59);
  fts_s_times      = fts_new_builtin_symbol("*", 60);
  fts_s_div        = fts_new_builtin_symbol("/", 61);
  fts_s_open_par   = fts_new_builtin_symbol("(", 62);
  fts_s_closed_par = fts_new_builtin_symbol(")", 63);

  /* Last number user: 64 (max, 255 predefined symbols !! ) */
}


/******************************************************************************/
/*                                                                            */
/*             SYMBOLS: utilities and handling                                */
/*                                                                            */
/******************************************************************************/

#define SYMTABSIZE 511		/* Initial Size of the symbol table  */

#define BUILTIN_SYMBOL_SIZE 256	/* symbols that have and associated index, used in the
				 * protocols and binary format (to reduce symbol table size)
				 * The system support 256 symbols like this.
				 */
fts_heap_t symbol_heap;

static struct _symbol_table
{
  /* the hash table */

  fts_symbol_t symbol_hash_table[SYMTABSIZE];

  fts_symbol_t builtin_symbols[BUILTIN_SYMBOL_SIZE];
} symbol_table;



static void
fts_print_symbol_table(const char *msg, const char *name)
{
  int i;

  fprintf(stderr, "Symbol table %s adding %s\n", msg, name);

  for (i = 0; i < SYMTABSIZE; i++)
    {
      struct fts_symbol_descr *sp;		/* use the structure, not the type, so it is not 'const' */

      sp = (struct fts_symbol_descr *) symbol_table.symbol_hash_table[i];

      for (; sp; sp = sp->next_in_table)
	fprintf(stderr, "-\t%s\n", sp->name);
    }
}

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
    {
      if (! strcmp(name, sp->name))
	return sp;
    }

  /* Second, the symbol do not exist already: 
     and make a new one */

  sp = (struct fts_symbol_descr *) fts_heap_alloc(&symbol_heap);
  sp->name = name;
  sp->index = -1;
  sp->operator = -1;

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
    {
      if (! strcmp(name, sp->name))
	return sp;
    }

  /* Second, the symbol do not exist already: copy
     the string, and make a symbol one */

  s = fts_malloc(strlen(name)+1);
  strcpy(s, name);

  sp = (struct fts_symbol_descr *) fts_heap_alloc(&symbol_heap);

  sp->name = s;
  sp->index = -1;
  sp->operator = -1;

  /* third, add the new symbol in the has table */

  sp->next_in_table = (struct fts_symbol_descr *) symbol_table.symbol_hash_table[hash];
  symbol_table.symbol_hash_table[hash] = sp;

  return sp;
}

/* USed here to define the predefined symbols */

static fts_symbol_t 
fts_new_builtin_symbol(const char *name, int code)
{
  struct fts_symbol_descr *s;		/* use the structure, not the type, so it is not 'const' */

  s = (struct fts_symbol_descr *) fts_new_symbol(name);

  if (code < BUILTIN_SYMBOL_SIZE)
    {
      s->index = code;
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


