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

#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  int i;
} int_t;

static void
int_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_t *this = (int_t *) o;

  fts_outlet_int(o, 0, this->i);
}

static void
int_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  int_t *this = (int_t *) o;

  if (fts_is_number(at))
    this->i = fts_get_number_int(at);
}

static void
int_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_t *this = (int_t *) o;

  if (fts_is_number(at))
    {
      this->i = fts_get_number_int(at);
      fts_outlet_int(o, 0, this->i);
    }
}

static void
int_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_input(o, winlet, 0, 1, at);
}

static void
int_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_t *this = (int_t *) o;
  
  this->i = 0.0;

  if (ac > 0 && fts_is_number(at))
    this->i = fts_get_number_int(at);
}

static void
int_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(int_t), int_init, 0);

  fts_class_message_varargs(cl, fts_s_bang, int_bang);
  fts_class_inlet_int(cl, 0, int_input);
  fts_class_inlet_float(cl, 0, int_input);
  fts_class_inlet_varargs(cl, 0, int_list);

  fts_class_inlet_int(cl, 1, int_set);
  fts_class_inlet_float(cl, 1, int_set);

  fts_class_outlet_int(cl, 0);
}

void
int_config(void)
{
  fts_class_t *cl = fts_class_install(fts_s_int, int_instantiate);
  fts_class_alias(cl, fts_new_symbol("i"));
}
