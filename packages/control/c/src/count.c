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
  int value;
  int begin;
  int end;
  int step;
  int reverse; /* running direction when for reverse mode */
} count_int_t;

typedef struct 
{
  fts_object_t o;
  enum count_mode mode;
  double value;
  double begin;
  double end;
  double step;
  double reverse;
} count_float_t;

/************************************************************
 *
 *  int methods
 *
 */

static void
count_int_step(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_int_t *this = (count_int_t *)o;
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

      fts_outlet_bang(o, 1);      
    }
  else
    this->value = value + step;
  
  fts_outlet_int(o, 0, value);
}

static void
count_int_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_int_t *this = (count_int_t *)o;

  this->value = fts_get_number_int(at);  
  this->reverse = 1;
}

static void
count_int_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_int_t *this = (count_int_t *)o;

  this->begin = fts_get_number_int(at);
}

static void
count_int_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_int_t *this = (count_int_t *)o;

  this->end = fts_get_number_int(at);
}

static void
count_int_set_step(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_int_t *this = (count_int_t *)o;
  int step = fts_get_number_int(at);

  if(step <= 0)
    this->step = 0;
  else
    this->step = step;
}

static void
count_int_set_parameters(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_int_t *this = (count_int_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	this->step = fts_get_number_int(at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->end = fts_get_number_int(at + 1);
    case 1:
      if(fts_is_number(at + 0))
	this->begin = fts_get_number_int(at + 0);
    case 0:
      break;
    }
}

static void
count_int_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_int_t *this = (count_int_t *)o;

  switch (ac)
    {
    default:
      if(fts_is_number(at + 1))
	count_int_set_parameters(o, 0, 0, ac - 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	count_int_set_value(o, 0, 0, 1, at + 0);
    case 0:
      break;
    }
}

static void
count_int_set_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  if(fts_is_array(value))
    {
      fts_array_t *list = fts_get_array(value);

      count_int_set(o, 0, 0, fts_array_get_size(list), fts_array_get_atoms(list));
    }
  else if(fts_is_number(value))
    count_int_set_value(o, 0, 0, 1, value);
}

static void
count_int_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_int_t *this = (count_int_t *)o;

  this->value = this->begin;
}

static void
count_int_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_int_t *this = (count_int_t *)o;
  fts_symbol_t mode = fts_get_symbol(at);
  int step = (this->begin < this->end)? this->step: -this->step;

  this->value -= step * this->reverse;

  if(mode == sym_clip)
    {
      this->mode = mode_clip;
      this->reverse = 1;
    }
  else if(mode == sym_wrap)
    {
      this->mode = mode_wrap;
      this->reverse = 1;
    }
  else if(mode == sym_reverse)
    this->mode = mode_reverse;

  this->value += step * this->reverse;
}

static void
count_int_set_mode_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  count_int_set_mode(o, 0, 0, 1, value);
}

/************************************************************
 *
 *  float methods
 *
 */

static void
count_float_step(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_float_t *this = (count_float_t *)o;
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
      
      fts_outlet_bang(o, 1);
    }
  else
    this->value = value + step;
  
  fts_outlet_float(o, 0, value);
}

static void
count_float_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_float_t *this = (count_float_t *)o;

  this->value = fts_get_number_float(at);
  this->reverse = 1;
}

static void
count_float_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_float_t *this = (count_float_t *)o;

  this->begin = fts_get_number_float(at);
}

static void
count_float_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_float_t *this = (count_float_t *)o;

  this->end = fts_get_number_float(at);
}

static void
count_float_set_step(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_float_t *this = (count_float_t *)o;
  double step = fts_get_number_float(at);

  if(step <= 0)
    this->step = 0;
  else
    this->step = step;
}

static void
count_float_set_parameters(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_float_t *this = (count_float_t *)o;

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
count_float_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_float_t *this = (count_float_t *)o;

  switch (ac)
    {
    default:
      if(fts_is_number(at + 1))
	count_float_set_parameters(o, 0, 0, ac - 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	count_float_set_value(o, 0, 0, 1, at + 0);
    case 0:
      break;
    }
}

static void
count_float_set_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  if(fts_is_array(value))
    {
      fts_array_t *list = fts_get_array(value);

      count_float_set(o, 0, 0, fts_array_get_size(list), fts_array_get_atoms(list));
    }
  else if(fts_is_number(value))
    count_float_set_value(o, 0, 0, 1, value);
}

static void
count_float_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_float_t *this = (count_float_t *)o;

  this->value = this->begin;
}

