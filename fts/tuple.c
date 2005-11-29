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

fts_class_t *fts_tuple_class = 0;

static unsigned int
tuple_hash_function(const fts_atom_t *p)
{
  fts_tuple_t *self = (fts_tuple_t *)fts_get_object(p);
  int size = fts_tuple_get_size(self);
  fts_atom_t *atoms = fts_tuple_get_atoms(self);
  unsigned int sum = 0;
  int i;
  
  for(i=0; i<size; i++)
  {
    if(fts_is_int(atoms + i))
    {
      sum <<= 8;
      sum += fts_get_int(atoms + i);
    }
    else if(fts_get_float(atoms + i))
      sum += (unsigned int)(1000.0 * fts_get_float(atoms + i));
    else
      sum += (unsigned int)fts_get_pointer(atoms + i) >> 3;
  }
  
  return sum;
}

static int
tuple_equals_function(const fts_object_t *a, const fts_object_t *b)
{
  fts_tuple_t *a_tup = (fts_tuple_t *)a;
  fts_tuple_t *b_tup = (fts_tuple_t *)b;
  int a_n = fts_tuple_get_size(a_tup);
  int b_n = fts_tuple_get_size(b_tup);
  
  if(a_n == b_n)
  {
    fts_atom_t *ap = fts_tuple_get_atoms(b_tup);
    fts_atom_t *bp = fts_tuple_get_atoms(a_tup);
    int i;
    
    for(i=0; i<a_n; i++)
      if(!fts_atom_equals(ap + i, bp + i))
        return 0;
    
    return 1;
  }
  
  return 0;
}

static void 
tuple_copy(fts_tuple_t *org, fts_tuple_t *copy)
{
  int size = fts_tuple_get_size(org);
  fts_atom_t *org_atoms = fts_tuple_get_atoms(org);
  fts_atom_t *copy_atoms;
  int i;
  
  fts_tuple_set_size(copy, size);
  copy_atoms = fts_tuple_get_atoms(copy);
  
  for (i = 0; i < size; i++)
  { /* fts_atom_copy handles release/refer of copy_atoms */
    fts_atom_copy(org_atoms + i, copy_atoms + i);
  }
}

static void 
tuple_copy_function(const fts_object_t *from, fts_object_t *to)
{
  tuple_copy((fts_tuple_t *)from, (fts_tuple_t *)to);
}

static void
tuple_array_function(fts_object_t *o, fts_array_t *array)
{
  fts_tuple_t *self = (fts_tuple_t *)o;
  int size = fts_tuple_get_size(self);
  fts_atom_t *atoms = fts_tuple_get_atoms(self);
  
  fts_array_append(array, size, atoms);
}

static void
tuple_description_function(fts_object_t *o, fts_array_t *array)
{
  fts_tuple_t *self = (fts_tuple_t *)o;
  int size = fts_tuple_get_size(self);
  fts_atom_t *atoms = fts_tuple_get_atoms(self);
  
  fts_array_append_symbol(array, fts_s_tuple);
  fts_array_append(array, size, atoms);
}

static fts_method_status_t
tuple_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_tuple_t *self = (fts_tuple_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_class, 0, 0);

  fts_object_refer((fts_object_t *)mess);
  
  fts_message_set(mess, fts_s_set, 0, 0);
  fts_message_append(mess, fts_array_get_size(&self->args), fts_array_get_atoms(&self->args));
  fts_dumper_message_send(dumper, mess);

  fts_object_release((fts_object_t *)mess);
  
  return fts_ok;
}

static fts_method_status_t
tuple_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_tuple_t *self = (fts_tuple_t *) o;
  
  fts_set_int(ret, fts_tuple_get_size(self));
  
  return fts_ok;
}

static fts_method_status_t
tuple_element(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_tuple_t *self = (fts_tuple_t *)o;
  int index = fts_get_int_arg(ac, at, 0, -1);
  
  if (index >= 0 && index < fts_array_get_size( &self->args))
    *ret = fts_array_get_atoms( &self->args)[index];
  
  return fts_ok;
}

static fts_method_status_t
tuple_first(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_tuple_t *self = (fts_tuple_t *) o;
  
  if (fts_array_get_size( &self->args) > 0)
    *ret = fts_array_get_atoms( &self->args)[0];
  
  return fts_ok;
}

static fts_method_status_t
tuple_second(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_tuple_t *self = (fts_tuple_t *) o;
  
  if (fts_array_get_size( &self->args) > 1)
    *ret = fts_array_get_atoms( &self->args)[1];
  
  return fts_ok;
}

static fts_method_status_t
tuple_third(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_tuple_t *self = (fts_tuple_t *) o;
  
  if (fts_array_get_size( &self->args) > 2)
    *ret = fts_array_get_atoms( &self->args)[2];
  
  return fts_ok;
}

static fts_method_status_t
tuple_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_tuple_t *self = (fts_tuple_t *)o;
  
  fts_array_init(&self->args, ac, at);
  
  return fts_ok;
}

static fts_method_status_t
tuple_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_tuple_t *self = (fts_tuple_t *)o;
  
  fts_array_destroy(&self->args);
  
  return fts_ok;
}

static void
tuple_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_tuple_t), tuple_init, tuple_delete);
  
  fts_class_message_varargs(cl, fts_s_dump_state, tuple_dump_state);  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  
  fts_class_message_varargs(cl, fts_s_get_element, tuple_element);
  fts_class_message_void(cl, fts_s_size, tuple_size);
  
  fts_class_message_void(cl, fts_new_symbol("first"), tuple_first);
  fts_class_message_void(cl, fts_new_symbol("second"), tuple_second);
  fts_class_message_void(cl, fts_new_symbol("third"), tuple_third);
  
  fts_class_set_hash_function(cl, tuple_hash_function);
  fts_class_set_equals_function(cl, tuple_equals_function);
  fts_class_set_copy_function(cl, tuple_copy_function);
  fts_class_set_array_function(cl, tuple_array_function);
  fts_class_set_description_function(cl, tuple_description_function);
  
  fts_class_doc(cl, fts_s_tuple, "[<any: value> ...]", "immutable array of any values");
  fts_class_doc(cl, fts_new_symbol("first"), NULL, "get first value");
  fts_class_doc(cl, fts_new_symbol("second"), NULL, "get second value");
  fts_class_doc(cl, fts_new_symbol("third"), NULL, "get third value");
  fts_class_doc(cl, fts_s_size, NULL, "get size");
  fts_class_doc(cl, fts_s_print, NULL, "print values");
}

FTS_MODULE_INIT(tuple)
{
  fts_tuple_class = fts_class_install(fts_s_tuple, tuple_instantiate);
}
