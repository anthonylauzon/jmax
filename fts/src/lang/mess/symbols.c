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
fts_symbol_t fts_s_atom_array;
fts_symbol_t fts_s_connection;
fts_symbol_t fts_s_true;
fts_symbol_t fts_s_false;
fts_symbol_t fts_s_data;


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
fts_symbol_t fts_s_size;
fts_symbol_t fts_s_error;
fts_symbol_t fts_s_error_description;
fts_symbol_t fts_s_state;

/* Symbols related to builtin classes */

fts_symbol_t fts_s_old_patcher;
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
fts_symbol_t fts_s_open_sqpar;
fts_symbol_t fts_s_closed_sqpar;
fts_symbol_t fts_s_open_cpar;
fts_symbol_t fts_s_closed_cpar;
fts_symbol_t fts_s_dot;
fts_symbol_t fts_s_remainder;
fts_symbol_t fts_s_shift_left;
fts_symbol_t fts_s_shift_right;
fts_symbol_t fts_s_bit_and;
fts_symbol_t fts_s_bit_or;
fts_symbol_t fts_s_bit_xor;
fts_symbol_t fts_s_bit_not;
fts_symbol_t fts_s_logical_and;
fts_symbol_t fts_s_logical_or;
fts_symbol_t fts_s_logical_not;
fts_symbol_t fts_s_equal;
fts_symbol_t fts_s_not_equal;
fts_symbol_t fts_s_greater;
fts_symbol_t fts_s_greater_equal;
fts_symbol_t fts_s_smaller;
fts_symbol_t fts_s_smaller_equal;
fts_symbol_t fts_s_conditional;
fts_symbol_t fts_s_column;
fts_symbol_t fts_s_assign;

/* Predefined variable names */

fts_symbol_t fts_s_args;

