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
stripnote_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stripnote_t *x = (stripnote_t *)o;

  if (x->n)
    {
      fts_outlet_int(o, 1, x->n);
      fts_outlet_int(o, 0, fts_get_number_int(at));
    }
}

static void
stripnote_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stripnote_t *x = (stripnote_t *)o;

  x->n = fts_get_number_int(at);
}


static void
stripnote_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 1 && fts_is_number(at + 1))
    stripnote_number_1(o, winlet, s, 1, at + 1);

  if (ac > 0 && fts_is_number(at))
    stripnote_number(o, winlet, s, 1, at);
}

static void
stripnote_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(stripnote_t), 0, 0); 

  fts_class_inlet_varargs(cl, 0, stripnote_list);
  fts_class_inlet_number(cl, 0, stripnote_number);
  fts_class_inlet_number(cl, 1, stripnote_number_1);

  fts_class_outlet_int(cl, 0);
  fts_class_outlet_int(cl, 1);
}

void
stripnote_config(void)
{
  fts_class_install(fts_new_symbol("stripnote"),stripnote_instantiate);
}
