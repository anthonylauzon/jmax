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

typedef struct 
{
  fts_object_t o;
  fts_array_t compare;
} select_t;

static void
select_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  int n = fts_array_get_size(&this->compare);
  fts_atom_t *a = fts_array_get_atoms(&this->compare);
  int i;
  
  for(i=0; i<n; i++)
    {
      if(fts_atom_compare(at, a + i))
	{
	  fts_outlet_bang(o, i);
	  return;
	}
    }

  fts_outlet_atom(o, n, at);
}

static void
select_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  fts_atom_t *a = fts_array_get_atoms(&this->compare);

  fts_atom_assign(a + winlet - 1, at);
}

/***************************************
 *
 *  class
 *
 */

static void
select_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  int n = ac + 1;

  if(n < 2)
    n = 2;

  if(ac > 0)
    fts_array_init(&this->compare, ac , at);
  else
    {
      fts_array_init(&this->compare, 0 , 0);
      fts_array_append_int(&this->compare, 0);
    }

  fts_object_set_inlets_number(o, n);
  fts_object_set_outlets_number(o, n);
}

static void
select_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;

  fts_array_destroy(&this->compare);
}

static void 
select_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(select_t), select_init, select_delete);
  
  fts_class_inlet_atom(cl, 0, select_input);
  fts_class_inlet_atom(cl, 1, select_set);

  fts_class_outlet_bang(cl, 0);
  fts_class_outlet_varargs(cl, 1);
}

void
select_config(void)
{
  fts_class_t *cl = fts_class_install(fts_new_symbol("select"), select_instantiate);
  fts_class_alias(cl, fts_new_symbol("sel"));
}
