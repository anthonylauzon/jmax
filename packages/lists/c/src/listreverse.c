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

typedef struct 
{
  fts_object_t o;
  fts_array_t list;
} listreverse_t;

static void
listreverse_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;
  fts_atom_t *ta;
  int i, j;

  fts_array_set_size(&this->list, ac);
  ta = fts_array_get_atoms(&this->list);

  for(i=0, j=ac-1; i<ac; i++, j--)
    fts_atom_assign(ta + i, at + j);

  fts_outlet_atoms(o, 0, ac, ta);
}

static void
listreverse_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;

  fts_array_init(&this->list, 0, 0);
}

static void
listreverse_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;

  fts_array_destroy(&this->list);
}

static fts_status_t
listreverse_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(listreverse_t), 1, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listreverse_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, listreverse_delete);

  fts_method_define_varargs(cl, 0, fts_s_list, listreverse_list);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listreverse_config(void)
{
  fts_class_install(fts_new_symbol("listreverse"), listreverse_instantiate);
  fts_alias_install(fts_new_symbol("lrev"), fts_new_symbol("listreverse"));
}
