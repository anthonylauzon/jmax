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
  int opened;
} gate_t;

static void 
gate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gate_t *this = (gate_t *)o;

  if(winlet == 1)
    {
      if(this->opened)
	fts_outlet_send(o, 0, s, ac, at);
    }
  else if(s == NULL && ac > 0 && fts_is_number(at))
    this->opened = fts_get_number_int(at);
  else
    fts_object_error(o, "bad input at inlet 0");
}

static void
gate_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gate_t *this = (gate_t *)o;

  if(ac > 0 && fts_is_number(at))
    this->opened = fts_get_number_int(at);
}

static void
gate_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(gate_t), gate_init, 0);

  fts_class_input_handler(cl, gate_input);

  fts_class_inlet_thru(cl, 1);
  fts_class_outlet_thru(cl, 0);
}

void
gate_config(void)
{
  fts_class_install(fts_new_symbol("gate"), gate_instantiate);
}
