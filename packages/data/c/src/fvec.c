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
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "fvec.h"

fts_symbol_t fvec_symbol = 0;
fts_type_t fvec_type = 0;
fts_class_t *fvec_class = 0;

static fts_symbol_t sym_text = 0;
static fts_symbol_t sym_load = 0;
static fts_symbol_t sym_open_file = 0;

/********************************************************
 *
 *  utility functions
 *
 */

/* local */
static void
set_size(fvec_t *vec, int size)
{
  int i;

  if(size > vec->alloc)
    {
      if(vec->alloc)
	vec->values = (float *)fts_realloc((void *)vec->values, sizeof(float) * size);
      else
	vec->values = (float *)fts_malloc(sizeof(float) * size);

      vec->alloc = size;
    }

  /* when shortening: zero old values */
  for(i=size; i<vec->size; i++)
    vec->values[i] = 0.0;

  vec->size = size;
}

void
fvec_set_const(fvec_t *vec, float c)
{
  float *values = vec->values;
  int i;
  
  for(i=0; i<vec->size; i++)
    values[i] = c;
}

void
fvec_set_size(fvec_t *vec, int size)
{
  int old_size = vec->size;
  int i;

  set_size(vec, size);

  /* when extending: zero new values */
  for(i=old_size; i<size; i++)
    vec->values[i] = 0.0;
}

void
fvec_set_from_atom_list(fvec_t *vec, int offset, int ac, const fts_atom_t *at)
{
  int size = fvec_get_size(vec);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	vec->values[i + offset] = fts_get_number_float(at + i);
      else
	vec->values[i + offset] = 0.0f;
    }
}

/* sum, min, max */
float 
fvec_get_sum(fvec_t *vec)
{
  float sum = 0;
  int i;

  for(i=0; i<vec->size; i++)
    sum += vec->values[i];

  return sum;
}

float
fvec_get_sub_sum(fvec_t *vec, int from, int to)
{
  float sum = 0;
  int i;
  
  if(from < 0)
    from = 0;

  if(to >= vec->size)
    to = vec->size - 1;

  for(i=from; i<=to; i++)
    sum += vec->values[i];

  return sum;
}

float
fvec_get_min_value(fvec_t *vec)
{
  float min;
  int i;

  min = vec->values[0];

  for (i=1; i<vec->size; i++)
    if (vec->values[i] < min)
      min = vec->values[i];

  return min;
}


float
fvec_get_max_value(fvec_t *vec)
{
  float max;
  int i;

  max = vec->values[0];

  for (i=1; i<vec->size; i++)
    if (vec->values[i] > max)
      max = vec->values[i];

  return max;
}

/********************************************************
 *
 *  files
 *
 */

#define FVEC_BLOCK_SIZE 256

static void
fvec_grow(fvec_t *vec, int size)
{
  int alloc = vec->alloc;

  while(!alloc || size > alloc)
    alloc += FVEC_BLOCK_SIZE;

  set_size(vec, alloc);
}

int 
fvec_read_atom_file(fvec_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int n = 0;
  fts_atom_t a;
  char c;

  if(file)
    {
      while(fts_atom_file_read(file, &a, &c))
	{
	  if(n >= vec->alloc)
	    fvec_grow(vec, n);
	  
	  if(fts_is_number(&a))
	    fvec_set_element(vec, n, fts_get_number_float(&a));
	  else
	    fvec_set_element(vec, n, 0.0f);
	  
	  n++;
	}
      
      if(n > 0)
	fvec_set_size(vec, n);
      
      fts_atom_file_close(file);
    }
  
  return n;
}

int
fvec_write_atom_file(fvec_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int size = fvec_get_size(vec);
  int i;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the vec */
  for(i=0; i<size; i++)     
    {
      fts_atom_t a;
      
      fts_set_float(&a, fvec_get_element(vec, i));
      fts_atom_file_write(file, &a, '\n');
    }

  fts_atom_file_close(file);

  return (i);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
fvec_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_atom_t a[1];

  fvec_atom_set(a, this);
  fts_outlet_send(o, 0, fvec_symbol, 1, a);
}

static void
fvec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_set_const(this, 0.0f);
}

static void
fvec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float constant = fts_get_float_arg(ac, at, 0, 0);

  fvec_set_const(this, constant);
}

static void
fvec_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      int size = fvec_get_size(this);
      int offset = fts_get_number_int(at);

      if(offset >= 0 && offset < size)
	fvec_set_from_atom_list(this, offset, ac - 1, at + 1);
    }
}

static void
fvec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac == 1 && fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      if(size >= 0)
	fvec_set_size(this, size);
    }
}