static void
count_float_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  count_float_t *this = (count_float_t *)o;
  fts_symbol_t mode = fts_get_symbol(at);
  double step = (this->begin < this->end)? this->step: -this->step;

  this->value -= step * this->reverse;

  if(mode == sym_clip)
    {
      this->mode = mode_clip;
      this->reverse = 1;
    }
  else if(mode == sym_wrap)
    {
      this->mode = mode_wrap;
      this->reverse = 1;
    }
  else if(mode == sym_reverse)
    this->mode = mode_reverse;

  this->value += step * this->reverse;
}

static void
count_float_set_mode_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  count_float_set_mode(o, 0, 0, 1, value);
}

/************************************************************
 *
 *  class
 *
 */

static void
count_int_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_int_t *this = (count_int_t *)o;

  ac--;
  at++;
  
  this->mode = mode_clip;
  this->value = 0;
  this->begin = 0;
  this->end = 127;
  this->step = 1;
  this->reverse = 1;

  count_int_set_parameters(o, 0, 0, ac, at);
  count_int_reset(o, 0, 0, 0, 0);
}

static void
count_float_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  count_float_t *this = (count_float_t *)o;

  ac--;
  at++;
  
  this->mode = mode_clip;
  this->value = 0.0;
  this->begin = 0.0;
  this->end = 0.1;
  this->step = 0.01;
  this->reverse = 1;

  count_float_set_parameters(o, 0, 0, ac, at);
  count_float_reset(o, 0, 0, 0, 0);
}

int
count_is_int(int ac, const fts_atom_t *at)
{
  int i;

  for(i=0; i<ac; i++)
    {
      if(!fts_is_int(at + i))
	return 0;
    }
  
  return 1;
}

static fts_status_t
count_float_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];
  int i;

  ac--;
  at++;

  for(i=0; i<ac; i++)
    if(!fts_is_number(at + i))
      return &fts_CannotInstantiate;

  if(count_is_int(ac, at))
    {
      fts_class_init(cl, sizeof(count_int_t), 4, 2, 0); 

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, count_int_init);

      fts_method_define_varargs(cl, 0, fts_new_symbol("mode"), count_int_set_mode);
      fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), count_int_set_mode_prop);

      fts_method_define_varargs(cl, 0, fts_s_set, count_int_set);
      fts_class_add_daemon(cl, obj_property_put, fts_s_set, count_int_set_prop);

      fts_method_define_varargs(cl, 0, fts_new_symbol("reset"), count_int_reset);
      fts_method_define_varargs(cl, 0, fts_s_bang, count_int_step);

      fts_method_define_varargs(cl, 1, fts_s_int, count_int_set_begin);
      fts_method_define_varargs(cl, 1, fts_s_float, count_int_set_begin);

      fts_method_define_varargs(cl, 2, fts_s_int, count_int_set_end);
      fts_method_define_varargs(cl, 2, fts_s_float, count_int_set_end);

      fts_method_define_varargs(cl, 3, fts_s_int, count_int_set_step);
      fts_method_define_varargs(cl, 3, fts_s_float, count_int_set_step);
    }
  else
    {
      fts_class_init(cl, sizeof(count_float_t), 4, 2, 0); 

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, count_float_init);

      fts_method_define_varargs(cl, 0, fts_new_symbol("mode"), count_float_set_mode);
      fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), count_float_set_mode_prop);

      fts_method_define_varargs(cl, 0, fts_s_set, count_float_set);
      fts_class_add_daemon(cl, obj_property_put, fts_s_set, count_float_set_prop);

      fts_method_define_varargs(cl, 0, fts_new_symbol("reset"), count_float_reset);
      fts_method_define_varargs(cl, 0, fts_s_bang, count_float_step);

      fts_method_define_varargs(cl, 1, fts_s_int, count_float_set_begin);
      fts_method_define_varargs(cl, 1, fts_s_float, count_float_set_begin);

      fts_method_define_varargs(cl, 2, fts_s_int, count_float_set_end);
      fts_method_define_varargs(cl, 2, fts_s_float, count_float_set_end);

      fts_method_define_varargs(cl, 3, fts_s_int, count_float_set_step);
      fts_method_define_varargs(cl, 3, fts_s_float, count_float_set_step);
    }

  return fts_Success;
}

int
count_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return count_is_int(ac0 - 1, at0 + 1) == count_is_int(ac1 - 1, at1 + 1);
}

void
count_config(void)
{
  sym_clip = fts_new_symbol("clip");
  sym_wrap = fts_new_symbol("wrap");
  sym_reverse = fts_new_symbol("reverse");

  fts_metaclass_install(fts_new_symbol("count"), count_float_instantiate, count_equiv);
}
