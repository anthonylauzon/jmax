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
#include "vector.h"

static fts_symbol_t sym_text = 0;

/******************************************************************
 *
 *  object vec
 *
 *  vector of atoms
 *
 */

typedef struct 
{
  fts_object_t ob;
  vector_t *vec; /* atom vector */
  fts_atom_t buf;
} vec_t;

static void
vec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  this->vec = vector_create(ac - 1, at + 1);
  vector_refer(this->vec);

  fts_set_void(&this->buf);
}

static void
vec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  vector_release(this->vec);
}
/********************************************************************
 *
 *   user methods
 *
 */

static void
vec_send(fts_object_t *o, vector_t *vec)
{
  fts_atom_t a;
  
  vector_atom_set(&a, vec);
  fts_outlet_send(o, 0, vector_symbol, 1, &a);  
}

static void
vec_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  vec_send(o, this->vec);
}

static void
vec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  vector_t *vec = (vector_t *)this->vec;
  int size = vector_get_size(vec);
  
  if(ac > 0 && fts_is_number(at))
    {
      int i = fts_get_number_int(at);

      if(i >= 0 && i < size)
	vector_void_element(vec, i);
    }
  else
    vector_void(vec);
}

static void
vec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  vector_t *vec = (vector_t *)this->vec;

  if(ac > 0)
    vector_fill(vec, at[0]);
}

static void
vec_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  vector_t *vec = (vector_t *)this->vec;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int size = vector_get_size(vec);
      int i = fts_get_number_int(at);

      if(i >= 0 && i < size)
	{
	  vector_set_from_atom_list(vec, i, ac - 2, at + 2);
	}
    }
}

static void
vec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vector_t *vec = ((vec_t *)o)->vec;
  int size = 0;

  if(ac > 0 && fts_is_number(at))
    {
      size = fts_get_number_int(at);
      
      if(size >= 0)
	vector_set_size(vec, size);
    }  
  
  if(size >= 0)
    vec_send(o, vec);
}

static void
vec_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);
  vector_t *vec = this->vec;

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      int size = 0;

      size = vector_read_atom_file(vec, file_name);

      if(size >= 0)
	vec_send(o, vec);
      else
	post("vec: can not import from text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("vec: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
vec_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);
  vector_t *vec = this->vec;

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      int size = 0;

      size = vector_write_atom_file(vec, file_name);

      if(size < 0)
	post("vec: can not export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("vec: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
vec_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "vector of values");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "vector commands");
	  break;
	}
    }
  else if (cmd == fts_s_outlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "<vec>: the vector", n);
	  break;
	}
    }
}

/********************************************************************
 *
 *   class
 *
 */

static void
vec_get_vector(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  vec_t *this = (vec_t *)obj;

  vector_atom_set(value, this->vec);
}

static fts_status_t
vec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if(vector_get_constructor(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(vec_t), 1, 1, 0);
  
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, vec_delete);

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, vec_get_vector);

      /* .bmax load and save */
      /*fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, vec_set_from_atom_list);*/
      /*fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, vec_save_bmax);*/

      /* help */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), vec_assist); 
      
      /* user methods */
      fts_method_define_varargs(cl, 0, fts_s_bang, vec_output); 
      
      fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), vec_clear);
      fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), vec_fill);      
      fts_method_define_varargs(cl, 0, fts_new_symbol("set"), vec_set);

      fts_method_define_varargs(cl, 0, fts_new_symbol("size"), vec_size);
            
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), vec_import); 
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), vec_export); 

      /* type outlet */
      fts_outlet_type_define(cl, 0, vector_symbol, 1, &vector_type);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;

}

static int
vec_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (vector_get_constructor(ac0 - 1, at0 + 1) == vector_get_constructor(ac1 - 1, at1 + 1));
}

void
vec_config(void)
{
  sym_text = fts_new_symbol("text");

  fts_metaclass_install(fts_new_symbol("vec"), vec_instantiate, vec_equiv);
}
