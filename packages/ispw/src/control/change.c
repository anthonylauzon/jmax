/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */



#include "fts.h"

/*------------------------- change class -------------------------------------*/

typedef struct {
  fts_object_t o;
  long r_state;
} change_t;

static void
change_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((change_t *)o)->r_state = (long) fts_get_int_arg(ac, at, 1, 0);
}

static void
change_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  long n;
  if ((n = fts_get_number_int(at)) != ((change_t *)o)->r_state)
    {
      fts_outlet_send(o, 0, fts_s_int, ac, at);
      ((change_t *)o)->r_state = n;
    }
}

static void
change_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((change_t *)o)->r_state = fts_get_long_arg(ac, at, 0, 0);
}

static fts_status_t
change_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(change_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, change_init, 2, a, 1);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, change_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, change_int, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_set, change_set, 1, a);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

void
change_config(void)
{
  fts_class_install(fts_new_symbol("change"),change_instantiate);
}

