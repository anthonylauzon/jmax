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

static void
dispatch_values(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_varargs(o, 1, ac, at);
}

static void
dispatch_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* use selector to find target */
  fts_atom_t *a = fts_name_get_value(fts_object_get_patcher(o), s);

  if(a && fts_is_object(a))
    {
      fts_object_t *obj = fts_get_object(a);
      fts_class_t *class = fts_object_get_class(obj);
      fts_method_t method = NULL;;

      if(method)
	method(obj, fts_system_inlet, fts_s_send, ac, at);
      else
	fts_object_signal_runtime_error(o, "cannot dispatch to object %s", s);
    }
  else
    fts_outlet_send(o, 2, s, ac, at);
}

static void
dispatch_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);

  propagate_fun(propagate_context, o, 0);
}

static void
dispatch_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dispatch_t), NULL, NULL);

  fts_class_message_varargs(cl, fts_s_propagate_input, dispatch_propagate_input);

  fts_class_input_handler(cl, dispatch_send);

  fts_dsp_declare_outlet(cl, 0);
  fts_class_outlet_varargs(cl, 1);
  fts_class_outlet_thru(cl, 2);
}

void
dispatch_config(void)
{
  fts_class_install(fts_new_symbol("dispatch"), dispatch_instantiate);
}
