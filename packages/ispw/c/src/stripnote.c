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

/* Stripnote class */

#include <fts/fts.h>

typedef struct
{
  fts_object_t ob;
  long n;
} stripnote_t;

static void
stripnote_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stripnote_t *x = (stripnote_t *)o;

  if (x->n)
    {
      fts_outlet_int(o, 1, x->n);
      fts_outlet_send(o, 0, fts_s_int, ac, at);
    }
}

static void
stripnote_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stripnote_t *x = (stripnote_t *)o;

  if (x->n)
    fts_outlet_int(o, 0, (long) fts_get_float(at));
}

static void
stripnote_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stripnote_t *x = (stripnote_t *)o;

  x->n = fts_get_int_arg(ac, at, 0, 0);
}


static void
stripnote_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 2) && fts_is_number(&at[1]))
    stripnote_number_1(o, winlet, s, 1, at + 1);

  if (ac >= 1)
    {
      if (fts_is_long(&at[0]))
	stripnote_int(o, winlet, s, 1, at);
      else
	if (fts_is_float(&at[0]))
	  stripnote_float(o, winlet, s, 1, at);
    }
}

static fts_status_t
stripnote_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[10];

  /* initialize the class */

  fts_class_init(cl, sizeof(stripnote_t), 2, 2, 0); 

  /* Stripnote methods */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, stripnote_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, stripnote_float, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, stripnote_number_1, 1, a);

  fts_method_define_varargs(cl, 1, fts_s_list, stripnote_list);

  /* Type the outlet */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 1,	fts_s_int, 1, a);

  return fts_Success;
}

void
stripnote_config(void)
{
  fts_class_install(fts_new_symbol("stripnote"),stripnote_instantiate);
}
