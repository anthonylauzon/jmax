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
#include <ftsconfig.h>

#include <stdlib.h>
#include <string.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

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

  if(!fts_atom_identical(value, &def->value))
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

static fts_status_description_t define_status_arg_error_descr = {"int index and default value required"};
static fts_status_t define_status_arg_error = &define_status_arg_error_descr;

static fts_status_description_t define_status_empty_error_descr = {"empty definition"};
static fts_status_t define_status_empty_error = &define_status_empty_error_descr;

enum define_type {define_none, define_const, define_arg, define_args};

typedef struct
{
  fts_object_t o;
  fts_symbol_t type;
  fts_symbol_t name;
  fts_atom_t value;
  fts_patcher_t *patcher;
  fts_array_t descr;
  fts_expression_t *expression;
  fts_symbol_t string;
  fts_status_t error;
} define_t;

static fts_symbol_t sym_expression = 0;
static fts_symbol_t sym_const = 0;
static fts_symbol_t sym_arg = 0;
static fts_symbol_t sym_args = 0;

static fts_memorystream_t *define_memory_stream ;

static fts_memorystream_t * define_get_memory_stream()
{
  if(!define_memory_stream)
    define_memory_stream = (fts_memorystream_t *)fts_object_create(fts_memorystream_class, 0, 0);

  return define_memory_stream;
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
          return define_status_arg_error;

        /* assign argument or given default value */
        if(index < size)
          fts_atom_assign(&this->value, atoms + index);
        else
          fts_atom_assign(&this->value, at + 1);
      }
      else
        return define_status_arg_error;
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
      return fts_status_new("unknown definition type");
  }

  return fts_ok;
}

static void
define_evaluate(define_t *this)
{
  int ac = fts_array_get_size(&this->descr);
  const fts_atom_t *at = fts_array_get_atoms(&this->descr);
  fts_status_t error = fts_expression_set(this->expression, ac, at);

  fts_set_void(&this->value);

  /* evaluate expression */
  if(error == fts_ok)
  {
    fts_patcher_t *patcher = this->patcher;
    error = fts_expression_reduce(this->expression, patcher, 0, NULL, define_expression_callback, this);

    if(error == fts_ok)
    {
      /* reset definition */
      if(this->name != fts_s_empty_string && !fts_is_void(&this->value))
        fts_name_set_value(this->patcher, this->name, fts_null);
    }
  }

  /* update gui */
  if(error != this->error)
  {
    fts_atom_t a;

    this->error = error;

    if(error != fts_ok)
    {
      /* set error */
      fts_set_string(&a, (char *)fts_status_get_description(error));
      fts_client_send_message((fts_object_t *)this, fts_s_error, 1, &a);
    }
    else
      fts_client_send_message((fts_object_t *)this, fts_s_error, 0, 0); /* reset error */
  }
}

static void
define_set_type(define_t *this, fts_symbol_t type)
{
  fts_atom_t a;

  this->type = type;

  /* set type at client */
  fts_set_symbol(&a, this->type);
  fts_client_send_message((fts_object_t *)this, fts_s_type, 1, &a);
}

static void
define_set_expression(define_t *this, int ac, const fts_atom_t *at)
{
  fts_memorystream_t *stream = define_get_memory_stream();
  fts_atom_t a;

  fts_array_set(&this->descr, ac, at);

  if(ac > 0)
  {
    /* set name of object */
    fts_memorystream_reset(stream);
    fts_spost_object_description_args((fts_bytestream_t *)stream, ac, (fts_atom_t *)at);
    fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');

    this->string = fts_new_symbol((char *)fts_memorystream_get_bytes(stream));
  }
  else
    this->string = fts_s_empty_string;

  /* set expression at client */
  fts_set_symbol(&a, this->string);
  fts_client_send_message((fts_object_t *)this, sym_expression, 1, &a);
}

static void
define_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;

  if(fts_is_symbol(at))
    define_set_type(this, fts_get_symbol(at));

  define_evaluate(this);
}

static void
define_expression(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *)o;

  define_set_expression(this, ac, at);

  define_evaluate(this);
}

static void
define_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *)o;

  if(fts_is_symbol(at))
    define_set_type(this, fts_get_symbol(at));

  define_set_expression(this, ac - 1, at + 1);

  define_evaluate(this);
}

static void
define_name(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *)o;
  fts_atom_t a;

  /* reset definition */
  if(this->name != fts_s_empty_string && !fts_is_void(&this->value))
    fts_name_set_value(this->patcher, this->name, fts_null);

  /* set_name */
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t name = fts_name_get_unused(this->patcher, fts_get_symbol(at));

    if(name != fts_s_empty_string)
    {
      this->name = name;

      /* set definition */
      if(!fts_is_void(&this->value))
        fts_name_set_value(this->patcher, this->name, &this->value);

      /* set name at client */
      fts_set_symbol(&a, name);
      fts_client_send_message(o, fts_s_name, 1, &a);
    }
  }
}

static void
define_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = fts_dumper_message_new(dumper, fts_s_set);
  fts_atom_t a;

  fts_message_append_symbol(mess, this->type);
  fts_message_append(mess, fts_array_get_size( &this->descr), fts_array_get_atoms(&this->descr));
  fts_dumper_message_send(dumper, mess);

  if(this->name != fts_s_empty_string)
  {
    fts_set_symbol(&a, this->name);
    fts_dumper_send(dumper, fts_s_name, 1, &a);
  }
}

static void
define_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;
  fts_atom_t a;

  fts_set_symbol(&a, this->type);
  fts_client_send_message(o, fts_s_type, 1, &a);

  if(this->string != NULL)
  {
    /* set expression at client */
    fts_set_symbol(&a, this->string);
    fts_client_send_message((fts_object_t *)this, sym_expression, 1, &a);
  }

  if(this->name != fts_s_empty_string)
  {
    fts_set_symbol(&a, this->name);
    fts_client_send_message(o, fts_s_name, 1, &a);
  }

  if(this->error != fts_ok)
  {
    fts_set_string(&a, (char *)fts_status_get_description(this->error));
    fts_client_send_message((fts_object_t *)this, fts_s_error, 1, &a);
  }
}

static void
define_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *)o;

  this->type = sym_const;
  this->name = fts_s_empty_string;
  fts_set_void(&this->value);
  this->patcher = fts_object_get_patcher(o);
  this->string = NULL;

  /* expression description */
  fts_array_init(&this->descr, 0, 0);

  this->error = define_status_empty_error;

  fts_expression_new(0, 0, &this->expression);
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
  fts_class_message_varargs(cl, fts_s_name, define_name);

  fts_class_message_varargs(cl, fts_s_type, define_type);
  fts_class_message_varargs(cl, sym_expression, define_expression);

  fts_class_message_varargs(cl, fts_s_set, define_set);

  fts_class_message_varargs(cl, fts_s_update_gui, define_update_gui);
}

void
fts_kernel_variable_init(void)
{
  fts_class_install( fts_s_define, define_instantiate);

  sym_expression = fts_new_symbol("expression");
  sym_const = fts_new_symbol("const");
  sym_arg = fts_new_symbol("arg");
  sym_args = fts_new_symbol("args");

  definition_heap = fts_heap_new(sizeof(fts_definition_t));
  definition_listener_heap = fts_heap_new(sizeof(fts_definition_listener_t));
}
