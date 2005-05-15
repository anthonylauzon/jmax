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

typedef struct
{
  fts_object_t o;
  int count;
} nbangs_t;

static void
nbangs_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  nbangs_t *this = (nbangs_t *) o;
  int winlet = fts_object_get_message_inlet(o);
  
  if(winlet == 0)
    {
      int i;
      
      if(ac > 0 && fts_is_number(at))
        this->count = fts_get_number_int(at);
      
      for(i=0; i<this->count; i++)
	{
	  fts_outlet_int(o, 1, i);
	  fts_outlet_bang(o, 0);
	}
    }
  else if(s == 0 && ac > 0 && fts_is_number(at))
    this->count = fts_get_number_int(at);
}

static void
nbangs_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
}

static void
nbangs_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  nbangs_t *this = (nbangs_t *) o;
  
  if(ac > 0 && fts_is_number(at + 0))
    this->count = fts_get_number_int(at);
}

static void
nbangs_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(nbangs_t), nbangs_init, 0);

  fts_class_input_handler(cl, nbangs_input);
  fts_class_inlet_int(cl, 1, nbangs_set);  
  
  fts_class_outlet_bang(cl, 0);
  fts_class_outlet_int(cl, 1);
}

void
nbangs_config(void)
{
  fts_class_install(fts_new_symbol("nbangs"), nbangs_instantiate);
}

