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
  retard_t *this = (retard_t *)o;

  if(fts_is_tuple(at))
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(at);

      fts_tuple_output(o, 0, tuple);
      fts_object_release((fts_object_t *)tuple);
    }
  else
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
retard_input_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  retard_t *this = (retard_t *)o;
  
  if(this->time > 0.0)
    {
      if(ac == 1)
	retard_input_atom(o, 0, 0, 1, at);
      else if(ac > 1)
	{
	  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, ac, at);
	  fts_atom_t a;
	  
	  fts_set_object(&a, (fts_object_t *)tuple);
	  fts_object_refer((fts_object_t *)tuple);
	  retard_input_atom(o, 0, 0, 1, &a);
	}
    }
  else
    fts_outlet_atoms(o, 0, ac, at);
}

static void
retard_input_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  retard_t *this = (retard_t *)o;

  if(ac == 1 && s == fts_get_selector(at))
    {
      if(this->time > 0.0)
	retard_input_atom(o, 0, 0, 1, at);
      else
	fts_outlet_atom(o, 0, at);
    }
  else
    fts_object_signal_runtime_error(o, "Don't understand message %s", s);
}

static void
retard_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  retard_t *this = (retard_t *)o;

  fts_timebase_flush_object(fts_get_timebase(), o);
}

static void
retard_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  retard_t *this = (retard_t *)o;

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
  retard_t *this = (retard_t *)o;
  int size = 0;
  int bytes;
  fts_atom_t *atoms;
  int i;
  
  if(ac > 0 && fts_is_number(at))
    retard_set_time(o, 0, 0, 1, at);
}

static void
retard_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  retard_t *this = (retard_t *)o;

  retard_clear(o, 0, 0, 0, 0);
}

static fts_status_t
retard_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(retard_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, retard_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, retard_delete);

  fts_method_define_varargs(cl, 0, fts_s_int, retard_input_atom);
  fts_method_define_varargs(cl, 0, fts_s_float, retard_input_atom);
  fts_method_define_varargs(cl, 0, fts_s_symbol, retard_input_atom);
  fts_method_define_varargs(cl, 0, fts_s_list, retard_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_anything, retard_input_anything);

  fts_method_define_varargs(cl, 0, fts_s_flush, retard_flush);
  fts_method_define_varargs(cl, 0, fts_s_clear, retard_clear);
  fts_method_define_varargs(cl, 0, fts_s_stop, retard_clear);

  fts_method_define_varargs(cl, 1, fts_s_int, retard_set_time);
  fts_method_define_varargs(cl, 1, fts_s_float, retard_set_time);

  return fts_ok;
}

void
retard_config(void)
{
  fts_class_install(fts_new_symbol("retard"), retard_instantiate);
}
