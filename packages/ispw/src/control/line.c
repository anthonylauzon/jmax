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

#include "fts.h"

typedef struct 
{
  fts_object_t ob;
  fts_alarm_t alarm;
  double target;
  double cur;
  double inc;
  double grain;
  long steps;	
  long inval;	/* last value sent to inlet */
} line_t;

/* Tick function */

static void
line_int_tick(fts_alarm_t *alarm, void *o)
{
  line_t *this = (line_t *)o;

  if (this->steps)
    this->steps--;

  if (this->steps)
    {
      this->cur += this->inc;
      fts_alarm_set_delay(alarm, this->grain);
      fts_alarm_arm(alarm);
      fts_outlet_int((fts_object_t *)o, 0, this->cur);	
    }
  else
    {
      fts_alarm_unarm(alarm);
      fts_outlet_int((fts_object_t *)o, 0, this->target);
    }
}

static void
line_float_tick(fts_alarm_t *alarm, void *o)
{
  line_t *this = (line_t *)o;

  if (this->steps)
    this->steps--;

  if (this->steps)
    {
      this->cur += this->inc;

      fts_alarm_set_delay(alarm, this->grain);
      fts_alarm_arm(alarm);

      fts_outlet_float((fts_object_t *)o, 0, this->cur);	
    }
  else
    {
      fts_alarm_unarm(alarm);
      fts_outlet_float((fts_object_t *)o, 0, this->target);
    }
}

/* Methods */

/* STOP alarm */
static void
line_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;

  this->steps = 0;
  this->target = this->cur;
  fts_alarm_unarm(&this->alarm);
}


/* jump to given target val and STOP alarm */

static void
line_int_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double target = (double) fts_get_int_arg(ac, at, 0, 0);

  /* if some dweezul has talked to middle inlet, do fixfix */

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

      this->steps = (( this->inval - 1)/this->grain ) + 1;
      this->inc   = distance / this->steps;
      this->cur   = old_target;
      this->target = target;
      this->inval = 0;
      fts_alarm_arm(&this->alarm);
      line_int_tick(&this->alarm, this);
    }
  else
    {
      this->target = target;
      this->steps = 0;
      fts_alarm_unarm(&this->alarm);
      fts_outlet_int(o, 0, this->target);
    }
}

static void
line_float_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double target = (double) fts_get_float_arg(ac, at, 0, 0.0f);

  /* if some dweezul has talked to middle inlet, do fixfix */

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

      this->steps = (( this->inval - 1)/this->grain ) + 1;
      this->inc   = distance / this->steps;
      this->cur   = old_target;
      this->target = target;
      this->inval = 0;
      fts_alarm_arm(&this->alarm);
      line_float_tick(&this->alarm, this);
    }
  else
    {
      this->target = target;
      this->steps = 0;
      fts_alarm_unarm(&this->alarm);
      fts_outlet_float(o, 0, this->target);
    }
}

/* jump to given target val and STOP alarm */

static void
line_int_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double target = (double)fts_get_int_arg(ac, at, 0, 0);

  this->target = target;
  this->steps = 0;

  fts_alarm_unarm(&this->alarm);
}

static void
line_float_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double target = (double)fts_get_float_arg(ac, at, 0, 0.0f);

  this->target = target;
  this->steps = 0;

  fts_alarm_unarm(&this->alarm);
}

/* fix message into rite corner */

static void
line_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  double grain = fts_get_double_arg(ac, at, 0, 0);

  if( grain < 1.0)
    this->grain = 20.0;
  else
    this->grain = grain;
}

/* fix message into middle corner */

static void
line_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  if (n < 0)
    this->inval = 0;
  else 
    this->inval = n;
}

/* list method */

static void
line_int_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 3) && (fts_is_number(&at[2])))
    line_number_2(o, winlet, s, 1, at + 2);

  if ((ac >= 2) && (fts_is_number(&at[1])))
    line_number_1(o, winlet, s, 1, at + 1);

  if ((ac >= 1) && (fts_is_number(&at[0])))
    line_int_number(o, winlet, s, 1, at + 0);
}

static void
line_float_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 3) && (fts_is_number(&at[2])))
    line_number_2(o, winlet, s, 1, at + 2);

  if ((ac >= 2) && (fts_is_number(&at[1])))
    line_number_1(o, winlet, s, 1, at + 1);

  if ((ac >= 1) && (fts_is_number(&at[0])))
    line_float_number(o, winlet, s, 1, at + 0);
}

static void
line_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this   = (line_t *)o;
  void (* line_tick)(fts_alarm_t *, void *) = 0;
  fts_symbol_t clock = 0;
  double target;
  double grain;

  if(ac > 1 && fts_is_symbol(at + 1))
    {
      clock = fts_get_symbol(at + 1);
      target = (double) fts_get_float_arg(ac, at, 2, 0.0f);
      grain = fts_get_double_arg(ac, at, 3, 0);
    }
  else
    {
      target = (double) fts_get_float_arg(ac, at, 1, 0.0f);
      grain =  fts_get_double_arg(ac, at, 2, 0);    
    }

   if(ac < 1 || fts_is_long(at+1))
     line_tick = line_int_tick;
   else
     line_tick = line_float_tick;

  if(clock)
    {
      if(!fts_clock_exists(clock))
	post("line: warning clock %s does not exists, yet\n", fts_symbol_name(clock));
      fts_alarm_init(&this->alarm, clock, line_tick, this);
    }
  else
    fts_alarm_init(&this->alarm, 0, line_tick, this);

  this->target = target;
  this->steps  = 0;
  this->inval  = 0;

  if (grain < 1.0)
    this->grain = 20.0;
  else
    this->grain = grain;
}

static void
line_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  line_t *this = (line_t *)o;

  fts_alarm_unarm(&this->alarm);
}


static fts_status_t
line_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];

  /* initialize the class */

  fts_class_init(cl, sizeof(line_t), 3, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  a[2] = fts_s_number;
  a[3] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, line_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, line_delete, 0, 0);

  /* Line args */

  fts_method_define(cl, 0, fts_new_symbol("stop"), line_stop, 0, 0);

  if(ac <= 1  || fts_is_long(at+1))
    {
      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, line_int_number, 1, a);
      a[0] = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, line_int_number, 1, a);

      fts_method_define_varargs(cl, 0, fts_s_list, line_int_list);
 
      a[0] = fts_s_number;
      fts_method_define(cl, 0, fts_new_symbol("set"), line_int_set, 1, a);
 
      a[0] = fts_s_int;
      fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
    }
  else
    {
      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, line_float_number, 1, a);
      a[0] = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, line_float_number, 1, a);

      fts_method_define_varargs(cl, 0, fts_s_list, line_float_list);

      a[0] = fts_s_number;
      fts_method_define(cl, 0, fts_new_symbol("set"), line_float_set, 1, a);
 
      a[0] = fts_s_float;
      fts_outlet_type_define(cl, 0, fts_s_float, 1, a);
    }

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, line_number_1, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, line_number_1, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int, line_number_2, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, line_number_2, 1, a);

  return fts_Success;
}

static int
line_class_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return(
	 ((ac0 < 2 || fts_is_long(at0+1)) && (ac1 < 2 || fts_is_long(at1+1))) ||
	 ((ac0 >= 2 && fts_is_float(at0+1)) && (ac0 >= 2 && fts_is_float(at1+1)))
	 );
}

void
line_config(void)
{
  fts_metaclass_install(fts_new_symbol("line"), line_instantiate, line_class_equiv);
}




