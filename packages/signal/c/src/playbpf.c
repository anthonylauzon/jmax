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

#define MAX_DOUBLE ((double)179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.0)

/************************************************************
 *
 *  play
 *
 */

typedef struct _play_bpf_
{
  fts_object_t o;

  bpf_t *bpf;
  int index; /* current break point index */

  double position; /* current segment position */
  double incr; /* position increment (speed / sample rate) */
  double target; /* target pasition */

  double speed; /* play speed */
  double sp; /* sample rate */

  enum play_mode {mode_play, mode_loop, mode_repeat} mode;
  double loop_position;
  
  fts_alarm_t alarm;
} play_bpf_t;

static fts_symbol_t sym_play = 0;
static fts_symbol_t sym_end = 0;

/************************************************************
 *
 *  methods
 *
 */

static int
play_bpf_adjust_start(bpf_t *bpf, int index, double position, double direction)
{
  if(direction > 0.0 && bpf_get_time(bpf, index + 1) == bpf_get_time(bpf, index + 2))
    return index + 2;
  else if(direction < 0.0 && bpf_get_time(bpf, index - 1) == bpf_get_time(bpf, index))
    return index - 2;
  else
    return index;
}

static int
play_bpf_advance(bpf_t *bpf, int index, double position)
{
  /* position must be > 0.0 */
  if(position > bpf_get_time(bpf, index + 1))
    {
      index++;
      
      while(position > bpf_get_time(bpf, index + 1))
	index++;
    }
  else if(position < bpf_get_time(bpf, index))
    {
      index--;
      
      while(position < bpf_get_time(bpf, index))
	index--;
    }
  else if(bpf_get_position(bpf, index) == bpf_get_time(bpf, index + 1))
    index++;

  return index;
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

  if(this->bpf)
    fts_object_release((fts_object_t *)this->bpf);

  this->bpf = bpf;

  fts_object_refer((fts_object_t *)bpf);
}

static void 
play_bpf_position(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;

  this->position = fts_get_number_float(at);
  this->incr = 0.0;
}

static void 
play_bpf_target(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      double target = fts_get_number_float(at);
      double duration = bpf_get_duration(this->bpf);
  
      this->mode = mode_play;

      if(target < 0.0)
	target = 0.0;

      if(ac > 2 && fts_is_number(at + 1))
	{
	  double period = fts_get_number_float(at + 1);
	  
	  if(period > 0.0)
	    {
	      double incr = this->sp * (target - this->position) / period;

	      this->incr = incr;
	      this->target = target;	      

	      this->index = play_bpf_adjust_start(this->bpf, this->index, this->position, incr);

	      return;
	    }
	}
      
      this->position = target;
      this->incr = 0.0; 
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
	  double incr = this->sp * (duration - this->position) / period;

	  this->incr = incr;
	  this->target = MAX_DOUBLE;

	  this->index = play_bpf_adjust_start(this->bpf, this->index, this->position, incr);

	  return;
	}
    }

  this->position = duration;
  this->incr = 0.0;
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
  double speed, incr;
  
  if(ac && fts_is_number(at))
    {
      speed = fts_get_number_float(at);
      
      if(speed < 0.0)
	speed = 0.0;
    }
  else
    speed = 1.0;
  
  this->target = duration;
  this->speed = speed;
  this->incr = speed * this->sp;

  this->index = play_bpf_adjust_start(this->bpf, this->index, this->position, 1.0);
    
  this->mode = mode_play;
}

static void 
play_bpf_backward(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = this->bpf;
  double speed, incr;
   
  if(ac && fts_is_number(at))
    {
      speed = fts_get_number_float(at);
      
      if(speed < 0.0)
	speed = 0.0;
    }
  else
    speed = 1.0;
  
  this->target = 0.0;
  this->speed = speed;
  this->incr = -speed * this->sp;

  this->index = play_bpf_adjust_start(this->bpf, this->index, this->position, -1.0);
    
  this->mode = mode_play;
}

static void 
play_bpf_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;

  this->position = 0.0;
  play_bpf_forward(o, 0, 0, 0, 0);
}

static void 
play_bpf_loop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = this->bpf;
  double duration = bpf_get_duration(bpf);
  double speed = 1.0;
  double position = this->position;
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
      this->loop_position = start;

      this->position = start;

      this->target = start;
      this->incr = 0.0;

      this->mode = mode_play;

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

  this->loop_position = start;

  /* jump to beginning when not running towards loop end */
  if((position > end || start > end || incr <= 0.0) && (position < end || start < end || incr >= 0.0))
    this->position = start;

  this->index = play_bpf_adjust_start(this->bpf, this->index, start, incr);

  this->target = end;
  this->incr = incr;

  this->mode = mode;
}