static void
fvec_import(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      size = fvec_read_atom_file(this, file_name);
      
      if(size > 0)
	fvec_output(o, 0, 0, 0, 0);
      else
	post("fvec: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
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
  int size = 0;

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      size = fvec_write_atom_file(this, file_name);
      
      if(size < 0)
	post("fvec: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("fvec: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
fvec_load(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    {
      fts_symbol_t file_name = fts_get_symbol(at);
      fts_soundfile_t *sf = 0;
      int size = 0;
      float sr = 0.0;
      int onset, n_read;

      if(ac > 1 && fts_is_number(at + 1))
	onset = fts_get_number_int(at + 1);
      else
	onset = 0;

      if(ac > 2 && fts_is_number(at + 2))
	n_read = fts_get_number_int(at + 2);
      else
	n_read = 0;

      if(ac > 3 && fts_is_number(at + 3))
	sr = fts_get_number_float(at + 3);
      else if(this->sr > 0.0)
	/* force sampling rate to given property */
	sr = this->sr;

      if (fts_file_is_text( file_name))
	{
	  size = fvec_read_atom_file(this, file_name);

	  if(size > 0)
	    fvec_output(o, 0, 0, 0, 0);
	  else
	    post("fvec: can't load from file \"%s\"\n", fts_symbol_name(file_name));	  
	}
      else
	{
	  sf = fts_soundfile_open_read_float(file_name, 0, sr, onset);
      
	  if(sf) /* soundfile successfully opened */
	    {
	      float file_sr = fts_soundfile_get_samplerate(sf);
	      float *ptr;

	      if(!n_read)
		n_read = fts_soundfile_get_size(sf);

	      /* make enough space for resampled file */
	      if(sr > 0.0 && sr != file_sr)
		n_read = (int)((float)n_read * sr / file_sr + 0.5f);
	      else
		/* get temporary sample rate from file */
		this->sr = -file_sr;

	      fvec_set_size(this, n_read);
	      ptr = fvec_get_ptr(this);

	      size = fts_soundfile_read_float(sf, ptr, n_read);

	      fts_soundfile_close(sf);

	      if(size > 0)
		fvec_output(o, 0, 0, 0, 0);
	      else
		post("fvec: can't load from soundfile \"%s\"\n", fts_symbol_name(file_name));
	  
	      return;
	    }
	}
    }
  else
    {
      fts_atom_t a[4];
      
      fts_set_symbol(a, sym_load);
      fts_set_symbol(a + 1, sym_open_file);
      fts_set_symbol(a + 2, fts_get_project_dir());
      fts_set_symbol(a + 3, fts_new_symbol(" "));
      fts_client_send_message(o, fts_s_openFileDialog, 4, a);
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
      int vec_size = fvec_get_size(this);
      fts_soundfile_t *sf = 0;
      int size = 0;
      
      if(sr <= 0.0)
	sr = fts_get_sample_rate();

      sf = fts_soundfile_open_write_float(file_name, 0, sr);

      if(sf) /* soundfile successfully opened */
	{
	  float *ptr = fvec_get_ptr(this);

	  if(n_write <= 0 || n_write > vec_size)
	    n_write = vec_size;
      
	  size = fts_soundfile_write_float(sf, ptr, n_write);

	  fts_soundfile_close(sf);

	  if(size <= 0)
	    post("fvec: can't save to soundfile \"%s\"\n", fts_symbol_name(file_name));
	}
      else
	post("fvec: can't open soundfile to write \"%s\"\n", fts_symbol_name(file_name));
    }
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
fvec_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  int i;

  post("{");

  if(size > 8)
    {
      int size8 = (size / 8) * 8;
      int i, j;

      for(i=0; i<size8; i+=8)
	{
	  /* print one line of 8 with indent */
	  post("\n  ");
	  for(j=0; j<8; j++)
	    post("%f ", fvec_get_element(this, i + j));
	}
	  
      /* print last line with indent */
      if(i < size)
	{
	  post("\n  ");
	  for(; i<size; i++)
	    post("%f ", fvec_get_element(this, i));
	}

      post("\n}");
    }
  else if(size)
    {
      for(i=0; i<size-1; i++)
	post("%f ", fvec_get_element(this, i));

      post("%f}", fvec_get_element(this, size - 1));
    }
  else
    post("}");
}

void 
fvec_save_bmax(fvec_t *vec, fts_bmax_file_t *f)
{
  fts_atom_t av[256];
  int ac = 0;
  int i;
  int offset = 0;

  for(i=0; i<vec->size; i++)
    {
      fts_set_float(&av[ac], vec->values[i]);

      ac++;

      if (ac == 256)
	{
	  /* Code a push of all the values */
	  fts_bmax_code_push_atoms(f, ac, av);

	  /* Code a push of the offset */
	  fts_bmax_code_push_int(f, offset);

	  /* Code a "set" message for 256 values plus offset */
	  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);
	  offset = offset + ac;

	  /* Code a pop of all the values  */
	  fts_bmax_code_pop_args(f, ac);

	  ac = 0;
	}
    }

  if (ac != 0)
    {
      /* Code a push of all the values */
      fts_bmax_code_push_atoms(f, ac, av);

      /* Code a push of the offset */
      fts_bmax_code_push_int(f, offset);

      /* Code an "append" message for the values left */
      fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);

      /* Code a pop of all the values  */
      fts_bmax_code_pop_args(f, ac);
    }
}

static void
fvec_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fvec_t *this = (fvec_t *)obj;

  fvec_atom_set(value, this);
}

