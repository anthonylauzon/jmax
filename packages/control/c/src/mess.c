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
  fts_object_t *object;
  fts_symbol_t selector;
  fts_method_t method;
} mess_t;

static void
mess_args(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess_t *this = (mess_t *)o;

  if(this->method)
    this->method(this->object, 0, 0, ac, at);
  else
    fts_outlet_send(o, 0, this->selector, ac, at);
}

static void
mess_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess_t *this = (mess_t *)o;

  if(ac == 1 && fts_get_selector(at) == s)
    mess_args(o, 0, 0, 1, at);
  else
    fts_object_signal_runtime_error(o, "Doesn't understand '%s'", s);
}

static void
mess_set_selector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess_t *this = (mess_t *)o;
  fts_symbol_t selector = fts_get_symbol(at);
  fts_class_t *cl;
  
  this->selector = selector;
  
  if(this->object)
    {
      fts_object_release(this->object);
      
      this->object = 0;
      this->method = 0;
    }
}

static void
mess_set_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess_t *this = (mess_t *)o;

  if(ac == 1 && fts_is_symbol(at))
    mess_set_selector(o, 0, 0, 1, at);
  else if(ac >= 2 && fts_is_object(at) && fts_is_symbol(at + 1))
    {
      fts_object_t *object = fts_get_object(at);
      fts_symbol_t selector = fts_get_symbol(at + 1);

      if(object != this->object || selector != this->selector)
	{
	  fts_class_t *class = fts_object_get_class(object);
	  fts_method_t method = fts_class_get_method(class, 0, selector);
	  
	  if(method)
	    {
	      if(this->object)
		fts_object_release(this->object);
	      
	      this->object = object;
	      
	      fts_object_refer(object);
	      
	      this->selector = selector;
	      this->method = method;
	    }
	  else
	    {
	      fts_symbol_t classname = fts_object_get_class_name(object);
	      fts_object_signal_runtime_error(o, "class %s doesn't understand %s", 
					      classname, selector);
	    }
	}
    }
  else
    fts_object_signal_runtime_error(o, "wrong arguments");
}

/************************************************************
 *
 *  class
 *
 */
static void
mess_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  mess_t *this = (mess_t *)o;
  
  ac--;
  at++;

  this->selector = 0;
  this->object = 0;
  this->method = 0;

  if(ac > 0)
    mess_set_method(o, 0, 0, ac, at);
  
  if(this->selector == 0)
    fts_object_set_error(o, "Wrong arguments");
}

static void
mess_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  mess_t *this = (mess_t *)o;

  if(this->object)
    fts_object_release(this->object);
}
  
static fts_status_t
mess_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(mess_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, mess_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, mess_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang, mess_args);
  fts_method_define_varargs(cl, 0, fts_s_int, mess_args);
  fts_method_define_varargs(cl, 0, fts_s_float, mess_args);
  fts_method_define_varargs(cl, 0, fts_s_symbol, mess_args);
  fts_method_define_varargs(cl, 0, fts_s_list, mess_args);
  fts_method_define_varargs(cl, 0, fts_s_anything, mess_anything);

  fts_method_define_varargs(cl, 1, fts_s_symbol, mess_set_selector);
  fts_method_define_varargs(cl, 1, fts_s_list, mess_set_method);

  return fts_Success;
}

void
mess_config(void)
{
  fts_class_install(fts_new_symbol("mess"), mess_instantiate);
}
