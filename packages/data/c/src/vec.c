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
#include "mat.h"
#include "vec.h"

fts_type_t vec_type = 0;
fts_symbol_t vec_symbol = 0;
fts_class_t *vec_class = 0;

static fts_symbol_t sym_text = 0;

extern void mat_alloc(mat_t *mat, int m, int n);
extern void mat_free(mat_t *mat);

/********************************************************************
 *
 *   user methods
 *
 */

static void
vec_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  fts_atom_t a[1];

  vec_atom_set(a, this);
  fts_outlet_send(o, 0, vec_symbol, 1, a);
}

static void
vec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  int size = vec_get_size(this);
  
  if(ac > 0 && fts_is_number(at))
    {
      int i = fts_get_number_int(at);

      if(i >= 0 && i < size)
	vec_void_element(this, i);
    }
  else
    vec_void(this);
}

static void
vec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  if(ac > 0)
    vec_set_const(this, at[0]);
}

static void
vec_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int size = vec_get_size(this);
      int i = fts_get_number_int(at);

      if(i >= 0 && i < size)
	{
	  vec_set_from_atom_list(this, i, ac - 2, at + 2);
	}
    }
}

static void
vec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  int size = 0;

  if(ac > 0 && fts_is_number(at))
    {
      size = fts_get_number_int(at);
      
      if(size >= 0)
	vec_set_size(this, size);
    }  
}

static void
vec_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_format = fts_get_symbol_arg(ac, at, 1, sym_text);

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      int size = 0;

      size = vec_read_atom_file(this, file_name);

      if(size <= 0)
	post("vec: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
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

  if(!file_name)
    return;

  if(file_format == sym_text)
    {
      int size = 0;

      size = vec_write_atom_file(this, file_name);

      if(size < 0)
	post("vec: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    post("vec: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
vec_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "vec of values");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "vec commands");
	  break;
	}
    }
}

/********************************************************
 *
 *  files
 *
 */

#define VEC_BLOCK_SIZE 64

static void
vec_grow(vec_t *vec, int size)
{
  int alloc = vec->alloc;

  while(size > alloc)
    alloc += VEC_BLOCK_SIZE;

  vec_set_size(vec, alloc);
}

int 
vec_read_atom_file(vec_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int n = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;

  vec_void(vec);

  while(fts_atom_file_read(file, &a, &c))
    {
      if(n >= vec->alloc)
	vec_grow(vec, n);

      vec->data[n] = a;
      n++;
    }

  vec_set_size(vec, n);
  
  fts_atom_file_close(file);

  return (n);
}

int
vec_write_atom_file(vec_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int size = vec_get_size(vec);
  int i;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the vec */
  for(i=0; i<size; i++)     
    {
      fts_atom_t *data = vec->data;

      fts_atom_file_write(file, data + i, '\n');
    }

  fts_atom_file_close(file);

  return (i);
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
vec_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  post("{");
  post_atoms(vec_get_size(this), vec_get_ptr(this));
  post("}");
}

static void
vec_get_vec(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  vec_t *this = (vec_t *)obj;

  vec_atom_set(value, this);
}

/********************************************************************
 *
 *  class
 *
 */

static void
vec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  ac--;
  at++;

  if(ac == 0)
    mat_alloc((mat_t *)this, 0, 0);
  else if(ac == 1 && fts_is_int(at))
    mat_alloc((mat_t *)this, fts_get_int(at), 1);
  else if(ac == 1 && fts_is_list(at))
    {
      fts_list_t *aa = fts_get_list(at);
      int size = fts_list_get_size(aa);
      
      mat_alloc((mat_t *)this, size, 1);
      vec_set_from_atom_list(this, 0, size, fts_list_get_ptr(aa));
    }
  else if(ac > 1)
    {
      mat_alloc((mat_t *)this, ac, 1);
      vec_set_from_atom_list(this, 0, ac, at);
    }
}

static void
vec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  mat_free((mat_t *)this);
}

static int
vec_check(int ac, const fts_atom_t *at)
{
  return (ac == 0 || (ac == 1 && (fts_is_int(at) || fts_is_list(at))) || ac > 1);
}

static fts_status_t
vec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(vec_check(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(vec_t), 1, 1, 0);
  
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, vec_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, vec_delete);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, vec_print); 
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), vec_assist); 
      
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, vec_get_vec);

      /* user methods */
      fts_method_define_varargs(cl, 0, fts_s_bang, vec_output);

      fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), vec_clear);
      fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), vec_fill);      
      fts_method_define_varargs(cl, 0, fts_new_symbol("set"), vec_set);

      fts_method_define_varargs(cl, 0, fts_new_symbol("size"), vec_size);
            
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), vec_import); 
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), vec_export); 

      /* type outlet */
      fts_outlet_type_define(cl, 0, vec_symbol, 1, &vec_type);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;

}

static int
vec_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return vec_check(ac1 - 1, at1 + 1);
}

void
vec_config(void)
{
  sym_text = fts_new_symbol("text");
  vec_symbol = fts_new_symbol("vec");
  vec_type = vec_symbol;

  fts_metaclass_install(vec_symbol, vec_instantiate, vec_equiv);
  vec_class = fts_class_get_by_name(vec_symbol);

  fts_atom_type_register(vec_symbol, vec_class);
}
