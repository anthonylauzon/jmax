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
  int index;
} listelement_t;

static void
listelement_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listelement_t *this = (listelement_t *)o;

  this->index = fts_get_number_int(at);
}

static void
listelement_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listelement_t *this = (listelement_t *)o;
  int i = this->index;

  if(i > 0 && i < ac && !fts_is_void(at + i))
    fts_outlet_atom(o, 0, at + i);
}

static void
listelement_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listelement_t *this = (listelement_t *)o;

  ac--;
  at++;

  if(ac == 0)
    this->index = 0;
  else if(ac > 0 && fts_is_number(at))
    this->index = fts_get_number_int(at);
  else
    fts_object_set_error(o, "First argument of number required");
}

static fts_status_t
listelement_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(listelement_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listelement_init);

  fts_method_define_varargs(cl, 0, fts_s_int, listelement_list);
  fts_method_define_varargs(cl, 0, fts_s_float, listelement_list);
  fts_method_define_varargs(cl, 0, fts_s_symbol, listelement_list);
  fts_method_define_varargs(cl, 0, fts_s_list, listelement_list);

  fts_method_define_varargs(cl, 1, fts_s_int, listelement_index);
  fts_method_define_varargs(cl, 1, fts_s_float, listelement_index);

  return fts_Success;
}

void
listelement_config(void)
{
  fts_class_install(fts_new_symbol("listelement"), listelement_instantiate);
  fts_alias_install(fts_new_symbol("lelem"), fts_new_symbol("listelement"));
}





