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

fts_class_t *fts_class_class;

const int fts_system_inlet = -1;
static int typeid = FTS_FIRST_OBJECT_TYPEID;

#define CLASS_INLET_MAX 255
#define CLASS_TYPEID_MAX ((1 << 23) - 1)

/* Return Status declarations */
fts_status_description_t fts_DuplicatedClass = {"Duplicated class"};


/***********************************************************************
 *
 * Class Handling
 *
 */

static unsigned int default_hash_function( const fts_atom_t *p)
{
  return (unsigned int)fts_get_object( p) >> 3;;
}

static int default_equals_function( const fts_atom_t *p1, const fts_atom_t *p2)
{
  return fts_get_object( p1) == fts_get_object( p2);
}

fts_class_t *
fts_class_install(fts_symbol_t name, fts_instantiate_fun_t instantiate_fun)
{
  fts_class_t *cl = (fts_class_t *)fts_object_create( fts_class_class, NULL, 0, 0);

  cl->name = name;
  cl->instantiate_fun = instantiate_fun;
  cl->typeid = typeid++;

  fts_class_set_hash_function( cl, default_hash_function);
  fts_class_set_equals_function( cl, default_equals_function);

  if(name != NULL)
    {
      if(fts_package_add_class(fts_get_current_package(), cl, name) != fts_ok)
	return 0;
    }

  return cl;
}

void
fts_class_instantiate(fts_class_t *cl)
{
  if (!cl->size)
    (*cl->instantiate_fun)(cl);
}

/* for now just recreate the same class and add it to the current package */
void
fts_class_alias(fts_class_t *cl, fts_symbol_t alias)
{
  fts_package_add_class(fts_get_current_package(), cl, alias);
}

static fts_class_t *
get_class( fts_symbol_t package_name, fts_symbol_t class_name)
{
  fts_package_t *pkg;

  pkg = fts_package_get( package_name);
  if (pkg == NULL)
    return NULL;

  return fts_package_get_class( pkg, class_name);
}

fts_class_t *
fts_class_get_by_name(fts_symbol_t package_name, fts_symbol_t class_name)
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

  if ((cl = fts_package_get_class(pkg, class_name)) != NULL)
    return cl;

  /* ask the current package */
  pkg = fts_get_current_package();
  if ((cl = fts_package_get_class(pkg, class_name)) != NULL)
    return cl;

  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &iter);

  while ( fts_iterator_has_more( &iter)) 
    {
      fts_atom_t a;

      fts_iterator_next( &iter, &a);
      if ((cl = get_class( fts_get_symbol( &a), class_name)) != NULL)
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

static int 
class_outlet_get_declarations(fts_class_outlet_t *out, fts_iterator_t *iter)
{
  int n = fts_list_get_size(out->declarations);

  if(n > 0)
    fts_list_get_values(out->declarations, iter);
  
  return n;
}

static int
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
fts_class_init(fts_class_t *cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor)
{
  cl->size = size;
  cl->heap = fts_heap_new(size);

  cl->constructor = (constructor != NULL) ? constructor: dummy_method;
  cl->deconstructor = (deconstructor != NULL) ? deconstructor: dummy_method;

  cl->methods = fts_hashtable_new( FTS_HASHTABLE_MEDIUM);
  cl->input_handler = NULL;
  cl->ninlets = 0;

  cl->noutlets = 0;
  cl->out_alloc = 0;
  cl->outlets = NULL;
}

/********************************************
*
*  method hashtable
*
*/

typedef struct
{
  fts_object_t o;
  const void *selector;
  fts_class_t *type;
} method_key_t;

static fts_class_t *method_key_class;
static method_key_t *method_key;
static fts_atom_t method_key_atom;

static unsigned int
method_key_hash(const fts_atom_t *a)
{
  method_key_t *key = (method_key_t *)fts_get_object(a);

  return (unsigned int)key->selector + (unsigned int)key->type;
}

static int
method_key_equals(const fts_atom_t *a, const fts_atom_t *b)
{
  method_key_t *key_a = (method_key_t *)fts_get_object(a);
  method_key_t *key_b = (method_key_t *)fts_get_object(b);

  return (key_a->selector == key_b->selector && key_a->type == key_b->type);
}

static void
method_key_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(method_key_t), NULL, NULL);

  fts_class_set_hash_function(cl, method_key_hash);
  fts_class_set_equals_function(cl, method_key_equals);
}

