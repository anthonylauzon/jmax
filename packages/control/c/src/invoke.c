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
 */

#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  fts_symbol_t selector;
  fts_array_t args;
  fts_class_t *class;
  fts_method_t method;
} invoke_t;

static void
invoke_object(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  invoke_t *this = (invoke_t *)o;

  if(fts_is_object(at))
    {
      fts_object_t *object = fts_get_object(at);
      fts_class_t *class = fts_object_get_class(object);

      if(class != this->class)
	{
	  fts_method_t method = fts_class_get_method(class, this->selector);

	  if(method)
	    {
	      this->class = class;
	      this->method = method;
	    }
	  else
	    {
	      fts_symbol_t classname = fts_object_get_class_name(object);

	      fts_object_signal_runtime_error(o, "object of class %s doesn't understand %s", classname, this->selector);

	      this->class = NULL;
	      this->method = NULL;

	      return;
	    }
	}

      this->method(object, 0, 0, fts_array_get_size(&this->args), fts_array_get_atoms(&this->args));
    }
  else
    fts_object_signal_runtime_error(o, "input is not an object");
}

static void
invoke_set_selector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  invoke_t *this = (invoke_t *)o;
  fts_symbol_t selector = fts_get_symbol(at);

  if(this->selector != selector)
    {
      this->selector = selector;
      this->class = 0; /* empty class cash */
    }
}

static void
invoke_set_arguments(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  invoke_t *this = (invoke_t *)o;

  fts_array_set(&this->args, ac, at);
}

static void
invoke_set_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  invoke_t *this = (invoke_t *)o;

  if(ac == 1 && fts_get_selector(at) == s)
    invoke_set_arguments(o, 0, 0, 1, at);
  else
    fts_object_signal_runtime_error(o, "Don't understand message %s", s);
}

/************************************************************
 *
 *  class
 *
 */
static void
invoke_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  invoke_t *this = (invoke_t *)o;
  
  this->selector = NULL;
  this->class = NULL;
  this->method = NULL;
  
  if(ac > 0 && fts_is_symbol(at))
    {
      invoke_set_selector(o, 0, 0, 1, at);
      fts_array_init(&this->args, ac - 1, at + 1);
    }
  else
    fts_object_set_error(o, "First argument of symbol required");
}

static void
invoke_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  invoke_t *this = (invoke_t *)o;

  fts_array_destroy(&this->args);
}
  
static fts_status_t
invoke_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(invoke_t), 3, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, invoke_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, invoke_delete);

  fts_method_define_varargs(cl, 0, fts_s_anything, invoke_object);

  fts_method_define_varargs(cl, 1, fts_s_symbol, invoke_set_selector);

  fts_method_define_varargs(cl, 2, fts_s_int, invoke_set_arguments);
  fts_method_define_varargs(cl, 2, fts_s_float, invoke_set_arguments);
  fts_method_define_varargs(cl, 2, fts_s_symbol, invoke_set_arguments);
  fts_method_define_varargs(cl, 2, fts_s_list, invoke_set_arguments);
  fts_method_define_varargs(cl, 2, fts_s_anything, invoke_set_arguments);

  return fts_ok;
}

void
invoke_config(void)
{
  fts_class_install(fts_new_symbol("invoke"), invoke_instantiate);
}
