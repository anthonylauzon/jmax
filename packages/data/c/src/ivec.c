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

static fts_symbol_t sym_ascii = 0;

/********************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t ob;
  int_vector_t *vec;
} ivec_t;

static void
ivec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  int_vector_t *vec = int_vector_create(ac - 1, at + 1);

  this->vec = vec;
  int_vector_refer(vec);
  int_vector_set_creator(vec, o);
}

static void
ivec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
ivec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((ivec_t *)o)->vec;

  int_vector_set_const(vec, 0.0f);
}

static void
ivec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((ivec_t *)o)->vec;
  int constant = fts_get_int_arg(ac, at, 0, 0);

  int_vector_set_const(vec, constant);
}

static void
ivec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((ivec_t *)o)->vec;

  if(ac == 0)
    {
      int size = int_vector_get_size(vec);

      fts_outlet_int(o, 0, size);
    }
  else if(ac == 1 && fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      if(size >= 0)
	{
	  int_vector_set_size(vec, size);
	  fts_outlet_int(o, 0, size);
	}
    }
}

static void
ivec_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  ivec_t *this = (ivec_t *)o;
  enum {ascii} file_type = ascii;
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_name;
  int n_read = 0;

  if(file_format == sym_ascii)
    {
      file_type = ascii;
      ac--;
      at++;
    }      

  file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if(!file_name)
    {
      post("ivec import: no file name given\n");
      return;
    }

  if(file_type == ascii)
    n_read = int_vector_import_ascii(this->vec, file_name);

  if(n_read > 0)
    fts_outlet_int(o, 0, n_read);
  else
    post("ivec: can not import int vector from file \"%s\"\n", fts_symbol_name(file_name));
}

/********************************************************************
 *
 *  class
 *
 */

static void
ivec_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "vector of ints");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  /* fts_object_blip(o, "no comment"); */
	  break;
	}
    }
  else if (cmd == fts_s_outlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "<int>: size");
	  break;
	}
    }
}

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

static fts_status_t
ivec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if(int_vector_get_constructor(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(ivec_t), 1, 1, 0);
  
      /* init / delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ivec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, ivec_delete);
  
      /* get data for editor */
      fts_class_add_daemon(cl, obj_property_get, fts_s_data, ivec_get_data);

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, ivec_get_state);

      /* user methods */
      fts_method_define(cl, 0, fts_new_symbol("clear"), ivec_clear, 0, 0);
      fts_method_define(cl, 0, fts_new_symbol("fill"), ivec_fill, 1, a);
      
      fts_method_define_varargs(cl, 0, fts_new_symbol("size"), ivec_size);
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), ivec_import);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), ivec_assist); 

      /* size outlet */
      fts_outlet_type_define(cl, 0, fts_s_int, 1, &fts_s_int);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

int
ivec_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (int_vector_get_constructor(ac0 - 1, at0 + 1) == int_vector_get_constructor(ac1 - 1, at1 + 1));
}

void
ivec_config(void)
{
  sym_ascii = fts_new_symbol("ascii");

  fts_metaclass_install(fts_new_symbol("ivec"), ivec_instantiate, ivec_equiv);
}
