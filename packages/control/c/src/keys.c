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

/************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
} keys_t;

/************************************************************
 *
 *  user methods
 *
 */

static void
keys_down(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send(o, 0, s, 1, at);
}

static void
keys_up(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send(o, 1, s, 1, at);
}

static void
keys_shift(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send(o, 2, s, 1, at);
}

static void
keys_alt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send(o, 3, s, 1, at);
}

static void
keys_ctrl(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send(o, 4, s, 1, at);
}

/************************************************************
 *
 *  class
 *
 */
static fts_status_t
keys_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(keys_t), 0, 5, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("down"), keys_down);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("up"), keys_up);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("shift"), keys_shift);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("alt"), keys_alt);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("ctrl"), keys_ctrl);

  return fts_Success;
}

void
keys_config(void)
{
  fts_metaclass_install(fts_new_symbol("keys"), keys_instantiate, fts_always_equiv);
}
