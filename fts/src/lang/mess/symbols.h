#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

/* Symbols related functions and macros*/

extern fts_symbol_t fts_new_symbol(const char *name);
extern fts_symbol_t fts_new_symbol_copy(const char *name);

/* Get the symbol name from a symbol;
   note that users should never access the internal structure
   of the symbol, but they should use this macros

extern const char *fts_symbol_name(fts_symbol_t sym)

 */

#define fts_symbol_name(sym)      ((sym)->name)

/* Predefined symbols used in the lang module */


/* Tags for types  in Atoms */

extern fts_symbol_t fts_s_void;
extern fts_symbol_t fts_s_float;
extern fts_symbol_t fts_s_int;
extern fts_symbol_t fts_s_number;
extern fts_symbol_t fts_s_ptr;
extern fts_symbol_t fts_s_string;
extern fts_symbol_t fts_s_symbol;
extern fts_symbol_t fts_s_object;
extern fts_symbol_t fts_s_true;
extern fts_symbol_t fts_s_false;

/* Symbols related to messages, objects and classes */

extern fts_symbol_t fts_s_init;
extern fts_symbol_t fts_s_delete;
extern fts_symbol_t fts_s_replace;
extern fts_symbol_t fts_s_connect_inlet;
extern fts_symbol_t fts_s_connect_outlet;
extern fts_symbol_t fts_s_disconnect_inlet;
extern fts_symbol_t fts_s_disconnect_outlet;
extern fts_symbol_t fts_s_send_update;
extern fts_symbol_t fts_s_value;
extern fts_symbol_t fts_s_ninlets;
extern fts_symbol_t fts_s_noutlets;
extern fts_symbol_t fts_s_bang;
extern fts_symbol_t fts_s_list;
extern fts_symbol_t fts_s_set;
extern fts_symbol_t fts_s_anything;

#endif
