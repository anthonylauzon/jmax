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

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/connection.h>

fts_metaclass_t *fts_t_void;
fts_metaclass_t *fts_t_int;
fts_metaclass_t *fts_t_float;
fts_metaclass_t *fts_t_symbol;
fts_metaclass_t *fts_t_pointer;
fts_metaclass_t *fts_t_string;
/* To be removed */
fts_metaclass_t *fts_t_connection;

const fts_atom_t fts_null[] = {FTS_NULL};

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

  /* To be removed */
  if ( fts_is_connection( p1))
    return fts_get_connection( p1) == fts_get_connection( p2);

  return 0;
}

/***********************************************************************
 *
 * Initialization
 *
 */
#define make_primitive_typeid(M) ((fts_metaclass_t *)UINT_TO_POINTER( POINTER_TO_UINT(M) | 1))

void fts_kernel_atom_init( void)
{
  fts_metaclass_t *mcl;

  mcl = fts_metaclass_install( fts_new_symbol( "__PRIMITIVE_VOID"), 0, 0);
  fts_t_void = make_primitive_typeid( mcl);

  mcl = fts_metaclass_install( fts_new_symbol( "__PRIMITIVE_INT"), 0, 0);
  fts_metaclass_set_selector( mcl, fts_s_int);
  fts_t_int = make_primitive_typeid( mcl);

  mcl = fts_metaclass_install( fts_new_symbol( "__PRIMITIVE_FLOAT"), 0, 0);
  fts_metaclass_set_selector( mcl, fts_s_float);
  fts_t_float = make_primitive_typeid( mcl);

  mcl = fts_metaclass_install( fts_new_symbol( "__PRIMITIVE_SYMBOL"), 0, 0);
  fts_metaclass_set_selector( mcl, fts_s_symbol);
  fts_t_symbol = make_primitive_typeid( mcl);

  mcl = fts_metaclass_install( fts_new_symbol( "__PRIMITIVE_POINTER"), 0, 0);
  fts_metaclass_set_selector( mcl, fts_s_pointer);
  fts_t_pointer = make_primitive_typeid( mcl);

  mcl = fts_metaclass_install( fts_new_symbol( "__PRIMITIVE_STRING"), 0, 0);
  fts_metaclass_set_selector( mcl, fts_s_string);
  fts_t_string = make_primitive_typeid( mcl);

  mcl = fts_metaclass_install( fts_new_symbol( "__PRIMITIVE_CONNECTION"), 0, 0);
  fts_metaclass_set_selector( mcl, fts_s_connection);
  fts_t_connection = make_primitive_typeid( mcl);
}

