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



#include <limits.h>
#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  fts_atom_t state;
} change_t;

static void
change_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  if(!fts_is_number(&this->state) || fts_get_number_int(&this->state) != fts_get_int(at))
    {
      fts_atom_release(&this->state);
      this->state = at[0];
      fts_outlet_atom(o, 0, at);
    }
}

static void
change_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  if(!fts_is_number(&this->state) || fts_get_number_float(&this->state) != fts_get_float(at))
    {
      fts_atom_release(&this->state);
      this->state = at[0];
      fts_outlet_atom(o, 0, at);
    }
}

static void
change_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  if(!fts_is_symbol(&this->state) || fts_get_symbol(&this->state) != fts_get_symbol(at))
    {
      fts_atom_release(&this->state);
      this->state = at[0];
      fts_outlet_atom(o, 0, at);
    }
}

static void
change_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  if(!fts_atom_compare(&this->state, at))
    {
      fts_atom_assign(&this->state, at);
      fts_outlet_atom(o, 0, at);
    }
}

static void
change_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  fts_atom_assign(&this->state, at);
}

static void
change_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  fts_set_void(&this->state);

  if(ac > 0)
    fts_send_message(o, fts_s_set, ac, at);
}

static void
change_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  fts_atom_void(&this->state);
}

static void
change_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(change_t), change_init, change_delete);

  fts_class_message_varargs(cl, fts_s_set, change_set);
  
  fts_class_inlet_int(cl, 0, change_int);
  fts_class_inlet_float(cl, 0, change_float);
  fts_class_inlet_symbol(cl, 0, change_symbol);
  fts_class_inlet_atom(cl, 0, change_input);

  fts_class_inlet_atom(cl, 1, change_set);

  fts_class_outlet_atom(cl, 0);
}

void
change_config(void)
{
  fts_class_install(fts_new_symbol("change"), change_instantiate);
}

