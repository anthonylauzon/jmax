/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.b
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
 * Authors: Norbert Schnell
 *
 */

#include <fts/fts.h>
#include <utils.h>
#include "delay.h"

static fts_symbol_t sym_dline = 0;
static fts_symbol_t sym_drain = 0;
static fts_symbol_t sym_tap = 0;
static fts_symbol_t sym_tap_signal = 0;

fts_metaclass_t *dline_metaclass;
fts_metaclass_t *drain_metaclass;

/************************************************************
 *
 *  delay line
 *
 */

#define DELAYLINE_DEFAULT_LENGTH 1000
#define DELAYLINE_ALLOC_TAIL 2
#define DELAYLINE_ALLOC_HEAD 1

static void 
delayline_clear(delayline_t *this)
{
  int i;
      
  for(i=-DELAYLINE_ALLOC_HEAD; i<this->size+DELAYLINE_ALLOC_TAIL; i++)
    this->samples[i] = 0.0;

  this->phase = 0;
}

static void
delayline_reset(delayline_t *this, double sr, int n_tick)
{
  int size = 0.001 * this->length * sr + n_tick;
  
  /* check if new size matches old size */
  if(size > this->alloc) /* need to reallocate */
    {
      /* allocate delayline */
      this->samples = (float *)fts_realloc(this->samples, (size + DELAYLINE_ALLOC_HEAD + DELAYLINE_ALLOC_TAIL) * sizeof(float));
      this->samples += DELAYLINE_ALLOC_HEAD;

      this->alloc = size;
    }

  this->size = size;
  this->n_tick = n_tick;
  this->sr = sr;

  delayline_clear(this);    
}

static void
delayline_destroy(delayline_t *this)
{
  if(this->samples != NULL)
    fts_free(this->samples - DELAYLINE_ALLOC_HEAD);
}

static void
dline_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delayline_t *this = (delayline_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[3];
  
  delayline_reset(this, sr, n_tick);  
  
  fts_set_object(a + 0, o);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(sym_dline, 3, a);
}

static void 
dline_ftl(fts_word_t *argv)
{
  delayline_t * restrict this = (delayline_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float * restrict samples = this->samples;
  int size = this->size;
  int phase = this->phase;
  int tail = size - phase + DELAYLINE_ALLOC_TAIL;
  int i;

  if(tail > n_tick)
    {
      for(i=0; i<n_tick; i++)
	samples[phase + i] = in[i];

      this->phase += n_tick;
    }
  else
    {
      int k;

      for(i=0; i<tail; i++)
	samples[phase + i] = in[i];

      samples[-1] = samples[size - 1];
      samples[0] = samples[size];
      samples[1] = samples[size + 1];
	
      /* copy last points to beginning of buffer for interpolation */
      /*for(k=-DELAYLINE_ALLOC_HEAD; k<DELAYLINE_ALLOC_TAIL, i<n_tick; k++, i++)
	samples[k] = samples[phase + i];*/

      for(k=2; i<n_tick; k++, i++)
	samples[k] = in[i];

      this->phase = k;
    }
}

static void
drain_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delayline_t *this = (delayline_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t a[3];
  
  delayline_reset(this, sr, n_tick);  
  
  fts_set_object(a + 0, o);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(sym_drain, 3, a);
}

static void 
drain_ftl(fts_word_t *argv)
{
  delayline_t * restrict this = (delayline_t *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float * restrict samples = this->samples;
  int phase = this->phase;
  int tail = this->size - phase;
  int i;

  if(tail > n_tick)
    {
      for(i=0; i<n_tick; i++)
	{
	  out[i] = samples[phase + i];
	  samples[phase + i] = 0.0;
	}

      this->phase += n_tick;
    }
  else
    {
      int k;

      for(i=0; i<n_tick; i++)
	{
	  out[i] = samples[phase + i];
	  samples[phase + i] = 0.0;
	}

      for(k=0; i<n_tick; k++)
	{
	  out[i] = samples[k];
	  samples[k] = 0.0;
	}

      this->phase = k;
    }
}

static void
delayline_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delayline_t *this = (delayline_t *)o;

  ac--;
  at++;

  this->samples = NULL;
  this->phase = 0;
  this->size = 0;
  this->alloc = 0;
  this->length = DELAYLINE_DEFAULT_LENGTH;
  this->stage = o;
  this->sr = 1.0;
  this->n_tick = 0;

  if(ac > 0)
    {
      if(fts_is_number(at))
	this->length = fts_get_number_float(at);
      else
	fts_object_set_error(o, "First argument of number required");
    }
  
  delayline_reset(this, fts_dsp_get_sample_rate(), fts_dsp_get_tick_size());  

  fts_dsp_add_object(o);
}

static void
delayline_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delayline_t *this = (delayline_t *)o;

  delayline_destroy(this);
  fts_dsp_remove_object(o);
}

