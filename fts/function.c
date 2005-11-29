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

static fts_status_description_t invalid_object_creation = {
  "invalid object creation in function new"
};
static fts_status_t invalid_object_creation_status = &invalid_object_creation;

static fts_status_description_t undefined_class = {
  "undefined class in function new"
};
static fts_status_t undefined_class_status = &undefined_class;

static fts_status_description_t class_required = {
  "class required in function new"
};
static fts_status_t class_required_status = &class_required;

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
static fts_status_t
unique_function(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  static int seed = 1;
  
  fts_set_int(ret, seed++);
  
  return fts_ok;
}

static fts_status_t
new_function(int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_class_t *cl = NULL;
  fts_object_t *obj = NULL;
  
  if(ac == 0)
    return class_required_status;
  
  if(fts_is_symbol(at))
    cl = fts_get_class_by_name(fts_get_symbol(at));
  else if(fts_is_a(at, fts_class_class))
    cl = (fts_class_t *)fts_get_object(at);
  else
    return class_required_status;
  
  if(cl != NULL)
    obj = fts_object_create(cl, ac - 1, at + 1);
  else if (fts_is_symbol(at))
    return fts_status_format("undefined class %s in function new", fts_symbol_name(fts_get_symbol(at)));
  else
    return undefined_class_status;
  
  if(obj == NULL)
  {
    fts_symbol_t clname = fts_class_get_name(cl);
    
    fts_set_void(ret);
    
    if(clname != NULL)
      return fts_status_format("invalid arguments for %s object in function new", fts_symbol_name(clname));
    else
      return invalid_object_creation_status;
  }
  
  fts_set_object(ret, obj);
  
  return fts_ok;
}

static void
declare_functions( void)
{
  fts_function_install( fts_new_symbol( "unique"), unique_function);
  fts_function_install( fts_new_symbol( "new"), new_function);
}

/***********************************************************************
*
* Kernel initialization
*
*/

FTS_MODULE_INIT(function)
{
  declare_functions();
}
