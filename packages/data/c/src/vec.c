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
#include "mat.h"
#include "vec.h"

fts_symbol_t vec_symbol = 0;
fts_metaclass_t *vec_type = 0;

static fts_symbol_t sym_text = 0;

/********************************************************************
 *
 *   user methods
 *
 */

static void
vec_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_object(o, 0, o);
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
vec_set_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      int size = vec_get_size(this);
      int i = fts_get_number_int(at);

      if(i >= 0 && i < size)
	vec_set_with_onset_from_atoms(this, i, ac - 1, at + 1);
    }
}

static void
vec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      if(size >= 0)
	{
	  int old_size = vec_get_size(this);
	  int i;
	  
	  vec_set_size(this, size);

	  /* set newly allocated region to void */
	  for(i=old_size; i<size; i++)
	    fts_set_void(this->data + i);
	}
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
      int size = vec_read_atom_file(this, file_name);

      if(size <= 0)
	post("vec: can't import from text file \"%s\"\n", file_name);
    }
  else
    post("vec: unknown import file format \"%s\"\n", file_format);
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
      int size = vec_write_atom_file(this, file_name);

      if(size < 0)
	post("vec: can't export to text file \"%s\"\n", file_name);
    }
  else
    post("vec: unknown export file format \"%s\"\n", file_format);
}

static void
vec_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  fts_atom_t *values = vec_get_ptr(this);
  int size = vec_get_size(this);
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);
  int onset = fts_array_get_size(array);
  fts_atom_t *atoms;
  int i;
  
  fts_array_set_size(array, onset + size);  
  atoms = fts_array_get_atoms(array) + onset;

  for(i=0; i<size; i++)
    fts_atom_assign(atoms + i, values + i);
}

static void
vec_set_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  vec_set_with_onset_from_atoms(this, 0, ac, at);
}

static void
vec_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_copy(mat_atom_get(at), (mat_t *)o);
}

static void
vec_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);      
  fts_atom_t *values = vec_get_ptr(this);
  int size = vec_get_size(this);
  fts_message_t *mess;
  int i;

  /* send size message */
  mess = fts_dumper_message_new(dumper, fts_s_size);  
  fts_message_append_int(mess, size);
  fts_dumper_message_send(dumper, mess);

  /* get new set message and append onset 0 */
  mess = fts_dumper_message_new(dumper, fts_s_set);
  fts_message_append_int(mess, 0);
  
  for(i=0; i<size; i++)
    {
      if(fts_is_object(values + i))
	fts_message_append_int(mess, 0);
      else
	fts_message_append(mess, 1, values + i);
      
      if(fts_message_get_ac(mess) >= 256)
	{
	  fts_dumper_message_send(dumper, mess);

	  /* new set message and append onset i + 1 */
	  mess = fts_dumper_message_new(dumper, fts_s_set);
	  fts_message_append_int(mess, i + 1);
	}
    }
  
  if(fts_message_get_ac(mess) > 1) 
    fts_dumper_message_send(dumper, mess);
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
  fts_atom_file_t *file = fts_atom_file_open(file_name, "r");
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

  file = fts_atom_file_open(file_name, "w");

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
  post("}\n");
}

static void
vec_get_vec(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
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

  data_object_set_keep((data_object_t *)o, fts_s_no);

  if(ac == 0)
    mat_set_size((mat_t *)this, 0, 0);
  else if(ac == 1 && fts_is_int(at))
    mat_set_size((mat_t *)this, fts_get_int(at), 1);
  else if(ac == 1 && fts_is_tuple(at))
    {
      fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at);
      int size = fts_tuple_get_size(tup);
      
      mat_set_size((mat_t *)this, size, 1);
      vec_set_with_onset_from_atoms(this, 0, size, fts_tuple_get_atoms(tup));

      data_object_set_keep((data_object_t *)o, fts_s_args);
    }
  else if(ac > 1)
    {
      mat_set_size((mat_t *)this, ac, 1);
      vec_set_with_onset_from_atoms(this, 0, ac, at);

      data_object_set_keep((data_object_t *)o, fts_s_args);
    }
  else
    fts_object_set_error(o, "Wrong arguments");
}

static void
vec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  
  if((mat_t *)this->data)
    fts_free((mat_t *)this->data);
}

static fts_status_t
vec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(vec_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, vec_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, vec_delete);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set, vec_set_elements);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set_from_instance, vec_set_from_instance);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set_from_array, vec_set_from_array);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_size, vec_size);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_get_array, vec_get_array);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_dump, vec_dump);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_print, vec_print); 
  
  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, data_object_daemon_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, data_object_daemon_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, vec_get_vec);
  
  /* user methods */
  fts_method_define_varargs(cl, 0, fts_s_bang, vec_output);
  
  fts_method_define_varargs(cl, 0, fts_s_clear, vec_clear);
  fts_method_define_varargs(cl, 0, fts_s_fill, vec_fill);      
  fts_method_define_varargs(cl, 0, fts_s_set, vec_set_elements);
  
  fts_method_define_varargs(cl, 0, fts_s_size, vec_size);
  
  fts_method_define_varargs(cl, 0, fts_s_import, vec_import); 
  fts_method_define_varargs(cl, 0, fts_s_export, vec_export); 
  
  /* type outlet */
  fts_outlet_type_define(cl, 0, vec_symbol, 1, &vec_symbol);
  
  return fts_ok;
}

void
vec_config(void)
{
  sym_text = fts_new_symbol("text");
  vec_symbol = fts_new_symbol("vec");

  vec_type = fts_class_install(vec_symbol, vec_instantiate);
}
