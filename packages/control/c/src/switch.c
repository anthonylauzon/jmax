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
switch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
switch_default(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch_t *this = (switch_t *)o;

  if(winlet == 0 && this->on != 0)
    fts_outlet_send(o, 0, s, ac, at);
}

static void
switch_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch_t *this = (switch_t *)o;

  this->on = fts_get_number_int(at);
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

  fts_class_inlet_varargs(cl, 0, switch_default);
  fts_class_set_default_handler(cl, switch_default);

  fts_class_inlet_int(cl, 1, switch_set);
  fts_class_inlet_float(cl, 1, switch_set);

  fts_class_outlet_anything(cl, 0);
}

void
switch_config(void)
{
  fts_class_install(fts_new_symbol("switch"), switch_instantiate);
}
