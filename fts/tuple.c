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
 */

#include <fts/fts.h>

fts_metaclass_t *fts_tuple_metaclass = 0;

static void
tuple_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;
  
  post("(");
  post_atoms(fts_tuple_get_size(this), fts_tuple_get_atoms(this));
  post(")\n");
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

static fts_status_t
tuple_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_tuple_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, tuple_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, tuple_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, tuple_print);
  
  return fts_Success;
}

void
fts_kernel_tuple_init(void)
{
  fts_tuple_metaclass = fts_class_install(fts_s_tuple, tuple_instantiate);
}
