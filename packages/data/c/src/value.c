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
#include "value.h"

fts_symbol_t value_symbol = 0;
fts_metaclass_t *value_type = 0;

/********************************************************************
 *
 *   object
 *
 */

static void
value_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;
  
  fts_outlet_atom(o, 0, &this->a);
}

static void
value_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;

  if(ac > 0) 
    fts_atom_assign(&this->a, at);
}

static void
value_set_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_set_value(o, 0, 0, 1, at);
  value_output(o, 0, 0, 1, at);
}

static void
value_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
   
  fts_spost(stream, "(:val ");
  fts_spost_atoms(stream, 1, &this->a);
  fts_spost(stream, ")");
}

static void
value_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);

  fts_array_set(array, 1, &this->a);
}

static void
value_set_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;

  fts_atom_assign(&this->a, at);
}

static void
value_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;
  value_t *in = (value_t *)fts_get_object(at);
  
  fts_atom_assign(&this->a, &in->a);
}

static void
value_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);

  fts_dumper_send(dumper, fts_s_set, 1, &this->a);
}

/********************************************************************
 *
 *   class
 *
 */

static void
value_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;

  fts_set_void(&this->a);
  data_object_set_keep((data_object_t *)o, fts_s_no);

  if(ac > 0)
    {
      value_set_value(o, 0, 0, 1, at);
      data_object_set_keep((data_object_t *)o, fts_s_args);
    }
}

static void
value_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *) o;

  fts_set_void(&this->a);
}

static void
value_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

static fts_status_t
value_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(value_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, value_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, value_delete);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_post, value_post);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set_from_instance, value_set_from_instance);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_dump, value_dump);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_get_array, value_get_array);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set_from_array, value_set_from_array);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set, value_set_value);

  fts_method_define_varargs(cl, 0, fts_s_bang, value_output);
  fts_method_define_varargs(cl, 0, fts_s_anything, value_set_and_output);

  fts_method_define_varargs(cl, 1, fts_s_anything, value_set_value);

  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, data_object_daemon_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, data_object_daemon_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, value_get_state);
  
  return fts_ok;
}

void
value_config(void)
{
  value_symbol = fts_new_symbol("value");

  value_type = fts_class_install(value_symbol, value_instantiate);
  fts_metaclass_alias(value_type, fts_new_symbol("val"));
}
