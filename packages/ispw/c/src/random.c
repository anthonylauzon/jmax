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
  long range;
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
random_in1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  random_t *this = (random_t *)o;
  long n = fts_get_int_arg(ac, at, 0, 0);

  if (n <= 1)
    n = 1;

  this->range = n;
}

static void
random_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  random_t *this = (random_t *)o;
  long n = fts_get_int_arg(ac, at, 1, 0);

  if (n < 1)
    n = 1;
  this->range = n;
}

static fts_status_t
random_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(random_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, random_init);

  fts_method_define_varargs(cl, 0, fts_s_bang, random_bang);
  fts_method_define_varargs(cl, 1, fts_s_int, random_in1);

  fts_outlet_type_define_varargs(cl, 0, fts_s_int);

  return fts_Success;
}

void
random_config(void)
{
  fts_class_install(fts_new_symbol("random"),random_instantiate);
}
