/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"

void
ftl_memory_declaration_init( ftl_memory_declaration *decl, int size)
{
  decl->size = size;
  decl->address = 0;
}

ftl_memory_declaration *
ftl_memory_declaration_new( int size )
{
  ftl_memory_declaration *decl;

  decl = (ftl_memory_declaration *) fts_malloc(sizeof( ftl_memory_declaration));
  if (decl)
    ftl_memory_declaration_init( decl, size);
  return decl;
}



void
ftl_function_declaration_init( ftl_function_declaration *decl, ftl_wrapper_t wrapper)
{
  decl->wrapper = wrapper;
}

ftl_function_declaration *
ftl_function_declaration_new( ftl_wrapper_t wrapper)
{
  ftl_function_declaration *decl;

  decl = (ftl_function_declaration *) fts_malloc(sizeof( ftl_function_declaration));
  if (decl)
    ftl_function_declaration_init( decl, wrapper);
  return decl;
}



void
ftl_function_declaration_free( ftl_function_declaration *decl)
{
  fts_free(decl);
}


