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

typedef struct gate
{
  fts_object_t o;
  int opened;
} gate_t;

static void gate_open(fts_object_t *o, int winlet, fts_symbol_t s,
		      int ac, const fts_atom_t *at)
{
  ((gate_t *)o)->opened = fts_get_int_arg(ac, at, 0, 0);
}

static void gate_realize(fts_object_t *o, int winlet, fts_symbol_t s,
			 int ac, const fts_atom_t *at)
{
  if (((gate_t *)o)->opened)
    fts_outlet_send(o, 0, s, ac, at);
}

static void
gate_init(fts_object_t *o, int winlet, fts_symbol_t s,
	  int ac, const fts_atom_t *at)
{
  ((gate_t *)o)->opened = fts_get_long_arg(ac, at, 1, 0);
}

static fts_status_t
gate_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(gate_t), 2, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, gate_init, 2, a, 1);


  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_s_anything, gate_open, 1, a);

  fts_method_define_varargs(cl, 1, fts_s_anything, gate_realize);

  return fts_Success;
}

void
gate_config(void)
{
  fts_class_install(fts_new_symbol("gate"),gate_instantiate);
}
