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
#include <ftsprivate/connection.h>

const fts_atom_t fts_null[] = {FTS_NULL};

void fprintf_atoms( FILE *file, int ac, const fts_atom_t *at)
{
  int i;

  for ( i = 0; i < ac; i++, at++)
    {
      if ( fts_is_void( at))
	fprintf( file,"<void>");
      else if ( fts_is_int( at))
	fprintf( file,"%d", fts_get_int( at));
      else if ( fts_is_float( at))
	fprintf( file,"%f", fts_get_float( at));
      else if ( fts_is_symbol( at))
	fprintf( file,"%s", fts_get_symbol( at));
      else if ( fts_is_object( at))
	fprintf_object( file, fts_get_object( at));
      else if ( fts_is_pointer( at) )
	fprintf( file,"%p", fts_get_pointer( at));
      else if ( fts_is_string( at))
	fprintf( file,"%s", fts_get_string( at));
      /* To be removed */
      else if ( fts_is_connection( at))
	fprintf_connection( file, fts_get_connection( at));
      else
	fprintf( file, "<UNKNOWN TYPE>%x", fts_get_int( at));

      if ( i != ac-1)
	fprintf( file, " ");
    }
}

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

void fts_kernel_atom_init( void)
{
}

