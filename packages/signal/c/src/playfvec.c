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
#include "fvec.h"

#define MAX_DOUBLE ((double)179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.0)

/************************************************************
 *
 *  play
 *
 */

typedef struct _play_fvec_
{
  fts_object_t o;

  fvec_t *fvec;

  double start; /* start pasition */
  double target; /* target pasition */
  double speed; /* play speed */
  double position; /* current position */

  double sr; /* sample rate (in kHz) */
  double sp; /* sample period (in msec) */

  enum play_mode {mode_stop, mode_pause, mode_play, mode_loop, mode_cycle} mode;
  int cycle_direction;
  
  fts_alarm_t alarm;
} play_fvec_t;

static fts_symbol_t sym_play = 0;

/************************************************************
 *
 *  methods
 *
 */

static void 
play_fvec_bang_at_end(fts_alarm_t *alarm, void *o)
{
  fts_outlet_bang((fts_object_t *)o, 1);
}

static void
play_fvec_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  fvec_t *fvec = fvec_atom_get(at);
  int size = fvec_get_size(fvec);

  if(this->fvec)
    fts_object_release((fts_object_t *)this->fvec);

  this->fvec = fvec;

  fts_object_refer((fts_object_t *)fvec);
}

static void 
play_fvec_set_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->speed = value;
}

static void 
play_fvec_set_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->start = value;
}

static void 
play_fvec_set_target(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->target = value;
}

static void 
play_fvec_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double value = fts_get_number_float(at);
  double start = this->start;
  double target = this->target;
  
  if(value < 0.0)
    value = 0.0;

  if(target > start)
    this->speed = (target - start) / value;
  else if(start > target)
    this->speed = (target - start) / value;
  else
    this->speed = 0.0;
}

static void 
play_fvec_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  switch (ac)
    {
    default:
    case 4:
      play_fvec_set_speed(o, 0, 0, 1, at + 3);
    case 3:
      play_fvec_set_target(o, 0, 0, 1, at + 2);
    case 2:
      play_fvec_set_start(o, 0, 0, 1, at + 1);
    case 1:
      play_fvec_set_fvec(o, 0, 0, 1, at);
      break;
    case 0:
      break;
    }
}

static void 
play_fvec_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  this->position = this->start;
  this->mode = mode_play;
}

static void 
play_fvec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  play_fvec_set(o, 0, 0, ac, at);
  play_fvec_bang(o, 0, 0, 0, 0);
}

static void 
play_fvec_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  switch(this->mode)
    {
    case mode_stop:
      this->position = this->start;
    case mode_pause:
      this->mode = mode_play;
    default:
      break;
    }
}

static void 
play_fvec_loop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  this->mode = mode_loop;
}

static void 
play_fvec_cycle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  this->mode = mode_cycle;
  this->cycle_direction = 0;
}

static void
play_fvec_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  this->mode = mode_pause;
}

static void
play_fvec_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  this->mode = mode_stop;
}

/************************************************************
 *
 *  dsp
 *
 */

static void
play_fvec_reset(play_fvec_t *this, int n_tick, double sr)
{
  double sp = 1000. / sr;

  this->sp = sp;
  this->sr = 0.001 * sr;
}

static void
play_fvec_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  play_fvec_reset(this, n_tick, sr);

  fts_set_ptr(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  dsp_add_funcall(sym_play, 3, a);
}

