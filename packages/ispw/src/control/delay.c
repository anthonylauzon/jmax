/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* delay class: delay a bang for a given amount of time. */

#include "fts.h"

typedef struct 
{
  fts_object_t ob;
  fts_alarm_t alarm;
  double del;
} delay_t;

static void
delay_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *x = (delay_t *)o;

  fts_alarm_set_delay(&x->alarm, x->del);
  fts_alarm_arm(&x->alarm);
}

static void
delay_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *x = (delay_t *)o;

  fts_alarm_unarm(&x->alarm);
}

static void
delay_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *x = (delay_t *)o;
  double n = fts_get_double_arg(ac, at, 0, 0);

  if (n == (double) 0.0)
    n = 0.001;
  
  if (n <= (double) 0.0)
    n = 0.001;

  x->del = n;
}

static void
delay_tick(fts_alarm_t *alarm, void *o)
{
  fts_alarm_unarm(alarm);	/* unarm the alarm, one shot only here */
  fts_outlet_bang((fts_object_t *)o, 0);
}

static void
delay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *x = (delay_t *)o;
  fts_symbol_t clock = 0;
  double n;


  if(ac > 1 && fts_is_symbol(at + 1))
    {
      clock = fts_get_symbol(at + 1);
      n =  fts_get_double_arg(ac, at, 2, 0.0);
    }
  else
    {
      n = fts_get_double_arg(ac, at, 1, 0.0);    
    }

  if (n == (double) 0.0)
    n = 0.001;
  
  if (n <= (double) 0.0)
    n = 0.001;

  x->del = n;

  if (clock)
    {
      if (! fts_clock_exists(clock))
	post("delay: warning: clock %s does not exists, yet\n", fts_symbol_name(clock));

      fts_alarm_init(&x->alarm, clock, delay_tick, x);
    }
  else
    fts_alarm_init(&x->alarm, 0, delay_tick, x);
}

static void
delay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *x = (delay_t *)o;

  fts_alarm_unarm(&x->alarm);
}

static fts_status_t
delay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(delay_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, delay_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, delay_delete, 0, 0);

  /* Delay args */

  fts_method_define(cl, 0, fts_s_bang, delay_bang, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("stop"), delay_stop, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, delay_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, delay_number_1, 1, a);

  /* Type the outlet */

  fts_outlet_type_define(cl, 0,	fts_s_bang, 0, 0);

  return fts_Success;
}

void
delay_config(void)
{
  fts_class_install(fts_new_symbol("delay"),delay_instantiate);
  fts_class_alias(fts_new_symbol("del"), fts_new_symbol("delay"));
}

