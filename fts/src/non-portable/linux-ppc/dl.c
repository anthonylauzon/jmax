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
 * Author: Francois Dechelle.
 *
 */

/*
 * This file contains Linux platform implementation of the standard interface 
 * for dynamic linking.
 *
 * The Linux implementation uses dlopen() and dlsym().
 *
 */

#include <string.h>
#include <dlfcn.h>

#include "sys/hw.h"

/*
  Function: fts_dl_open
  Description:
   loads a dynamic library
  Arguments: 
   filename: the name of the library. This file will be searched using
   platform specific search algorithm.
   error: a string in which a description of the error will be stored
   in case of error.
  Returns: a "handle" to be used for further invocation of fts_dl_lookup
  0 if an error occured.
*/
void *fts_dl_open( const char *filename, char *error)
{
  void *handle;

  handle = dlopen( filename, RTLD_NOW | RTLD_GLOBAL);

  if (!handle)
    strcpy( error, dlerror());

  return handle;
}


/*
  Function: fts_dl_lookup
  Description:
   lookup the specified symbol in symbol table, returning the address
   of this symbol
  Arguments: 
   handle: a handle returned by fts_dl_open
   symbol: the symbol name
   address: a pointer to a pointer: the address of the symbol
   will be stored here upon return, if the function does not
   return an error.
   error: a string in which a description of the error will be stored
   in case of error.
  Returns: 1 if symbol is found, 0 if not
*/
int fts_dl_lookup( void *handle, const char *symbol, void **address, char *error)
{
  void *p;
  char *dlerr;

  p = dlsym( handle, symbol);

  dlerr = dlerror();
  if (dlerr != NULL)
    {
      strcpy( error, dlerr);
      return 0;
    }

  *address = p;
  
  return 1;
}