static void
delayline_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  delayline_t *this = (delayline_t *)o;
  
  fts_set_object(value, o);
}

static fts_status_t
dline_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(delayline_t), 1, 1, 0);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, delayline_get_state);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, delayline_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, delayline_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, dline_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  return fts_Success;
}

static fts_status_t
drain_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(delayline_t), 1, 1, 0);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, delayline_get_state);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, delayline_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, delayline_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, drain_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  return fts_Success;
}

/************************************************************
 *
 *  simple delay tap
 *
 */

typedef struct 
{
  delayline_t *delayline;
  int delay;
} tap_ftl_t;

typedef struct 
{
  fts_object_t o;
  ftl_data_t ftl_data;
} tap_t;

static void
tap_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tap_t *this = (tap_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  tap_ftl_t *data = ftl_data_get_ptr(this->ftl_data);

  if(data->delay >= 0)
    {
      fts_atom_t a[3];

      fts_set_ftl_data(a + 0, this->ftl_data);
      fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 2, n_tick);
      fts_dsp_add_function(sym_tap, 3, a);
    }
  else
    {
      fts_atom_t a[4];

      fts_set_ftl_data(a + 0, this->ftl_data);
      fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 3, n_tick);
      fts_dsp_add_function(sym_tap_signal, 4, a);
    }
}

static void 
tap_control_ftl(fts_word_t *argv)
{
  tap_ftl_t *data = (tap_ftl_t *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  delayline_t * restrict delayline = data->delayline;
  int size = delayline->size;
  int delay = data->delay + n_tick; /* write already happened */
  int onset, tail;
  int i, k;
  
  if(delay > size)
    delay = size;

  onset = delayline->phase - delay;
  if(onset < 0)
    onset += size; /* ring buffer wrap around */

  tail = size - onset;

  if(tail > n_tick)
    tail = n_tick;

  for(i=0; i<tail; i++)
    out[i] = delayline->samples[onset + i];

  for(k=0; i<n_tick; k++, i++)
    out[i] = delayline->samples[k];
}

static void 
tap_signal_ftl(fts_word_t *argv)
{
  tap_ftl_t * restrict data = (tap_ftl_t *) fts_word_get_pointer(argv + 0);
  float *in = (float *)fts_word_get_pointer(argv + 1);
  float *out = (float *)fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  delayline_t *delayline = data->delayline;
  float * restrict samples = delayline->samples;
  double conv = 0.001 * delayline->sr;
  int size = delayline->size;
  int phase = delayline->phase - n_tick;
  double f_size = (double)size;
  double f_phase = (double)phase;
  double f_max_delay =(double)(size - n_tick);
  int i;

  for(i=0; i<n_tick; i++)
    {
      double f_delay = in[i] * conv;

      if(f_delay <= 0.5)
	{
	  int index = phase + i;
	  
	  if(index < 0)
	    index += size;

	  out[i] = samples[index];
	}
      else if(f_delay < 1.0)
	{
	  int index = phase + i - 1;
	  
	  if(index < 0)
	    index += size;
	  
	  out[i] = samples[index];
	}
      else if(f_delay > f_max_delay)
	{
	  int index = delayline->phase + i;

	  if(index > delayline->size)
	    index -= delayline->size;

	  out[i] = samples[index];
	}
      else
	{
	  double f_onset = f_phase - f_delay;

	  if(f_onset < 0.0)
	    f_onset += f_size;
	  
	  fts_cubic_interpolate(samples, (int)f_onset, f_onset, out + i);
	}

      f_phase += 1.0;
    }
}

static void
tap_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  tap_ftl_t *data = ftl_data_get_ptr(this->ftl_data);
  double time = fts_get_number_float(at);
  
  if(time < 0.0)
    data->delay = 0;
  else
    data->delay = 0.001 * delayline_get_sr(data->delayline) * time;
}

