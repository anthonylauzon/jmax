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

/* delay class: delay a bang for a given amount of time. */

#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  double time;
  int active;
} delay_t;

static void
delay_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  delay_t *this = (delay_t *)o;

  this->active = 0;

  if(fts_object_outlet_is_connected(o, 0))
    fts_outlet_bang(o, 0);
}

static void
delay_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  delay_t *this = (delay_t *)o;

  if(this->active)
    fts_timebase_remove_object(fts_get_timebase(), o);

  if(this->time > 0.0)
    {
      fts_timebase_add_call(fts_get_timebase(), o, delay_output, 0, this->time);
      this->active = 1;
    }
  else
    fts_outlet_bang(o, 0);
}

static void
delay_stop(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  delay_t *this = (delay_t *)o;

  if(this->active)
    fts_timebase_remove_object(fts_get_timebase(), o);

  this->active = 0;
}

static void
delay_set_time(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  delay_t *this = (delay_t *)o;
  double time = fts_get_number_float(at);

  if(time > 0.0)
    this->time = time;
  else
    this->time = 0.0;

  this->time = time;
}

static void
delay_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  delay_t *this = (delay_t *)o;
  
  this->time = 0.0;
  this->active = 0;

  if(ac > 0 && fts_is_number(at))
    delay_set_time(o, 0, 1, at, fts_nix);
}

static void
delay_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(delay_t), delay_init, NULL);

  fts_class_message_varargs(cl, fts_s_stop, delay_stop);

  fts_class_inlet_bang(cl, 0, delay_input);
  fts_class_inlet_number(cl, 1, delay_set_time);

  fts_class_outlet_bang(cl, 0);
}

void
delay_config(void)
{
  fts_class_t *cl = fts_class_install(fts_new_symbol("delay"), delay_instantiate);

  fts_class_alias(cl, fts_new_symbol("del"));
}
