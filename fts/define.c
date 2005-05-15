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
#include <ftsprivate/patcher.h>

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
  define_t *self = (define_t *)data;

  if(ac > 0)
  {
    if(self->type == sym_const)
    {
      if(ac > 1)
      {
        fts_object_t *tuple = fts_object_create(fts_tuple_class, ac, at);
        fts_atom_t a;

        fts_set_object(&a, tuple);
        fts_atom_assign(&self->value, &a);
      }
      else
        fts_atom_assign(&self->value, at);

      return fts_ok;
    }
    else if(self->type == sym_arg)
    {
      /* define argument with default value */
      if(ac == 2)
      {
        fts_tuple_t *args = fts_patcher_get_args(fts_patcher_get_scope(self->patcher));
        int size = fts_tuple_get_size(args);
        fts_atom_t *atoms = fts_tuple_get_atoms(args);
        int index;

        /* get index */
        if(fts_is_int(at))
          index = fts_get_int(at);
        else
          return define_status_arg_error;

        /* assign argument or given default value */
        if(index < size)
          fts_atom_assign(&self->value, atoms + index);
        else
          fts_atom_assign(&self->value, at + 1);
      }
      else
        return define_status_arg_error;
    }
    else if(self->type == sym_args)
    {
      fts_tuple_t *args = fts_patcher_get_args(fts_patcher_get_scope(self->patcher));
      int size = fts_tuple_get_size(args);
      fts_atom_t *atoms = fts_tuple_get_atoms(args);

      /* concat args and default values */
      if(ac > 1 || size > 1)
      {
        fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, size, atoms);
        fts_atom_t a;

        if(ac > size)
          fts_tuple_append(tuple, ac - size, at + size);

        fts_set_object(&a, (fts_object_t *)tuple);
        fts_atom_assign(&self->value, &a);
      }
      else if(size > 0)
        fts_atom_assign(&self->value, atoms);
      else
        fts_atom_assign(&self->value, at);
    }
    else
      return fts_status_new("unknown definition type");
  }

  return fts_ok;
}

static void
define_evaluate(define_t *self)
{
  int ac = fts_array_get_size(&self->descr);
  const fts_atom_t *at = fts_array_get_atoms(&self->descr);
  fts_status_t error = fts_expression_set(self->expression, ac, at);

  fts_set_void(&self->value);

  /* evaluate expression */
  if(error == fts_ok)
  {
    fts_patcher_t *patcher = self->patcher;
    fts_patcher_t *scope   = fts_patcher_get_scope(patcher);

    error = fts_expression_reduce(self->expression, scope, 0, NULL, define_expression_callback, self);

    if(error == fts_ok)
    {
      if(self->name != fts_s_empty_string && !fts_is_void(&self->value))
      {
	fts_name_set_value(self->patcher, self->name, &self->value);
      }
    }
  }

  /* update gui */
  if(error != self->error)
  {
    fts_atom_t a;

    self->error = error;

    if(error != fts_ok)
    {
      /* set error */
      fts_set_string(&a, (char *)fts_status_get_description(error));
      fts_client_send_message((fts_object_t *)self, fts_s_error, 1, &a);
    }
    else
      fts_client_send_message((fts_object_t *)self, fts_s_error, 0, 0); /* reset error */
  }
}

static void
define_set_type(define_t *self, fts_symbol_t type)
{
  fts_atom_t a;

  self->type = type;

  /* set type at client */
  fts_set_symbol(&a, self->type);
  fts_client_send_message((fts_object_t *)self, fts_s_type, 1, &a);
}

static void
define_set_expression(define_t *self, int ac, const fts_atom_t *at)
{
  fts_memorystream_t *stream = define_get_memory_stream();
  fts_atom_t a;

  fts_array_set(&self->descr, ac, at);

  if(ac > 0)
  {
    /* set name of object */
    fts_memorystream_reset(stream);
    fts_spost_object_description_args((fts_bytestream_t *)stream, ac, (fts_atom_t *)at);
    fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');

    self->string = fts_new_symbol((char *)fts_memorystream_get_bytes(stream));
  }
  else
    self->string = fts_s_empty_string;

  /* set expression at client */
  fts_set_symbol(&a, self->string);
  fts_client_send_message((fts_object_t *)self, sym_expression, 1, &a);
}

