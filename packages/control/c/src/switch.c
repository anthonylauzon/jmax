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

/************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
  int on;
} switch_t;

static void
switch_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  switch_t *this = (switch_t *)o;
  
  this->on = fts_get_int_arg(ac, at, 0, 0);
}

/************************************************************
 *
 *  user methods
 *
 */

static void
switch_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  switch_t *this = (switch_t *)o;
  int winlet = fts_object_get_message_inlet(o);

  if(winlet == 0)
    {
      if(this->on != 0)
	fts_outlet_send(o, 0, s, ac, at);
    }
  else if(s == 0 && ac > 0 && fts_is_number(at))
    this->on = fts_get_number_int(at);    
}

static void
switch_dummy(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
}

/************************************************************
 *
 *  class
 *
 */
static void
switch_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(switch_t), switch_init, NULL);

  fts_class_input_handler(cl, switch_input);

  fts_class_inlet_thru(cl, 0);
  fts_class_inlet_number(cl, 1, switch_dummy); /* number dummy */

  fts_class_outlet_thru(cl, 0);
}

void
switch_config(void)
{
  fts_class_install(fts_new_symbol("switch"), switch_instantiate);
}