static void
fvec_set_sr(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fvec_t *this = (fvec_t *)obj;

  if(fts_is_number(value))
    {
      float sr = fts_get_number_float(value);
      
      if(sr > 0.0)
	this->sr = sr;
    }
}

static void
fvec_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "vec of floats");
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
}

/*********************************************************
 *
 *  class
 *
 */
static void
fvec_alloc(fvec_t *vec, int size)
{
  int i;

  if(size > 0)
    {
      vec->values = (float *) fts_malloc(size * sizeof(float));
      vec->size = size;
      fvec_zero(vec);

      /* init to zero */
      for(i=0; i<size; i++)
	vec->values[i] = 0.0;
    }
  else
    {
      vec->values = 0;
      vec->size = 0;
    }

  vec->alloc = size;
}

static void
fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  
  /* skip class name */
  ac--;
  at++;

  this->values = 0;
  this->size = 0;
  this->alloc = 0;

  this->sr = 0.0;

  if(ac == 0)
    fvec_alloc(this, 0);
  else if(ac == 1 && fts_is_int(at))
    fvec_alloc(this, fts_get_int(at));
  else if(ac == 1 && fts_is_list(at))
    {
      fts_list_t *aa = fts_get_list(at);
      int size = fts_list_get_size(aa);
      
      fvec_alloc(this, size);
      fvec_set_from_atom_list(this, 0, size, fts_list_get_ptr(aa));
    }
  else if(ac == 1 && fts_is_symbol(at))
    {
      fts_symbol_t file_name = fts_get_symbol(at);
      int size = 0;
      
      if (fts_file_is_text( file_name))
	{
	  size = fvec_read_atom_file(this, file_name);
	}
      else
	{
	  fts_soundfile_t *sf = fts_soundfile_open_read_float(file_name, 0, 0, 0);

	  if(sf) /* soundfile successfully opened */
	    {
	      float *ptr;
	  
	      size = fts_soundfile_get_size(sf);
	  
	      fvec_set_size(this, size);
	      ptr = fvec_get_ptr(this);
	  
	      size = fts_soundfile_read_float(sf, ptr, size);
	      fvec_set_size(this, size);

	      fts_soundfile_close(sf);
	    }
	}

      if(size == 0)
	fts_object_set_error(o, "can't load from file \"%s\"", fts_symbol_name(file_name));
    }
  else if(ac > 1)
    {
      fvec_alloc(this, ac);
      fvec_set_from_atom_list(this, 0, ac, at);
    }
  else
    fts_object_set_error(o, "Wrong arguments");
}

static void
fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  
  if(this->values)
    fts_free(this->values);
}

static fts_status_t
fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fvec_t), 1, 1, 0);
  
  /* init / delete */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fvec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fvec_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, fvec_print); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), fvec_assist); 

  fts_method_define_varargs(cl, fts_SystemInlet, sym_load, fvec_load);

  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, fvec_get_state);

  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("sr"), fvec_set_sr);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, fvec_output);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), fvec_clear);
  fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), fvec_fill);
  fts_method_define_varargs(cl, 0, fts_new_symbol("set"), fvec_set);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("size"), fvec_size);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("import"), fvec_import);
  fts_method_define_varargs(cl, 0, fts_new_symbol("export"), fvec_export);
  
  fts_method_define_varargs(cl, 0, sym_load, fvec_load);
  fts_method_define_varargs(cl, 0, fts_new_symbol("save"), fvec_save);
  
  /* type outlet */
  fts_outlet_type_define(cl, 0, fvec_symbol, 1, &fvec_type);      

  return fts_Success;
}

/********************************************************************
 *
 *  config
 *
 */

void 
fvec_config(void)
{
  sym_text = fts_new_symbol("text");
  sym_load = fts_new_symbol("load");
  sym_open_file = fts_new_symbol("open file");

  fvec_symbol = fts_new_symbol("fvec");
  fvec_type = fvec_symbol;

  fts_class_install(fvec_symbol, fvec_instantiate);
  fvec_class = fts_class_get_by_name(fvec_symbol);

  fts_atom_type_register(fvec_symbol, fvec_class);
}
