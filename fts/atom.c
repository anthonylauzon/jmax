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


#include <string.h>
#include <assert.h>

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/connection.h>

static fts_atom_t __fts_null;
const fts_atom_t *fts_null = &__fts_null;

int fts_atom_equals( const fts_atom_t *p1, const fts_atom_t *p2)
{
  if ( !fts_atom_same_type( p1, p2))
    return 0;

  if ( fts_is_void( p1))
    return fts_is_void( p2);
  if ( fts_is_int( p1))
    return fts_get_int( p1) == fts_get_int( p2);
  if ( fts_is_float( p1))
    return fts_get_float( p1) == fts_get_float( p2);
  if ( fts_is_symbol( p1))
    return fts_get_symbol( p1) == fts_get_symbol( p2);
  if ( fts_is_object( p1))
    return fts_get_object( p1) == fts_get_object( p2);
  if ( fts_is_pointer( p1))
    return fts_get_pointer( p1) == fts_get_pointer( p2);
  if ( fts_is_string( p1))
    return ! strcmp( fts_get_string( p1), fts_get_string( p2));

  return 0;
}

/***********************************************************************
 *
 * Initialization
 *
 */

/*
 * Note: here, we cheat a little bit by using symbols that are not obtained
 * by fts_new_symbol().
 * BUT: we are guaranteed that s == fts_new_symbol(s) the first time the symbol
 * is searched in the symbol table.
 * Thus it is possible to use directly the string before calling fts_new_symbol()
 * on it.
 */
static const char void_metaclass_name[] = "__PRIMITIVE_VOID";
static const char int_metaclass_name[] = "__PRIMITIVE_INT";
static const char double_metaclass_name[] = "__PRIMITIVE_DOUBLE";
static const char symbol_metaclass_name[] = "__PRIMITIVE_SYMBOL";
static const char pointer_metaclass_name[] = "__PRIMITIVE_POINTER";
static const char string_metaclass_name[] = "__PRIMITIVE_STRING";

static fts_metaclass_t void_metaclass = {
  void_metaclass_name,
  NULL,
  FTS_TYPEID_VOID,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_void = &void_metaclass;
static fts_metaclass_t int_metaclass = { 
  int_metaclass_name,
  NULL,
  FTS_TYPEID_INT,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_int = &int_metaclass;
static fts_metaclass_t double_metaclass = { 
  double_metaclass_name,
  NULL,
  FTS_TYPEID_DOUBLE,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_double = &double_metaclass;
static fts_metaclass_t symbol_metaclass = { 
  symbol_metaclass_name,
  NULL,
  FTS_TYPEID_SYMBOL,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_symbol = &symbol_metaclass;
static fts_metaclass_t pointer_metaclass = { 
  pointer_metaclass_name,
  NULL,
  FTS_TYPEID_POINTER,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_pointer = &pointer_metaclass;
static fts_metaclass_t string_metaclass = { 
  string_metaclass_name,
  NULL,
  FTS_TYPEID_STRING,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_string = &string_metaclass;

void fts_kernel_atom_init( void)
{
  assert( fts_new_symbol( void_metaclass_name) == void_metaclass_name);
  assert( fts_new_symbol( int_metaclass_name) == int_metaclass_name);
  assert( fts_new_symbol( double_metaclass_name) == double_metaclass_name);
  assert( fts_new_symbol( symbol_metaclass_name) == symbol_metaclass_name);
  assert( fts_new_symbol( pointer_metaclass_name) == pointer_metaclass_name);
  assert( fts_new_symbol( string_metaclass_name) == string_metaclass_name);

  fts_metaclass_set_selector( &int_metaclass, fts_s_int);
  fts_metaclass_set_selector( &double_metaclass, fts_s_double);
  fts_metaclass_set_selector( &symbol_metaclass, fts_s_symbol);
  fts_metaclass_set_selector( &pointer_metaclass, fts_s_pointer);
  fts_metaclass_set_selector( &string_metaclass, fts_s_string);

  fts_set_void( &__fts_null);
}

