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
  long count;
} nbangs_t;

static void
nbangs_set_n(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;

  this->count = fts_get_number_int(at);
}

static void
nbangs_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;
  
  if(winlet == 0)
    {
      int i;
      
      if(ac > 1 && fts_is_number(at + 1))
	nbangs_set_n(o, 0, 0, 1, at + 1);
      
      for(i=0; i<this->count; i++)
	{
	  fts_outlet_int(o, 1, i);
	  fts_outlet_bang(o, 0);
	}
    }
}

static void
nbangs_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nbangs_t *this = (nbangs_t *) o;

  if(ac > 1 && fts_is_number(at + 1))
    nbangs_set_n(o, 0, 0, 1, at);
}

static void
nbangs_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(nbangs_t), nbangs_init, 0);

  fts_class_inlet_varargs(cl, 0, nbangs_input);
  fts_class_set_default_handler(cl, nbangs_input);

  fts_class_inlet_int(cl, 1, nbangs_set_n);
  fts_class_inlet_float(cl, 1, nbangs_set_n);

  fts_class_outlet_bang(cl, 0);
  fts_class_outlet_int(cl, 1);
}

void
nbangs_config(void)
{
  fts_class_install(fts_new_symbol("nbangs"),nbangs_instantiate);
}

