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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include <limits.h>
#include <float.h>

static fts_symbol_t sym_clip = 0;
static fts_symbol_t sym_wrap = 0;
static fts_symbol_t sym_reverse = 0;

/************************************************************
 *
 *  object
 *
 */

enum count_mode {mode_clip, mode_wrap, mode_reverse};

typedef struct 
{
  fts_object_t o;
  enum count_mode mode;
  double value;
  double begin;
  double end;
  double step;
  double reverse;
  int is_int;
} count_t;

static void
count_int_step(count_t *this)
{
  int value = this->value;
  int begin = this->begin;
  int end = this->end;
  int reverse = this->reverse;
  int target = (reverse > 0)? end: begin;
  int sign = (begin < end)? reverse: -reverse;
  int step = sign * this->step;
  
  if((value - target) * sign >= 0)
    {
      switch(this->mode)
	{
	case mode_clip:

	  value = target;

	  this->value = target + step;

	  break;

	case mode_wrap:
	  {
	    value += begin - end;
	    
	    if((value - target) * sign > 0)
	      value = target;

	    this->value = value + step;
	  }
	  break;
	  
	case mode_reverse:
	  {
	    int root = (reverse > 0)? begin: end;
	    
	    value = 2 * target - value;
	    
	    if((value - root) * sign < 0)
	      value = root;
	    
	    this->reverse = -reverse;
	    this->value = value - step;
	  }
	  break;
	}

      fts_outlet_bang((fts_object_t *)this, 1);
    }
  else
    this->value = value + step;
  
  fts_outlet_int((fts_object_t *)this, 0, value);
}

static void
count_float_step(count_t *this)
{
  double value = this->value;
  double begin = this->begin;
  double end = this->end;
  double reverse = this->reverse;
  double target = (reverse > 0)? end: begin;
  double sign = (begin < end)? reverse: -reverse;
  double step = sign * this->step;
  
  if((value - target) * sign >= 0)
    {
      switch(this->mode)
	{
	case mode_clip:

	  value = target;

	  this->value = value + step;

	  break;

	case mode_wrap:
	  {
	    value += begin - end;
	    
	    if((value - target) * sign > 0)
	      value = target;
	    
	    this->value = value + step;
	  }
	  break;
	  
	case mode_reverse:
	  {
	    double root = (reverse > 0)? begin: end;
	    
	    value = 2 * target - value;
	    
	    if((value - root) * sign < 0)
	      value = root;
	    
	    this->reverse = -reverse;
	    this->value = value - step;
	  }
	  break;
	}
      
      fts_outlet_bang((fts_object_t *)this, 1);
    }
  else
    this->value = value + step;
  
  fts_outlet_float((fts_object_t *)this, 0, value);
}

static void
count_step(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_t *this = (count_t *)o;
  
  if(this->is_int)
    count_int_step(this);
  else
    count_float_step(this);
}

static void
count_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_t *this = (count_t *)o;

  this->value = fts_get_number_float(at);
  this->reverse = 1.0;
}

static void
count_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_t *this = (count_t *)o;

  this->begin = fts_get_number_float(at);
}

static void
count_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_t *this = (count_t *)o;

  this->end = fts_get_number_float(at);
}

static void
count_set_step(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_t *this = (count_t *)o;
  double step = fts_get_number_float(at);

  if(step <= 0)
    this->step = 0;
  else
    this->step = step;
}

static void
count_set_parameters(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_t *this = (count_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	this->step = fts_get_number_float(at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->end = fts_get_number_float(at + 1);
    case 1:
      if(fts_is_number(at + 0))
	this->begin = fts_get_number_float(at + 0);
    case 0:
      break;
    }
}

static void
count_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_t *this = (count_t *)o;

  switch (ac)
    {
    default:
      if(fts_is_number(at + 1))
	count_set_parameters(o, 0, 0, ac - 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	count_set_value(o, 0, 0, 1, at + 0);
    case 0:
      break;
    }
}

static void
count_set_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  if(fts_is_tuple(value))
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(value);

      count_set(o, 0, 0, fts_tuple_get_size(tuple), fts_tuple_get_atoms(tuple));
    }
  else if(fts_is_number(value))
    count_set_value(o, 0, 0, 1, value);
}

static void
count_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_t *this = (count_t *)o;

  this->value = this->begin;
}

static void
count_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_t *this = (count_t *)o;
  fts_symbol_t mode = fts_get_symbol(at);
  double step = (this->begin < this->end)? this->step: -this->step;

  this->value -= step * this->reverse;

  if(mode == sym_clip)
    {
      this->mode = mode_clip;
      this->reverse = 1.0;
    }
  else if(mode == sym_wrap)
    {
      this->mode = mode_wrap;
      this->reverse = 1.0;
    }
  else if(mode == sym_reverse)
    this->mode = mode_reverse;

  this->value += step * this->reverse;
}

static void
count_set_mode_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  count_set_mode(o, 0, 0, 1, value);
}

/************************************************************
 *
 *  class
 *
 */

static void
count_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_t *this = (count_t *)o;
  int is_int = 1;
  int i;

  for(i=0; i<ac; i++)
    if(!fts_is_int(at + i))
      is_int = 0;

  this->mode = mode_wrap;
  this->value = 0.0;
  this->begin = 0.0;
  this->end = 0.5 * DBL_MAX;
  this->step = 0.01;
  this->reverse = 1.0;
  this->is_int = is_int;

  count_set_parameters(o, 0, 0, ac, at);
  count_reset(o, 0, 0, 0, 0);
}

static void
count_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(count_t), count_init, 0);
  
  fts_class_method_varargs(cl, fts_new_symbol("mode"), count_set_mode);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), count_set_mode_prop);
  
  fts_class_method_varargs(cl, fts_s_set, count_set);
  
  fts_class_method_varargs(cl, fts_new_symbol("reset"), count_reset);
  fts_class_method_varargs(cl, fts_s_bang, count_step);
  
  fts_class_inlet_number(cl, 1, count_set_begin);
  fts_class_inlet_number(cl, 2, count_set_end);
  fts_class_inlet_number(cl, 3, count_set_step);

  fts_class_outlet_number(cl, 0);
  fts_class_outlet_bang(cl, 1);  
}

void
count_config(void)
{
  sym_clip = fts_new_symbol("clip");
  sym_wrap = fts_new_symbol("wrap");
  sym_reverse = fts_new_symbol("reverse");

  fts_class_install(fts_new_symbol("count"), count_instantiate);
}
