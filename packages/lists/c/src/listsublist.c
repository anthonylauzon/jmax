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

#define MAXINT 2147483647

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
  int n = (this->index + this->length <= ac)? this->length: ac - this->index;
  int i = this->index;

  if(n > 0 && this->index < ac)
    fts_outlet_atoms(o, 0, n, at + this->index);
}

static void
listsublist_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listsublist_t *this = (listsublist_t *)o;

  this->index = fts_get_number_int(at);
}

static void
listsublist_length(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listsublist_t *this = (listsublist_t *)o;

  this->length = fts_get_number_int(at);
}

static void
listsublist_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listsublist_t *this = (listsublist_t *)o;

  ac--;
  at++;

  this->index = 0;
  this->length = MAXINT;

  if(ac == 1 && fts_is_number(at))
    this->length = fts_get_number_int(at);
  else if(ac == 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      this->index = fts_get_number_int(at);
      this->length = fts_get_number_int(at + 1);
    }
  else
    fts_object_set_error(o, "Two arguments of number required");
}

static fts_status_t
listsublist_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(listsublist_t), 3, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listsublist_init);

  fts_method_define_varargs(cl, 0, fts_s_list, listsublist_list);

  fts_method_define_varargs(cl, 1, fts_s_int, listsublist_index);
  fts_method_define_varargs(cl, 1, fts_s_float, listsublist_index);

  fts_method_define_varargs(cl, 2, fts_s_int, listsublist_length);
  fts_method_define_varargs(cl, 2, fts_s_float, listsublist_length);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listsublist_config(void)
{
  fts_class_install(fts_new_symbol("listsublist"), listsublist_instantiate);
  fts_alias_install(fts_new_symbol("lsub"), fts_new_symbol("listsublist"));
}