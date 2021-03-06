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

/* This file implement the class structure */

#include <string.h>

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/object.h>
#include <ftsprivate/package.h>
#include <ftsprivate/patcher.h>

fts_class_t *fts_class_class;

const int fts_system_inlet = -1;
static int type_id = FTS_FIRST_OBJECT_TYPEID;

static fts_heap_t *iterator_heap = 0;

#define CLASS_INLET_MAX 255

/***********************************************************************
*
* Class Handling
*
*/

static unsigned int
default_hash_function (const fts_atom_t * p)
{
  return (unsigned int) fts_get_object (p) >> 3;;
}

static int
default_equals_function (const fts_object_t *o, const fts_object_t *p)
{
  return o == p;
}

static void
default_array_function(fts_object_t *obj, fts_array_t *array)
{
}


static void
default_description_function(fts_object_t *obj, fts_array_t *array)
{
  fts_array_append_symbol(array, fts_object_get_class_name(obj));
}

static fts_object_t *
default_guiobject_function(fts_object_t *obj)
{
  return NULL;
}

fts_class_t *
fts_class_install(fts_symbol_t name, fts_instantiate_fun_t instantiate_fun)
{
  fts_class_t *cl = NULL;
  
  if(name != NULL)
    cl = fts_package_get_class(fts_get_current_package(), name);
  
  if(cl == NULL)
  {
    cl = (fts_class_t * )fts_object_create(fts_class_class, 0, 0);
    
    cl->name = name;
    cl->instantiate_fun = instantiate_fun;
    cl->type_id = type_id++;
    cl->super_class = NULL;
    
    fts_class_set_hash_function (cl, default_hash_function);
    fts_class_set_equals_function (cl, default_equals_function);
    fts_class_set_description_function (cl, default_description_function);
    fts_class_set_copy_function (cl, NULL);
    fts_class_set_array_function (cl, NULL);
    fts_class_set_spost_function (cl, NULL);
    fts_class_set_guiobject_function (cl, default_guiobject_function);
    
    fts_hashtable_init(&cl->import_handlers, FTS_HASHTABLE_SMALL);
    fts_hashtable_init(&cl->export_handlers, FTS_HASHTABLE_SMALL);
    
    if(name != NULL)
    {
      if(fts_package_add_class(fts_get_current_package(), cl, name) != fts_ok)
        return NULL;
    }
  }
  
  return cl;
}


void
fts_class_instantiate(fts_class_t * cl)
{
  if (!cl->size)
    (*cl->instantiate_fun) (cl);
}

/* for now just recreate the same class and add it to the current package */
void
fts_class_alias(fts_class_t * cl, fts_symbol_t alias)
{
  if(fts_package_get_class(fts_get_current_package(), alias) == NULL)
    fts_package_add_class(fts_get_current_package (), cl, alias);
}

/********************************************
*
* inlet/outlet utils
*
*/
#define CLASS_INOUT_BLOCK_SIZE 4

static void
class_set_inlets_number (fts_class_t * cl, int n)
{
  cl->ninlets = n;
}

static void
class_set_outlets_number (fts_class_t * cl, int n)
{
  int n_alloc = cl->out_alloc;
  int i;
  
  if (n > n_alloc)
  {
    while (n_alloc < n)
      n_alloc += CLASS_INOUT_BLOCK_SIZE;
    
    cl->outlets = fts_realloc (cl->outlets, n_alloc * sizeof (fts_class_outlet_t));
    
    /* init new outlets */
    for (i = cl->out_alloc; i < n_alloc; i++)
      fts_list_init (cl->outlets[i].declarations);
    
    cl->out_alloc = n_alloc;
  }
  
  /* delete outlets cut off */
  for (i = n; i < cl->noutlets; i++)
    fts_list_delete (cl->outlets[i].declarations);
  
  cl->noutlets = n;
}

static fts_class_outlet_t *
class_get_outlet (fts_class_t * cl, int woutlet)
{
  if (cl->outlets != NULL)
  {
    if (woutlet < 0)
      woutlet = 0;
    else if (woutlet >= cl->noutlets)
      woutlet = cl->noutlets - 1;
    
    return cl->outlets + woutlet;
  }
  else
    return NULL;
}

