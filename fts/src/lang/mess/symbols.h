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

#define fts_symbol_name(sym)                 ((sym)->name)

#define fts_symbol_get_operator(sym)         ((sym)->operator)
#define fts_symbol_set_operator(sym, op)     (((struct fts_symbol_descr *) sym)->operator = (op))
#define fts_symbol_is_operator(sym)          ((sym)->operator != (-1))

#define fts_symbol_get_cache_index(sym)      ((sym)->cache_index)
#define fts_symbol_set_cache_index(sym, idx) (((struct fts_symbol_descr *) sym)->cache_index = (idx))

#define fts_symbol_is_cached(sym)            ((sym)->cache_index > 0)
#define fts_symbol_is_reserve_cached(sym)    ((sym)->cache_index < 0)
#define fts_symbol_cache(sym)                (((struct fts_symbol_descr *) sym)->cache_index = ((-1) * ((sym)->cache_index)))

extern void fts_symbol_reserve_cache(fts_symbol_t s);


/* Predefined symbols used in the lang module */


/* Tags for types  in Atoms */

extern fts_symbol_t fts_s_void;
extern fts_symbol_t fts_s_float;
extern fts_symbol_t fts_s_int;
extern fts_symbol_t fts_s_number;
extern fts_symbol_t fts_s_ptr;
extern fts_symbol_t fts_s_fun;
extern fts_symbol_t fts_s_string;
extern fts_symbol_t fts_s_symbol;
extern fts_symbol_t fts_s_object;
extern fts_symbol_t fts_s_connection;
extern fts_symbol_t fts_s_true;
extern fts_symbol_t fts_s_false;
extern fts_symbol_t fts_s_data;
extern fts_symbol_t fts_s_atom_array;

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
extern fts_symbol_t fts_s_upload;
extern fts_symbol_t fts_s_close;
extern fts_symbol_t fts_s_load;
extern fts_symbol_t fts_s_read;
extern fts_symbol_t fts_s_write;
extern fts_symbol_t fts_s_save_bmax;
extern fts_symbol_t fts_s_comma;
extern fts_symbol_t fts_s_quote;
extern fts_symbol_t fts_s_dollar;
extern fts_symbol_t fts_s_semi;
extern fts_symbol_t fts_s_find;
extern fts_symbol_t fts_s_find_errors;
extern fts_symbol_t fts_s_find_friends;
extern fts_symbol_t fts_s_anything;
extern fts_symbol_t fts_s_send_properties;
extern fts_symbol_t fts_s_send_ui_properties;

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
extern fts_symbol_t fts_s_size;
extern fts_symbol_t fts_s_error;
extern fts_symbol_t fts_s_error_description;
extern fts_symbol_t fts_s_state;
extern fts_symbol_t fts_s_abstraction;
extern fts_symbol_t fts_s_template;
extern fts_symbol_t fts_s_patcher_type;

/* Symbols related to builtin classes */

extern fts_symbol_t fts_s_old_patcher;
extern fts_symbol_t fts_s_patcher;
extern fts_symbol_t fts_s_inlet;
extern fts_symbol_t fts_s_outlet;
extern fts_symbol_t fts_s_qlist;
extern fts_symbol_t fts_s_table;
extern fts_symbol_t fts_s_explode;

/* Operator symbols (also $ is an operator) */

extern fts_symbol_t fts_s_plus;
extern fts_symbol_t fts_s_minus;
extern fts_symbol_t fts_s_times;
extern fts_symbol_t fts_s_div;
extern fts_symbol_t fts_s_open_par;
extern fts_symbol_t fts_s_closed_par;
extern fts_symbol_t fts_s_open_sqpar;
extern fts_symbol_t fts_s_closed_sqpar;
extern fts_symbol_t fts_s_open_cpar;
extern fts_symbol_t fts_s_closed_cpar;
extern fts_symbol_t fts_s_dot;
extern fts_symbol_t fts_s_percent;
extern fts_symbol_t fts_s_shift_left;
extern fts_symbol_t fts_s_shift_right;
extern fts_symbol_t fts_s_bit_and;
extern fts_symbol_t fts_s_bit_or;
extern fts_symbol_t fts_s_bit_xor;
extern fts_symbol_t fts_s_bit_not;
extern fts_symbol_t fts_s_logical_and;
extern fts_symbol_t fts_s_logical_or;
extern fts_symbol_t fts_s_logical_not;
extern fts_symbol_t fts_s_equal_equal;
extern fts_symbol_t fts_s_not_equal;
extern fts_symbol_t fts_s_greater;
extern fts_symbol_t fts_s_greater_equal;
extern fts_symbol_t fts_s_smaller;
extern fts_symbol_t fts_s_smaller_equal;
extern fts_symbol_t fts_s_conditional;
extern fts_symbol_t fts_s_column;
extern fts_symbol_t fts_s_equal;

/* Predefined variable names */

extern fts_symbol_t fts_s_args;

/* Kernel Parameter names */ 

extern fts_symbol_t fts_s_sampling_rate;
extern fts_symbol_t fts_s_fifo_size;

#endif




