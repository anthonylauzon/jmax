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

#include <stdlib.h>
#include <fts/fts.h>
#include <ftsconfig.h>

#define abs(x) ((x) > 0 ? (x) : -(x))

static void
abs_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_int(o, 0, abs(fts_get_int(at)));
}

static void
abs_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_float(o, 0, abs(fts_get_float(at)));
}

static void
abs_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t *out = alloca(ac * sizeof(fts_atom_t));
  int i;

  for(i=0; i<ac; i++)
    {
      if (fts_is_int(at + i))
	fts_set_int(out + i, abs(fts_get_int(at + i)));
      else if(fts_is_float(at + i))
	fts_set_float(out + i, abs(fts_get_float(at + i)));
      else
	out[i] = at[i];
    }

  fts_outlet_atoms(o, 0, ac, out);
}

static fts_status_t
abs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_object_t), 1, 1, 0);

  fts_method_define_varargs(cl, 0, fts_s_int, abs_int);
  fts_method_define_varargs(cl, 0, fts_s_float, abs_float);
  fts_method_define_varargs(cl, 0, fts_s_list, abs_atoms);

  return fts_ok;
}

void
abs_config(void)
{
  fts_class_install(fts_new_symbol("abs"), abs_instantiate);
}
