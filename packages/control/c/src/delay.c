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
delay_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;

  this->active = 0;

  if(fts_object_outlet_is_connected(o, 0))
    fts_outlet_bang(o, 0);
}

static void
delay_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
delay_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;

  if(this->active)
    fts_timebase_remove_object(fts_get_timebase(), o);

  this->active = 0;
}

static void
delay_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
delay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  
  this->time = 0.0;
  this->active = 0;

  if(fts_is_number(at))
    delay_set_time(o, 0, 0, 1, at);
}

static fts_status_t
delay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(delay_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, delay_init);

  fts_method_define_varargs(cl, 0, fts_s_bang, delay_input);
  fts_method_define_varargs(cl, 0, fts_s_stop, delay_stop);

  fts_method_define_varargs(cl, 1, fts_s_int, delay_set_time);
  fts_method_define_varargs(cl, 1, fts_s_float, delay_set_time);

  fts_outlet_type_define(cl, 0,	fts_s_bang);

  return fts_ok;
}

void
delay_config(void)
{
  fts_metaclass_t *mcl = fts_class_install(fts_new_symbol("delay"), delay_instantiate);

  fts_metaclass_alias(mcl, fts_new_symbol("del"));
}
