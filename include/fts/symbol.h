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


/**
 * Symbols are canonical representations of C strings that can be 
 * compared for equality using == instead of strcmp() for better
 * efficiency. They are the equivalent of the JAVA "interned" strings.
 *
 * @defgroup symbols symbols
 */


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


/*
 * Compatibility
 */
FTS_API const char *__OLD_fts_symbol_name( const char *file, int line, fts_symbol_t symbol);
#define fts_symbol_name(symbol) __OLD_fts_symbol_name(__FILE__, __LINE__, (symbol))

/*
  Predefined symbols. See predefsymbols.h
*/
#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) FTS_API fts_symbol_t V;
#include <fts/predefsymbols.h>






