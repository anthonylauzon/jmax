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
#include <float.h>
#include <data/c/include/bpf.h>

static fts_symbol_t env_symbol = 0;
static fts_symbol_t sym_sustain = 0;
static fts_symbol_t sym_continue = 0;

typedef struct _env_
{
  fts_object_t o;

  enum env_mode {mode_continue, mode_sustain} mode;
  enum env_status {status_hold, status_running} status;

  bpf_t *current; /* reference to current bpf */
  int editid;

  bpf_t *local; /* internal bpf */
  int index; /* target index */

  double base_time;
  double base_value;
  double base_slope;

  double time; /* current time */
  double value; /* current value */

  double duration; /* fixed duration in mode continue (adjust speed for duration > 0.0) */
  double speed; /* reading speed */
  double conv; /* 1000. / sr */
} env_t;


static void
env_set_current(env_t *this, bpf_t *bpf)
{
  if(this->current != bpf)
    {
      if(this->current != NULL)
	fts_object_release(this->current);

      /* set new reference */
      this->current = bpf;
      fts_object_refer(bpf);
    }
}

/************************************************************
 *
 *  methods
 *
 */

static void 
env_output_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang(o, 1);
}

static void 
env_start(env_t *this)
{
  bpf_t *bpf = this->current;
  int size;

  this->index = 0;

  /* set time to zero */
  this->base_time = 0.0;
  this->time = 0.0;

  size = bpf_get_size(bpf);

  if(size)
    {
      double first_time = bpf_get_time(bpf, 0);

      if(this->mode == mode_continue && this->duration > 0.0)
	this->speed = bpf_get_duration(bpf) / this->duration;

      if(first_time == 0.0)
	{
	  int index = 1;

	  /* jump over points at time zero */
	  while(index < size && bpf_get_time(bpf, index) == 0.0)
	    index++;

	  this->index = index;
	  this->value = bpf_get_value(bpf, index - 1);
	  
	  this->base_value = bpf_get_value(bpf, index - 1);
	  this->base_slope = bpf_get_slope(bpf, index - 1);

	  if(this->mode == mode_sustain && this->base_slope == 0.0)
	    this->status = status_hold;
	}
      else
	{
	  this->base_value = this->value;
	  this->base_slope = (bpf_get_value(bpf, 0) - this->value) / first_time;
	}
    }

  if(this->index >= size)
    this->status = status_hold;
  else
    this->status = status_running;    
}

static void 
env_set_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;
  bpf_t *bpf = bpf_atom_get(at);

  env_set_current(this, bpf);
}

static void 
env_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;

  env_set_bpf(o, 0, 0, 1, at);
  env_start(this);
}

static void 
env_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;
  double value = fts_get_float(at);

  /* stop envelope */
  this->status = status_hold;

  /* reset bpf */
  env_set_current(this, this->local);
  bpf_clear(this->local);
      
  /* set current value */
  this->value = value;
}

static void 
env_set_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;
  bpf_t *bpf = this->local;
  double last_time = 0.0;
  int i;
  
  /* reset bpf */
  env_set_current(this, bpf);
  bpf_clear(bpf);
  
  if(ac & 1)
    {
      double value = 0.0;
      
      if(fts_is_number(at))
	value = fts_get_number_float(at);
      
      bpf_append_point(bpf, 0.0, value);
      
      ac--;
      at++;
    }
  
  for(i=0; i<ac; i+=2)
    {
      double time = 0.0;
      double value = 0.0;
      
      if(fts_is_number(at + i))
	time = fts_get_number_float(at + i);
      
      if(fts_is_number(at + i + 1))
	value = fts_get_number_float(at + i + 1);
      
      bpf_append_point(bpf, last_time + time, value);
      
      last_time = time;
    }
}

static void 
env_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;

  if(ac)
    {
      env_set_array(o, 0, 0, ac, at);
      env_start(this);
    }
}

static void 
env_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac == 1 && bpf_atom_is(at))
    env_set_bpf(o, 0, 0, 1, at);
  else
    env_set_array(o, 0, 0, ac, at);
}

