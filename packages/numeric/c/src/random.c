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

typedef struct random
{
  fts_object_t o;
  int range;
} random_t;

extern int rand (void);

static void
random_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  random_t *this = (random_t *)o;
  int n = (this->range * (rand() & 0x7fffL)) >> 15;

  fts_outlet_int(o, 0, n);
}

static void
random_set_range(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  random_t *this = (random_t *)o;
  int n = fts_get_number_int(at);

  if(n <= 1)
    n = 1;

  this->range = n;
}

static void
random_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  random_t *this = (random_t *)o;

  if(ac > 0 && fts_is_number(at))
    random_set_range(o, 0, 0, 1, at);
}

static void
random_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(random_t), random_init, 0);

  fts_class_message_varargs(cl, fts_s_bang, random_bang);
  fts_class_inlet_number(cl, 1, random_set_range);
  fts_class_outlet_int(cl, 0);
}

void
random_config(void)
{
  fts_class_install(fts_new_symbol("random"),random_instantiate);
}
