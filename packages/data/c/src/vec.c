/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <fts/packages/data/data.h>

fts_symbol_t vec_symbol = 0;
fts_class_t *vec_type = 0;

static fts_symbol_t sym_text = 0;

static int
vec_equals_function(const fts_object_t *a, const fts_object_t *b)
{
  vec_t *o = (vec_t *)a;
  vec_t *p = (vec_t *)b;
  int o_n = vec_get_size(o);
  int p_n = vec_get_size(p);
  
  if(o_n == p_n)
  {
    fts_atom_t *o_ptr = vec_get_ptr(o);
    fts_atom_t *p_ptr = vec_get_ptr(p);
    int i;
    
    for(i=0; i<o_n; i++)
      if(!fts_atom_equals(o_ptr + i, p_ptr + i))
        return 0;
    
    return 1;
  }
  
  return 0;
}

static void
vec_array_function(fts_object_t *o, fts_array_t *array)
{
  vec_t *this = (vec_t *)o;
  fts_atom_t *values = vec_get_ptr(this);
  int size = vec_get_size(this);
  int onset = fts_array_get_size(array);
  fts_atom_t *atoms;
  int i;
  
  fts_array_set_size(array, onset + size);
  atoms = fts_array_get_atoms(array) + onset;
  
  for(i=0; i<size; i++)
    fts_atom_assign(atoms + i, values + i);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
vec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;

  if(ac > 0)
    vec_set_const(this, at);
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
vec_return_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  fts_atom_t a;

  fts_set_int(&a, vec_get_size(this));
  fts_return(&a);
}

static void
vec_change_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
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

static void
vec_return_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  int index = fts_get_int_arg(ac, at, 0, -1);

  if (index >= 0 && index < vec_get_size(this))
    fts_return(vec_get_element(this, index));
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
	fts_post("vec: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    fts_post("vec: unknown import file format \"%s\"\n", fts_symbol_name(file_format));
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
	fts_post("vec: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
    }
  else
    fts_post("vec: unknown export file format \"%s\"\n", fts_symbol_name(file_format));
}

static void
vec_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_copy((mat_t *)fts_get_object(at), (mat_t *)o);
}

static void
vec_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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

  vec_set_size(vec, 0);

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
  int size = vec_get_size(this);
  fts_atom_t *p = vec_get_ptr(this);
  fts_bytestream_t* stream = fts_get_default_console_stream();
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
  
  if(size == 0)
    fts_spost(stream, "<empty vec>\n");
  else if(size == 1)
    {
      fts_spost(stream, "<vec of 1 element: ");
      fts_spost_atoms(stream, 1, p);
      fts_spost(stream, ">\n");
    }
  else if(size <= FTS_POST_MAX_ELEMENTS)
    {
      fts_spost(stream, "<vec of %d elements: ", size);
      fts_spost_atoms(stream, size, p);
      fts_spost(stream, ">\n");
    }
  else
    {
      int i;

      fts_spost(stream, "<vec of %d elements>\n", size);
      fts_spost(stream, "{\n");
      
      for(i=0; i<size; i+=FTS_POST_MAX_ELEMENTS)
	{
	  int n = size - i;

	  if(n > FTS_POST_MAX_ELEMENTS)
	    n = FTS_POST_MAX_ELEMENTS;
	  
	  fts_spost(stream, "  ");
	  fts_spost_atoms(stream, n, p + i);
	  fts_spost(stream, "\n");
	}

      fts_spost(stream, "}\n");
    }
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
  }
  else if(ac > 1)
  {
    mat_set_size((mat_t *)this, ac, 1);
    vec_set_with_onset_from_atoms(this, 0, ac, at);
  }
  else
    fts_object_error(o, "bad arguments");
}

static void
vec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vec_t *this = (vec_t *)o;
  
  if((mat_t *)this->data)
    fts_free((mat_t *)this->data);
}

static void
vec_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(vec_t), vec_init, vec_delete);
  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, vec_dump_state);

  fts_class_set_equals_function(cl, vec_equals_function);
  fts_class_set_array_function(cl, vec_array_function);

  fts_class_message_varargs(cl, fts_s_set_from_instance, vec_set_from_instance);
  fts_class_message_varargs(cl, fts_s_print, vec_print); 
    
  fts_class_message_varargs(cl, fts_s_fill, vec_fill);      
  fts_class_message_varargs(cl, fts_s_set, vec_set_elements);
  
  fts_class_message_varargs(cl, fts_s_import, vec_import); 
  fts_class_message_varargs(cl, fts_s_export, vec_export);

  fts_class_message_void(cl, fts_s_size, vec_return_size);
  fts_class_message_number(cl, fts_s_size, vec_change_size);

  fts_class_message_number(cl, fts_s_get_element, vec_return_element);
  
  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);
}

void
vec_config(void)
{
  sym_text = fts_new_symbol("text");
  vec_symbol = fts_new_symbol("vec");

  vec_type = fts_class_install(vec_symbol, vec_instantiate);
}
