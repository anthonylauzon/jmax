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
  double cycle;
  long run;
  fts_alarm_t alarm;
} metro_t;


static void
metro_tick(fts_alarm_t *alarm, void *o)
{
  metro_t *x = (metro_t *)o;

  fts_alarm_set_delay(&x->alarm, x->cycle);
  
  fts_outlet_bang((fts_object_t *)o, 0);
}


static void
metro_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;

  x->run = 1;
  fts_alarm_set_delay(&x->alarm, x->cycle);
  fts_outlet_bang((fts_object_t *)o, 0);
}


static void
metro_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;

  x->run = 0;
  fts_alarm_reset(&x->alarm);
}


static void
metro_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  if (n)
    {
      x->run = 1;
      fts_alarm_set_delay(&x->alarm, x->cycle);
      fts_outlet_bang((fts_object_t *)o, 0);
    }
  else
    {
      x->run = 0;
      fts_alarm_reset(&x->alarm);
    }
}


/* Installed as int method for inlet 1 */

static void
metro_set_metro(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;
  double n = fts_get_double_arg(ac, at, 0, 0);

  if (n <= 0.0)
    n = 5.0;

  x->cycle = n;
}


static void
metro_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;

  fts_alarm_reset(&x->alarm);
}

static void
metro_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;
  float n;

  n = (float) fts_get_float_arg(ac, at, 1, 0.0f);    
  
  fts_alarm_init(&x->alarm, 0, metro_tick, x);

  if (n <= 0.0)
    x->cycle = 5.0;
  else
    x->cycle = n;
}


static fts_status_t
metro_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(metro_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, metro_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, metro_delete, 0, 0);

  /* Metro args */

  fts_method_define(cl, 0, fts_s_bang, metro_bang, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("stop"), metro_stop, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, metro_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, metro_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, metro_set_metro, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, metro_set_metro, 1, a);

  /* Type the outlet */

  fts_outlet_type_define(cl, 0,	fts_s_bang, 0, 0);

  return fts_Success;
}


void
metro_config(void)
{
  fts_class_install(fts_new_symbol("metro"),metro_instantiate);
}
