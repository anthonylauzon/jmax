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

  this->index = fts_get_long(at);
}

static void
listelement_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listelement_t *this = (listelement_t *)o;
  int i;

  i = this->index;

  if(ac > i)
    {
      if (!fts_is_void(at + i))
	fts_outlet_send(o, 0, fts_type_get_selector(fts_get_type(at + i)), 1, at + i);
    }
}

static void
listelement_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listelement_index(o, 0, 0, 1, at+1);
}

static fts_status_t
listelement_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listelement_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, listelement_init, 2, a);

  /* define the methods */

  fts_method_define_varargs(cl, 0, fts_s_list, listelement_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, listelement_index, 1, a);

  return fts_Success;
}

void
listelement_config(void)
{
  fts_class_install(fts_new_symbol("listelement"), listelement_instantiate);
  fts_class_alias(fts_new_symbol("lelem"), fts_new_symbol("listelement"));
}
