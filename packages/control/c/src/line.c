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
  double target;
  double cur;
  double inc;
  double period;
  int steps;	
  int inval; /* last value sent to inlet */
} line_t;

static void
line_int_advance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
 if(fts_object_outlet_is_connected(o, 0))
    {
      line_t *this = (line_t *)o;
      
      if(this->steps)
	this->steps--;
      
      if(this->steps)
	{
	  this->cur += this->inc;
	  fts_timebase_add_call(fts_get_timebase(), o, line_int_advance, 0, this->period);
	  fts_outlet_int(o, 0, this->cur);	
	}
      else
	{
	  fts_outlet_int(o, 0, this->target);
	  this->running = 0;
	}
    }
}

static void
line_float_advance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
 if(fts_object_outlet_is_connected(o, 0))
    {
      line_t *this = (line_t *)o;
      
      if(this->steps)
	this->steps--;
      
      if(this->steps)
	{
	  this->cur += this->inc;
	  fts_timebase_add_call(fts_get_timebase(), o, line_int_advance, 0, this->period);
	  fts_outlet_float(o, 0, this->cur);	
	}
      else
	{
	  fts_outlet_float(o, 0, this->target);
	  this->running = 0;
	}
    }
}

static void
line_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;

  this->steps = 0;
  this->target = this->cur;

  fts_timebase_remove_object(fts_get_timebase(), o);
  this->running = 0;
}


static void
line_int_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double target = (double) fts_get_int_arg(ac, at, 0, 0);

  if(this->running)
    {
      fts_timebase_remove_object(fts_get_timebase(), o);
      this->running = 0;
    }

  if (this->inval)
    {
      double old_target, distance;

      if (this->steps)
	old_target = this->cur;
      else
	old_target = this->target;

      distance = target - old_target;

      if(this->inval < 1)
	this->inval = 1;

      this->steps = (( this->inval - 1) / this->period ) + 1;
      this->inc   = distance / this->steps;
      this->cur   = old_target;
      this->target = target;
      this->inval = 0;

      line_int_advance(o, 0, 0, 0, 0);
    }
  else
    {
      this->target = target;
      this->steps = 0;

      fts_outlet_int(o, 0, this->target);
    }
}

static void
line_float_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double target = (double) fts_get_float_arg(ac, at, 0, 0.0f);

  if(this->running)
    {
      fts_timebase_remove_object(fts_get_timebase(), o);
      this->running = 0;
    }

  if (this->inval)
    {
      double old_target, distance;

      if (this->steps)
	old_target = this->cur;
      else
	old_target = this->target;

      distance = target - old_target;

      if(this->inval < 1)
	this->inval = 1;

      this->steps = (( this->inval - 1)/this->period ) + 1;
      this->inc   = distance / this->steps;
      this->cur   = old_target;
      this->target = target;
      this->inval = 0;

      line_float_advance(o, 0, 0, 0, 0);
    }
  else
    {
      this->target = target;
      this->steps = 0;

      fts_outlet_float(o, 0, this->target);
    }
}

static void
line_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double target = 0.0;

  if(fts_is_number(at))
    target = fts_get_number_float(at);

  if(this->running)
    {
      fts_timebase_remove_object(fts_get_timebase(), o);
      this->running = 0;
    }

  this->target = target;
  this->steps = 0;
}

static void
line_set_period(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double period = fts_get_number_float(at);

  if( period < 1.0)
    this->period = 20.0;
  else
    this->period = period;
}

static void
line_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  int time = fts_get_number_int(at);

  if(time < 0)
    this->inval = 0;
  else 
    this->inval = time;
}

static void
line_int_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	line_set_period(o, winlet, s, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	line_set_time(o, winlet, s, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	line_int_number(o, winlet, s, 1, at + 0);
    case 0:
      break;
    }
}

static void
line_float_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	line_set_period(o, winlet, s, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	line_set_time(o, winlet, s, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	line_float_number(o, winlet, s, 1, at + 0);
    case 0:
      break;
    }
}

static void
line_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;

  this->target = 0.0;
  this->steps = 0;
  this->inval = 0;
  this->running = 0;

  switch(ac)
    {
    case 2:
      line_set_period(o, 0, 0, 1, at + 1);
    case 1:
      line_set_value(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

static fts_status_t
line_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(line_t), 3, 1, 0); 

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, line_init);

  fts_method_define_varargs(cl, 0, fts_s_stop, line_stop);

  if(ac == 0  || fts_is_int(at))
    {
      fts_method_define_varargs(cl, 0, fts_s_int, line_int_number);
      fts_method_define_varargs(cl, 0, fts_s_float, line_int_number);
      fts_method_define_varargs(cl, 0, fts_s_list, line_int_list); 
 
      fts_outlet_type_define_varargs(cl, 0, fts_s_int);
    }
  else
    {
      fts_method_define_varargs(cl, 0, fts_s_int, line_float_number);
      fts_method_define_varargs(cl, 0, fts_s_float, line_float_number);
      fts_method_define_varargs(cl, 0, fts_s_list, line_float_list);
 
      fts_outlet_type_define_varargs(cl, 0, fts_s_float);
    }

  fts_method_define_varargs(cl, 0, fts_s_set, line_set_value);

  fts_method_define_varargs(cl, 1, fts_s_int, line_set_time);
  fts_method_define_varargs(cl, 1, fts_s_float, line_set_time);

  fts_method_define_varargs(cl, 2, fts_s_int, line_set_period);
  fts_method_define_varargs(cl, 2, fts_s_float, line_set_period);

  return fts_ok;
}

void
line_config(void)
{
  fts_metaclass_install(fts_new_symbol("line"), line_instantiate, fts_arg_type_equiv);
}
