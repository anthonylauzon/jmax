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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */


#include "fts.h"

/*------------------------- gint class -------------------------------------*/

typedef struct
{
  fts_object_t o;
  long count;
} nbangs_t;



static void
nbangs_any(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;
  int i;
    
  for (i = 0; i < this->count; i++)
    {
      fts_outlet_int(o, 1, i);
      fts_outlet_bang(o, 0);
    }
}


static void
nbangs_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;
  int i;

  if ((ac >= 2) && fts_is_number(&at[1]))
    this->count = fts_get_number_int(&at[1]);
    
  for (i = 0; i < this->count; i++)
    {
      fts_outlet_int(o, 1, i);
      fts_outlet_bang(o, 0);
    }
}


static void
nbangs_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;

  this->count = fts_get_number_int(at);
}


static void
nbangs_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;

  this->count = fts_get_int_arg(ac, at, 1, 0);
}


static fts_status_t
nbangs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(nbangs_t), 2, 2, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, nbangs_init, 2, a, 1);

  fts_method_define_varargs(cl, 0, fts_s_list, nbangs_list);

  fts_method_define_varargs(cl, 0, fts_s_anything, nbangs_any);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, nbangs_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, nbangs_number_1, 1, a);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_bang, 0, 0);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 1, fts_s_int, 1, a);

  return fts_Success;
}

void
nbangs_config(void)
{
  fts_class_install(fts_new_symbol("nbangs"),nbangs_instantiate);
}

