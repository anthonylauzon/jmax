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
 * Lookup a string and insert it in the symbol table if the symbol table
 * does not already contain a string that is equals (using strcmp()) to the
 * passed string.
 * 
 * @fn fts_symbol_t fts_new_symbol( const char *name)
 * @param name the string to lookup
 * @return the string that was retrieved or created. Its content will be
 * the same as the passed argument, but the pointer value is guaranteed
 * to be unique
 * @ingroup symbols
*/
FTS_API fts_symbol_t fts_new_symbol( const char *name);

/*
  Predefined symbols. See predefsymbols.h
*/
#undef PREDEF_SYMBOL
#define PREDEF_SYMBOL(V,S) FTS_API fts_symbol_t V;
#include <fts/predefsymbols.h>









