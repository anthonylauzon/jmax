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

static fts_symbol_t sym_argument = 0;

/********************************************************************
 *
 *   object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_atom_t *arg;
  fts_atom_t def;
} argument_t;

static void
argument_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  argument_t *this = (argument_t *)o;

  if((ac == 1 || ac == 2) && fts_is_int(at))
    {
      fts_tuple_t *args = fts_patcher_get_args(fts_object_get_patcher(o));
      fts_atom_t *ptr = fts_tuple_get_atoms(args);
      int size = fts_tuple_get_size(args);
      int index;
      
      fts_set_void(&this->def);
      
      index = fts_get_int(at);
      
      if(args && index < size)
	this->arg = ptr + index;
      else if(ac > 1)
	{
	  fts_atom_assign(&this->def, at + 1);
	  
	  this->arg = &this->def;
	}
      else
	{
	  fts_object_set_error(o, "Argument %d is not defined for this patcher", index);
	  return;
	}

      fts_variable_add_user(fts_object_get_patcher(o), fts_s_args, o);
    }
  else
    fts_object_set_error(o, "Wrong arguments");
}

static void
argument_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  argument_t *this = (argument_t *)o;

  fts_set_void(&this->def);
}

static void
argument_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  argument_t *this = (argument_t *) o;

  fts_outlet_atom(o, 0, this->arg);
}

/********************************************************************
 *
 *   class
 *
 */

/* when defining a variable: daemon for getting the property "state". */
static void
argument_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  argument_t *this = (argument_t *) obj;

  *value = *(this->arg);
}

static fts_status_t
argument_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(argument_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, argument_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, argument_delete);
  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, argument_get_state);
  
  return fts_ok;
}

void
argument_config(void)
{
  fts_metaclass_t *mcl;

  sym_argument = fts_new_symbol("argument");

  mcl = fts_class_install(sym_argument, argument_instantiate);
  fts_metaclass_alias(mcl, fts_new_symbol("arg"));
}
