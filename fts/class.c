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


/* This file implement the class structure */

#include <string.h>

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/object.h>
#include <ftsprivate/package.h>
#include <ftsprivate/patcher.h>

const int fts_system_inlet = -1;
static int typeid = FTS_FIRST_OBJECT_TYPEID;

#define CLASS_INLET_MAX 255
#define CLASS_TYPEID_MAX ((1 << 23) - 1)

enum fts_connection_type;

/* Return Status declarations */
fts_status_description_t fts_DuplicatedClass = {"Duplicated class"};
fts_status_description_t fts_ClassAlreadyInitialized = {"class already initialized"};
fts_status_description_t fts_InletOutOfRange = {"inlet out of range"};
fts_status_description_t fts_OutletOutOfRange = {"outlet out of range"};
fts_status_description_t fts_CannotInstantiate = {"Cannot instantiate class"};


/***********************************************************************
 *
 * Class Handling
 *
 */

fts_class_t *
fts_class_install(fts_symbol_t name, fts_instantiate_fun_t instantiate_fun)
{
  fts_class_t *cl;

  cl = fts_zalloc(sizeof(fts_class_t));

  cl->name = name;
  cl->instantiate_fun = instantiate_fun;
  cl->typeid = typeid++;

  if(name != NULL)
    {
      if(fts_package_add_class(fts_get_current_package(), cl, name) != fts_ok)
	return 0;
    }

  return cl;
}

/* for now just recreate the same class and add it to the current package */
void
fts_class_alias(fts_class_t *cl, fts_symbol_t alias)
{
  fts_package_add_class(fts_get_current_package(), cl, alias);
}


static fts_class_t *get_class( fts_symbol_t package_name, fts_symbol_t class_name)
{
  fts_package_t *pkg;
  fts_class_t *cl;

  pkg = fts_package_get( package_name);
  if (pkg == NULL)
    return NULL;

  return fts_package_get_class( pkg, class_name);
}

fts_class_t *
fts_class_get_by_name( fts_symbol_t package_name, fts_symbol_t class_name)
{
  fts_package_t *pkg;
  fts_class_t *cl;
  fts_iterator_t iter;

  if (package_name != NULL)
    return get_class( package_name, class_name);

  /* ask the kernel package before any other package. The kernel
     classes should not be redefined anyway. If we search the kernel
     package before the required packages, we avoid the loading of all
     (required) packages to find the patcher class.  */
  pkg = fts_get_system_package();

  cl = fts_package_get_class(pkg, class_name);
  if (cl != NULL)
    return cl;

  /* ask the current package */
  pkg = fts_get_current_package();
  cl = fts_package_get_class(pkg, class_name);
  if (cl != NULL)
    return cl;

  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &iter);

  while ( fts_iterator_has_more( &iter)) 
    {
      fts_atom_t a;

      fts_iterator_next( &iter, &a);

      cl = get_class( fts_get_symbol( &a), class_name);
      if (cl != NULL)
	return cl;
  }

  return NULL;
}

/********************************************
 *
 * inlet/outlet utils
 *
 */
#define CLASS_INOUT_BLOCK_SIZE 4

static void
class_set_inlets_number(fts_class_t *cl, int n)
{
  cl->ninlets = n;
}

static void
class_set_outlets_number(fts_class_t *cl, int n)
{
  int n_alloc = cl->out_alloc;
  int i;

  if(n > n_alloc)
    {
      while(n_alloc < n)
	n_alloc += CLASS_INOUT_BLOCK_SIZE;

      cl->outlets = fts_realloc(cl->outlets, n_alloc * sizeof(fts_class_outlet_t));

      /* init new outlets */
      for(i=cl->out_alloc; i<n_alloc; i++)
	fts_list_init(cl->outlets[i].declarations);

      cl->out_alloc = n_alloc;  
    }

  /* delete outlets cut off */
  for(i=n; i<cl->noutlets; i++)
    fts_list_delete(cl->outlets[i].declarations);

  cl->noutlets = n;
}

static int
class_inlet_key(int winlet, fts_class_t *type)
{
  if(type == NULL)
    return winlet;
  else
    return (fts_class_get_typeid(type) << 8) + winlet;
}

static fts_class_outlet_t *
class_get_outlet(fts_class_t *cl, int woutlet)
{
  if(cl->outlets != NULL)
    {
      if(woutlet < 0)
	woutlet = 0;
      else if(woutlet >= cl->noutlets)
	woutlet = cl->noutlets - 1;

      return cl->outlets + woutlet;
    }
  else
    return NULL;
}

static void
class_outlet_add_declaration(fts_class_outlet_t *out, const fts_atom_t *a)
{
  out->declarations = fts_list_append(out->declarations, a);
}

int 
class_outlet_get_declarations(fts_class_outlet_t *out, fts_iterator_t *iter)
{
  int n = fts_list_get_size(out->declarations);

  if(n > 0)
    fts_list_get_values(out->declarations, iter);
  
  return n;
}

int
class_outlet_has_declaration(fts_class_outlet_t *out, const fts_atom_t *p)
{
  fts_iterator_t iter;

  if(class_outlet_get_declarations(out, &iter) > 0)
    {
      while(fts_iterator_has_more(&iter)) 
	{
	  fts_atom_t a;

	  fts_iterator_next(&iter, &a);

	  if(fts_atom_equals(p, &a))
	    return 1;
	}
    }
  
  return 0;
}

void
fts_class_default_error_handler(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(s == 0)
    {
      if(ac > 0)
	fts_object_signal_runtime_error(o, "no tuple method at inlet %d", s, winlet);
      else
	fts_object_signal_runtime_error(o, "no method for %s at inlet %d", fts_get_class_name(at), winlet);
    }
  else
    fts_object_signal_runtime_error(o, "don't understand %s", s);    
}

