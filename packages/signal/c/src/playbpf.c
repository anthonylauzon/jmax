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
 * Authors: Riccardo Borghesi, Francois Dechelle, Norbert Schnell
 *
 */

#include <fts/fts.h>
#include "bpf.h"

/************************************************************
 *
 *  play
 *
 */

typedef struct _play_bpf_
{
  fts_object_t o;

  bpf_t *bpf;
  int index; /* next break point index */
  double slope; /* line segment slope */

  double time; /* current segement time */
  double incr; /* time increment (speed / sample rate) */
  double target; /* target pasition */

  enum play_mode {mode_stop, mode_play, mode_loop, mode_repeat} mode;
  double loop_time;
  
  double speed; /* play speed */
  double sp; /* sample rate */

  fts_alarm_t alarm;
} play_bpf_t;

static fts_symbol_t sym_play = 0;
static fts_symbol_t sym_end = 0;

/************************************************************
 *
 *  methods
 *
 */

/* set index and slope according to desired time */
static void
play_bpf_locate(play_bpf_t *this, double time, double incr)
{
  bpf_t *bpf = this->bpf;
  int size = bpf_get_size(bpf);
  double duration = bpf_get_duration(bpf);

  if(size < 2)
    {
      this->index = 0;
      this->slope = 0.0;

      this->time = 0.0;
      this->incr = 0.0;
    }
  else
    {
      int index = this->index;

      if(time < 0.0)
	time = 0.0;
      else if(time > duration)
	time = duration;
      
      if(time > bpf_get_time(bpf, index + 1))
	{
	  do 
	    index++;
	  while(time > bpf_get_time(bpf, index + 1));
	}
      else if(time < bpf_get_time(bpf, index))
	{
	  do 
	    index--;
	  while(time < bpf_get_time(bpf, index));
	}
      else if(bpf_get_time(bpf, index) == bpf_get_time(bpf, index + 1))
	{
	  if(time == duration)
	    index--;
	  else if(incr >= 0.0 && index < size - 2)
	    index += 1;
	}	
      
      this->index = index;  
      this->slope = (bpf_get_value(bpf, index + 1) - bpf_get_value(bpf, index)) / (bpf_get_time(bpf, index + 1) - bpf_get_time(bpf, index));

      this->time = time;
      this->incr = incr;
    }
}

static void 
play_bpf_bang_at_end(fts_alarm_t *alarm, void *o)
{
  fts_outlet_bang((fts_object_t *)o, 1);
}

static void
play_bpf_set_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = bpf_atom_get(at);
  double time = 0.0;

  if(this->bpf)
    {
      time = this->time;
      fts_object_release((fts_object_t *)this->bpf);
    }

  this->bpf = bpf;

  fts_object_refer((fts_object_t *)bpf);

  /* force search for new index */
  play_bpf_locate(this, time, this->incr);
}

static void 
play_bpf_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  double time = fts_get_number_float(at);

  play_bpf_locate(this, time, 0.0);
}

static void 
play_bpf_target(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      double target = fts_get_number_float(at);
      double duration = bpf_get_duration(this->bpf);
  
      if(target < 0.0)
	target = 0.0;
      else if(target > duration)
	target = duration;

      this->mode = mode_play;

      if(ac > 2 && fts_is_number(at + 1))
	{
	  double period = fts_get_number_float(at + 1);
	  
	  if(period > 0.0)
	    {
	      this->target = target;
	      this->incr = (target - this->time) * this->sp / period;
	      	      
	      return;
	    }
	}

      play_bpf_locate(this, target, 0.0);
    }
}

static void 
play_bpf_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  double duration = bpf_get_duration(this->bpf);

  this->mode = mode_play;

  if(ac > 0 && fts_is_number(at))
    {
      double period = fts_get_number_float(at);

      if(period > 0.0)
	{	  
	  this->target = duration;
	  this->incr = (duration - this->time) * this->sp / period;

	  return;
	}
    }
  
  play_bpf_locate(this, duration, 0.0);
}

static void 
play_bpf_set_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = this->bpf;
  double speed = fts_get_number_float(at);
  double incr = this->incr;

  if(speed < 0.0)
    speed = 0.0;

  if(incr > 0.0)
    this->incr = speed * this->sp;
  else if(incr < 0.0)
    this->incr = -speed * this->sp;

  this->speed = speed;
}

static void 
play_bpf_forward(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = this->bpf;
  double duration = bpf_get_duration(bpf);
  double speed;
  
  if(ac && fts_is_number(at))
    {
      speed = fts_get_number_float(at);
      
      if(speed < 0.0)
	speed = 0.0;
      
      this->speed = speed;
    }
  else
    speed = this->speed;
  
  this->target = duration;
  this->incr = speed * this->sp;
      
  this->mode = mode_play;
}

