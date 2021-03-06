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

static fts_symbol_t sym_fork = 0;

static void
fork_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int n = fts_object_get_outlets_number(o);

  while(n--)
    fts_outlet_send(o, n, s, ac, at);
}

static void 
fork_set_outlets(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int n = fts_get_int(at);
  fts_atom_t a[2];

  fts_set_symbol(a + 0, sym_fork);
  fts_set_int(a + 1, n);      
  fts_object_set_description(o, 2, a);

  fts_object_set_outlets_number(o, n);
}

static void
fork_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int n = 2;

  if(ac > 0 && fts_is_number(at))
    n = fts_get_number_int(at);

  fts_object_set_outlets_number(o, n);
}

static void 
fork_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), fork_init, 0);

  fts_class_message_varargs(cl, fts_new_symbol("set_outlets"), fork_set_outlets);

  fts_class_input_handler(cl, fork_input);
}

void fork_config(void)
{
  sym_fork = fts_new_symbol("fork");

  fts_class_install(sym_fork, fork_instantiate);
}
