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
#include <stdlib.h>
#include <string.h>

#include <fts/fts.h>

#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/variable.h>

fts_heap_t *definition_heap = NULL;
fts_heap_t *definition_listener_heap = NULL;

/****************************************************************************
 *
 *  definitions (low level interface)
 *
 */

struct fts_definition_listener
{
  fts_object_t *object;
  fts_definition_listener_t *next;
};

static fts_definition_t *
fts_definition_new(fts_symbol_t name)
{
  fts_definition_t *def = (fts_definition_t *)fts_heap_zalloc(definition_heap);
  
  def->name = name;
  fts_set_void(&def->value);
  def->listeners = NULL;

  return def;
}

static void
define_spost_description(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int descr_ac = fts_object_get_description_size(o);
  fts_atom_t *descr_at = fts_object_get_description_atoms(o);
  
  fts_spost_object_description_args( (fts_bytestream_t *)fts_get_object(at), descr_ac - 2, descr_at + 2);
}

fts_definition_t *
fts_definition_get(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_hashtable_t *hash = fts_patcher_get_definitions(scope);
  fts_definition_t *def = NULL;
  fts_atom_t k, a;

  fts_set_symbol(&k, name);

  if(hash)
    {
      if(fts_hashtable_get(hash, &k, &a))
	return (fts_definition_t *)fts_get_pointer(&a);
    }
  else
    {
      hash = (fts_hashtable_t *)fts_malloc(sizeof(fts_hashtable_t));
      
      fts_hashtable_init( hash, FTS_HASHTABLE_MEDIUM);
      fts_patcher_set_definitions(scope, hash);
    }

  def = fts_definition_new(name);
  
  fts_set_pointer(&a, def);
  fts_hashtable_put(hash, &k, &a);

  return def;
}

void 
fts_definition_add_listener(fts_definition_t *def, fts_object_t *obj)
{
  fts_definition_listener_t *l;

  /* make sure that we don't add the same listener twice */
  for(l = def->listeners; l != NULL; l = l->next)
    {
      if (l->object == obj)
	return;
    }

  /* create new list entry */
  l = (fts_definition_listener_t *)fts_heap_alloc(definition_listener_heap);

  /* add it to the beginning of the list of the definition */
  l->object = obj;
  l->next = def->listeners;
  def->listeners = l;
}

void 
fts_definition_remove_listener(fts_definition_t *def, fts_object_t *obj)
{
  fts_definition_listener_t *l;

  /* search for listener at the definition */
  for(l = def->listeners; l != NULL; l = l->next)
    {
      /* just set it to NULL */
      if (l->object == obj)
	{
	  l->object = NULL;
	  return;
	}
    }
}

static void 
definition_recompute_listeners(fts_definition_t *def)
{
  fts_definition_listener_t **p = &def->listeners;

  while(*p != NULL)
    {
      fts_object_t *obj = (*p)->object;

      if (obj != NULL)
	{
	  fts_object_recompute(obj);
	  p = &(*p)->next;
	}
      else
	{
	  fts_definition_listener_t *remove = *p;

	  /* remove empty listener entry */
	  *p = remove->next;
	  fts_heap_free(remove, definition_listener_heap);
	}
    }
}

void
fts_definition_update(fts_definition_t *def, const fts_atom_t *value)
{
  fts_definition_set_value(def, value);
  definition_recompute_listeners(def);  
}

/*************************************************************
 *
 *  names (high level API)
 *
 *
 */
void
fts_name_set_value(fts_patcher_t *patcher, fts_symbol_t name, const fts_atom_t *value)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  if(!fts_atom_equals(value, &def->value))
    fts_definition_update(def, value);
}

void
fts_name_add_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  fts_definition_add_listener(def, obj);
  fts_object_add_binding(obj, def);
}

void
fts_name_remove_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  fts_definition_remove_listener(def, obj);
}

fts_atom_t *
fts_name_get_value(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  return &def->value;
}