static void
method_key_init(void)
{
  method_key_class = fts_class_install(NULL, method_key_instantiate);
  method_key = (method_key_t *)fts_object_create(method_key_class, NULL, 0, 0);

  fts_set_object(&method_key_atom, (fts_object_t *)method_key);
}

static method_key_t *
method_key_new(fts_symbol_t selector, fts_class_t *type)
{
  method_key_t *key = (method_key_t *)fts_object_create(method_key_class, NULL, 0, 0);

  key->selector = selector;
  key->type = type;

  return key;
}

typedef struct
{
  fts_method_t method;
  int varargs;
} method_handle_t;

static method_handle_t *
method_handle_new(fts_method_t method, int varargs)
{
  method_handle_t *handle = (method_handle_t *)fts_malloc(sizeof(method_handle_t));

  handle->method = method;
  handle->varargs = varargs;

  return handle;
}

enum method_check_e {found_no_method = -1, found_method = 0, found_varargs_method = 1};

static enum method_check_e
method_check(fts_class_t *cl, const void *selector, fts_class_t *type)
{
  fts_atom_t a;

  method_key->selector = selector;
  method_key->type = type;

  if(fts_hashtable_get(cl->methods, &method_key_atom, &a))
  {
    method_handle_t *handle = fts_get_pointer(&a);
    return handle->varargs;
  }

  return -1; /* method not found */
}

static fts_method_t
method_get(fts_class_t *cl, const void *selector, fts_class_t *type, int *varargs)
{
  fts_atom_t a;
  
  method_key->selector = selector;
  method_key->type = type;
  
  if(fts_hashtable_get(cl->methods, &method_key_atom, &a))
  {
    method_handle_t *handle = fts_get_pointer(&a);

    *varargs = handle->varargs;
    return handle->method;
  }

  return NULL;  
}

static void
method_put(fts_class_t *cl, const void *selector, fts_class_t *type, fts_method_t method, int varargs)
{
  method_key_t *key = method_key_new(selector, type);
  method_handle_t *handle = method_handle_new(method, varargs);
  fts_atom_t k, a;

  fts_set_object(&k, key);
  fts_set_pointer(&a, handle);

  fts_hashtable_put(cl->methods, &k, &a);
}

/********************************************
 *
 * inlet/outlet definitions
 *
 */
void
fts_class_message(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, fts_method_t method)
{
  enum method_check_e check = method_check(cl, s, type);
  
  if(check == found_varargs_method && type != fts_void_class)
    post("warning: redefinition of varargs method for message %s of class %s\n", s, fts_class_get_name(cl));
  else if(check != found_no_method && type != NULL)
    post("warning: redefinition of %s method for message %s of class %s\n", fts_class_get_name(type), s, fts_class_get_name(cl));
  else if(check != found_no_method && type == NULL)
    post("warning: redefinition of generic atom method for message %s of class %s\n", s, fts_class_get_name(cl));      

  method_put(cl, s, type, method, 0);
}

void
fts_class_message_varargs(fts_class_t *cl, fts_symbol_t s, fts_method_t method)
{
  enum method_check_e check = method_check(cl, s, NULL);  

  if(check == found_method)
    post("warning: redefinition of generic atom method for message %s of class %s by varargs declaration\n", s, fts_class_get_name(cl));
  else if(check == found_varargs_method)
    post("warning: redefinition of varargs method for message %s of class %s\n", s, fts_class_get_name(cl));    
  
  if(method_check(cl, s, fts_tuple_class) != found_no_method)
    post("warning: redefinition of tuple method for message %s of class %s by varargs declaration\n", s, fts_class_get_name(cl));

  /* register method void if void method not already defined */
  if(method_check(cl, s, fts_tuple_class) != found_method)
    method_put(cl, s, fts_void_class, method, 1);

  /* register method for NULL and tuple */
  method_put(cl, s, NULL, method, 1);
  method_put(cl, s, fts_tuple_class, method, 1);
}

static int
class_adjust_inlet(fts_class_t *cl, int winlet)
{
  if(winlet < 0)
    winlet = 0;
  else if(winlet > CLASS_INLET_MAX)
    winlet = CLASS_INLET_MAX;

  if(winlet >= cl->ninlets)
    class_set_inlets_number(cl, winlet + 1);

  return winlet;
}

static int
class_clip_inlet(fts_class_t *cl, int winlet)
{
  if(winlet < 0)
    return 0;
  else if(winlet >= cl->ninlets)
    return cl->ninlets  - 1;

  return winlet;
}

