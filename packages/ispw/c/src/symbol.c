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
 */

#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  fts_symbol_t s;
} symbol_obj_t;

static void
symbol_obj_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  symbol_obj_t *this = (symbol_obj_t *) o;

  fts_outlet_symbol(o, 0, this->s);
}

static void
symbol_obj_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  symbol_obj_t *this = (symbol_obj_t *) o;

  if(fts_is_symbol(at))
    this->s = fts_get_symbol(at);

  fts_outlet_symbol(o, 0, this->s);
}

static void
symbol_obj_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  symbol_obj_t *this = (symbol_obj_t *) o;

  this->s = fts_get_symbol(at);
}


static void
symbol_obj_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  symbol_obj_t *this = (symbol_obj_t *) o;
  
  if(fts_is_symbol(at))
    this->s = fts_get_symbol(at);
}

static void
symbol_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  symbol_obj_t *this = (symbol_obj_t *) o;
  
  if(ac > 0 && fts_is_symbol(at))
    this->s = fts_get_symbol(at);
  else
    fts_object_error(o, "argument required");
}

static void
symbol_obj_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(symbol_obj_t), symbol_obj_init, 0);

  fts_class_inlet_bang(cl, 0, symbol_obj_bang);

  fts_class_inlet_symbol(cl, 1, symbol_obj_symbol);
  fts_class_inlet_varargs(cl, 0, symbol_obj_varargs);
  fts_class_inlet_symbol(cl, 1, symbol_obj_set);

  fts_class_outlet_symbol(cl, 0);
}

void
symbol_obj_config(void)
{
  fts_class_install(fts_s_symbol, symbol_obj_instantiate);
}
