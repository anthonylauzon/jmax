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
  fts_object_t ob;
  double del;
  int active;
} delay_t;

static void
delay_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;

  this->active = 0;
  fts_outlet_bang(o, 0);
}

static void
delay_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;

  if(this->active)
    fts_timebase_remove_object(fts_get_timebase(), o);
  else
    this->active = 1;

  fts_timebase_add_call(fts_get_timebase(), o, delay_output, 0, this->del);
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
delay_set_delay_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  double delay = fts_get_number_float(at);

  if (delay == (double) 0.0)
    delay = 0.001;
  
  if (delay <= (double) 0.0)
    delay = 0.001;

  this->del = delay;
}

static void
delay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  
  ac--;
  at++;

  this->del = 0.001;
  this->active = 0;

  if(fts_is_number(at))
    delay_set_delay_time(o, 0, 0, 1, at);
}

static fts_status_t
delay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  /* initialize the class */
  fts_class_init(cl, sizeof(delay_t), 2, 1, 0); 

  /* define the system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, delay_init);

  fts_method_define_varargs(cl, 0, fts_s_bang, delay_input);
  fts_method_define_varargs(cl, 0, fts_s_stop, delay_stop);

  fts_method_define_varargs(cl, 1, fts_s_int, delay_set_delay_time);
  fts_method_define_varargs(cl, 1, fts_s_float, delay_set_delay_time);

  /* Type the outlet */
  fts_outlet_type_define(cl, 0,	fts_s_bang, 0, 0);

  return fts_Success;
}

void
delay_config(void)
{
  fts_class_install(fts_new_symbol("delay"),delay_instantiate);
  fts_alias_install(fts_new_symbol("del"), fts_new_symbol("delay"));
}
