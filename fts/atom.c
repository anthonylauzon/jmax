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

  switch( fts_class_get_typeid( fts_get_class( p1)) ) {
  case FTS_TYPEID_VOID:
    return fts_is_void( p2);
  case FTS_TYPEID_INT:
    return fts_get_int( p1) == fts_get_int( p2);
  case FTS_TYPEID_FLOAT:
    return fts_get_float( p1) == fts_get_float( p2);
  case FTS_TYPEID_SYMBOL:
    return fts_get_symbol( p1) == fts_get_symbol( p2);
  case FTS_TYPEID_POINTER:
    return fts_get_pointer( p1) == fts_get_pointer( p2);
  case FTS_TYPEID_STRING :
    return ! strcmp( fts_get_string( p1), fts_get_string( p2));
  default:
    return fts_get_object( p1) == fts_get_object( p2);
  }

  return 0;
}

int fts_atom_compare( const fts_atom_t *p1, const fts_atom_t *p2)
{
  if(fts_is_int(p1))
  {
    if(fts_is_int(p2))
      return fts_get_int( p1) == fts_get_int( p2);
    else if(fts_is_float(p2))
      return (double)fts_get_int( p1) == fts_get_float( p2);
  }
  else if(fts_is_float(p1) && fts_is_number(p2))
    return fts_get_float( p1) == fts_get_number_float( p2);
  else if (fts_atom_same_type( p1, p2))
  {
    if ( fts_is_symbol( p1))
      return fts_get_symbol( p1) == fts_get_symbol( p2);
    else if ( fts_is_object( p1))
    {
      fts_object_t *obj = fts_get_object( p1);

      if(obj == fts_get_object( p2))
        return 1;
      else
      {
        fts_class_t *class = fts_object_get_class(obj);
        fts_equals_function_t equals = fts_class_get_equals_function(class);

        if(fts_class_get_equals_function(class) != NULL)
          return (*equals)(p1, p2);
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

static fts_class_t void_class = { typeid : FTS_TYPEID_VOID};
fts_class_t *fts_void_class = &void_class;

static fts_class_t int_class = { typeid : FTS_TYPEID_INT};
fts_class_t *fts_int_class = &int_class;

static fts_class_t float_class = { typeid : FTS_TYPEID_FLOAT};
fts_class_t *fts_float_class = &float_class;

static fts_class_t symbol_class = { typeid : FTS_TYPEID_SYMBOL};
fts_class_t *fts_symbol_class = &symbol_class;

static fts_class_t pointer_class = { typeid : FTS_TYPEID_POINTER};
fts_class_t *fts_pointer_class = &pointer_class;

static fts_class_t string_class = { typeid : FTS_TYPEID_STRING};
fts_class_t *fts_string_class = &string_class;

void fts_kernel_atom_init( void)
{
/*   void_class.typeid = FTS_TYPEID_VOID; */
/*   int_class.typeid = FTS_TYPEID_INT; */
/*   float_class.typeid = FTS_TYPEID_FLOAT; */
/*   symbol_class.typeid = FTS_TYPEID_SYMBOL; */
/*   pointer_class.typeid = FTS_TYPEID_POINTER; */
/*   string_class.typeid = FTS_TYPEID_STRING; */

  fts_class_set_name( &int_class, fts_s_int);
  fts_class_set_name( &float_class, fts_s_float);
  fts_class_set_name( &symbol_class, fts_s_symbol);
  fts_class_set_name( &pointer_class, fts_s_pointer);
  fts_class_set_name( &string_class, fts_s_string);

  fts_set_void( &__fts_null);
}
