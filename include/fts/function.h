/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/**
 * Functions
 *
 * FTS functions can be used in expressions (object instanciation, messages)
 * They are defined using <code>fts_function_install</code> and are installed
 * in a package.
 *
 * @ingroup function
 */

/**
 * Install a function
 *
 * @fn fts_fun_t fts_function_install(fts_symbol_t name, fts_fun_t fun)
 * @param name the name
 * @param fun the function
 * @return the function or NULL if installation failed
 * @ingroup function
 */
FTS_API fts_fun_t fts_function_install(fts_symbol_t name, fts_fun_t fun);

