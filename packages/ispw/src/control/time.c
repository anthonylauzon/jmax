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

/* 
   The time class; return on outlet 0 the logical time of the
   given clock, when it receive the bang.
   Accept an optional argument, declaring the clock used; default
   to ms.

   Don't call this object time_t: it conflict with system types
   in many platforms.
*/

#include "fts.h"

typedef struct 
{
  fts_object_t ob;
  fts_symbol_t clock_sym;
} timeobj_t;

static void
time_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timeobj_t *x = (timeobj_t *)o;

  fts_outlet_float(o, 0, fts_clock_get_time(x->clock_sym));
}

static void
time_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timeobj_t *x = (timeobj_t *)o;

  x->clock_sym = fts_get_symbol_arg(ac, at, 1, 0);
}


static fts_status_t
time_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(timeobj_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, time_init, 2, a, 1);

  /* user methods */

  fts_method_define(cl, 0, fts_s_bang, time_bang, 0, 0);

  /* Type the outlet */

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);

  return fts_Success;
}


void
time_config(void)
{
  fts_class_install(fts_new_symbol("time"),time_instantiate);
}
