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

#include "fts.h"

static fts_symbol_t sym_val = 0;

/********************************************************************
 *
 *   object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_atom_t a;		
} val_t;

static void
val_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  val_t *this = (val_t *)o;

  this->a = at[1];
}

static void
val_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  val_t *this = (val_t *) o;

  fts_outlet_send(o, 0, fts_get_selector(&this->a), 1, &this->a);
}

static void
val_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  val_t *this = (val_t *) o;
  
  this->a = at[0];
}

static void
val_store_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  val_t *this = (val_t *) o;
  
  this->a = at[0];

  fts_outlet_send(o, 0, fts_get_selector(&this->a), 1, &this->a);
}

/********************************************************************
 *
 *   class
 *
 */

/* when defining a variable: daemon for getting the property "state". */
static void
val_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  val_t *this = (val_t *) obj;
}

static fts_status_t
val_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 2)
    {
      fts_symbol_t selector = fts_get_selector(at + 1);

      fts_class_init(cl, sizeof(val_t), 2, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, val_init);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, val_bang);

      if(fts_is_number(at + 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_int, val_store_and_output);
	  fts_method_define_varargs(cl, 0, fts_s_float, val_store_and_output);
	  fts_method_define_varargs(cl, 1, fts_s_int, val_store);
	  fts_method_define_varargs(cl, 1, fts_s_float, val_store);
	}
      else if(fts_is_symbol(at + 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_symbol, val_store_and_output);
	  fts_method_define_varargs(cl, 1, fts_s_symbol, val_store);
	  
	}
      else
	return &fts_CannotInstantiate;

      fts_outlet_type_define(cl, 0, selector, 1, &fts_get_type(at + 1));
      
      /* fts_class_add_daemon(cl, obj_property_get, fts_s_state, val_get_state); */
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

static fts_object_t *
val_doctor_set_default_arg_zero(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_atom_t a[2];
  int i;

  if(ac == 1)
    {
      a[0] = at[0];
      fts_set_int(a + 1, 0);
      
      obj = fts_eval_object_description(patcher, 2, a);

      return obj;
    }
  else
    return 0;
}

void
val_config(void)
{
  sym_val = fts_new_symbol("val");

  fts_register_object_doctor(fts_new_symbol("val"), val_doctor_set_default_arg_zero);
  fts_metaclass_install(sym_val, val_instantiate, fts_arg_type_equiv);
}
