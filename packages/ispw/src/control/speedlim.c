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

  fts_alarm_t alarm;
} speedlim_t;


static void
speedlim_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *x = (speedlim_t *)o;

  if (fts_alarm_is_armed(&x->alarm))
    {
      x->atom = at[0];
    }
  else if (fts_alarm_is_in_future(&x->alarm))
    {
      x->atom = at[0];      
      fts_alarm_arm(&x->alarm);      
    }
  else
    {
      fts_outlet_send(o, 0, fts_get_type(at), 1, at);
      fts_alarm_set_delay(&x->alarm, x->time);      
    }
}

static void
speedlim_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *x = (speedlim_t *)o;

  x->time = fts_get_double_arg(ac, at, 0, 0);
}

static void
speedlim_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *x = (speedlim_t *)o;

  
  x->time = fts_get_double_arg(ac, at, 0, 0);
}

static void
speedlim_tick(fts_alarm_t *alarm, void *o)
{
  speedlim_t *x = (speedlim_t *)o;
  fts_atom_t *at = &(x->atom);

  if (fts_is_long(at))
    fts_outlet_int((fts_object_t *)o, 0, fts_get_long(at));
  else if (fts_is_float(at))
    fts_outlet_float((fts_object_t *)o, 0, fts_get_float(at));
  else
    fts_outlet_symbol((fts_object_t *)o, 0, fts_get_symbol(at));

  fts_alarm_unarm(alarm);
  fts_alarm_set_delay(alarm, x->time);
}


static void
speedlim_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *x = (speedlim_t *)o;
  fts_symbol_t clock = 0;

  if(ac > 1 && fts_is_symbol(at + 1))
    {
      clock = fts_get_symbol(at + 1);
      x->time = fts_get_double_arg(ac, at, 2, 0);
    }
  else
    {
      x->time = fts_get_double_arg(ac, at, 1, 0);    
    }

  if (clock)
    {
      if (!fts_clock_exists(clock))
	post("speedlim: warning: clock %s do not yet exists\n", fts_symbol_name(clock));

      fts_alarm_init(&x->alarm, clock, speedlim_tick, x);
    }
  else
    fts_alarm_init(&x->alarm, 0, speedlim_tick, x);
}

static void
speedlim_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  speedlim_t *x = (speedlim_t *)o;

  fts_alarm_unarm(&x->alarm);
}


static fts_status_t
speedlim_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[10];

  /* initialize the class */

  fts_class_init(cl, sizeof(speedlim_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, speedlim_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, speedlim_delete, 0, 0);

  /* Speedlim args */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, speedlim_atom, 1, a);
  fts_method_define(cl, 1, fts_s_int, speedlim_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, speedlim_atom, 1, a);
  fts_method_define(cl, 1, fts_s_float, speedlim_number_1, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_symbol, speedlim_atom, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, speedlim_list);

  return fts_Success;
}


void
speedlim_config(void)
{
  fts_class_install(fts_new_symbol("speedlim"),speedlim_instantiate);
}
