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

#include "fts.h"


typedef struct 
{
  fts_object_t o;
  int index;
  int length;
} listsublist_t;


static void
listsublist_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listsublist_t *this = (listsublist_t *)o;
  int n;

  if(!ac) return;

  n = (this->index + this->length <= ac)? this->length: ac - this->index;

  if(n > 0 && this->index < ac)
    fts_outlet_send(o, 0, fts_s_list, n, at + this->index);
}

static void
listsublist_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listsublist_t *this = (listsublist_t *)o;

  this->index = fts_get_long(at);
}

static void
listsublist_length(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listsublist_t *this = (listsublist_t *)o;

  this->length = fts_get_long(at);
}

static void
listsublist_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listsublist_index(o, 0, 0, 1, at + 1);
  listsublist_length(o, 0, 0, 1, at + 2);  
}

static fts_status_t
listsublist_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listsublist_t), 3, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, listsublist_init, 3, a);

  /* define the system methods */

  fts_method_define_varargs(cl, 0, fts_s_list, listsublist_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, listsublist_index, 1, a);
  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int, listsublist_length, 1, a);

  /* Type the outlet */

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listsublist_config(void)
{
  fts_class_install(fts_new_symbol("listsublist"), listsublist_instantiate);
}





