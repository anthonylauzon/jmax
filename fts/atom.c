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
#include <ftsprivate/message.h>

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

int fts_atom_compare( const fts_atom_t *p1, const fts_atom_t *p2)
{
  if (fts_atom_same_type( p1, p2))
    {
      if (fts_is_number( p1))
	return fts_get_number_float( p1) == fts_get_number_float( p2);
      else if ( fts_is_symbol( p1))
	return fts_get_symbol( p1) == fts_get_symbol( p2);
      else if ( fts_is_object( p1))
	{
	  fts_object_t *obj = fts_get_object( p1);

	  if(obj == fts_get_object( p2))
	    return 1;
	  else
	    {
	      fts_class_t *class = fts_object_get_class(obj);
	      fts_method_t meth_compare = fts_class_get_method(class, fts_s_compare);
	      
	      meth_compare(obj, 0, 0, 1, p2);
	      return(fts_get_int(fts_get_return_value()));
	    }
	}
    }

  return 0;
}

/***********************************************************************
 *
 * Initialization
 *
 */

#define FTS_TYPEID_VOID     1
#define FTS_TYPEID_INT      2
#define FTS_TYPEID_FLOAT    3
#define FTS_TYPEID_SYMBOL   4
#define FTS_TYPEID_POINTER  5
#define FTS_TYPEID_STRING   6

static fts_metaclass_t void_metaclass = {
  NULL,
  NULL,
  FTS_TYPEID_VOID,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_void = &void_metaclass;

static fts_metaclass_t int_metaclass = { 
  NULL,
  NULL,
  FTS_TYPEID_INT,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_int = &int_metaclass;

static fts_metaclass_t float_metaclass = { 
  NULL,
  NULL,
  FTS_TYPEID_FLOAT,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_float = &float_metaclass;

static fts_metaclass_t symbol_metaclass = { 
  NULL,
  NULL,
  FTS_TYPEID_SYMBOL,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_symbol = &symbol_metaclass;

static fts_metaclass_t pointer_metaclass = { 
  NULL,
  NULL,
  FTS_TYPEID_POINTER,
  NULL,
  NULL,
  NULL,
  NULL
}; 
fts_metaclass_t *fts_t_pointer = &pointer_metaclass;

static fts_metaclass_t string_metaclass = { 
  NULL,
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
  fts_metaclass_set_selector( &int_metaclass, fts_s_int);
  fts_metaclass_set_selector( &float_metaclass, fts_s_float);
  fts_metaclass_set_selector( &symbol_metaclass, fts_s_symbol);
  fts_metaclass_set_selector( &pointer_metaclass, fts_s_pointer);
  fts_metaclass_set_selector( &string_metaclass, fts_s_string);

  fts_set_void( &__fts_null);
}