static void 
play_bpf_backward(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = this->bpf;
  double speed;
   
  if(ac && fts_is_number(at))
    {
      speed = fts_get_number_float(at);
      
      if(speed < 0.0)
	speed = 0.0;
      
      this->speed = speed;
    }
  else
    speed = this->speed;
  
  this->target = 0.0;
  this->incr = -speed * this->sp;
      
  this->mode = mode_play;
}

static void 
play_bpf_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;

  play_bpf_locate(this, 0.0, 1.0);
  play_bpf_forward(o, 0, 0, 0, 0);
}

static void 
play_bpf_loop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = this->bpf;
  double duration = bpf_get_duration(bpf);
  double speed = this->speed;
  double time = this->time;
  double start = 0.0;
  double end = duration;
  enum play_mode mode = mode_repeat;
  double incr;
  
  switch(ac)
    {
    case 3:
      
      if(fts_is_number(at))	
	{
	  start = fts_get_number_float(at);

	  if(start < 0.0)
	    start = 0.0;
	  else if(start > duration)
	    start = duration;
	}
      else if(fts_is_symbol(at) && fts_get_symbol(at) == sym_end)
	start = duration;

      at++;

    case 2:
      
      if(fts_is_number(at))	
	{
	  end = fts_get_number_float(at);

	  if(end < 0.0)
	    end = 0.0;
	  else if(end > duration)
	    end = duration;
	}
      else if(fts_is_symbol(at) && fts_get_symbol(at) == sym_end)
	end = duration;

      at++;

    case 1:
      
      if(fts_is_number(at))	
	speed = fts_get_number_float(at);

    case 0:
      
      break;
      
    default:
      
      return;
    }

  if(speed == 0.0 || start == end)
    {
      /* stop */
      this->target = start;
      this->speed = 0.0;
      this->loop_time = 0.0;

      if(start != this->time)
	play_bpf_locate(this, start, 0.0);

      this->mode = mode_stop;

      return;
    }
  else if(speed < 0.0)
    {
      speed = -speed;
      mode = mode_loop;
    }

  if(start < end)
    incr = speed * this->sp;
  else
    incr = -speed * this->sp;

  /* jump to beginning when not running towards loop end */
  if((time > end || start > end || incr <= 0.0) && (time < end || start < end || incr >= 0.0))
    {
      if(start != this->time)
	play_bpf_locate(this, start, incr);
    }
  else
    this->incr = incr;

  this->loop_time = start;
  this->target = end;

  this->mode = mode;
}

static void 
play_bpf_goto(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = this->bpf;
  int index = this->index;
  double duration = bpf_get_duration(bpf);
  double time = this->time;
  double target = 0.0;
  double speed = 1.0;

  switch(ac)
    {
    case 2:

      if(fts_is_number(at + 1))
	speed = fts_get_number_float(at + 1);

    case 1:

      if(fts_is_number(at))	
	{
	  target = fts_get_number_float(at);

	  if(target < 0.0)
	    target = 0.0;
	  else if(target > duration)
	    target = duration;
	}
      else if(fts_is_symbol(at) && fts_get_symbol(at) == sym_end)
	target = duration;

      if(speed < 0.0)
	speed = 0.0;

      this->target = target;
      this->speed = speed;

      /* adjust increment to time and target */
      if(target > time)
	this->incr = speed * this->sp;
      else if(target < time)
	this->incr = -speed * this->sp;
      else
	this->incr = 0.0;
      
      this->mode = mode_play;

      break;

    default:
      /* unreported runtime error */
      break;
    }
}

static void
play_bpf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;

  this->incr = 0.0;
}

/************************************************************
 *
 *  dsp
 *
 */

static void
play_bpf_reset(play_bpf_t *this, int n_tick, double sr)
{
  double sp = 1000. / sr;

  this->sp = sp;

  play_bpf_locate(this, this->time, 0.0);
}

static void
play_bpf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  play_bpf_stop(o, 0, 0, 0, 0);
  play_bpf_reset(this, n_tick, sr);

  fts_set_ptr(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  dsp_add_funcall(sym_play, 3, a);
}

