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

#ifndef _FTLUTILS_H_
#define _FTLUTILS_H_

typedef struct ftl_memory_declaration {
  int size;
  void *address;
} ftl_memory_declaration;

extern void ftl_memory_declaration_init( ftl_memory_declaration *decl, int size);
extern ftl_memory_declaration *ftl_memory_declaration_new( int size );


typedef struct ftl_function_declaration {
  ftl_wrapper_t wrapper;
} ftl_function_declaration;

extern void ftl_function_declaration_init( ftl_function_declaration *decl, ftl_wrapper_t wrapper);
extern ftl_function_declaration *ftl_function_declaration_new( ftl_wrapper_t wrapper);
extern void ftl_function_declaration_free( ftl_function_declaration *decl);
#endif