fts_symbol_t
fts_name_get_unused(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_hashtable_t *hash = fts_patcher_get_definitions(scope);
  fts_atom_t a, k;

  fts_set_symbol(&k, name);

  if(hash != NULL && fts_hashtable_get(hash, &k, &a))
    {
      fts_definition_t *def = fts_get_pointer(&a);
	      
      if(!fts_is_void(&def->value))
	{
	  const char *str = name;
	  int len = strlen(str);
	  char *new_str = alloca((len + 10) * sizeof(char));
	  int num = 0;
	  int dec = 1;
	  int i;
	  
	  /* separate base name and index */
	  for(i=len-1; i>=0; i--) 
	    {
	      if(len == (i + 1) && str[i] >= '0' && str[i] <= '9')
		num += (str[len = i] - '0') * dec;
	      else
		new_str[i] = str[i];
	      
	      dec *= 10;
	    }
	  
	  /* generate new name */
	  sprintf(new_str + len, "%d", ++num);
	  name = fts_new_symbol(new_str);
	  fts_set_symbol(&k, name);
	  
	  while(fts_hashtable_get(hash, &k, &a)) 
	    {
	      fts_definition_t *def = fts_get_pointer(&a);
	      
	      if(fts_is_void(&def->value))
		break;
	      
	      sprintf(new_str + len, "%d", ++num);
	      name = fts_new_symbol(new_str);
	      fts_set_symbol(&k, name);
	    }
	}
    }

  return name;
}

void
fts_name_set_method( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_symbol(at) && fts_object_get_patcher(o) != NULL)
    {
      fts_object_set_name(o, fts_get_symbol(at));
      /*fts_patcher_set_dirty(fts_object_get_patcher(o), 1);*/
    }
}

void
fts_name_dump_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_symbol_t name = fts_object_get_name(o);
  fts_atom_t a;

  if(name != fts_s_empty_string)
    {
      fts_set_symbol(&a, name);
      fts_dumper_send(dumper, fts_s_name, 1, &a);
    }
}

void
fts_name_gui_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;

  fts_set_symbol(&a, fts_object_get_name(o));
  fts_client_send_message(o, fts_s_name, 1, &a);
}

/****************************************************************************
 *
 * "define" object
 *
 */

enum define_type {define_none, define_const, define_arg, define_args};

typedef struct
{
  fts_object_t o;
  int valid;
  fts_symbol_t type;
  fts_symbol_t name;
  fts_atom_t value;
  fts_patcher_t *patcher;
  fts_array_t descr;
  fts_expression_t *expression;
} define_t;

static fts_symbol_t sym_type = 0;
static fts_symbol_t sym_const = 0;
static fts_symbol_t sym_arg = 0;
static fts_symbol_t sym_args = 0;

static void
define_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;
  fts_atom_t a;

  fts_set_int(&a, this->valid);
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void
define_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;
  fts_atom_t a;

  fts_set_symbol(&a, this->type);
  fts_client_send_message(o, sym_type, 1, &a);

  fts_name_gui_method(o, 0, 0, 0, 0);
}

static fts_status_t
define_expression_callback(int ac, const fts_atom_t *at, void *data)
{
  define_t *this = (define_t *)data;

  if(ac > 0)
  {
    if(this->type == sym_const)
    {
      if(ac > 1)
      {
        fts_object_t *tuple = fts_object_create(fts_tuple_class, ac, at);
        fts_atom_t a;

        fts_set_object(&a, tuple);
        fts_atom_assign(&this->value, &a);
      }
      else
        fts_atom_assign(&this->value, at);

      return fts_ok;
    }
    else if(this->type == sym_arg)
    {
      /* define argument with default value */
      if(ac == 2)
      {
        fts_array_t *args = fts_patcher_get_args(fts_patcher_get_scope(this->patcher));
        int size = fts_array_get_size(args);
        fts_atom_t *atoms = fts_array_get_atoms(args);
        int index;

        /* get index */
        if(fts_is_int(at))
          index = fts_get_int(at);
        else
          return fts_status_new("bad index value");

        /* assign argument or given default value */
        if(index < size)
          fts_atom_assign(&this->value, atoms + index);
        else
          fts_atom_assign(&this->value, at + 1);
      }
      else
        return fts_status_new("bad argument definition");
    }
    else if(this->type == sym_args)
    {
      fts_array_t *args = fts_patcher_get_args(fts_patcher_get_scope(this->patcher));
      int size = fts_array_get_size(args);
      fts_atom_t *atoms = fts_array_get_atoms(args);

      /* concat args and default values */
      if(ac > 1 || size > 1)
      {
        fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, size, atoms);
        fts_atom_t a;

        if(ac > size)
          fts_tuple_append(tuple, ac - size, at + size);
        
        fts_set_object(&a, (fts_object_t *)tuple);
        fts_atom_assign(&this->value, &a);
      }
      else if(size > 0)
        fts_atom_assign(&this->value, atoms);
      else
        fts_atom_assign(&this->value, at);
    }
    else
      return fts_status_new("bad type");
  }

  return fts_ok;
}

