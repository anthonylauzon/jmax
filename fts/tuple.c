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

static void
tuple_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  
  fts_spost(stream, "{");
  fts_spost_atoms(stream, fts_tuple_get_size(this), fts_tuple_get_atoms(this));
  fts_spost(stream, "}");
}

static void
tuple_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_array_set(&this->args, ac, at);
}

static void
tuple_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = fts_dumper_message_new(dumper, fts_s_set);
  
  fts_message_append(mess, fts_array_get_size(&this->args), fts_array_get_atoms(&this->args));
  fts_dumper_message_send(dumper, mess);
}

static int
tuple_equals(const fts_atom_t *a, const fts_atom_t *b)
{
  fts_tuple_t *a_tup = (fts_tuple_t *)fts_get_object(a);
  fts_tuple_t *b_tup = (fts_tuple_t *)fts_get_object(b);
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
tuple_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_array_init(&this->args, ac, at);
}

static void
tuple_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_array_destroy(&this->args);
}

static void
tuple_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;
  int index = fts_get_int_arg(ac, at, 0, -1);

  if (index >= 0 && index < fts_array_get_size( &this->args))
    fts_return( fts_array_get_atoms( &this->args) + index);
}

static void
tuple_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *) o;
  fts_atom_t   ret;

  if (ac == 0)
  {
      fts_set_int(&ret, fts_tuple_get_size(this));
      fts_return(&ret);
  }
}

static void
tuple_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_tuple_t), tuple_init, tuple_delete);

  fts_class_message_varargs(cl, fts_s_post, tuple_post);
  fts_class_message_varargs(cl, fts_s_get_element, tuple_element);
  fts_class_message_varargs(cl, fts_s_size, tuple_size);

  fts_class_message_varargs(cl, fts_s_dump_state, tuple_dump_state);

  fts_class_set_equals_function(cl, tuple_equals);

  fts_class_message_varargs(cl, fts_s_set, tuple_set);
}

void
fts_kernel_tuple_init(void)
{
  fts_tuple_class = fts_class_install(fts_s_tuple, tuple_instantiate);
}
