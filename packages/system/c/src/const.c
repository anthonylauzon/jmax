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

static fts_symbol_t sym_const = 0;

typedef struct
{
  fts_object_t o;
  fts_atom_t a;		
} const_t;

static void
const_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const_t *this = (const_t *)o;

  if(ac > 1)
    {
      fts_object_t *tuple = fts_object_create(fts_tuple_metaclass, ac, at);
      
      fts_object_refer(tuple);
      fts_set_object(&this->a, tuple);
    }
  if(ac > 0)
    fts_atom_assign(&this->a, at);
  else
    fts_object_set_error(o, "No value given");
}

static void
const_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const_t *this = (const_t *) o;
}

static void
const_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const_t *this = (const_t *) o;

  fts_outlet_atom(o, 0, &this->a);
}

static void
const_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  const_t *this = (const_t *) obj;

  *value = this->a;
}

static fts_status_t
const_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(const_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, const_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, const_delete);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, const_bang);
  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, const_get_state);
  
  return fts_ok;
}

void
const_config(void)
{
  sym_const = fts_new_symbol("const");

  fts_class_install(sym_const, const_instantiate);
}