void
fts_class_inlet(fts_class_t *cl, int winlet, fts_class_t *type, fts_method_t method)
{
  int n = class_adjust_inlet(cl, winlet);
  enum method_check_e check = method_check(cl, (const void *)n, type);

  if(check == found_varargs_method && type != fts_void_class)
    post("warning: redefinition of varargs method for inlet %d of class %s\n", n, fts_class_get_name(cl));
  else if(check != found_no_method && type != NULL)
    post("warning: redefinition of %s method for inlet %d of class %s\n", fts_class_get_name(type), n, fts_class_get_name(cl));
  else if(check != found_no_method && type == NULL)
    post("warning: redefinition of generic atom method for inlet %d of class %s\n", fts_class_get_name(type), n, fts_class_get_name(cl));

  method_put(cl, (const void *)n, type, method, 0);
}

void
fts_class_inlet_varargs(fts_class_t *cl, int winlet, fts_method_t method)
{
  int n = class_adjust_inlet(cl, winlet);
  enum method_check_e check = method_check(cl, (const void *)n, NULL);

  if(check == found_method)
    post("warning: redefinition of generic atom method for inlet %d of class %s by varargs declaration\n", n, fts_class_get_name(cl));
  else if(check == found_varargs_method)
    post("warning: redefinition of varargs method for inlet %d of class %s\n", n, fts_class_get_name(cl));

  if(method_check(cl, (const void *)n, fts_tuple_class) != found_no_method)
    post("warning: redefinition of tuple method for inlet %d of class %s by varargs declaration\n", n, fts_class_get_name(cl));

  /* register method void if void method not already defined */
  if(method_check(cl, (const void *)n, fts_void_class) != found_method)
    method_put(cl, (const void *)n, fts_void_class, method, 1);

  /* register method for NULL and tuple */
  method_put(cl, (const void *)n, NULL, method, 1);
  method_put(cl, (const void *)n, fts_tuple_class, method, 1);
}

void
fts_class_inlet_thru(fts_class_t *cl, int winlet)
{
  class_adjust_inlet(cl, winlet);
}

void
fts_class_input_handler(fts_class_t *cl, fts_method_t method)
{
  cl->input_handler = method;
}

/**************************************************
 *
 *  outlet types definition
 *
 */
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

/**************************************************
 *
 *  request inlet/outlet methods and definitions
 *
 */
fts_method_t
fts_class_get_method(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, int *varargs)
{
  fts_method_t method = method_get(cl, (const void *)s, type, varargs);

  if(method == NULL && type != fts_void_class)
    method = method_get(cl, (const void *)s, NULL, varargs);

  return method;
}

fts_method_t
fts_class_get_method_varargs(fts_class_t *cl, fts_symbol_t s)
{
  int varargs = 0;
  fts_method_t method = method_get(cl, (const void *)s, NULL, &varargs);

  if(varargs != NULL)
    return method;
  else
    return NULL;
}

fts_method_t
fts_class_get_inlet_method(fts_class_t *cl, int winlet, fts_class_t *type, int *varargs)
{
  int n = class_clip_inlet(cl, winlet);
  fts_method_t method = method_get(cl, (const void *)n, type, varargs);

  if(method == NULL && type != fts_void_class)
    method = method_get(cl, (const void *)n, NULL, varargs);

  return method;
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

/***********************************************************************
 *
 * Initialization
 *
 */

static void class_class_instantiate( fts_class_t *cl)
{
}

void fts_kernel_class_init( void)
{  
  /* As the 'class' class is used to create a class, it cannot be created using standard ways. */
  fts_heap_t *heap = fts_heap_new( sizeof( fts_class_t));

  fts_class_class = (fts_class_t *)fts_heap_zalloc( heap);

  fts_class_class->head.cl = fts_class_class;
  fts_class_class->name = NULL;
  fts_class_class->instantiate_fun = class_class_instantiate;
  fts_class_class->typeid = typeid++;

  fts_class_class->size = sizeof( fts_class_t);
  fts_class_class->heap = heap;
  fts_class_class->constructor = dummy_method;
  fts_class_class->deconstructor = dummy_method;
  fts_class_class->methods = fts_hashtable_new( FTS_HASHTABLE_MEDIUM);
  fts_class_class->input_handler = NULL;

  fts_class_set_name( fts_class_class, fts_s_class);

  method_key_init();
}
