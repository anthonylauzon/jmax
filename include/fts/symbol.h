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

#ifndef _FTS_SYMBOL_H_
#define _FTS_SYMBOL_H_

/**
 * Symbols are canonical representations of C strings that can be 
 * compared for equality using == instead of strcmp() for better
 * efficiency. They are the equivalent of the JAVA "interned" strings.
 *
 * @defgroup symbols symbols
 */


struct fts_symbol_descr
{
  const char *name;		/* name */
  int cache_index;		/* index in the client cache if any */
  int operator;			/* index in the operator table, for the expression eval */
  struct fts_symbol_descr *next_in_table; /* next in hash table for fts_new_symbol */
};


/**
 * Lookup a symbol and create a new definition if symbol
 * was not found.
 * 
 * NOTE: this function must be used ONLY for constant strings
 * because it stores the string that is passed and does not 
 * copy it !
 * 
 * @fn fts_symbol_t fts_new_symbol( const char *name)
 * @param name a string that will be stored in the symbol definition.
 * This string must be a constant, as in fts_new_symbol( "foo")
 * @return the fts_symbol_t that was retrieved or created. This is
 * a pointer to an opaque structure
 * @ingroup symbols
*/
FTS_API fts_symbol_t fts_new_symbol( const char *name);

/**
 * Lookup a symbol and create a new definition if symbol
 * was not found.
 * 
 * NOTE: this function is the same as <TT>fts_new_symbol</TT>, except
 * that it copies the string that is passed to a memory area
 * allocated using fts_malloc. It can be safely used with non-constant 
 * strings.
 * 
 * @param name     a string that will be stored in the symbol definition
 * @return the fts_symbol_t that was retrieved or created. This is
 * a pointer to an opaque structure
 * @ingroup symbols
 */
FTS_API fts_symbol_t fts_new_symbol_copy( const char *name);


/**
   Get the symbol name from a symbol.
   @param symbol   the symbol
   @return the symbol name (i.e. the string that was passed
            to the fts_new_symbol() that created this symbol).
*/
#define fts_symbol_name(symbol)                 ((symbol)->name)

/*
  Predefined symbols. See predefsymbols.h
*/
#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) FTS_API fts_symbol_t V;
#include <fts/predefsymbols.h>

/* 
 * fts_type_t equivalent of symbols
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
/*#define fts_t_data fts_s_data*/
#define fts_t_list fts_s_list

#endif