static void
play_fvec_ftl(fts_word_t *argv)
{
  play_fvec_t *this = (play_fvec_t *) fts_word_get_ptr(argv + 0);
  float *out = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  fvec_t *fvec = this->fvec;
  float *buf = fvec_get_ptr(fvec);
  double duration = (double)fvec_get_size(fvec) * this->sp;
  int invert = (this->mode == mode_cycle && this->cycle_direction);
  double start = (invert)? this->target: this->start;
  double target = (invert)? this->start: this->target;
  double position = this->position;
  double speed = (target > start)? this->speed: -this->speed;
  double incr = speed * this->sp;
  fts_fourpoint_index_t frac, frac_incr;
  double index_float;
  int index;

  /* clip start and target into size */
  if(start >= duration)
    start = duration;
  if(target > duration)
    target = duration;

  /* turn movement into range */
  if((target - position) * incr < 0.0)
    position = target;
  else if((position - start) * incr < 0.0)
    position = start;

  /* convert to integer representation */
  index_float = position * this->sr;
  index = (int)index_float;
  frac = fts_fourpoint_index_scale(index_float - index);
  frac_incr = fts_fourpoint_index_scale(incr * this->sr);

  if(this->mode <= mode_pause)
    {
      /* stop or pause */
      int i;

      for(i=0; i<n_tick; i++)
	out[i] = 0.0;
    }
  else
    {
      double end_position = position + n_tick * incr;

      if((target - end_position) * incr >= 0.0)
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      fts_fourpoint_interpolate(buf + index, frac, out + i);
	      frac += frac_incr;
	      index += fts_fourpoint_index_get_int(frac);
	      frac = fts_fourpoint_index_get_frac(frac);
	    }
	  
	  this->position = end_position;
	}
      else
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      fts_fourpoint_interpolate(buf + index, frac, out + i);
	      
	      if(incr != 0.0)
		{
		  /* increment */
		  frac += frac_incr;
		  index += fts_fourpoint_index_get_int(frac);
		  frac = fts_fourpoint_index_get_frac(frac);
		  position += incr;
		  
		  if((position - target) * incr >= 0.0)
		    {
		      /* target reached */
		      switch (this->mode)
			{
			case mode_loop:
			  {
			    /* repeat from loop start */
			    position += start - target;

			    if((position - target) * incr >= 0.0)
			      {
				position = target;
				incr = 0.0;
				frac_incr = 0;
			      }

			    index_float = position * this->sr;
			    index = (int)index_float;
			    frac = fts_fourpoint_index_scale(index_float - index);
			  }
			  break;
			  
			case mode_cycle:
			  {
			    /* turn and loop back */
			    double old_start = start;

			    /* mirror position at target */ 
			    position = 2.0 * target - position;

			    if((position - start) * incr <= 0.0)
			      {
				position = start;
				incr = 0.0;
				frac_incr = 0;
			      }
			    else
			      {
				/* turn */
				incr = -incr;
				frac_incr = -frac_incr;
				
				start = target;
				target = old_start;
			  
				/* swap direction */
				this->cycle_direction = 1 - this->cycle_direction;
			      }
			    
			    index_float = position * this->sr;
			    index = (int)index_float;
			    frac = fts_fourpoint_index_scale(index_float - index);
			  }
			  break;
			  
			default:
			  {
			    /* reached end of playing */
			    incr = 0.0;
			    frac_incr = 0;
			    
			    fts_alarm_set_delay(&this->alarm, 0.00001);
			    fts_alarm_arm(&this->alarm);
			    
			    position = target;
			    index_float = position * this->sr;
			    index = (int)index_float;
			    frac = fts_fourpoint_index_scale(index_float - index);

			    this->mode = mode_stop;
			  }
			}
		    }
		}
	    }

	  this->position = position;  
	}
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
play_fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_fvec_t *this = (play_fvec_t *)o;

  ac--;
  at++;

  dsp_list_insert(o);

  this->position = 0.0;

  this->mode = mode_stop;
  this->cycle_direction = 0;

  this->fvec = 0;
  this->start = 0.0;
  this->target = MAX_DOUBLE;
  this->speed = 1.0;

  this->sr = 0.001 * fts_get_sample_rate();
  this->sp = 1000. / fts_get_sample_rate();

  /* init output alarm */
  fts_alarm_init(&this->alarm, 0, play_fvec_bang_at_end, this);    

  if(ac > 0 && fvec_atom_is(at))
    play_fvec_set(o, 0, 0, ac, at);
  else
    fts_object_set_error(o, "First argument of fvec required");
}

static void
play_fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_fvec_t *this = (play_fvec_t *)o;

  dsp_list_remove(o);
}

static fts_status_t
play_fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;

  fts_class_init(cl, sizeof(play_fvec_t), 4, 2, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, play_fvec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, play_fvec_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, play_fvec_put);

  fts_method_define_varargs(cl, 0, fts_s_bang, play_fvec_bang);
  fts_method_define_varargs(cl, 0, fts_new_symbol("play"), play_fvec_play);
  fts_method_define_varargs(cl, 0, fts_new_symbol("loop"), play_fvec_loop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("cycle"), play_fvec_cycle);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), play_fvec_pause);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), play_fvec_stop);

  fts_method_define_varargs(cl, 0, fts_new_symbol("start"), play_fvec_set_start);
  fts_method_define_varargs(cl, 0, fts_new_symbol("target"), play_fvec_set_target);
  fts_method_define_varargs(cl, 0, fts_new_symbol("speed"), play_fvec_set_speed);

  fts_method_define_varargs(cl, 0, fts_s_set, play_fvec_set);
  fts_method_define_varargs(cl, 0, fts_s_list, play_fvec_list);

  fts_method_define_varargs(cl, 0, fvec_symbol, play_fvec_set_fvec);
  
  fts_method_define_varargs(cl, 1, fts_s_int, play_fvec_set_start);
  fts_method_define_varargs(cl, 1, fts_s_float, play_fvec_set_start);

  fts_method_define_varargs(cl, 2, fts_s_int, play_fvec_set_target);
  fts_method_define_varargs(cl, 2, fts_s_float, play_fvec_set_target);

  fts_method_define_varargs(cl, 3, fts_s_int, play_fvec_set_speed);
  fts_method_define_varargs(cl, 3, fts_s_float, play_fvec_set_speed);

  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
signal_play_fvec_config(void)
{
  sym_play = fts_new_symbol("play~");

  fts_metaclass_install(sym_play, play_fvec_instantiate, fts_arg_type_equiv);

  dsp_declare_function(sym_play, play_fvec_ftl);
}
