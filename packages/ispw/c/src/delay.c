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
  fts_timer_t *timer;
  double del;
} delay_t;

static void
delay_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;

  fts_timer_reset(this->timer);
  fts_timer_set_delay(this->timer, this->del, 0);
}

static void
delay_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;

  fts_timer_reset(this->timer);
}

static void
delay_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  double n = fts_get_double_arg(ac, at, 0, 0);

  if (n == (double) 0.0)
    n = 0.001;
  
  if (n <= (double) 0.0)
    n = 0.001;

  this->del = n;
}

static void
delay_tick(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang((fts_object_t *)o, 0);
}

static void
delay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  
  ac--;
  at++;

  this->del = 0.001;

  if(fts_is_number(at))
    {
      double n = fts_get_number_float(at);    

      if (n == (double)0.0)
	n = 0.001;
      
      if (n <= (double)0.0)
	n = 0.001;
      
      this->del = n;
    }

  this->timer = fts_timer_new(o, 0);
}

static void
delay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;

  fts_timer_delete(this->timer);
}

static fts_status_t
delay_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  /* initialize the class */
  fts_class_init(cl, sizeof(delay_t), 2, 1, 0); 

  /* define the system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, delay_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, delay_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_timer_alarm, delay_tick);

  fts_method_define_varargs(cl, 0, fts_s_bang, delay_bang);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), delay_stop);

  fts_method_define_varargs(cl, 1, fts_s_int, delay_number_1);
  fts_method_define_varargs(cl, 1, fts_s_float, delay_number_1);

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
