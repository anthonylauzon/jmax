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

#ifndef _FTS_PRIVATE_SYMBOL_H_
#define _FTS_PRIVATE_SYMBOL_H_

/*
   Symbol cache index access.
   These macros are used by the client protocol handling code.
*/
/*
   Get the symbol's cache index.
*/
#define fts_symbol_get_cache_index(sym)      ((sym)->cache_index)
/*
   Set the symbol's cache index
*/
#define fts_symbol_set_cache_index(sym, index) (((struct fts_symbol_descr *) sym)->cache_index = (index))

/*
 * non documented 
 */
#define fts_symbol_get_operator(sym)         ((sym)->operator)
#define fts_symbol_set_operator(sym, op)     (((struct fts_symbol_descr *) sym)->operator = (op))
#define fts_symbol_is_operator(sym)          ((sym)->operator != (-1))

#endif




