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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>


typedef struct dispatch
{
  fts_object_t  o;
} dispatch_t;

static fts_symbol_t sym__remote_value = 0;

static void
dispatch_values(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send(o, 1, s, ac, at); 
}

static void
dispatch_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* use selector to find target */
  fts_atom_t *a = fts_variable_get_value(fts_object_get_patcher(o), s);

  if(a && fts_is_object(a))
    {
      fts_object_t *obj = fts_get_object(a);
      fts_class_t *class = fts_object_get_class(obj);
      fts_method_t method = fts_class_get_method(class, fts_SystemInlet, sym__remote_value);

      if(method)
	method(obj, fts_SystemInlet, sym__remote_value, ac, at);
      else
	fts_object_signal_runtime_error(o, "cannot dispatch to object %s", fts_symbol_name(s));
    }
  else
    fts_outlet_send(o, 2, s, ac, at);
}

static void
dispatch_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dispatch_t *this = (dispatch_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_fun(at + 0);
  void *propagate_context = fts_get_ptr(at + 1);

  propagate_fun(propagate_context, o, 0);
}

static fts_status_t
dispatch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(dispatch_t), 1, 3, 0);

  fts_method_define_varargs(cl, 0, fts_s_int, dispatch_values);
  fts_method_define_varargs(cl, 0, fts_s_float, dispatch_values);
  fts_method_define_varargs(cl, 0, fts_s_symbol, dispatch_values);
  fts_method_define_varargs(cl, 0, fts_s_list, dispatch_values);

  fts_method_define_varargs(cl, 0, fts_s_anything, dispatch_send);

  fts_class_define_thru(cl, dispatch_propagate_input);

  return fts_Success;
}

void
dispatch_config(void)
{
  sym__remote_value = fts_new_symbol("_remote_value");
  fts_class_install(fts_new_symbol("dispatch"), dispatch_instantiate);
}