static void 
env_adsr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;
  bpf_t *bpf = this->local;
  double attack_time = 0.0;
  double decay_time = 0.0;
  double sustain_level = 0.0;
  double release_time = 0.0;

  /* force sustain mode */
  this->mode = mode_sustain;

  /* reset bpf */
  env_set_current(this, bpf);
  bpf_clear(bpf);
      
  switch(ac)
    {
    default:
    case 4:
      if(fts_is_number(at + 3))
	release_time = fts_get_number_float(at + 3);

      if(release_time < 0.0)
	release_time = 0.0;

    case 3:
      if(fts_is_number(at + 2))
	sustain_level = fts_get_number_float(at + 2);

      if(sustain_level < 0.0)
	sustain_level = 0.0;

    case 2:
      if(fts_is_number(at + 1))
	decay_time = fts_get_number_float(at + 1);

      if(decay_time < 0.0)
	decay_time = 0.0;

    case 1:
      if(fts_is_number(at + 0))
	attack_time = fts_get_number_float(at + 0);

      if(attack_time < 0.0)
	attack_time = 0.0;

    case 0:
      break;
    }

  /* construct bpf (sustain time = attack + decay + release) */
  bpf_append_point(bpf, attack_time, 1.0);
  bpf_append_point(bpf, attack_time + decay_time, sustain_level);
  bpf_append_point(bpf, 2 * (attack_time + decay_time) + release_time, sustain_level);
  bpf_append_point(bpf, 2 * (attack_time + decay_time + release_time), 0.0);

  /* go */
  env_start(this);
}

static void 
env_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;

  /* just go */
  env_start(this);
}

static void 
env_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;
  
  this->status = status_hold;
}

static void 
env_release(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;
  bpf_t *bpf = this->current;
  int size = bpf_get_size(bpf);
  int index = this->index;
  
  if(index < 1)
    index = 1;

  if(index < size)
    this->status = status_running;
      
  /* advance after next sustain or to last segment */
  if(index < size - 1)
    {
      double time = bpf_get_time(bpf, size - 2);
      double value = this->value;
	  
      if(this->mode == mode_sustain)
	{
	  /* search for release slope after sustain */
	  while(index < size - 1)
	    {
	      if(bpf_get_slope(bpf, index - 1) == 0.0)
		{
		  /* get time after sustain */
		  time = bpf_get_time(bpf, index);
		  index++;
		  break;
		}
	      
	      index++;
	    }
	  
	  /* skip jump values after sustain */
	  while(index < size - 1 && time == bpf_get_time(bpf, index))
	    index++;
	}
      else
	index = size - 1;

      /* set jump value */
      if(time == bpf_get_time(bpf, index - 2))
	value = bpf_get_value(bpf, index - 1);
	  
      /* set base values */
      this->base_time = time;
      this->base_value = value;
      this->base_slope = (bpf_get_value(bpf, index) - value) / (bpf_get_time(bpf, index) - time);
      
      this->index = index;
      this->time = time;
      this->value = value;
    }
}

static void
env_set_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;

  if(fts_is_number(at))
    {
      double speed = fts_get_number_float(at);
	
      if(speed > 0.0)
	{
	  this->speed = speed;

	  /* unable duration adjustment */
	  this->duration = 0.0;
	}
      else
	fts_object_signal_runtime_error(o, "speed must be > 0.0");
    }
  else
    fts_object_signal_runtime_error(o, "cannot set speed from non number value");
}

static void
env_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;

  if(fts_is_number(at))
    this->duration = fts_get_number_float(at);
  else
    fts_object_signal_runtime_error(o, "cannot set duration from non number value");
}

/************************************************************
 *
 *  dsp
 *
 */

static void
env_reset(env_t *this, int n_tick, double sr)
{
  this->conv = 1000.0 / sr;
}

static void
env_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  env_reset(this, n_tick, sr);

  fts_set_pointer(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  fts_dsp_add_function(env_symbol, 3, a);
}

