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
#include "floatvec.h"

typedef struct 
{
  fts_object_t ob;
  float_vector_t *vec; /* float vec */
  fts_atom_t buf;
} fvec_t;

static void
fvec_init_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at + 1);

  this->vec = vec;
  float_vector_refer(vec);

  fts_set_void(&this->buf);
}

static void
fvec_init_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fts_get_int_arg(ac, at, 1, 0);
  float_vector_t *vec = float_vector_new(size);

  this->vec = vec;
  float_vector_refer(vec);
  float_vector_set_creator(vec, o);

  fts_set_void(&this->buf);
}

static void
fvec_delete_release(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  float_vector_release(this->vec);
}

static void
fvec_delete_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  float_vector_release(this->vec);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
fvec_set_buffer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  this->buf = at[0];
}

static void
fvec_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float_vector_t *vec = this->vec;
  int index = fts_get_int_arg(ac, at, 0, 0);
  int size = float_vector_get_size((float_vector_t *)this->vec);

  if(index >= 0 && index < float_vector_get_size(this->vec))
    {      
      if(fts_is_number(&this->buf))
	{
	  float_vector_set_element(vec, index, fts_get_number_float(&this->buf));
	  fts_set_void(&this->buf);
	}
      else
	fts_outlet_float(o, 0, float_vector_get_element(vec, index));
    }
}


/* set by atom list */
static void
fvec_set_from_atom_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_vector_t *vec = ((fvec_t *)o)->vec;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    float_vector_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

static void
fvec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_vector_t *vec = ((fvec_t *)o)->vec;

  float_vector_set_const(vec, 0.0f);
}

static void
fvec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_vector_t *vec = ((fvec_t *)o)->vec;
  float constant = fts_get_float_arg(ac, at, 0, 0);

  float_vector_set_const(vec, constant);
}

static void
fvec_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_vector_t *vec = ((fvec_t *)o)->vec;
  int size = fts_get_int_arg(ac, at, 0, -1);

  if(size >= 0)
    float_vector_set_size(vec, size);
}

static void
fvec_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "table of floats");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "<int>: index to recall/store value from/to table");
	  break;
	case 1:
	  fts_object_blip(o, "<float>: set value to be stored");
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
fvec_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fvec_t *this = (fvec_t *)obj;

  float_vector_atom_set(value, this->vec);
}

static void
fvec_get_data(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fvec_t *this = (fvec_t *)obj;

  fts_set_data(value, (fts_data_t *)this->vec);
}

static void
fvec_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_vector_t *vec = ((fvec_t *)o)->vec;
  fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);
  
  float_vector_save_bmax(vec, f);
}

static fts_status_t
fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(fvec_t), 2, 2, 0);
  
  /* get data for editor */
  fts_class_add_daemon(cl, obj_property_get, fts_s_data, fvec_get_data);

  if(ac > 1 && float_vector_atom_is(at + 1))
    {
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fvec_init_refer);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fvec_init_define);
    }
  else if(float_vector_get_constructor(ac - 1, at + 1))
    {
      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, fvec_get_state);

      /* .bmax load and save */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, fvec_set_from_atom_list);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, fvec_save_bmax);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fvec_init_define);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fvec_delete_define);
    }
  else
    return &fts_CannotInstantiate;
  
  /* user methods */
  fts_method_define_number(cl, 0, fvec_index);
  fts_method_define_number(cl, 1, fvec_set_buffer);

  fts_method_define_varargs(cl, 0, fts_s_set, fvec_set_from_atom_list);

  fts_method_define(cl, 0, fts_new_symbol("fill"), fvec_fill, 1, a);
  fts_method_define(cl, 0, fts_s_clear, fvec_clear, 0, 0);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("size"), fvec_resize);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), fvec_assist); 

  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

int
fvec_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (
	  float_vector_get_constructor(ac0 - 1, at0 + 1) == float_vector_get_constructor(ac1 - 1, at1 + 1) &&
	  (ac0 == 2 && float_vector_atom_is(at0 + 1)) == (ac1 == 2 && float_vector_atom_is(at1 + 1)));
}

void
fvec_config(void)
{
  fts_metaclass_install(fts_new_symbol("fvec"), fvec_instantiate, fvec_equiv);
}
