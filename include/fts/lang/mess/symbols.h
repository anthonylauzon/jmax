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

#ifndef _FTS_SYMBOLS_H_
#define _FTS_SYMBOLS_H_

#include <stdio.h>

/**
   Symbols are canonical representations of C strings that can be 
   compared for equality using == instead of strcmp() for better
   efficiency, i.e.:
   if a and b are 2 C strings such as strcmp( a, b) == 0,
   then it is guaranted that fts_new_symbol( a) == fts_new_symbol( b)
 */

/**
   Lookup a symbol and create a new definition if symbol
   was not found.
   
   NOTE: this function must be used ONLY for constant strings
   because it stores the string that is passed and does not 
   copy it !

   [in] name     a string that will be stored in the symbol definition.
                 This string must be a constant, as in
		 fts_new_symbol( "foo")
   [return] the fts_symbol_t that was retrieved or created. This is
            a pointer to an opaque structure
*/
extern fts_symbol_t fts_new_symbol( const char *name);

/**
   Lookup a symbol and create a new definition if symbol
   was not found.
   
   NOTE: this function is the same as fts_new_symbol, except
   that it copies the string that is passed to a memory area
   allocated using fts_malloc. It can be safely used with non-constant 
   strings.

   [in] name     a string that will be stored in the symbol definition
   [return] the fts_symbol_t that was retrieved or created. This is
            a pointer to an opaque structure
*/
extern fts_symbol_t fts_new_symbol_copy( const char *name);


/**
   Get the symbol name from a symbol.
   [in] symbol   the symbol
   [return] the symbol name (i.e. the string that was passed
            to the fts_new_symbol() that created this symbol).
*/
#define fts_symbol_name(symbol)                 ((symbol)->name)


/**
   Debug utility function.
   Prints a full description of a symbol on a FILE *.
   [in] symbol the symbol
*/
extern void fprintf_symbol( FILE *file, fts_symbol_t symbol);

/*
   Symbol cache index access.
   These macros are used by the client protocol handling code.
*/
/**
   Get the symbol's cache index.
*/
#define fts_symbol_get_cache_index(sym)      ((sym)->cache_index)
/**
   Set the symbol's cache index
*/
#define fts_symbol_set_cache_index(sym, index) (((struct fts_symbol_descr *) sym)->cache_index = (index))


/*
 * non documented 
 */
#define fts_symbol_get_operator(sym)         ((sym)->operator)
#define fts_symbol_set_operator(sym, op)     (((struct fts_symbol_descr *) sym)->operator = (op))
#define fts_symbol_is_operator(sym)          ((sym)->operator != (-1))


/*
  Predefined symbols. See predefsymbols.h
*/
#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) extern fts_symbol_t V;
#include <fts/lang/mess/predefsymbols.h>

/* (fd)
   See mess_types.h for comment on fts_type_t
*/
#define fts_t_anything fts_s_anything
#define fts_t_void fts_s_void
#define fts_t_float fts_s_float
#define fts_t_int fts_s_int
#define fts_t_number fts_s_number
#define fts_t_ptr fts_s_ptr
#define fts_t_fun fts_s_fun
#define fts_t_string fts_s_string
#define fts_t_symbol fts_s_symbol
#define fts_t_object fts_s_object
#define fts_t_connection fts_s_connection
#define fts_t_true fts_s_true
#define fts_t_false fts_s_false
#define fts_t_data fts_s_data
#define fts_t_list fts_s_list

#endif