static void
env_ftl(fts_word_t *argv)
{
  env_t *this = (env_t *)fts_word_get_pointer(argv + 0);
  float *out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  bpf_t *bpf = this->current;
  double time = this->time;
  double value = this->value;
  double step = this->speed * this->conv;
  double incr = step * this->base_slope;

  if(this->status == status_hold)
    {
      int i;
      
      for(i=0; i<n_tick; i++)
	out[i] = value;
    }
  else
    {
      double end_of_tick_time = time + step * n_tick;
      double target_time = bpf_get_time(bpf, this->index);

      if(end_of_tick_time < target_time)
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      out[i] = value;
	      value += incr;
	    }

	  this->value = this->base_value + (end_of_tick_time - this->base_time) * this->base_slope;
	  this->time = end_of_tick_time;
	}
      else 
	{
	  enum env_status status = this->status;
	  int size = bpf_get_size(bpf);
	  int index = this->index;
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      out[i] = value;
	      
	      if(status != status_hold)
		{
		  /* step forward */
		  time += step;

		  if(time >= target_time)
		    {
		      /* advance in bpf */
		      index++;

		      while(index < size && time >= bpf_get_time(bpf, index))
			index++;

		      if(this->mode == mode_sustain && bpf_get_slope(bpf, index - 1) == 0.0)
			status = status_hold;

		      /* hold at end */
		      if(index == size)
			{
			  status = status_hold;

			  /* output bang */
			  fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, env_output_bang, 0, 0.0);
			}
		      else
			{
			  /* advance current point */
			  this->base_time = bpf_get_time(bpf, index - 1);
			  this->base_value = bpf_get_value(bpf, index - 1);
			  this->base_slope = bpf_get_slope(bpf, index - 1);
			}
		      
		      /* hold at sustain or end */
		      if(status == status_hold)
			{
			  time = bpf_get_time(bpf, index - 1);
			  value = bpf_get_value(bpf, index - 1);
			}
		      else
			{
			  /* new target time */
			  target_time = bpf_get_time(bpf, index);

			  /* new value and increment */
			  value = this->base_value + (time - this->base_time) * this->base_slope;
			  incr = step * this->base_slope;
			}
		    }
		  else
		    value += incr;
		}
	    }

	  this->status = status;
	  this->index = index;
	  this->time = time;
	  this->value = value;
	}
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
env_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;
  fts_symbol_t mode = fts_get_symbol(at);

  if(mode == sym_sustain)
    this->mode = mode_sustain;
  else if(mode == sym_continue)
    this->mode = mode_continue;
}

static void
env_set_mode_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  env_set_mode(o, 0, 0, 1, value);
}

static void
env_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  env_t *this = (env_t *)o;

  this->status = status_hold;
  this->mode = mode_continue;

  this->local = (bpf_t *)fts_object_create(bpf_type, 0, 0);
  fts_object_refer((fts_object_t *)this->local);

  this->current = 0;
  this->index = 0;
  this->base_time = 0.0;
  this->base_value = 0.0;
  this->base_slope = 0.0;

  this->time = 0.0; /* current time */
  this->value = 0.0; /* current value */

  this->duration = 0.0; /* fixed duration in mode continue (adjust speed for duration > 0.0) */
  this->speed = 1.0; /* reading speed */
  this->conv = 1.0; /* 1000.0 / sr */

  this->editid = 0;

  if(bpf_atom_is(at))
    env_set_bpf(o, 0, 0, 1, at);
  else
    env_set_current(this, this->local);

  fts_dsp_add_object(o);
}

static void
env_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  env_t *this = (env_t *)o;

  if(this->current)
    fts_object_release(this->current);

  fts_object_destroy((fts_object_t *)this->local);
  fts_dsp_remove_object(o);
}

static fts_status_t
env_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(env_t), 2, 2, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, env_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, env_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, env_put);

  fts_method_define_varargs(cl, 0, fts_new_symbol("mode"), env_set_mode);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), env_set_mode_prop);

  fts_method_define_varargs(cl, 0, fts_s_bang, env_go);
  fts_method_define_varargs(cl, 1, fts_s_bang, env_release);

  fts_method_define_varargs(cl, 0, fts_s_stop, env_stop);

  fts_method_define_varargs(cl, 0, bpf_symbol, env_bpf);
  fts_method_define_varargs(cl, 0, fts_s_int, env_number);
  fts_method_define_varargs(cl, 0, fts_s_float, env_number);
  fts_method_define_varargs(cl, 0, fts_s_list, env_array);

  fts_method_define_varargs(cl, 0, fts_new_symbol("adsr"), env_adsr);
  fts_method_define_varargs(cl, 0, fts_new_symbol("speed"), env_set_speed);
  fts_method_define_varargs(cl, 0, fts_new_symbol("duration"), env_set_duration);

  fts_dsp_declare_outlet(cl, 0);

  return fts_ok;
}

void
signal_env_config(void)
{
  env_symbol = fts_new_symbol("env~");
  sym_continue = fts_new_symbol("continue");
  sym_sustain = fts_new_symbol("sustain");

  fts_class_install(env_symbol, env_instantiate);
  fts_dsp_declare_function(env_symbol, env_ftl);
}
