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
} listhead_t;

static void
listhead_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0 && !fts_is_void(at))
    fts_outlet_send(o, 0, fts_type_get_selector(fts_get_type(at)), 1, at);
}

static fts_status_t
listhead_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */
  fts_class_init(cl, sizeof(listhead_t), 1, 1, 0); 

  /* define the methods */
  fts_method_define_varargs(cl, 0, fts_s_list, listhead_list);

  return fts_Success;
}

void
listhead_config(void)
{
  fts_class_install(fts_new_symbol("listhead"), listhead_instantiate);
  fts_class_alias(fts_new_symbol("lhead"), fts_new_symbol("listhead"));
}





