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
 */


#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  double value;
  int is_int;
} accum_t;

static void
accum_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  accum_t *this = (accum_t *)o;

  if(this->is_int)
    {
      int i = fts_get_number_int(at);

      this->value = i;
      fts_outlet_int(o, 0, i);
    }
  else
    {
      double f = fts_get_number_float(at);

      this->value = f;
      fts_outlet_float(o, 0, f);
    }
}

static void
accum_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  accum_t *this = (accum_t *)o;

  if(this->is_int)
    fts_outlet_int(o, 0, this->value);
  else
    fts_outlet_float(o, 0, this->value);
}

static void
accum_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  accum_t *this = (accum_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      if(this->is_int)
	this->value = fts_get_number_int(at);
      else
	this->value = fts_get_number_float(at);
    }
}

static void
accum_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  accum_t *this = (accum_t *)o;

  if(this->is_int)
    this->value += fts_get_number_int(at);    
  else
    this->value += fts_get_number_float(at);
}

static void
accum_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  accum_t *this = (accum_t *)o;

  if(this->is_int)
    this->value *= fts_get_number_int(at);
  else
    this->value *= fts_get_number_float(at);
}

static void
accum_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  accum_set(o, 0, 0, ac, at);
}

static void
accum_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(accum_t), accum_init, NULL);

  fts_class_message_varargs(cl, fts_s_set, accum_set);

  fts_class_inlet_int(cl, 1, accum_add);
  fts_class_inlet_float(cl, 1, accum_add);

  fts_class_inlet_int(cl, 2, accum_mul);
  fts_class_inlet_float(cl, 2, accum_mul);

  fts_class_inlet_bang(cl, 0, accum_bang);
  fts_class_inlet_int(cl, 0, accum_number);
  fts_class_inlet_float(cl, 0, accum_number);

  fts_class_outlet_number(cl, 0);
}

void
accum_config(void)
{
  fts_class_install(fts_new_symbol("accum"), accum_instantiate);
}