static void
play_bpf_ftl(fts_word_t *argv)
{
  play_bpf_t *this = (play_bpf_t *) fts_word_get_ptr(argv + 0);
  float *out = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  bpf_t *bpf = this->bpf;
  int size = bpf_get_size(bpf);
  int index = this->index;
  double curr_time = bpf_get_time(bpf, index);
  double curr_value = bpf_get_value(bpf, index);
  double slope = this->slope;
  double time = this->time;
  double incr = this->incr;

  if(size < 2)
    {
      time = 0.0;
      
      index = 0;
      slope = 0.0;
      
      curr_time = 0.0;
      curr_value = bpf_get_value(bpf, 0);
      
      incr = 0.0;
    }

  if(incr == 0.0)
    {
      float value = curr_value + slope * (time - curr_time);
      int i;
      
      for(i=0; i<n_tick; i++)
	out[i] = value;
    }
  else
    {
      double target = this->target;
      double next_time = bpf_get_time(bpf, index + 1);
      double end_time = time + incr * n_tick;
      
      if((incr > 0.0 && end_time <= target && end_time <= next_time) || (incr < 0.0 && end_time >= target && end_time >= curr_time))
	{
	  double t = time - curr_time;
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      out[i] = curr_value + slope * t;
	      t += incr;
	    }
	  
	  this->time = end_time;
	}
      else 
	{
	  int i;
      
	  for(i=0; i<n_tick; i++)
	    {
	      out[i] = curr_value + slope * (time - curr_time);
	  
	      if(incr != 0.0)
		{
		  /* increment */
		  time += incr;
	      
		  /* check if target reached */
		  if((incr > 0.0 && time >= target) || (incr < 0.0 && time <= target))
		    {
		      switch (this->mode)
			{
			case mode_loop:
			  {
			    /* looping back and forth */
			    double loop_time = this->loop_time;
			
			    this->loop_time = target;
			    target = loop_time;
			
			    /* turn */
			    incr = -incr;
			    time = 2.0 * target - time;
			  }
			  break;
		      
			case mode_repeat:
			  {
			    /* reapeating loop*/
			  
			    time += this->loop_time - target;
			  }
			  break;
			
			default:
			  {
			    /* reached end of playing */
			    incr = 0.0;
			
			    fts_alarm_set_delay(&this->alarm, 0.00001);
			    fts_alarm_arm(&this->alarm);
			
			    time = target;
			  }
			}
		    }

		  /* advance */
		  if(time > next_time)
		    {
		      double next_value;
		  
		      do
			{
			  index++;
			  next_time = bpf_get_time(bpf, index + 1);
			}
		      while(time > next_time);
		  
		      curr_time = bpf_get_time(bpf, index);
		      curr_value = bpf_get_value(bpf, index);
		      next_value = bpf_get_value(bpf, index + 1);
		  
		      slope = (next_value - curr_value) / (next_time - curr_time);
		    }
		  else if(time < curr_time)
		    {
		      double next_value;
		  
		      do 
			{
			  index--;
			  curr_time = bpf_get_time(bpf, index);
			}
		      while(time < curr_time);
		  
		      next_time = bpf_get_time(bpf, index + 1);
		      curr_value = bpf_get_value(bpf, index);
		      next_value = bpf_get_value(bpf, index + 1);
		  
		      slope = (next_value - curr_value) / (next_time - curr_time);
		    }
		}
	    }

	  this->index = index;
	  this->slope = slope;
	  this->time = time;

	  this->target = target;
	  this->incr = incr;
	}
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
play_bpf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_bpf_t *this = (play_bpf_t *)o;

  ac--;
  at++;

  dsp_list_insert(o);

  this->index = 0;
  this->slope = 0.0;

  this->time = 0.0;
  this->incr = 0.0;
  this->target = 0.0;

  this->loop_time = 0.0;
  this->mode = mode_stop;

  this->speed = 1.0;
  this->sp = 1000. / fts_get_sample_rate();

  /* init output alarm */
  fts_alarm_init(&this->alarm, 0, play_bpf_bang_at_end, this);    
  
  if(bpf_atom_is(at))
    play_bpf_set_bpf(o, 0, 0, 1, at);
  else
    fts_object_set_error(o, "First argument of bpf required");
}

static void
play_bpf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_bpf_t *this = (play_bpf_t *)o;

  dsp_list_remove(o);
}

static fts_status_t
play_bpf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;

  fts_class_init(cl, sizeof(play_bpf_t), 2, 2, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, play_bpf_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, play_bpf_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, play_bpf_put);

  fts_method_define_varargs(cl, 0, fts_s_bang, play_bpf_forward);
  fts_method_define_varargs(cl, 0, fts_new_symbol("play"), play_bpf_play);
  fts_method_define_varargs(cl, 0, fts_new_symbol("forward"), play_bpf_forward);
  fts_method_define_varargs(cl, 0, fts_new_symbol("backward"), play_bpf_backward);
  fts_method_define_varargs(cl, 0, fts_new_symbol("goto"), play_bpf_goto);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), play_bpf_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("end"), play_bpf_end);
  
  fts_method_define_varargs(cl, 0, fts_s_int, play_bpf_time);
  fts_method_define_varargs(cl, 0, fts_s_float, play_bpf_time);
  fts_method_define_varargs(cl, 0, fts_s_list, play_bpf_target);

  fts_method_define_varargs(cl, 1, fts_s_int, play_bpf_set_speed);
  fts_method_define_varargs(cl, 1, fts_s_float, play_bpf_set_speed);

  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
signal_play_bpf_config(void)
{
  sym_play = fts_new_symbol("play~");
  sym_end = fts_new_symbol("end");

  fts_metaclass_install(sym_play, play_bpf_instantiate, fts_arg_type_equiv);

  dsp_declare_function(sym_play, play_bpf_ftl);
}
