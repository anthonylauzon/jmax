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
  fts_object_t o;		/* fts_object_t inheritance */
  float f;			/* the accumulator of a float object */
} float_t;

static void
float_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_t *this = (float_t *) o;

  fts_outlet_float(o, 0, this->f);
}

static void
float_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  float_t *this = (float_t *) o;

  if (fts_is_number(at))
    this->f = fts_get_number_float(at);
}

static void
float_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_t *this = (float_t *) o;

  if (fts_is_number(at))
    {
      this->f = fts_get_number_float(at);
      fts_outlet_float(o, 0, this->f);
    }
}

static void
float_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_input(o, winlet, 0, 1, at);
}

static void
float_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_t *this = (float_t *) o;
  
  this->f = 0.0;

  if (ac > 0 && fts_is_number(at))
    this->f = fts_get_number_float(at);
}

static void
float_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(float_t), float_init, 0);

  fts_class_message_varargs(cl, fts_s_bang, float_bang);

  fts_class_inlet_varargs(cl, 0, float_list);
  fts_class_inlet_number(cl, 0, float_input);
  fts_class_inlet_number(cl, 1, float_set);

  fts_class_outlet_float(cl, 0);
}

void
float_config(void)
{
  fts_class_t *cl = fts_class_install(fts_new_symbol("float"),float_instantiate);
  fts_class_alias(cl, fts_new_symbol("f"));
}