static void 
play_bpf_goto(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  bpf_t *bpf = this->bpf;
  int index = this->index;
  double duration = bpf_get_duration(bpf);
  double position = this->position;
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
      this->mode = mode_play;

      /* adjust increment to position and target */
      if(target > position)
	{
	  this->incr = speed * this->sp;
	  this->index = play_bpf_adjust_start(this->bpf, this->index, position, 1.0);
	}
      else if(target < position)
	{
	  this->incr = -speed * this->sp;
	  this->index = play_bpf_adjust_start(this->bpf, this->index, position, -1.0);
	}
      else
	this->incr = 0.0;

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
  this->incr = 0.0;
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
  double duration = bpf_get_duration(bpf);
  double position = this->position;
  double incr = this->incr;
  int index;
  double curr_position, curr_value;
  double slope;
  
  if(size < 2)
    {
      incr = 0.0;

      position = 0.0;
      index = 0;

      curr_position = bpf_get_time(bpf, index);
      curr_value = bpf_get_value(bpf, index);

      slope = 0.0;
    }
  else
    {
      if(position <= 0.0)
	{
	  position = 0.0;
	  index = 0;
	}
      else if(position >= duration)
	{
	  position = duration;
	  index = size - 2;
	}
      else
	index = play_bpf_advance(bpf, this->index, position);
      
      curr_position = bpf_get_time(bpf, index);
      curr_value = bpf_get_value(bpf, index);

      slope = (bpf_get_value(bpf, index + 1) - curr_value) / (bpf_get_time(bpf, index + 1) - curr_position);
    }
  
  if(incr == 0.0)
    {
      float value = curr_value + slope * (position - curr_position);
      int i;
      
      for(i=0; i<n_tick; i++)
	out[i] = value;
    }
  else
    {
      double next_position = bpf_get_time(bpf, index + 1);
      double end_position = position + incr * n_tick;
      double target;

      if(this->target < duration)
	target = this->target;
      else
	target = duration;
	
      if(end_position >= curr_position && end_position <= next_position && (target - end_position) * incr >= 0.0)
	{
	  double dposition = position - curr_position;
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      out[i] = curr_value + slope * dposition;
	      dposition += incr;
	    }
	  
	  position = end_position;
	}
      else 
	{
	  int new_index;
	  int i;
      
	  for(i=0; i<n_tick; i++)
	    {
	      out[i] = curr_value + slope * (position - curr_position);
	  
	      if(incr != 0.0)
		{
		  /* increment */
		  position += incr;
	      
		  /* check if target reached */
		  if((position - target) * incr >= 0.0)
		    {
		      switch (this->mode)
			{
			case mode_loop:
			  {
			    /* turn and loop back */
			    double loop_position = this->loop_position;
			
			    /* turn */
			    incr = -incr;
			    position = 2.0 * target - position;

			    this->loop_position = target;
			    target = loop_position;			
			  }
			  break;
		      
			case mode_repeat:
			  /* repeat from loop start */
			  position += this->loop_position - target;
			  break;
			
			default:
			  {
			    /* reached end of playing */
			    incr = 0.0;
			
			    fts_alarm_set_delay(&this->alarm, 0.00001);
			    fts_alarm_arm(&this->alarm);
			
			    position = target;
			  }
			}
		    }

		  new_index = play_bpf_advance(bpf, index, position);

		  if(new_index != index)
		    {
		      curr_position = bpf_get_time(bpf, new_index);
		      curr_value = bpf_get_value(bpf, new_index);
		      
		      slope = (bpf_get_value(bpf, new_index + 1) - curr_value) / (bpf_get_time(bpf, new_index + 1) - curr_position);

		      index = new_index;
		    }
		}
	    }

	  this->target = target;
	}
    }

  this->incr = incr;
  this->position = position;
  this->index = index;
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

  this->position = 0.0;

  this->target = 0.0;
  this->incr = 0.0;

  this->loop_position = 0.0;
  this->mode = mode_play;

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
  fts_method_define_varargs(cl, 0, fts_new_symbol("loop"), play_bpf_loop);
  
  fts_method_define_varargs(cl, 0, fts_s_int, play_bpf_position);
  fts_method_define_varargs(cl, 0, fts_s_float, play_bpf_position);
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
