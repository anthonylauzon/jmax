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
 * Authors: Norbert Schnell.
 *
 */

#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  fts_atom_t state;
  double time;
  int gate;
} speedlim_t;

static void
speedlim_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  if(fts_object_outlet_is_connected(o, 0))
    {
      if(!fts_is_void(&this->state))
	{
	  fts_atom_t output = this->state;
	  
	  /* clear state (for eventual recursion) */
	  fts_set_void(&this->state);
	  
	  /* send atom */
	  fts_outlet_varargs(o, 0, 1, &output);
	  
	  /* clear stack */
	  fts_atom_void(&output);
	}
    }
  else
    fts_atom_void(&this->state);
    
  /* open gate */
  this->gate = 1;
}

static void
speedlim_thru_and_close(fts_object_t *o, int winlet, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  /* close gate */
  this->gate = 0;
  
  /* shedule next output */
  fts_timebase_add_call(fts_get_timebase(), o, speedlim_output, NULL, this->time);
  
  /* let values thru */
  fts_outlet_varargs(o, 0, ac, at);
}

static void
speedlim_input_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  if(this->gate)
    speedlim_thru_and_close(o, 0, 1, at);
  else
    fts_atom_assign(&this->state, at); /* schedule for next opening */
}

static void
speedlim_input_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;
  
  if(this->gate)
   speedlim_thru_and_close(o, 0, ac, at);
  else
    {
      if(ac == 1)
	speedlim_input_atom(o, 0, 0, 1, at);
      else if(ac > 1)
	{
	  fts_object_t *tuple = fts_object_create(fts_tuple_class, NULL, ac, at);
	  fts_atom_t a;
	  
	  fts_set_object(&a, tuple);
	  speedlim_input_atom(o, 0, 0, 1, &a);
	}
    }
}

static void
speedlim_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;
  double time = fts_get_number_float(at);

  if(time > 0.0)
    this->time = time;
  else
    this->time = 0.0;
}

static void
speedlim_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  fts_set_void(&this->state);
  this->gate = 1;
  this->time = 0.0;

  if(ac > 0 && fts_is_number(at))
    speedlim_set_time(o, 0, 0, 1, at);
}

static void
speedlim_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(speedlim_t), speedlim_init, 0);

  fts_class_inlet_varargs(cl, 0, speedlim_input_atoms);
  fts_class_inlet_number(cl, 0, speedlim_input_atom);
  fts_class_inlet_symbol(cl, 0, speedlim_input_atom);
  fts_class_inlet_number(cl, 1, speedlim_set_time);

  fts_class_outlet_varargs(cl, 0);
}

void
speedlim_config(void)
{
  fts_class_install(fts_new_symbol("speedlim"), speedlim_instantiate);
}
