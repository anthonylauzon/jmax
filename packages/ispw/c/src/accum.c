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
  accum_t *this = (accum_t *)o;

  accum_set(o, 0, 0, 1, at);
}

static fts_status_t
accum_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(accum_t), 3, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, accum_init);

  fts_method_define_varargs(cl, 0, fts_s_set, accum_set);

  fts_method_define_varargs(cl, 1, fts_s_int, accum_add);
  fts_method_define_varargs(cl, 1, fts_s_float, accum_add);

  fts_method_define_varargs(cl, 2, fts_s_int, accum_mul);
  fts_method_define_varargs(cl, 2, fts_s_float, accum_mul);

  fts_method_define_varargs(cl, 0, fts_s_bang, accum_bang);
  fts_method_define_varargs(cl, 0, fts_s_int, accum_number);
  fts_method_define_varargs(cl, 0, fts_s_float, accum_number);
  
  return fts_ok;
}

void
accum_config(void)
{
  fts_class_install(fts_new_symbol("accum"), accum_instantiate);
}

