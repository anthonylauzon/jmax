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
fts_class_t *value_type = 0;

/********************************************************************
 *
 *   object
 *
 */

static void
value_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;
  fts_atom_t a = this->a;
  
  fts_atom_refer(&a);
  fts_outlet_varargs(o, 0, 1, &a);
  fts_atom_release(&a);
}

static void
value_set_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;

  fts_atom_assign(&this->a, at);
}

static void
value_set_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;

  if(ac == 1)
    value_set_atom(o, winlet, s, 1, at);
  else
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, NULL, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, (fts_object_t *)tuple);
      value_set_atom(o, winlet, s, 1, &a);
    }
}

static void
value_set_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_set_atoms(o, 0, 0, ac, at);
  value_output(o, 0, 0, 0, 0);
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

/********************************************************************
 *
 *   class
 *
 */

static void
value_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *)o;

  data_object_init(o);

  fts_set_int(&this->a, 0);

  if(ac > 0)
    value_set_atom(o, 0, 0, 1, at);
}

static void
value_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *this = (value_t *) o;

  fts_atom_release(&this->a);
}

static void
value_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(value_t), value_init, value_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_dump, fts_name_dump_method);
  fts_class_message_varargs(cl, fts_s_update_gui, fts_name_gui_method);

  fts_class_message_varargs(cl, fts_s_post, value_post);

  fts_class_message_varargs(cl, fts_s_set_from_instance, value_set_from_instance);

  fts_class_message_varargs(cl, fts_s_get_array, value_get_array);
  fts_class_message_varargs(cl, fts_s_set_from_array, value_set_from_array);
  
  fts_class_message_varargs(cl, fts_s_bang, value_output);
  fts_class_message_varargs(cl, fts_s_set, value_set_atom);

  fts_class_inlet_varargs(cl, 0, value_set_and_output);
  fts_class_inlet_varargs(cl, 1, value_set_atoms);

  fts_class_outlet_varargs(cl, 0);
}

void
value_config(void)
{
  value_symbol = fts_new_symbol("value");

  value_type = fts_class_install(value_symbol, value_instantiate);
  fts_class_alias(value_type, fts_new_symbol("val"));
}
