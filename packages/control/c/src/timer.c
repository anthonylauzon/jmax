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
 */

#include <fts/fts.h>

typedef struct 
{
  fts_object_t ob;
  int running;
  double start;
  double time;
} timer_t;

static void
timer_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
timer_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_t *this = (timer_t *)o;

  this->time = 0.0;
  this->start = fts_get_time();
}

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

  this->running = 0;
  this->start = fts_get_time();
  this->time = 0.0;
}

static void
timer_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(timer_t), timer_init, 0);

  fts_class_message_varargs(cl, fts_new_symbol("reset"), timer_reset);
  fts_class_message_varargs(cl, fts_s_start, timer_start);
  fts_class_message_varargs(cl, fts_s_stop, timer_stop);

  fts_class_inlet_bang(cl, 0, timer_send_time);
  fts_class_outlet_float(cl, 0);
}

void
timer_config(void)
{
  fts_class_install(fts_new_symbol("timer"), timer_instantiate);
}