static void
fts_predefine_symbols(void)
{
  fts_s_void       = fts_new_symbol("void");
  fts_s_float      = fts_new_symbol("float");
  fts_s_int        = fts_new_symbol("int");
  fts_s_number     = fts_new_symbol("number");
  fts_s_ptr        = fts_new_symbol("ptr");
  fts_s_string     = fts_new_symbol("string");
  fts_s_symbol     = fts_new_symbol("symbol");
  fts_s_object     = fts_new_symbol("object");
  fts_s_atom_array = fts_new_symbol("atom_array");
  fts_s_connection = fts_new_symbol("connection");
  fts_s_true       = fts_new_symbol("true");
  fts_s_false      = fts_new_symbol("false");
  fts_s_init 	   = fts_new_symbol("$init");
  fts_s_delete	   = fts_new_symbol("$delete");
  fts_s_ninlets    = fts_new_symbol("ins");
  fts_s_noutlets   = fts_new_symbol("outs");
  fts_s_bang       = fts_new_symbol("bang");
  fts_s_list       = fts_new_symbol("list");
  fts_s_set        = fts_new_symbol("set");
  fts_s_append     = fts_new_symbol("append");
  fts_s_print      = fts_new_symbol("print");
  fts_s_clear      = fts_new_symbol("clear");
  fts_s_stop       = fts_new_symbol("stop");
  fts_s_start      = fts_new_symbol("start");
  fts_s_restore    = fts_new_symbol("restore");
  fts_s_open       = fts_new_symbol("open");
  fts_s_upload     = fts_new_symbol("upload");
  fts_s_close      = fts_new_symbol("close");
  fts_s_load       = fts_new_symbol("load");
  fts_s_read       = fts_new_symbol("read");
  fts_s_write      = fts_new_symbol("write");
  fts_s_save_bmax  = fts_new_symbol("save_bmax");
  fts_s_anything   = fts_new_symbol("anything");
  fts_s_comma      = fts_new_symbol(",");
  fts_s_quote      = fts_new_symbol("'");
  fts_s_dollar     = fts_new_symbol("$");
  fts_s_semi       = fts_new_symbol(";");
  fts_s_value      = fts_new_symbol("value");
  fts_s_max_value  = fts_new_symbol("maxValue");
  fts_s_min_value  = fts_new_symbol("minValue");
  fts_s_name       = fts_new_symbol("name");
  fts_s_x          = fts_new_symbol("x");
  fts_s_wx         = fts_new_symbol("wx");
  fts_s_y          = fts_new_symbol("y");
  fts_s_wy         = fts_new_symbol("wy");
  fts_s_width      = fts_new_symbol("w");
  fts_s_ww         = fts_new_symbol("ww");
  fts_s_height     = fts_new_symbol("h");
  fts_s_wh         = fts_new_symbol("wh");
  fts_s_range      = fts_new_symbol("range");
  fts_s_font       = fts_new_symbol("font");
  fts_s_fontSize   = fts_new_symbol("fs");
  fts_s_error      = fts_new_symbol("error");
  fts_s_error_description  = fts_new_symbol("errdesc");
  fts_s_size       = fts_new_symbol("size");
  fts_s_state      = fts_new_symbol("state");

  /* Symbols related to builtin classes */

  fts_s_old_patcher    = fts_new_symbol("patcher");
  fts_s_patcher    = fts_new_symbol("jpatcher");
  fts_s_inlet      = fts_new_symbol("inlet");
  fts_s_outlet     = fts_new_symbol("outlet");
  fts_s_qlist      = fts_new_symbol("qlist");
  fts_s_table      = fts_new_symbol("table");
  fts_s_explode    = fts_new_symbol("explode");

  /* Espression operators */

  fts_s_plus       = fts_new_symbol("+");
  fts_s_minus      = fts_new_symbol("-");
  fts_s_times      = fts_new_symbol("*");
  fts_s_div        = fts_new_symbol("/");
  fts_s_open_par   = fts_new_symbol("(");
  fts_s_closed_par = fts_new_symbol(")");
  fts_s_open_sqpar   = fts_new_symbol("[");
  fts_s_closed_sqpar = fts_new_symbol("]");
  fts_s_open_cpar   = fts_new_symbol("{");
  fts_s_closed_cpar = fts_new_symbol("}");
  fts_s_dot        = fts_new_symbol(".");
  fts_s_remainder  = fts_new_symbol("%");
  fts_s_shift_left = fts_new_symbol("<<");
  fts_s_shift_right = fts_new_symbol(">>");
  fts_s_bit_and    = fts_new_symbol("&");
  fts_s_bit_or     = fts_new_symbol("|");
  fts_s_bit_xor     = fts_new_symbol("^");
  fts_s_bit_not    = fts_new_symbol("~");
  fts_s_logical_and = fts_new_symbol("&&");
  fts_s_logical_or  = fts_new_symbol("||");
  fts_s_logical_not = fts_new_symbol("!");
  fts_s_equal       = fts_new_symbol("==");
  fts_s_not_equal   = fts_new_symbol("!=");
  fts_s_greater     = fts_new_symbol(">");
  fts_s_greater_equal = fts_new_symbol(">=");
  fts_s_smaller     = fts_new_symbol("<");
  fts_s_smaller_equal = fts_new_symbol("<=");
  fts_s_conditional = fts_new_symbol("?");
  fts_s_column        = fts_new_symbol(":");
  fts_s_assign      = fts_new_symbol("=");

  /* FTS Data  */

  fts_s_data        = fts_new_symbol("data");

  /* Predefined variable names */

  fts_s_args        = fts_new_symbol("args");
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
static fts_heap_t *symbol_heap;

static struct _symbol_table
{
  /* the hash table */

  fts_symbol_t symbol_hash_table[SYMTABSIZE];

  fts_symbol_t builtin_symbols[BUILTIN_SYMBOL_SIZE];
} symbol_table;


void
fts_symbols_init(void)
{
  int i;

  for (i = 0; i < SYMTABSIZE; i++)
    symbol_table.symbol_hash_table[i] = 0;

  symbol_heap = fts_heap_new(sizeof(struct fts_symbol_descr));

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

  sp = (struct fts_symbol_descr *) fts_heap_alloc(symbol_heap);
  sp->name = name;
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

  sp = (struct fts_symbol_descr *) fts_heap_alloc(symbol_heap);

  sp->name = s;
  sp->operator = -1;

  /* third, add the new symbol in the has table */

  sp->next_in_table = (struct fts_symbol_descr *) symbol_table.symbol_hash_table[hash];
  symbol_table.symbol_hash_table[hash] = sp;

  return sp;
}