static void
define_evaluate(define_t *this)
{
  int ac = fts_array_get_size(&this->descr);
  const fts_atom_t *at = fts_array_get_atoms(&this->descr);
  fts_status_t status = fts_expression_set(this->expression, ac, at);
  int valid = 0;

  fts_set_void(&this->value);
  
  /* evaluate expression */
  if(status == fts_ok)
  {
    fts_patcher_t *patcher = this->patcher;
    fts_status_t status = fts_expression_reduce(this->expression, patcher, 0, NULL, define_expression_callback, this);

    if(status == fts_ok)
    {
      valid = 1;

      /* reset definition */
      if(this->name != NULL && !fts_is_void(&this->value))
        fts_name_set_value(this->patcher, this->name, fts_null);
    }
  }
  
  /* update gui */
  if(valid != this->valid)
  {
    this->valid = valid;
    fts_update_request((fts_object_t *)this);
  }
}

static void
define_set_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;

  if(fts_is_symbol(at))
    this->type = fts_get_symbol(at);

  define_evaluate(this);
}

static void
define_set_expression(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *)o;

  fts_array_set(&this->descr, ac, at);
  define_evaluate(this);
}

static void
define_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *)o;

  if(fts_is_symbol(at))
    this->type = fts_get_symbol(at);

  fts_array_set(&this->descr, ac - 1, at + 1);

  define_evaluate(this);
}

static void
define_set_name(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *)o;
  
  /* reset definition */
  if(this->name != NULL && !fts_is_void(&this->value))
    fts_name_set_value(this->patcher, this->name, fts_null);

  /* set_name */
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t name = fts_get_symbol(at);

    if(name != fts_s_empty_string)
    {
      this->name = name;

      /* set definition */
      if(!fts_is_void(&this->value))
        fts_name_set_value(this->patcher, this->name, &this->value);

      /* set name of object */
      if(fts_object_has_id(o))
      {
        fts_atom_t a;
        
        fts_set_symbol(&a, name);
        fts_client_send_message(o, fts_s_name, 1, &a);
      }
    }
  }
}

static void
define_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = fts_dumper_message_new(dumper, fts_s_set);

  fts_message_append_symbol(mess, this->type);
  fts_message_append(mess, fts_array_get_size( &this->descr), fts_array_get_atoms(&this->descr));
  fts_dumper_message_send(dumper, mess);
}

static void 
define_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;
  fts_status_t status;

  this->valid = 0;
  this->type = sym_const;
  this->name = NULL;
  fts_set_void(&this->value);
  this->patcher = fts_object_get_patcher(o);

  /* expression description */
  fts_array_init(&this->descr, 0, 0);

  status = fts_expression_new(0, 0, &this->expression);
  if(status != fts_ok)
  {
    fts_object_error(o, "%s", fts_status_get_description(status));
    return;
  }

  define_set(o, 0, 0, ac, at);
}

static void 
define_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;  

  fts_expression_delete(this->expression);
  fts_array_destroy(&this->descr);
  
  fts_name_set_value(this->patcher, this->name, fts_null);
  fts_atom_release(&this->value);
}

static void
define_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(define_t), define_init, define_delete);
  
  fts_class_message_varargs(cl, fts_s_dump, define_dump);
  fts_class_message_varargs(cl, fts_s_name, define_set_name);
  
  fts_class_message_varargs(cl, fts_s_type, define_set_type);
  fts_class_message_varargs(cl, fts_new_symbol("expression"), define_set_expression);

  fts_class_message_varargs(cl, fts_s_set, define_set);

  fts_class_message_varargs(cl, fts_s_update_real_time, define_update_real_time);
  fts_class_message_varargs(cl, fts_s_update_gui, define_update_gui); 
  fts_class_message_varargs(cl, fts_s_spost_description, define_spost_description);
}

void 
fts_kernel_variable_init(void)
{
  fts_class_install( fts_s_define, define_instantiate);

  sym_type = fts_new_symbol("type");
  sym_const = fts_new_symbol("const");
  sym_arg = fts_new_symbol("arg");
  sym_args = fts_new_symbol("args");
  
  definition_heap = fts_heap_new(sizeof(fts_definition_t));
  definition_listener_heap = fts_heap_new(sizeof(fts_definition_listener_t));
}
