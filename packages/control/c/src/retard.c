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

typedef struct 
{
  fts_object_t o;
  double time;
} retard_t;

/************************************************************
 *
 *  user methods
 *
 */

static void
retard_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_atom(o, 0, at);
}

static void
retard_input_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  retard_t *this = (retard_t *)o;

  if(this->time > 0.0)
    fts_timebase_add_call(fts_get_timebase(), o, retard_output, at, this->time);
  else
    fts_outlet_atom(o, 0, at);
}

static void
retard_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_timebase_flush_object(fts_get_timebase(), o);
}

static void
retard_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_timebase_remove_object(fts_get_timebase(), o);
}

static void
retard_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  retard_t *this = (retard_t *)o;
  double time = fts_get_number_float(at);

  if(time < 0.0)
    this->time = 0.0;
  else
    this->time = time;
}

/************************************************************
 *
 *  class
 *
 */
static void
retard_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  if(ac > 0 && fts_is_number(at))
    retard_set_time(o, 0, 0, 1, at);
}

static void
retard_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  retard_clear(o, 0, 0, 0, 0);
}

static void
retard_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(retard_t), retard_init, retard_delete);

  fts_class_message_varargs(cl, fts_s_flush, retard_flush);
  fts_class_message_varargs(cl, fts_s_clear, retard_clear);
  fts_class_message_varargs(cl, fts_s_stop, retard_clear);

  fts_class_inlet_int(cl, 0, retard_input_atom);
  fts_class_inlet_float(cl, 0, retard_input_atom);
  fts_class_inlet_symbol(cl, 0, retard_input_atom);
  fts_class_inlet_atom(cl, 0, retard_input_atom);

  fts_class_inlet_int(cl, 1, retard_set_time);
  fts_class_inlet_float(cl, 1, retard_set_time);

  fts_class_outlet_atom(cl, 0);
}

void
retard_config(void)
{
  fts_class_install(fts_new_symbol("retard"), retard_instantiate);
}
