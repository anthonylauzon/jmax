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
  fts_object_t o;
  int running;
  double start_value;
  double target;
  double slope;
  double start_time;
  double duration;
  double period;
} ramp_t;


static void
ramp_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;

  if(fts_object_outlet_is_connected(o, 0))
    {
      fts_outlet_bang(o, 1);
      fts_outlet_float(o, 0, this->target);
      this->running = 0;
    }
}

static void
ramp_increment_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;

 if(this->period > 0.0 && fts_object_outlet_is_connected(o, 0))
    {
      double time = fts_get_time() - this->start_time;
      double value = this->start_value + time * this->slope;
      double left = this->duration - time;
      
      if(left > this->period)
	fts_timebase_add_call(fts_get_timebase(), o, ramp_increment_and_output, 0, this->period);
      else
	fts_timebase_add_call(fts_get_timebase(), o, ramp_end, 0, left);
      
      fts_outlet_float(o, 0, value);
    }
}

static void
ramp_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;

  if(this->running)
    {
      double time = fts_get_time() - this->start_time;
      double value = this->start_value + time * this->slope;
      
      fts_timebase_remove_object(fts_get_timebase(), o);
      this->running = 0;
      
      this->target = value;
    }
}


static void
ramp_set_target(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;
  double target = fts_get_number_float(at);

  if(this->duration > 0.0)
    {
      double value = this->target;

      this->start_time = fts_get_time();
      this->start_value = value;
      this->target = target;
      this->slope = (target - value) / this->duration;
      
      if(this->duration > this->period)
	fts_timebase_add_call(fts_get_timebase(), o, ramp_increment_and_output, 0, this->period);
      else
	fts_timebase_add_call(fts_get_timebase(), o, ramp_end, 0, this->duration);
      
      this->running = 1;
    }
  else
    {
      this->target = target;
      this->running = 0;

      fts_outlet_float(o, 0, target);
    }
}

static void
ramp_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;
  double duration = fts_get_number_float(at);

  if(duration < 0.0)
    duration = 0.0;

  if(duration != this->duration)
    {
      this->duration = duration;    
      
      if(this->running)
	{
	  double time = fts_get_time() - this->start_time;
	  
	  if(time > duration)
	    {
	      fts_timebase_remove_object(fts_get_timebase(), o);
	      this->running = 0;	  
	      
	      fts_outlet_bang(o, 1);
	      fts_outlet_float(o, 0, this->target);
	    }
	  else if(time + this->period >= duration)
	    {
	      fts_timebase_remove_object(fts_get_timebase(), o);
	      
	    }
	  else
	    {
	      double value = this->start_value + time * this->slope;
	      
	      this->slope = (this->target - value) / duration; 
	      
	      /* fake start value */
	      this->start_value = value - time * this->slope;
	    }
	}
    }
}

static void
ramp_set_period(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;
  double period = fts_get_number_float(at);

  if(period > 0.0)
    this->period = period;
  else if(this->running)
    this->period = 0.0;
}

static void
ramp_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;
  int running = this->running;

  if(running)
    {
      fts_timebase_remove_object(fts_get_timebase(), o);
      this->running = 0;
    }

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	ramp_set_period(o, winlet, s, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	ramp_set_duration(o, winlet, s, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	{
	  double value = fts_get_number_float(at);
	  
	  this->target = value;

	  if(running)	    	      
	    fts_outlet_float(o, 0, value);
	}
    case 0:
      break;
    }
}

static void
ramp_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;

  if(this->running)
    {
      fts_timebase_remove_object(fts_get_timebase(), o);
      this->running = 0;

      this->target = this->start_value + (fts_get_time() - this->start_time) * this->slope;
    }

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	ramp_set_period(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	ramp_set_duration(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	ramp_set_target(o, 0, 0, 1, at + 0);
    case 0:
      break;
    }
}

static void
ramp_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ramp_t *this = (ramp_t *)o;

  this->running = 0;
  this->period = 20.0;

  ramp_set(o, 0, 0, ac, at);
}

static void
ramp_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(ramp_t), ramp_init, NULL);

  fts_class_message_varargs(cl, fts_s_stop, ramp_stop);
  fts_class_message_varargs(cl, fts_s_set, ramp_set);

  fts_class_inlet_varargs(cl, 0, ramp_varargs);
  fts_class_inlet_number(cl, 0, ramp_set_target);
  fts_class_inlet_number(cl, 1, ramp_set_duration);
  fts_class_inlet_number(cl, 2, ramp_set_period);

  fts_class_outlet_number(cl, 0);
  fts_class_outlet_bang(cl, 1);
}

void
ramp_config(void)
{
  fts_class_install(fts_new_symbol("ramp"), ramp_instantiate);
}
