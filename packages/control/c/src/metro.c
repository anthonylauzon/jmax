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
  fts_object_t ob;
  double period;
  int active;
} metro_t;


static void
metro_tick(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *this = (metro_t *)o;

  if(fts_object_outlet_is_connected(o, 0))
    {
      fts_timebase_add_call(fts_get_timebase(), o, metro_tick, 0, this->period);
      fts_outlet_bang((fts_object_t *)o, 0);
    }
}

static void
metro_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *this = (metro_t *)o;

  if(this->active)
    fts_timebase_remove_object(fts_get_timebase(), o);

  fts_timebase_add_call(fts_get_timebase(), o, metro_tick, 0, this->period);
  this->active = 1;

  fts_outlet_bang((fts_object_t *)o, 0);
}

static void
metro_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *this = (metro_t *)o;

  if(this->active)
    fts_timebase_remove_object(fts_get_timebase(), o);

  this->active = 0;
}

static void
metro_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *this = (metro_t *)o;
  int active = fts_get_number_int(at);

  if(active)
    metro_start(o, 0, 0, 0, 0);
  else
    metro_stop(o, 0, 0, 0, 0);
}

static void
metro_set_period(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *this = (metro_t *)o;
  double period = fts_get_number_float(at);

  if(period > 0.01)
    this->period = period;
  else
    this->period = 0.01;
}

static void
metro_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *this = (metro_t *)o;

  this->period = 100.0;
  this->active = 0;

  if(ac > 0 && fts_is_number(at))
    metro_set_period(o, 0, 0, 1, at);
}

static void
metro_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *this = (metro_t *)o;

  metro_stop(o, 0, 0, 0, 0);
}

static void
metro_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(metro_t), metro_init, metro_delete);

  fts_class_message_varargs(cl, fts_s_bang, metro_start);
  fts_class_message_varargs(cl, fts_s_start, metro_start);
  fts_class_message_varargs(cl, fts_s_stop, metro_stop);

  fts_class_inlet_int(cl, 0, metro_number);
  fts_class_inlet_float(cl, 0, metro_number);

  fts_class_inlet_int(cl, 1, metro_set_period);
  fts_class_inlet_float(cl, 1, metro_set_period);

  fts_class_outlet_bang(cl, 0);
}


void
metro_config(void)
{
  fts_class_install(fts_new_symbol("metro"), metro_instantiate);
}
