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
} mess_t;

static void
mess_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess_t *this = (mess_t *)o;

  if(this->selector != NULL)
    fts_outlet_send(o, 0, this->selector, ac, at);
  else 
    {
      if(fts_is_symbol(at))
	fts_outlet_send(o, 0, fts_get_symbol(at), ac - 1, at + 1);
      else
	fts_object_signal_runtime_error(o, "Tuple doesn't start with a symbol");
    }
}

static void
mess_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess_t *this = (mess_t *)o;

  if(ac == 1 && fts_get_selector(at) == s)
    mess_atoms(o, 0, 0, 1, at);
  else
    fts_object_signal_runtime_error(o, "Don't understand message %s", s);
}

static void
mess_set_selector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess_t *this = (mess_t *)o;
  fts_symbol_t selector = fts_get_symbol(at);
  
  this->selector = selector;
}

static void
mess_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  mess_t *this = (mess_t *)o;
  
  this->selector = NULL;

  if(ac > 0 && fts_is_symbol(at))
    mess_set_selector(o, 0, 0, 1, at);
}

static fts_status_t
mess_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 0)
    {
      fts_class_init(cl, sizeof(mess_t), 2, 1, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, mess_init);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_int, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_float, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_symbol, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_list, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_anything, mess_anything);
      
      fts_method_define_varargs(cl, 1, fts_s_symbol, mess_set_selector);
    }
  else
    {
      fts_class_init(cl, sizeof(mess_t), 1, 1, 0);
  
      fts_method_define_varargs(cl, 0, fts_s_int, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_float, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_symbol, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_list, mess_atoms);
      fts_method_define_varargs(cl, 0, fts_s_anything, mess_anything);      
    }
      
  return fts_Success;
}

void
mess_config(void)
{
  fts_metaclass_install(fts_new_symbol("mess"), mess_instantiate, fts_arg_equiv);
}