static void
tap_set_delayline(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  tap_ftl_t *data = ftl_data_get_ptr(this->ftl_data);
  delayline_t *delayline = (delayline_t *)fts_get_object(at);

  if(delayline != data->delayline)
    {
      if(data->delayline == NULL || delayline_get_stage(data->delayline) == delayline_get_stage(delayline))
	{
	  if(data->delayline != NULL)
	    fts_object_release((fts_object_t *)data->delayline);

	  data->delayline = delayline;

	  fts_object_refer((fts_object_t *)delayline);
	}
      else
	fts_object_signal_runtime_error(o, "delay line must have same stage");
    }
}

static void
tap_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  ftl_data_t ftl_data = ftl_data_alloc(sizeof(tap_ftl_t));
  tap_ftl_t *data = ftl_data_get_ptr(ftl_data);

  ac--;
  at++;

  fts_dsp_add_object(o);

  this->ftl_data = ftl_data;

  data->delayline = NULL;
  data->delay = -1;

  if(ac > 0 && fts_is_a(at, dline_metaclass))
    tap_set_delayline(o, 0, 0, 1, at);
  else
    {
      fts_object_set_error(o, "First argument of delay~ required");
      return;
    }

  if(ac > 1 && fts_is_number(at + 1))
    tap_set_time(o, 0, 0, 1, at + 1);
  
  fts_dsp_force_order(delayline_get_stage(data->delayline), (fts_object_t *)this);
}

static void
tap_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  tap_ftl_t *data = ftl_data_get_ptr(this->ftl_data);

  if(data->delayline != NULL)
    fts_object_release((fts_object_t *)data->delayline);

  ftl_data_free(this->ftl_data);
  fts_dsp_remove_object(o);
}

static fts_status_t
tap_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(tap_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, tap_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, tap_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, tap_put);

  if(ac > 1)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, tap_set_time);
      fts_method_define_varargs(cl, 0, fts_s_float, tap_set_time);
    }

  fts_method_define_varargs(cl, 1, sym_dline, tap_set_delayline);  

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1); /* hidden orderforcing connection */
  fts_dsp_declare_outlet(cl, 0);

  return fts_Success;
}

/************************************************************
 *
 *  all delay config
 *
 */

void
signal_delay_config(void)
{
  sym_dline = fts_new_symbol("dline~");
  sym_drain = fts_new_symbol("drain~");
  sym_tap = fts_new_symbol("tap~");
  sym_tap_signal = fts_new_symbol("tap~ (signal)");

  dline_metaclass = fts_class_install(sym_dline, dline_instantiate);
  drain_metaclass = fts_class_install(sym_drain, drain_instantiate);
  fts_metaclass_install(sym_tap, tap_instantiate, fts_arg_type_equiv);

  fts_dsp_declare_function(sym_dline, dline_ftl);
  fts_dsp_declare_function(sym_drain, drain_ftl);
  fts_dsp_declare_function(sym_tap, tap_control_ftl);
  fts_dsp_declare_function(sym_tap_signal, tap_signal_ftl);
}
