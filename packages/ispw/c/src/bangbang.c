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

static void
bangbang_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int i;

  for (i=fts_object_get_outlets_number(o)-1; i>=0; i--)
    fts_outlet_bang(o, i);
}

static void
bangbang_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int noutlets;

  if (ac > 0 && fts_is_number(at))
    noutlets = fts_get_number_int(at);
  else
    noutlets = 2;

  fts_object_set_outlets_number(o, noutlets);
}

static void
bangbang_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), bangbang_init, 0);

  fts_class_input_handler(cl, bangbang_input);

  fts_class_outlet_bang(cl, 0);
}

void
bangbang_config(void)
{
  fts_class_install(fts_new_symbol("bangbang"), bangbang_instantiate);
}