/********************************************
 *
 *  class
 *
 */

static void
dummy_method( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

void
fts_class_init( fts_class_t *cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor)
{
  cl->size = size;
  cl->heap = fts_heap_new(size);

  cl->constructor = (constructor != NULL) ? constructor: dummy_method;
  cl->deconstructor = (deconstructor != NULL) ? deconstructor: dummy_method;

  cl->messages= fts_hashtable_new( FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);
  cl->default_handler = fts_class_default_error_handler;

  cl->ninlets = 0;
  cl->inlets = fts_hashtable_new( FTS_HASHTABLE_INT, FTS_HASHTABLE_MEDIUM);

  cl->noutlets = 0;
  cl->out_alloc = 0;
  cl->outlets = NULL;
}

/********************************************
 *
 * inlet/outlet definitions
 *
 */
void
fts_class_message_varargs(fts_class_t *cl, fts_symbol_t s, fts_method_t mth)
{
  if(s != NULL)
    {    
      fts_atom_t k, a;
      
      fts_set_symbol(&k, s);

      if(fts_hashtable_get( cl->messages, &k, &a))
	post("message \"%s\" doubly defined for class %s\n", s, fts_class_get_name(cl));
      else
	{
	  fts_set_pointer(&a, mth);
	  fts_hashtable_put( cl->messages, &k, &a);
	}
    }
  else
    post("method definition with NULL selector for class %s\n", fts_class_get_name(cl)); 
}

void
fts_class_inlet(fts_class_t *cl, int winlet, fts_class_t *type, fts_method_t mth)
{
  fts_atom_t k, a;
  
  if(winlet < 0)
    winlet = 0;
  else if(winlet > CLASS_INLET_MAX)
    winlet = CLASS_INLET_MAX;
  
  fts_set_int(&k, class_inlet_key(winlet, type));
  
  if (fts_hashtable_get( cl->inlets, &k, &a))
    post("%s method doubly defined for class %s at inlet %d\n", type->name, fts_class_get_name(cl), winlet);
  else
    {
      /* register inlet method */
      fts_set_pointer(&a, mth);
      fts_hashtable_put( cl->inlets, &k, &a);
      
      if(winlet >= cl->ninlets)
	class_set_inlets_number(cl, winlet + 1);
    }
}

void
fts_class_inlet_anything(fts_class_t *cl, int winlet)
{
  if(winlet < 0)
    winlet = 0;
  else if(winlet > CLASS_INLET_MAX)
    winlet = CLASS_INLET_MAX;
  
  if(winlet >= cl->ninlets)
    class_set_inlets_number(cl, winlet + 1);
}

void
fts_class_outlet(fts_class_t *cl, int woutlet, fts_class_t *class)
{
  fts_class_outlet_t *out;
  fts_atom_t a;

  if(woutlet >= cl->noutlets)
    class_set_outlets_number(cl, woutlet + 1);
    
  out = class_get_outlet(cl, woutlet);

  fts_set_pointer(&a, class);
  class_outlet_add_declaration(out, &a);
}

void
fts_class_outlet_message(fts_class_t *cl, int woutlet, fts_symbol_t selector)
{
  fts_class_outlet_t *out;
  fts_atom_t a;

  if(woutlet >= cl->noutlets)
    class_set_outlets_number(cl, woutlet + 1);
    
  out = class_get_outlet(cl, woutlet);

  fts_set_symbol(&a, selector);
  class_outlet_add_declaration(out, &a);
}

void
fts_class_outlet_anything(fts_class_t *cl, int woutlet)
{
  if(woutlet >= cl->noutlets)
    class_set_outlets_number(cl, woutlet + 1);
}

/********************************************
 *
 *  request inlet/outlet methods and definitions
 *
 */
fts_method_t 
fts_class_get_method(fts_class_t *cl, fts_symbol_t s)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, s);
  
  if (fts_hashtable_get( cl->messages, &k, &a))
    return (fts_method_t)fts_get_pointer(&a);
  else
    return NULL;	
}

fts_method_t
fts_class_inlet_get_method(fts_class_t *cl, int winlet, fts_class_t *type)
{
  fts_atom_t k, a;
  
  if(winlet < 0)
    winlet = 0;
  else if(winlet >= cl->ninlets)
    winlet = cl->ninlets - 1;
  
  fts_set_int(&k, class_inlet_key(winlet, type));
  
  if (fts_hashtable_get( cl->inlets, &k, &a))
    return (fts_method_t)fts_get_pointer(&a);
  else
    return NULL;	
}

int
fts_class_outlet_get_declarations(fts_class_t *cl, int woutlet, fts_iterator_t *iter)
{
  fts_class_outlet_t *out = class_get_outlet(cl, woutlet);

  if(out)
    return class_outlet_get_declarations(out, iter);
  else
    return 0;
}

int 
fts_class_outlet_has_type(fts_class_t *cl, int woutlet, fts_class_t *type)
{
  fts_class_outlet_t *out = class_get_outlet(cl, woutlet);

  if(out)  
    {
      fts_atom_t a;

      fts_set_pointer(&a, type);
      return class_outlet_has_declaration(out, &a);
    }
  else
    return 0;
}

int 
fts_class_outlet_has_message(fts_class_t *cl, int woutlet, fts_symbol_t selector)
{
  fts_class_outlet_t *out = class_get_outlet(cl, woutlet);

  if(out)  
    {
      fts_atom_t a;

      fts_set_symbol(&a, selector);
      return class_outlet_has_declaration(out, &a);
    }
  else
    return 0;
}

