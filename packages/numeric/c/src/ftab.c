/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

typedef struct 
{
  fts_object_t ob;
  fts_float_vector_t *vec; /* float vec */
  fts_atom_t buf;
} ftab_t;

static void
ftab_init_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftab_t *this = (ftab_t *)o;

  this->vec = (fts_float_vector_t *)fts_get_data(at + 1);
  fts_set_void(&this->buf);
}

static void
ftab_init_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftab_t *this = (ftab_t *)o;
  int size = fts_get_int_arg(ac, at, 1, 0);

  this->vec = fts_float_vector_new(size);
  fts_set_void(&this->buf);
}

static void
ftab_delete_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftab_t *this = (ftab_t *)o;

  fts_float_vector_delete(this->vec);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
ftab_set_buffer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftab_t *this = (ftab_t *)o;

  this->buf = at[0];
}

static void
ftab_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftab_t *this = (ftab_t *)o;
  fts_float_vector_t *vec = (fts_float_vector_t *)this->vec;
  int index = fts_get_int_arg(ac, at, 0, 0);
  int size = fts_float_vector_get_size((fts_float_vector_t *)this->vec);

  if(index >= 0 && index < fts_float_vector_get_size(this->vec))
    {      
      if(fts_is_number(&this->buf))
	{
	  fts_float_vector_set_element(vec, index, fts_get_number_float(&this->buf));
	  fts_set_void(&this->buf);
	}
      else
	fts_outlet_float(o, 0, fts_float_vector_get_element(vec, index));
    }
}


/* set by atom list */
static void
ftab_set_from_atom_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((ftab_t *)o)->vec;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    fts_float_vector_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

static void
ftab_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((ftab_t *)o)->vec;

  fts_float_vector_set_const(vec, 0.0f);
}

static void
ftab_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((ftab_t *)o)->vec;
  float constant = fts_get_float_arg(ac, at, 0, 0);

  fts_float_vector_set_const(vec, constant);
}

static void
ftab_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((ftab_t *)o)->vec;
  int size = fts_get_int_arg(ac, at, 0, -1);

  if(size >= 0)
    fts_float_vector_set_size(vec, size);
}

static void
ftab_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
	  fts_object_blip(o, "<number>: index to recall/store value from/to table");
	  break;
	case 1:
	  fts_object_blip(o, "<number>: set value to be stored");
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
	  fts_object_blip(o, "<int>: # of values read from file (= current table size)");
	}
    }
}

/********************************************************************
 *
 *   class
 *
 */

static void
ftab_get_float_vector(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  ftab_t *this = (ftab_t *)obj;

  fts_data_set_name((fts_data_t *)this->vec, fts_object_get_variable(obj));
  fts_set_data(value, (fts_data_t *)this->vec);
}

static void
ftab_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((ftab_t *)o)->vec;
  fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);
  
  fts_float_vector_save_bmax(vec, f);
}

static fts_status_t
ftab_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(ftab_t), 2, 2, 0);
  
  /* get data for editor */
  fts_class_add_daemon(cl, obj_property_get, fts_s_data, ftab_get_float_vector);

  if(ac > 1 && fts_is_data(at + 1) && fts_data_is(fts_get_data(at + 1), fts_s_float_vector))
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ftab_init_refer);
  else if(ac > 1 && fts_is_int(at + 1))
    {
      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, ftab_get_float_vector);

      /* .bmax load and save */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, ftab_set_from_atom_list);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, ftab_save_bmax);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ftab_init_define);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, ftab_delete_define);
    }
  else
    return &fts_CannotInstantiate;
  
  /* user methods */
  fts_method_define_number(cl, 0, ftab_index);
  fts_method_define_number(cl, 1, ftab_set_buffer);

  fts_method_define_varargs(cl, 0, fts_s_set, ftab_set_from_atom_list);

  fts_method_define(cl, 0, fts_new_symbol("fill"), ftab_fill, 1, a);
  fts_method_define(cl, 0, fts_s_clear, ftab_clear, 0, 0);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("size"), ftab_resize);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), ftab_assist); 

  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

void
ftab_config(void)
{
  fts_metaclass_install(fts_new_symbol("ftab"), ftab_instantiate, fts_arg_type_equiv);
}