static void
define_type(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  define_t *self = (define_t *) o;

  if(fts_is_symbol(at))
    define_set_type(self, fts_get_symbol(at));

  define_evaluate(self);
}

static void
define_expression(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  define_t *self = (define_t *)o;

  define_set_expression(self, ac, at);

  define_evaluate(self);
}

static void
define_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  define_t *self = (define_t *)o;

  if(fts_is_symbol(at))
    define_set_type(self, fts_get_symbol(at));

  define_set_expression(self, ac - 1, at + 1);

  define_evaluate(self);
}

static void
define_name(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  define_t *self = (define_t *)o;
  fts_atom_t a;

  /* reset definition */
  if(self->name != fts_s_empty_string && !fts_is_void(&self->value))
    fts_name_set_value(self->patcher, self->name, fts_null);

  /* set_name */
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t name = fts_name_get_unused(self->patcher, fts_get_symbol(at));

    if(name != fts_s_empty_string)
    {
      self->name = name;

      /* set definition */
      if(!fts_is_void(&self->value))
        fts_name_set_value(self->patcher, self->name, &self->value);

      /* set name at client */
      fts_set_symbol(&a, name);
      fts_client_send_message(o, fts_s_name, 1, &a);
    }
  }
}

static void
define_dump_gui(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  define_t *self = (define_t *) o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = fts_dumper_message_get(dumper, fts_s_set);
  fts_atom_t a;

  fts_message_append_symbol(mess, self->type);
  fts_message_append(mess, fts_array_get_size( &self->descr), fts_array_get_atoms(&self->descr));
  fts_dumper_message_send(dumper, mess);

  if(self->name != fts_s_empty_string)
  {
    fts_set_symbol(&a, self->name);
    fts_dumper_send(dumper, fts_s_name, 1, &a);
  }
}

static void
define_update_gui(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  define_t *self = (define_t *) o;
  fts_atom_t a;

  fts_set_symbol(&a, self->type);
  fts_client_send_message(o, fts_s_type, 1, &a);

  if(self->string != NULL)
  {
    /* set expression at client */
    fts_set_symbol(&a, self->string);
    fts_client_send_message((fts_object_t *)self, sym_expression, 1, &a);
  }

  if(self->name != fts_s_empty_string)
  {
    fts_set_symbol(&a, self->name);
    fts_client_send_message(o, fts_s_name, 1, &a);
  }

  if(self->error != fts_ok)
  {
    fts_set_string(&a, (char *)fts_status_get_description(self->error));
    fts_client_send_message((fts_object_t *)self, fts_s_error, 1, &a);
  }
}

static void
define_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  define_t *self = (define_t *)o;

  self->type = sym_const;
  self->name = fts_s_empty_string;
  fts_set_void(&self->value);
  self->patcher = fts_object_get_patcher(o);
  self->string = NULL;

  /* expression description */
  fts_array_init(&self->descr, 0, 0);

  self->error = define_status_empty_error;

  fts_expression_new(0, 0, &self->expression);
}

static void
define_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  define_t *self = (define_t *) o;

  fts_expression_delete(self->expression);
  fts_array_destroy(&self->descr);

  fts_name_set_value(self->patcher, self->name, fts_null);
  fts_atom_release(&self->value);
}

static void
define_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(define_t), define_init, define_delete);

  fts_class_message_varargs(cl, fts_s_dump_gui, define_dump_gui);
  fts_class_message_varargs(cl, fts_s_name, define_name);

  fts_class_message_varargs(cl, fts_s_type, define_type);
  fts_class_message_varargs(cl, sym_expression, define_expression);

  fts_class_message_varargs(cl, fts_s_set, define_set);

  fts_class_message_varargs(cl, fts_s_update_gui, define_update_gui);
}

void
fts_kernel_define_init(void)
{
  sym_expression = fts_new_symbol("expression");
  sym_const = fts_new_symbol("const");
  sym_arg = fts_new_symbol("arg");
  sym_args = fts_new_symbol("args");
  
  fts_class_install( fts_s_define, define_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
