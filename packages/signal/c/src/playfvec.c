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

#include <float.h>
#include <fts/fts.h>
#include <utils/c/include/utils.h>
#include <data/c/include/fvec.h>

static fts_symbol_t play_fvec_symbol = 0;

typedef struct _play_fvec_
{
  fts_object_t o;

  fvec_t *fvec;

  double position; /* current position */
  double begin; /* begin position */
  double end; /* end position */
  double step; /* playing step */
  double conv_position; /* position conversion factor */
  double conv_step; /* speed to step conversion factor */

  enum play_mode {mode_stop, mode_pause, mode_play, mode_loop, mode_cycle} mode;
  int cycle_direction;

} play_fvec_t;


void
play_fvec_set_conv_position(play_fvec_t *this, double c)
{
  if(this->begin < DBL_MAX)
    this->begin *= c / this->conv_position;

  if(this->end < DBL_MAX)
    this->end *= c / this->conv_position;

  if(this->position < DBL_MAX)
    this->position *= c / this->conv_position;

  this->conv_position = c;
}

void
play_fvec_set_conv_step(play_fvec_t *this, double c)
{
  this->step *= c / this->conv_step;
  this->conv_step = c;
}

/************************************************************
 *
 *  methods
 *
 */

void 
play_fvec_bang_at_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang((fts_object_t *)o, 1);
}

static void
play_fvec_set_object(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  fvec_t *fvec = fvec_atom_get(at);

  if(this->fvec)
    fts_object_release(this->fvec);

  this->fvec = fvec;

  fts_object_refer(fvec);
}

static void 
play_fvec_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->begin = value * this->conv_position;
}

static void 
play_fvec_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->end = value * this->conv_position;
}

static void 
play_fvec_set_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->step = value * this->conv_step;
}

static void 
play_fvec_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double value = fts_get_number_float(at);
  double begin = this->begin;
  double end = this->end;
  
  if(value > 0.0)
    {
      if(end > begin)
	this->step = this->conv_step * (end - begin) / (value * this->conv_position);
      else if(begin > end)
	this->step = this->conv_step * (end - begin) / (value * this->conv_position);
    }
  else
    this->position = end;
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
      play_fvec_set_end(o, 0, 0, 1, at + 2);
    case 2:
      play_fvec_set_begin(o, 0, 0, 1, at + 1);
    case 1:
      play_fvec_set_object(o, 0, 0, 1, at);
      break;
    case 0:
      break;
    }
}

static void 
play_fvec_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  this->position = this->begin;
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

  if(this->mode == mode_stop)
    this->position = this->begin;

  this->mode = mode_play;
}

static void 
play_fvec_loop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  if(this->mode == mode_stop)
    this->position = this->begin;

  this->mode = mode_loop;
}

static void 
play_fvec_cycle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  if(this->mode == mode_stop)
    this->position = this->begin;

  this->mode = mode_cycle;
  this->cycle_direction = 0;
}

static void
play_fvec_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  if(this->mode != mode_stop)
    this->mode = mode_pause;
}

static void
play_fvec_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  this->mode = mode_stop;
}

static void
play_fvec_rewind(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;

  this->position = 0.0;
}

/************************************************************
 *
 *  dsp
 *
 */

static void
play_fvec_reset(play_fvec_t *this, int n_tick, double sr)
{
  play_fvec_set_conv_step(this, 1000.0 * this->conv_position / sr);
}

static void
play_fvec_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  play_fvec_reset(this, n_tick, sr);

  fts_set_pointer(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(play_fvec_symbol, 3, a);
}

