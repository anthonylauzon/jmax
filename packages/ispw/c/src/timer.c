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
  int running;
  double start;
  double time;
} timer_t;

/* store time-now on default bang message */

static void
timer_zero(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_t *this = (timer_t *)o;

  this->time = 0.0;
  this->start = fts_get_time();
}

static void
timer_continue(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_t *this = (timer_t *)o;

  if(!this->running)
    {
      this->running = 1;
      this->start = fts_get_time();
    }
}

static void
timer_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_t *this = (timer_t *)o;

  this->running = 0;
  this->time += (fts_get_time() - this->start);
}

static void
timer_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_t *this = (timer_t *)o;
  double now = fts_get_time();

  this->time = now; 

  if(!this->running)
    {
      this->running = 1;
      this->start = now;
    }
}

/* bang message into rite corner sends fix out  */

static void
timer_send_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_t *this = (timer_t *)o;
  double time = this->time;

  if(this->running)
    time += (fts_get_time() - this->start);

  fts_outlet_float(o, 0, time);
}

static void
timer_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_t *this = (timer_t *)o;

  this->running = 1;
  this->start = fts_get_time();
  this->time = 0.0;
}

static fts_status_t
timer_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(timer_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, timer_init);

  /* user methods */
  fts_method_define_varargs(cl, 0, fts_new_symbol("zero"), timer_zero);
  fts_method_define_varargs(cl, 0, fts_new_symbol("start"), timer_start);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), timer_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("continue"), timer_continue);
  fts_method_define_varargs(cl, 0, fts_new_symbol("time"), timer_send_time);

  fts_method_define_varargs(cl, 0, fts_s_bang, timer_zero);
  fts_method_define_varargs(cl, 1, fts_s_bang, timer_send_time);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_int);

  return fts_Success;
}

void
timer_config(void)
{
  fts_class_install(fts_new_symbol("timer"), timer_instantiate);
}
