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
  fts_atom_t atom;
  double time;
  int gate;
} speedlim_t;

static void
speedlim_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  if(!fts_is_void(&this->atom))
    {
      /* copy atom to stack (allows correct recursion behaviour) */
      fts_atom_t atom = this->atom;
      
      fts_set_void(&this->atom);

      fts_outlet_send((fts_object_t *)o, 0, fts_get_selector(&atom), 1, &atom);
    }

  /* open gate */
  this->gate = 1;
}

static void
speedlim_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  if(this->gate)
    {
      this->gate = 0;

      fts_timebase_remove_object(fts_get_timebase(), o);
      fts_timebase_add_call(fts_get_timebase(), o, speedlim_output, 0, this->time);

      fts_outlet_send(o, 0, fts_get_selector(at), 1, at);      
    }
  else
    {
      /* store incoming atom  */
      this->atom = at[0];
    }
}

static void
speedlim_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  this->time = fts_get_number_float(at);
}

static void
speedlim_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  ac--;
  at++;

  fts_set_void(&this->atom);
  this->gate = 1;
  this->time = 0.0;

  if(fts_is_number(at))
    speedlim_set_time(o, 0, 0, 1, at);
}

static fts_status_t
speedlim_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(speedlim_t), 2, 1, 0); 

  /* define the system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, speedlim_init);

  fts_method_define_varargs(cl, 0, fts_s_int, speedlim_input);
  fts_method_define_varargs(cl, 0, fts_s_float, speedlim_input);
  fts_method_define_varargs(cl, 0, fts_s_symbol, speedlim_input);

  fts_method_define_varargs(cl, 1, fts_s_int, speedlim_set_time);
  fts_method_define_varargs(cl, 1, fts_s_float, speedlim_set_time);

  return fts_Success;
}

void
speedlim_config(void)
{
  fts_class_install(fts_new_symbol("speedlim"),speedlim_instantiate);
}