static void
play_fvec_ftl(fts_word_t *argv)
{
  play_fvec_t *this = (play_fvec_t *) fts_word_get_pointer(argv + 0);
  float *out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  fvec_t *fvec = this->fvec;
  float *buf = fvec_get_ptr(fvec);
  double position = this->position;
  fts_idefix_t index;
      
  /* convert to idefix representation */
  fts_idefix_set_float(&index, position);
  
  if(this->mode <= mode_pause)
    {
      /* stop or pause */
      float f;
      int i;

      fts_cubic_idefix_interpolate(buf, index, &f);

      for(i=0; i<n_tick; i++)
	out[i] = f;
    }
  else
    {
      double size = fvec_get_size(fvec);
      int invert = (this->mode == mode_cycle && this->cycle_direction);
      double begin = (invert)? this->end: this->begin;
      double end = (invert)? this->begin: this->end;
      double step = (end > begin)? this->step: -this->step;
      double tick_end_position = position + n_tick * step;
      fts_idefix_t incr;

      fts_idefix_set_float(&incr, step);

      /* clip begin and end into size */
      if(begin >= size)
	begin = size;
      if(end > size)
	end = size;

      if((end - tick_end_position) * step >= 0.0)
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      fts_cubic_idefix_interpolate(buf, index, out + i);
	      fts_idefix_incr(&index, incr);
	    }
	  
	  this->position = tick_end_position;
	}
      else
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      fts_cubic_idefix_interpolate(buf, index, out + i);
	      
	      if(step != 0.0)
		{
		  /* increment */
		  fts_idefix_incr(&index, incr);
		  position += step;
		  
		  if((position - end) * step >= 0.0)
		    {
		      /* end reached */
		      switch (this->mode)
			{
			case mode_loop:
			  {
			    /* repeat from loop begin */
			    position += begin - end;

			    if((position - end) * step > 0.0)
			      {
				position = end;
				step = 0.0;
			      }
			    
			    fts_idefix_set_float(&index, position);
			  }
			  break;
			  
			case mode_cycle:
			  {
			    /* turn and loop back */
			    double old_begin = begin;

			    /* mirror position at end */ 
			    position = 2.0 * end - position;

			    if((position - begin) * step < 0.0)
			      {
				position = begin;
				step = 0.0;
			      }
			    else
			      {
				/* turn */
				step = -step;
				fts_idefix_negate(&incr);
				
				begin = end;
				end = old_begin;
			  
				/* swap direction */
				this->cycle_direction = 1 - this->cycle_direction;
			      }
			    
			    fts_idefix_set_float(&index, position);
			  }
			  break;
			  
			default:
			  {
			    /* reached end of playing */
			    position = end;
			    step = 0.0;
			    
			    fts_idefix_set_float(&index, position);
			    this->mode = mode_stop;

			    fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, play_fvec_bang_at_end, 0, 0.0);
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

  this->fvec = 0;

  this->position = 0.0;
  this->begin = 0.0;
  this->end = DBL_MAX;
  this->step = 1.0;

  this->conv_position = 1.0;
  this->conv_step = 1.0;

  this->mode = mode_stop;
  this->cycle_direction = 0;

  if(ac > 0 && fts_is_object(at))
    play_fvec_set((fts_object_t *)this, 0, 0, ac, at);
  else
    fts_object_set_error((fts_object_t *)this, "Something to play required as first argument");

  play_fvec_set_conv_position(this, 0.001 * fts_dsp_get_sample_rate());

  fts_dsp_add_object((fts_object_t *)this);
}

static void
play_fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fts_dsp_remove_object(o);
}

static void
play_fvec_set_period_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double period = fts_get_number_float(value);

  play_fvec_set_conv_position(this, 1. / period);
  play_fvec_set_conv_step(this, 1000.0 / (fts_dsp_get_sample_rate() * period));
}

static fts_status_t
play_fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(play_fvec_t), 4, 2, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, play_fvec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, play_fvec_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, play_fvec_put);

  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("period"), play_fvec_set_period_prop);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, play_fvec_bang);
  fts_method_define_varargs(cl, 0, fts_new_symbol("play"), play_fvec_play);
  fts_method_define_varargs(cl, 0, fts_new_symbol("loop"), play_fvec_loop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("cycle"), play_fvec_cycle);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), play_fvec_pause);
  fts_method_define_varargs(cl, 0, fts_s_stop, play_fvec_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("rewind"), play_fvec_rewind);

  fts_method_define_varargs(cl, 0, fvec_symbol, play_fvec_set_object);
  fts_method_define_varargs(cl, 0, fts_new_symbol("begin"), play_fvec_set_begin);
  fts_method_define_varargs(cl, 0, fts_new_symbol("end"), play_fvec_set_end);
  fts_method_define_varargs(cl, 0, fts_new_symbol("speed"), play_fvec_set_speed);

  fts_method_define_varargs(cl, 0, fts_s_set, play_fvec_set);
  fts_method_define_varargs(cl, 0, fts_s_list, play_fvec_list);

  fts_method_define_varargs(cl, 1, fts_s_int, play_fvec_set_begin);
  fts_method_define_varargs(cl, 1, fts_s_float, play_fvec_set_begin);

  fts_method_define_varargs(cl, 2, fts_s_int, play_fvec_set_end);
  fts_method_define_varargs(cl, 2, fts_s_float, play_fvec_set_end);

  fts_method_define_varargs(cl, 3, fts_s_int, play_fvec_set_speed);
  fts_method_define_varargs(cl, 3, fts_s_float, play_fvec_set_speed);

  fts_dsp_declare_function(play_fvec_symbol, play_fvec_ftl);
  fts_dsp_declare_outlet(cl, 0);

  return fts_Success;
}

void
signal_play_fvec_config(void)
{
  play_fvec_symbol = fts_new_symbol("play~");

  fts_class_install(play_fvec_symbol, play_fvec_instantiate);
}
