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

static fts_symbol_t sym_text = 0;

/********************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t ob;
  float_vector_t *vec;
} fvec_t;

static void
fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float_vector_t *vec = float_vector_create(ac - 1, at + 1);

  this->vec = vec;
  float_vector_refer(vec);
  float_vector_set_creator(vec, o);
}

static void
fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
fvec_send(fts_object_t *o, float_vector_t *vec)
{
  fts_atom_t a;
  
  float_vector_atom_set(&a, vec);
  fts_outlet_send(o, 0, float_vector_symbol, 1, &a);  
}

static void
fvec_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_send(o, this->vec);
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
fvec_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float_vector_t *fvec = (float_vector_t *)this->vec;

  if(ac > 1 && fts_is_number(at))
    {
      int size = float_vector_get_size(fvec);
      int offset = fts_get_number_int(at);

      if(offset >= 0 && offset < size)
	float_vector_set_from_atom_list(fvec, offset, ac - 1, at + 1);
    }
}

static void
fvec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_vector_t *vec = ((fvec_t *)o)->vec;

  if(ac == 1 && fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      if(size >= 0)
	{
	  float_vector_set_size(vec, size);
	  fvec_send(o, vec);
	}
    }
}

static void
fvec_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);
  float_vector_t *vec = this->vec;
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      size = float_vector_read_atom_file(vec, file_name);
      
      if(size >= 0)
	fvec_send(o, vec);
      else
	post("fvec: can not import from text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("fvec: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
fvec_export(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);
  float_vector_t *vec = this->vec;
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      size = float_vector_write_atom_file(vec, file_name);
      
      if(size < 0)
	post("fvec: can not export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("fvec: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
fvec_load(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if(file_name)
    {
      int onset = fts_get_int_arg(ac, at, 1, 0);
      int n_read = fts_get_int_arg(ac, at, 2, 0);
      float sr = fts_get_float_arg(ac, at, 3, 0.0f);
      fts_soundfile_t *sf = fts_soundfile_open_read_float(file_name, 0, sr, onset);
      int size = 0;
      
      if(sf) /* soundfile successfully opened */
	{
	  float file_sr = fts_soundfile_get_samplerate(sf);
	  float *ptr;

	  if(!n_read)
	    n_read = fts_soundfile_get_size(sf);

	  if(sr < 0.0f && sr != file_sr)
	    n_read = (int)((float)n_read * sr / file_sr + 0.5f);

	  float_vector_set_size(this->vec, n_read);
	  ptr = float_vector_get_ptr(this->vec);

	  size = fts_soundfile_read_float(sf, ptr, n_read);

	  fts_soundfile_close(sf);

	  if(size > 0)
	    fvec_send(o, this->vec);
	  else
	    post("fvec: can not load from soundfile \"%s\"\n", fts_symbol_name(file_name));
	  
	  return;
	}
    }
}

static void
fvec_save(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if(file_name)
    {
      int n_write = fts_get_int_arg(ac, at, 1, 0);
      float sr = fts_get_float_arg(ac, at, 2, 0.0f);
      int vec_size = float_vector_get_size(this->vec);
      fts_soundfile_t *sf = 0;
      int size = 0;
      
      if(sr <= 0.0)
	sr = fts_param_get_float(fts_s_sampling_rate, 44100.);

      sf = fts_soundfile_open_write_float(file_name, 0, sr);

      if(sf) /* soundfile successfully opened */
	{
	  float *ptr = float_vector_get_ptr(this->vec);

	  if(n_write <= 0 || n_write > vec_size)
	    n_write = vec_size;
      
	  size = fts_soundfile_write_float(sf, ptr, n_write);

	  fts_soundfile_close(sf);

	  if(size <= 0)
	    post("fvec: can not save to soundfile \"%s\"\n", fts_symbol_name(file_name));
	}
      else
	post("fvec: can not open soundfile to write \"%s\"\n", fts_symbol_name(file_name));
    }
}

/********************************************************************
 *
 *  class
 *
 */

static void
fvec_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "vector of floats");
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

static fts_status_t
fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if(float_vector_get_constructor(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(fvec_t), 1, 1, 0);
  
      /* init / delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fvec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fvec_delete);
  
      /* get data for editor */
      fts_class_add_daemon(cl, obj_property_get, fts_s_data, fvec_get_data);

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, fvec_get_state);

      /* user methods */
      fts_method_define_varargs(cl, 0, fts_s_bang, fvec_output); 
      
      fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), fvec_clear);
      fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), fvec_fill);
      fts_method_define_varargs(cl, 0, fts_new_symbol("set"), fvec_set);
      
      fts_method_define_varargs(cl, 0, fts_new_symbol("size"), fvec_size);

      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), fvec_import);
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), fvec_export);

      fts_method_define_varargs(cl, 0, fts_new_symbol("load"), fvec_load);
      fts_method_define_varargs(cl, 0, fts_new_symbol("save"), fvec_save);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), fvec_assist); 

      /* type outlet */
      fts_outlet_type_define(cl, 0, float_vector_symbol, 1, &float_vector_type);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

int
fvec_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (float_vector_get_constructor(ac0 - 1, at0 + 1) == float_vector_get_constructor(ac1 - 1, at1 + 1));
}

void
fvec_config(void)
{
  sym_text = fts_new_symbol("text");

  fts_metaclass_install(fts_new_symbol("fvec"), fvec_instantiate, fvec_equiv);
}
