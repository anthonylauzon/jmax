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
 */

#include <fts/fts.h>

/*------------------------- bangbang class -------------------------------------*/

typedef struct
{
  fts_object_t o;
} bangbang_t;

static void
bangbang_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;

  for (i=fts_object_get_outlets_number(o)-1; i>=0; i--)
    fts_outlet_bang(o, i);
}


static fts_status_t
bangbang_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;
  int noutlets;

  if (ac >= 1  && fts_is_int(at))
    noutlets = fts_get_int(at);
  else
    noutlets = 2;

  fts_class_init(cl, sizeof(bangbang_t), 1, noutlets, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, bangbang_bang);

  for (i = 0; i < noutlets; i++)
    fts_outlet_type_define(cl, i, fts_s_bang, 0, 0);

  return fts_ok;
}

void
bangbang_config(void)
{
  fts_metaclass_install(fts_new_symbol("bangbang"),bangbang_instantiate, fts_arg_equiv);
}
