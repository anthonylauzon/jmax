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

  fts_timer_t *timer;
} speedlim_t;

static void
speedlim_tick(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
speedlim_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  if(this->gate)
    {
      this->gate = 0;

      fts_timer_reset(this->timer);
      fts_timer_set_delay(this->timer, this->time, 0);

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

  this->time = fts_get_double_arg(ac, at, 0, 0);
}

static void
speedlim_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  fts_set_void(&this->atom);
  this->gate = 1;
  this->time = fts_get_double_arg(ac, at, 1, 0);    

  this->timer = fts_timer_new(o, 0);
}

static void
speedlim_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *this = (speedlim_t *)o;

  fts_timer_delete(this->timer);
}

static fts_status_t
speedlim_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(speedlim_t), 2, 1, 0); 

  /* define the system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, speedlim_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, speedlim_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_timer_alarm, speedlim_tick);

  fts_method_define_varargs(cl, 0, fts_s_int, speedlim_atom);
  fts_method_define_varargs(cl, 0, fts_s_float, speedlim_atom);
  fts_method_define_varargs(cl, 0, fts_s_symbol, speedlim_atom);

  fts_method_define_varargs(cl, 1, fts_s_int, speedlim_set_time);
  fts_method_define_varargs(cl, 1, fts_s_float, speedlim_set_time);

  return fts_Success;
}

void
speedlim_config(void)
{
  fts_class_install(fts_new_symbol("speedlim"),speedlim_instantiate);
}
