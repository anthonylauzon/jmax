/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>

typedef struct {
  fts_object_t o;
  long int_bound[2];
  float float_bound[2];
} split_t;

static void
split_int(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  split_t *this = (split_t *)o;
  long l = fts_get_int(at);

  if (l <= this->int_bound[1] && l >= this->int_bound[0])
    fts_outlet_int(o, 0, l);
  else
    fts_outlet_int(o, 1, l);
}

static void
split_float(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  split_t *this = (split_t *)o;
  float f = fts_get_float(at);

  if (f <= this->float_bound[1] && f >= this->float_bound[0])
    fts_outlet_float(o, 0, f);
  else
    fts_outlet_float(o, 1, f);
}

static void
split_bound(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  split_t *this = (split_t *)o;
  float float_bound = fts_get_float_arg(ac, at, 0, 0.0f);
  long int_bound = float_bound;
  int winlet = fts_object_get_message_inlet(o);

  this->int_bound[winlet-1] = ((float)int_bound == float_bound)? int_bound: int_bound + 1;
  this->float_bound[winlet-1] = float_bound;
}

static void
split_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fts_object_set_message_inlet(o, 1);
  split_bound(o, 0, ac, at, fts_nix);
  
  fts_object_set_message_inlet(o, 2);
  split_bound(o, 0, ac - 1, at + 1, fts_nix);
}

static void
split_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  if(ac >= 1)
    {
      if(fts_is_float(at))
	split_float(o, 0, 1, at, fts_nix);
      else if(fts_is_float(at))
	split_int(o, 0, 1, at, fts_nix);	
    }

  if(ac >= 2)
  {
    fts_object_set_message_inlet(o, 1);
    split_bound(o, 0, 1, at + 1, fts_nix);
  }
  
  if(ac >= 3)
  {
    fts_object_set_message_inlet(o, 2);
    split_bound(o, 0, 1, at + 2, fts_nix);
  }
}

static void
split_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(split_t), split_init, 0);

  fts_class_inlet_varargs(cl, 0, split_varargs);
  fts_class_inlet_int(cl, 0, split_int);
  fts_class_inlet_float(cl, 0, split_float);
  fts_class_inlet_number(cl, 1, split_bound);
  fts_class_inlet_number(cl, 2, split_bound);

  fts_class_outlet_number(cl, 0);
  fts_class_outlet_number(cl, 1);
}

void
split_config(void)
{
  fts_class_install(fts_new_symbol("split"),split_instantiate);
}