static void
class_outlet_add_declaration (fts_class_outlet_t * out, const fts_atom_t * a)
{
  out->declarations = fts_list_append (out->declarations, a);
}

static int
class_outlet_get_declarations (fts_class_outlet_t * out, fts_iterator_t * iter)
{
  int n = fts_list_get_size (out->declarations);
  
  if (n > 0)
    fts_list_get_values (out->declarations, iter);
  
  return n;
}

static int
class_outlet_has_declaration (fts_class_outlet_t * out, const fts_atom_t * p)
{
  fts_iterator_t iter;
  
  if (class_outlet_get_declarations (out, &iter) > 0)
  {
    while (fts_iterator_has_more (&iter))
    {
      fts_atom_t a;
      
      fts_iterator_next (&iter, &a);
      
      if (fts_atom_identical (p, &a))
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

static fts_method_status_t
dummy_method (fts_object_t * o, fts_symbol_t s, int ac, const fts_atom_t *at,  fts_atom_t *ret)
{
  return fts_ok;
}


void
fts_class_init (fts_class_t * cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor)
{
  cl->size = size;
  cl->heap = fts_heap_new (size);
  
  cl->constructor = (constructor != NULL) ? constructor : dummy_method;
  cl->deconstructor = (deconstructor != NULL) ? deconstructor : dummy_method;
  
  cl->methods = fts_hashtable_new (FTS_HASHTABLE_MEDIUM);
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

static unsigned int
method_key_hash (const fts_atom_t * a)
{
  method_key_t *key = (method_key_t *) fts_get_object (a);
  
  return (unsigned int) key->selector + (unsigned int) key->type;
}

static int
method_key_equals (const fts_object_t *a, const fts_object_t *b)
{
  method_key_t *key_a = (method_key_t *)a;
  method_key_t *key_b = (method_key_t *)b;
  
  return (key_a->selector == key_b->selector && key_a->type == key_b->type);
}

static void
method_key_instantiate (fts_class_t * cl)
{
  fts_class_init (cl, sizeof (method_key_t), NULL, NULL);
  
  fts_class_set_hash_function (cl, method_key_hash);
  fts_class_set_equals_function (cl, method_key_equals);
}

static method_key_t *
method_key_new (fts_symbol_t selector, fts_class_t * type)
{
  method_key_t *key = (method_key_t *) fts_object_create (method_key_class, 0, 0);
  
  key->selector = selector;
  key->type = type;
  
  return key;
}

static fts_method_t
method_get (fts_class_t * cl, const void *selector, fts_class_t * type)
{
  fts_atom_t k, a;
  
  if(cl->methods)
  {
    method_key_t *method_key = method_key_new((fts_symbol_t)selector, type);
    fts_method_t method = NULL;
    
    fts_set_object(&k, (fts_object_t *) method_key);
    
    if(fts_hashtable_get (cl->methods, &k, &a))
      method = (fts_method_t)fts_get_pointer(&a);
    
    fts_object_destroy((fts_object_t *)method_key);
    
    return method;
  }
  
  return NULL;
}

static void
method_put (fts_class_t * cl, const void *selector, fts_class_t * type, fts_method_t method)
{
  method_key_t *key = method_key_new((fts_symbol_t) selector, type);
  fts_atom_t k, a;
  
  fts_set_object (&k, key);
  fts_set_pointer (&a, method);
  
  fts_hashtable_put (cl->methods, &k, &a);
}

/***********************************************
 *
 *  class method iterator
 *
 */

static void 
class_message_iterator_next(fts_iterator_t *i, fts_atom_t *a)
{
  method_key_t *key;
  fts_atom_t k;
  int n = 0;
  
  fts_iterator_next((fts_iterator_t *)i->data, &k);
  key = fts_get_pointer(&k);
  n = (int)key->selector;
  
  if(n > 64)
    fts_set_symbol(a, (fts_symbol_t)key->selector);
  else
    fts_set_int(a, n);
}

static int 
class_message_iterator_has_more(fts_iterator_t *i)
{
  if(fts_iterator_has_more((fts_iterator_t *)i->data))
    return 1;

  fts_heap_free(i->data, iterator_heap);
  
  return 0;
}

void
fts_class_get_messages(const fts_class_t *cl, fts_iterator_t *i)
{
  fts_iterator_t *iter = (fts_iterator_t *)fts_heap_alloc(iterator_heap);
  
  fts_hashtable_get_keys(cl->methods, iter);
  
  i->has_more = class_message_iterator_has_more;
  i->next = class_message_iterator_next;
  i->data = iter;
}

/********************************************
 *
 * inlet/outlet definitions
 *
 */
void
fts_class_message(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, fts_method_t method)
{
  fts_method_t declared = NULL;
  
  fts_class_instantiate(cl);
  declared = method_get(cl, (const void *)s, type);
  
  if(declared != NULL)
  {
    fts_symbol_t cl_name = fts_class_get_name(cl);
    
    if(type != NULL)
    {
      fts_symbol_t type_name = fts_class_get_name(type);
      fts_post("warning: redefinition of %s method for message %s of class %s\n", fts_symbol_name(type_name), fts_symbol_name(s), fts_symbol_name(cl_name));      
    }
    else
      fts_post("warning: redefinition of varargs method for message %s of class %s\n", fts_symbol_name(s), fts_symbol_name(cl_name));
  }
  
  method_put (cl, s, type, method);
}

static int
class_adjust_inlet(fts_class_t * cl, int winlet)
{
  if (winlet < 0)
    winlet = 0;
  else if (winlet > CLASS_INLET_MAX)
    winlet = CLASS_INLET_MAX;
  
  if (winlet >= cl->ninlets)
    class_set_inlets_number (cl, winlet + 1);
  
  return winlet;
}

static int
class_clip_inlet(fts_class_t * cl, int winlet)
{
  if (winlet < 0)
    return 0;
  else if (winlet >= cl->ninlets)
    return cl->ninlets - 1;
  
  return winlet;
}

void
fts_class_inlet(fts_class_t * cl, int winlet, fts_class_t * type, fts_method_t method)
{
  int n = class_adjust_inlet(cl, winlet);
  fts_method_t declared = method_get(cl, (const void *)n, type);
  
  if(declared != NULL)
  {
    fts_symbol_t cl_name = fts_class_get_name(cl);
    
    if(type != NULL)
    {
      fts_symbol_t type_name = fts_class_get_name(type);
      fts_post("warning: redefinition of %s method for inlet %d of class %s\n", fts_symbol_name(type_name), n, fts_symbol_name(cl_name));      
    }
    else
      fts_post("warning: redefinition of varargs method for inlet %d of class %s\n", n, fts_symbol_name(cl_name));
  }
  
  method_put (cl, (const void *) n, type, method);
}

void
fts_class_inlet_thru(fts_class_t * cl, int winlet)
{
  class_adjust_inlet(cl, winlet);
}

void
fts_class_input_handler(fts_class_t * cl, fts_method_t method)
{
  cl->input_handler = method;
  
  if (cl->ninlets == 0)
    class_set_inlets_number (cl, 1);
}

/**************************************************
 *
 *  outlet types definition
 *
 */
void
fts_class_outlet(fts_class_t * cl, int woutlet, fts_class_t * class)
{
  fts_class_outlet_t *out;
  fts_atom_t a;
  
  if (woutlet >= cl->noutlets)
    class_set_outlets_number (cl, woutlet + 1);
  
  out = class_get_outlet (cl, woutlet);
  
  fts_set_pointer (&a, class);
  class_outlet_add_declaration (out, &a);
}

/**************************************************
 *
 *  request inlet/outlet methods and definitions
 *
 */
fts_method_t
fts_class_get_method(fts_class_t * cl, fts_symbol_t s, fts_class_t * type)
{
  fts_method_t method = method_get (cl, (const void *) s, type);
  
  /* try varargs if not found with given type */
  if(method == NULL)
    method = method_get (cl, (const void *) s, NULL);
  
  return method;
}

fts_method_t
fts_class_get_method_novarargs(fts_class_t * cl, fts_symbol_t s, fts_class_t * type)
{
  fts_method_t method = method_get (cl, (const void *) s, type);

  return method;
}

fts_method_t
fts_class_get_method_varargs (fts_class_t * cl, fts_symbol_t s)
{
  return method_get (cl, (const void *) s, NULL);
}

fts_method_t
fts_class_get_inlet_method(fts_class_t * cl, int winlet, fts_class_t * type)
{
  int n = class_clip_inlet (cl, winlet);
  
  fts_method_t method = method_get(cl, (const void *) n, type);
  
  if (method == NULL && type != fts_void_class)
    method = method_get (cl, (const void *) n, NULL);
  
  return method;
}

int
fts_class_outlet_get_declarations (fts_class_t * cl, int woutlet, fts_iterator_t * iter)
{
  fts_class_outlet_t *out = class_get_outlet (cl, woutlet);
  
  if (out)
    return class_outlet_get_declarations (out, iter);
  else
    return 0;
}

int
fts_class_outlet_has_type (fts_class_t * cl, int woutlet, fts_class_t * type)
{
  fts_class_outlet_t *out = class_get_outlet (cl, woutlet);
  
  if (out)
  {
    fts_atom_t a;
    
    fts_set_pointer (&a, type);
    return class_outlet_has_declaration (out, &a);
  }
  else
    return 0;
}

int
fts_class_outlet_has_message (fts_class_t * cl, int woutlet, fts_symbol_t selector)
{
  fts_class_outlet_t *out = class_get_outlet (cl, woutlet);
  
  if (out)
  {
    fts_atom_t a;
    
    fts_set_symbol (&a, selector);
    return class_outlet_has_declaration (out, &a);
  }
  else
    return 0;
}




/***********************************************************************
*
*  class documentation
*
*/

void
fts_class_doc_post(fts_class_t *cl)
{
  fts_class_doc_t *doc = fts_class_get_doc(cl);
  fts_symbol_t class_name = fts_class_get_name(cl);
  enum {state_ready, state_constructor, state_messages} state = state_ready;
  
  if(class_name != NULL)
  {
    while(doc != NULL)
    {
      fts_symbol_t name = fts_class_doc_get_name(doc);
      const char *args = fts_class_doc_get_args(doc);
      const char *comment = fts_class_doc_get_comment(doc);
      
      if(args == NULL)
        args = "";
      
      /* constructor */
      if(name == class_name && state != state_messages)
      {
        fts_post("%s %s ... %s\n", fts_symbol_name(name), args, comment);
        state = state_constructor;
      }
      else
      {
        switch(state)
        {
          case state_ready:
            fts_post("%s\n", fts_symbol_name(class_name));
          case state_constructor:
            fts_post("{\n");
          case state_messages:
          default:
            fts_post("  %s %s ... %s\n", fts_symbol_name(name), args, comment);
            state = state_messages;
            break;
        }
      }
      
      doc = fts_class_doc_get_next(doc);
    }
    
    if(state == state_messages)
      fts_post("}\n");
  }
}


/* append triples of doc-symbols to array */
int fts_class_doc_get (fts_class_t *cl, fts_array_t *out)
{
  fts_class_doc_t *doc = fts_class_get_doc(cl);
  fts_symbol_t class_name = fts_class_get_name(cl);
  enum {state_ready, state_constructor, state_messages} state = state_ready;
  
  if (class_name != NULL)
  {
    while (doc != NULL)
    {
	    fts_symbol_t  name    = fts_class_doc_get_name(doc);
	    const char   *args    = fts_class_doc_get_args(doc);
	    const char   *comment = fts_class_doc_get_comment(doc);
      
	    if(args == NULL)
        args = "";
      
	    if (name == class_name  &&  state != state_messages)
	    {   /* constructor */
        fts_array_append_symbol(out, name);
        fts_array_append_symbol(out, fts_new_symbol(args));
        fts_array_append_symbol(out, fts_new_symbol(comment));
        
        state = state_constructor;
	    }
	    else
	    {
        switch(state)
        {
          case state_ready:
            fts_array_append_symbol(out, name);
            fts_array_append_symbol(out, fts_s_empty_string);
            fts_array_append_symbol(out, fts_s_empty_string);
            
          case state_constructor:
          case state_messages:
          default:
            fts_array_append_symbol(out, name);
            fts_array_append_symbol(out, fts_new_symbol(args));
            fts_array_append_symbol(out, fts_new_symbol(comment));
            
            state = state_messages;
            break;
        }
	    }
      
	    doc = fts_class_doc_get_next(doc);
    }
  }

return 3;	/* return number of columns (group of atoms in list) */
}


static fts_method_status_t
method_post_doc(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_class_t *cl = fts_object_get_class(o);
  
  fts_class_doc_post(cl);
  
  return fts_ok;
}

void
fts_class_doc(fts_class_t *cl, fts_symbol_t name, const char *args, const char *comment)
{
  fts_class_doc_t *line = fts_malloc(sizeof(fts_class_doc_t));
  fts_class_doc_t **list = &cl->doc;
  
  line->name = name;
  line->args = args;
  line->comment = comment;
  line->next = NULL;
  
  /* declare doc method */
  if(cl->doc == NULL)
    fts_class_message(cl, fts_s_doc, fts_void_class, method_post_doc);
  
  while(*list != NULL)
    list = &((*list)->next);
  
  *list = line;
}

void 
fts_class_import_handler(fts_class_t *cl, fts_symbol_t suffix, fts_method_t meth)
{
  fts_atom_t k, v;
  
  fts_set_symbol(&k, suffix);
  fts_set_pointer(&v, meth);
  fts_hashtable_put(&cl->import_handlers, &k, &v);
  
  if(!fts_class_get_method_varargs(cl, fts_s_import))
  {
    fts_class_message_varargs(cl, fts_s_import, fts_object_import);
    fts_class_message_varargs(cl, fts_s_importas, fts_object_import_as);
  }
}

void 
fts_class_export_handler(fts_class_t *cl, fts_symbol_t suffix, fts_method_t meth)
{
  fts_atom_t k, v;
  
  fts_set_symbol(&k, suffix);
  fts_set_pointer(&v, meth);
  fts_hashtable_put(&cl->export_handlers, &k, &v);

  if (fts_class_get_method_varargs(cl, fts_s_export) == NULL)
  { /* no export message was defined, so define it now */
    fts_class_message_varargs(cl, fts_s_export, fts_object_export);
    fts_class_message_varargs(cl, fts_s_exportas, fts_object_export_as);
  }
}

/***********************************************************************
*
* Initialization
*
*/

static void
class_class_instantiate (fts_class_t * cl)
{
}

FTS_MODULE_INIT(class)
{  
  fts_class_class = fts_shared_get(fts_s_class);
  
  if(fts_class_class == NULL)
  {
    fts_heap_t *heap = fts_heap_new (sizeof (fts_class_t));
    
    fts_class_class = (fts_class_t *) fts_heap_zalloc (heap);
    
    fts_class_class->head.cl = fts_class_class;
    fts_class_class->name = NULL;
    fts_class_class->instantiate_fun = class_class_instantiate;
    fts_class_class->type_id = type_id++;
    fts_class_class->super_class = NULL;
    
    fts_class_class->size = sizeof (fts_class_t);
    fts_class_class->heap = heap;
    fts_class_class->constructor = dummy_method;
    fts_class_class->deconstructor = dummy_method;
    fts_class_class->methods = fts_hashtable_new (FTS_HASHTABLE_MEDIUM);
    
    fts_class_set_name (fts_class_class, fts_s_class);
    
    fts_shared_set(fts_s_class, fts_class_class);
  }
  
  iterator_heap = fts_heap_new(sizeof(fts_iterator_t));
  method_key_class = fts_class_install (NULL, method_key_instantiate);
}
