#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

/* Symbols related functions and macros*/

extern fts_symbol_t fts_new_symbol(const char *name);
extern fts_symbol_t fts_new_symbol_copy(const char *name);

extern fts_symbol_t fts_get_builtin_symbol(int idx);
extern int fts_is_builtin_symbol(fts_symbol_t s);
extern int fts_get_builtin_symbol_index(fts_symbol_t s);

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
extern fts_symbol_t fts_s_send_update;
extern fts_symbol_t fts_s_ninlets;
extern fts_symbol_t fts_s_noutlets;
extern fts_symbol_t fts_s_bang;
extern fts_symbol_t fts_s_list;
extern fts_symbol_t fts_s_set;
extern fts_symbol_t fts_s_append;
extern fts_symbol_t fts_s_print;
extern fts_symbol_t fts_s_clear;
extern fts_symbol_t fts_s_stop;
extern fts_symbol_t fts_s_start;
extern fts_symbol_t fts_s_restore;
extern fts_symbol_t fts_s_open;
extern fts_symbol_t fts_s_close;
extern fts_symbol_t fts_s_load;
extern fts_symbol_t fts_s_read;
extern fts_symbol_t fts_s_write;
extern fts_symbol_t fts_s_save_bmax;
extern fts_symbol_t fts_s_comma;
extern fts_symbol_t fts_s_quote;
extern fts_symbol_t fts_s_dollar;
extern fts_symbol_t fts_s_semi;
extern fts_symbol_t fts_s_anything;

/* Symbols related to object properties */

extern fts_symbol_t fts_s_value;
extern fts_symbol_t fts_s_min_value;
extern fts_symbol_t fts_s_max_value;
extern fts_symbol_t fts_s_name;
extern fts_symbol_t fts_s_x;
extern fts_symbol_t fts_s_wx;
extern fts_symbol_t fts_s_y;
extern fts_symbol_t fts_s_wy;
extern fts_symbol_t fts_s_width;
extern fts_symbol_t fts_s_ww;
extern fts_symbol_t fts_s_height;
extern fts_symbol_t fts_s_wh;
extern fts_symbol_t fts_s_range;
extern fts_symbol_t fts_s_font;
extern fts_symbol_t fts_s_fontSize;

/* Symbols related to builtin classes */

extern fts_symbol_t fts_s_patcher;
extern fts_symbol_t fts_s_inlet;
extern fts_symbol_t fts_s_outlet;
extern fts_symbol_t fts_s_qlist;
extern fts_symbol_t fts_s_table;
extern fts_symbol_t fts_s_explode;

#endif




