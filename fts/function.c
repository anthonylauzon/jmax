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

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/package.h>

fts_fun_t
fts_function_install(fts_symbol_t name, fts_fun_t fun)
{
  fts_status_t status;

  if ( (status = fts_package_add_function( fts_get_current_package(), fun, name)) != fts_ok)
    return NULL;

  return fun;
}

/* **********************************************************************
 * 
 * Some usefull functions
 *
 */
static void unique_function(int ac, const fts_atom_t *at)
{
  static int seed = 1;
  fts_atom_t ret[1];						

  fts_set_int( ret, seed++);
  
  fts_return( ret);
}

static void new_function(int ac, const fts_atom_t *at)
{
  fts_atom_t a;
  fts_object_t *obj = NULL;
  
  if(ac > 0)
  {
    fts_class_t *class = NULL;
    
    if(fts_is_symbol(at))
      class = fts_get_class_by_name(fts_get_symbol(at));
    else if(fts_is_a(at, fts_class_class))
      class = (fts_class_t *)fts_get_object(at);

    if(class != NULL)
      obj = fts_object_create(class, ac - 1, at + 1);
  }
  
  if(obj != NULL)
    fts_set_object(&a, obj);
  else
    fts_set_void(&a);
  
  fts_return(&a);
}


static void
declare_functions( void)
{
  fts_function_install( fts_new_symbol( "unique"), unique_function);
  fts_function_install( fts_new_symbol( "new"), new_function);
}

/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_function_init( void)
{
  declare_functions();
}

