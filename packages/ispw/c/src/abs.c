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


#include <fts/fts.h>

#define LO 0
#define HI 1

typedef struct abs_t {
  fts_object_t _o;
} abs_t;

#define abs(x) ((x) > 0 ? (x) : -(x))

static void
abs_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_int(o, 0, abs(fts_get_long(at)));
}

static void
abs_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_float(o, 0, abs(fts_get_float(at)));
}

static fts_status_t
abs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(abs_t), 1, 1, 0);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, abs_float, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, abs_int, 1, a);

  return fts_Success;
}

void
abs_config(void)
{
  fts_class_install(fts_new_symbol("abs"), abs_instantiate);
}
