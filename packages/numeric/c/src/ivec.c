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
#include "intvec.h"

typedef struct 
{
  fts_object_t ob;
  int_vector_t *vec; /* integer vec */
  fts_atom_t buf;
} ivec_t;

static void
ivec_init_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at + 1);

  this->vec = vec;
  int_vector_refer(vec);

  fts_set_void(&this->buf);
}

static void
ivec_init_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int size = fts_get_int_arg(ac, at, 1, 0);
  int_vector_t *vec = int_vector_new(size);

  this->vec = vec;
  int_vector_refer(vec);
  int_vector_set_creator(vec, o);

  fts_set_void(&this->buf);
}

static void
ivec_delete_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  int_vector_release(this->vec);
}

static void
ivec_delete_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  int_vector_release(this->vec);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
ivec_set_buffer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;

  this->buf = at[0];
}

static void
ivec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int_vector_t *vec = this->vec;
  int index = fts_get_int_arg(ac, at, 0, 0);
  int size = int_vector_get_size(this->vec);

  if(index >= 0 && index < int_vector_get_size(this->vec))
    {      
      if(fts_is_number(&this->buf))
	{
	  int_vector_set_element(vec, index, fts_get_number_int(&this->buf));
	  fts_set_void(&this->buf);
	}
      else
	fts_outlet_int(o, 0, int_vector_get_element(vec, index));
    }
}


/* set by atom list */
static void
ivec_set_from_atom_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((ivec_t *)o)->vec;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    int_vector_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

static void
ivec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((ivec_t *)o)->vec;

  int_vector_set_const(vec, 0);
}

static void
ivec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((ivec_t *)o)->vec;
  int constant = fts_get_int_arg(ac, at, 0, 0);

  int_vector_set_const(vec, constant);
}

static void
ivec_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((ivec_t *)o)->vec;
  int size = fts_get_int_arg(ac, at, 0, -1);

  if(size >= 0)
    int_vector_set_size(vec, size);
}

static void
ivec_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "table of integers");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "<int>: index to recall/store value from/to table");
	  break;
	case 1:
	  fts_object_blip(o, "<int>: set value to be stored");
	}
    }
  else if (cmd == fts_s_outlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "<int>: recalled value", n);
	  break;
	case 1:
	  fts_object_blip(o, "<int>: # of values read from file");
	}
    }
}

/********************************************************************
 *
 *   class
 *
 */

static void
ivec_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  ivec_t *this = (ivec_t *)obj;

  int_vector_atom_set(value, this->vec);
}

static void
ivec_get_data(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  ivec_t *this = (ivec_t *)obj;

  fts_set_data(value, (fts_data_t *)this->vec);
}

static void
ivec_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((ivec_t *)o)->vec;
  fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);
  
  int_vector_save_bmax(vec, f);
}

static fts_status_t
ivec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(ivec_t), 2, 2, 0);
  
  /* get data for editor */
  fts_class_add_daemon(cl, obj_property_get, fts_s_data, ivec_get_data);

  if(ac == 2 && int_vector_atom_is(at + 1))
    {
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ivec_init_refer);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, ivec_delete_define);
    }
  else if(int_vector_get_constructor(ac - 1, at + 1))
    {
      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, ivec_get_state);

      /* .bmax load and save */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, ivec_set_from_atom_list);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, ivec_save_bmax);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ivec_init_define);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, ivec_delete_define);
    }
  else
    return &fts_CannotInstantiate;
  
  /* user methods */
  fts_method_define_number(cl, 0, ivec_index);
  fts_method_define_number(cl, 1, ivec_set_buffer);

  fts_method_define_varargs(cl, 0, fts_s_set, ivec_set_from_atom_list);

  fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), ivec_fill);
  fts_method_define_varargs(cl, 0, fts_s_clear, ivec_clear);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("size"), ivec_resize);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), ivec_assist); 

  fts_outlet_type_define(cl, 0, fts_s_int, 1, &fts_s_int);

  return fts_Success;
}

int
ivec_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (
	  int_vector_get_constructor(ac0 - 1, at0 + 1) == int_vector_get_constructor(ac1 - 1, at1 + 1) &&
	  (ac0 == 2 && int_vector_atom_is(at0 + 1)) == (ac1 == 2 && int_vector_atom_is(at1 + 1)));
}

void
ivec_config(void)
{
  fts_metaclass_install(fts_new_symbol("ivec"), ivec_instantiate, ivec_equiv);
}
