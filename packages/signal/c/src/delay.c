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
#include <utils/c/include/utils.h>
#include "delay.h"

static fts_symbol_t sym_delayline = 0;
static fts_symbol_t sym_delayline_with_input_and_drain = 0;
static fts_symbol_t sym_delayline_with_input = 0;
static fts_symbol_t sym_delayline_with_drain = 0;
static fts_symbol_t sym_tapin = 0;
static fts_symbol_t sym_tapout = 0;
static fts_symbol_t sym_vtap = 0;

fts_metaclass_t *delayline_metaclass;

/************************************************************
 *
 *  delay line
 *
 */

#define DELAYLINE_DEFAULT_LENGTH 1000

static void 
delayline_clear(delayline_t *this)
{
  int i;
      
  for(i=-DELAYLINE_ALLOC_HEAD; i<this->ring_size+DELAYLINE_ALLOC_TAIL; i++)
    this->samples[i] = 0.0;

  this->phase = 0;
}

static void
delayline_reset(delayline_t *this, double sr, int n_tick)
{
  int delay_size = 0.001 * this->delay_length * sr;
  int drain_size = 0.001 * this->drain_length * sr;
  int ring_size = delay_size + drain_size + 2 * n_tick + DELAYLINE_ALLOC_HEAD;

  ring_size = (int)ceil((double)ring_size / (double)n_tick) * n_tick; /* aligne ring buffer to tick size */
  
  /* check if new size matches old size */
  if(ring_size > this->alloc) /* need to reallocate */
    {
      /* allocate delayline (with head and tail for interpolation) */
      this->samples = (float *)fts_realloc(this->samples, (ring_size + DELAYLINE_ALLOC_HEAD + DELAYLINE_ALLOC_TAIL) * sizeof(float));
      this->samples += DELAYLINE_ALLOC_HEAD;

      this->alloc = ring_size;
    }

  this->delay_size = delay_size;
  this->drain_size = drain_size;
  this->ring_size = ring_size;
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
delayline_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delayline_t *this = (delayline_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  int input = (fts_dsp_is_input_null(dsp, 0) != 0);
  int drain = (this->drain_length > 0.0);
  fts_atom_t a[3];
  
  delayline_reset(this, sr, n_tick);

  if(1 || (input && drain))
    {
      fts_set_object(a + 0, o);
      fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
      fts_set_int(a + 2, n_tick);  
      fts_dsp_add_function(sym_delayline_with_input_and_drain, 3, a);
    }
  else if(input)
    {
      fts_set_object(a + 0, o);
      fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
      fts_set_int(a + 2, n_tick);  
      fts_dsp_add_function(sym_delayline_with_input, 3, a);
    }
  else if(drain)
    {
      fts_set_object(a + 0, o);
      fts_set_int(a + 1, n_tick);  
      fts_dsp_add_function(sym_delayline_with_drain, 2, a);
    }
  else
    {
      fts_set_object(a + 0, o);
      fts_set_int(a + 1, n_tick);  
      fts_dsp_add_function(sym_delayline, 2, a);
    }
}

static void 
delayline_ftl_with_input_and_drain(fts_word_t *argv)
{
  delayline_t * restrict this = (delayline_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float * restrict samples = this->samples;
  int ring_size = this->ring_size;
  int phase = this->phase;
  int zero = phase + this->drain_size;
  int tail;
  int i;

  if(phase == 0)
    {
      /* set head for cubic interpolation */
      samples[-1] = samples[ring_size - 1];

      /* add input */
      for(i=0; i<n_tick; i++)
	samples[phase + i] += in[i];
      
      /* set tail for cubic interpolation */
      samples[ring_size] = samples[0];
      samples[ring_size + 1] = samples[1];

      phase = n_tick;	
    }
  else
    {
      for(i=0; i<n_tick; i++)
	samples[phase + i] += in[i];
      
      phase += n_tick;
      
      if(phase == ring_size)
	phase = 0;
    }

  this->phase = phase;

  /* zero tail of drain buffer */
  if(zero >= ring_size)
    zero -= ring_size; /* ring buffer wrap around */

  tail = ring_size - zero;
  if(tail > n_tick)
    {
      for(i=0; i<n_tick; i++)
	samples[zero + i] = 0.0;
    }
  else
    {
      int k;

      for(i=0; i<tail; i++)
	samples[zero + i] = 0.0;

      for(k=0; i<n_tick; k++, i++)
	samples[k] = 0.0;
    }
}

static void 
delayline_ftl_with_input(fts_word_t *argv)
{
  delayline_t * restrict this = (delayline_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float * restrict samples = this->samples;
  int ring_size = this->ring_size;
  int phase = this->phase;
  int i;

}

static void 
delayline_ftl_with_drain(fts_word_t *argv)
{
  delayline_t * restrict this = (delayline_t *)fts_word_get_pointer(argv + 0);
  int n_tick = fts_word_get_int(argv + 1);
  float * restrict samples = this->samples;
  int ring_size = this->ring_size;
  int phase = this->phase;
  int zero = phase + this->drain_size;
  int tail;
  int i;

  /* advance phase and set head and tail */

  /* zero tail of drain buffer */
  if(zero >= ring_size)
    zero -= ring_size; /* ring buffer wrap around */

  tail = ring_size - zero;
  if(tail > n_tick)
    {
      for(i=0; i<n_tick; i++)
	samples[zero + i] = 0.0;
    }
  else
    {
      int k;

      for(i=0; i<tail; i++)
	samples[zero + i] = 0.0;

      for(k=0; i<n_tick; k++, i++)
	samples[k] = 0.0;
    }
}

static void 
delayline_ftl(fts_word_t *argv)
{
  delayline_t * restrict this = (delayline_t *)fts_word_get_pointer(argv + 0);
  int n_tick = fts_word_get_int(argv + 1);
  float * restrict samples = this->samples;
  int ring_size = this->ring_size;
  int phase = this->phase;
  int i;

  /* advance phase and set head and tail */
}

static void
delayline_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delayline_t *this = (delayline_t *)o;

  this->samples = NULL;
  this->phase = 0;
  this->delay_size = 0;
  this->drain_size = 0;
  this->ring_size = 0;
  this->alloc = 0;
  this->delay_length = DELAYLINE_DEFAULT_LENGTH;
  this->drain_length = 0.0;
  this->stage = (fts_dspstage_t *)o;
  this->sr = fts_dsp_get_sample_rate();
  this->n_tick = fts_dsp_get_tick_size();

  fts_dspstage_self(o);

  if(ac > 0 && fts_is_number(at))
    this->delay_length = fts_get_number_float(at);
  
  if(ac > 1 && fts_is_number(at + 1))
    this->drain_length = fts_get_number_float(at + 1);
  
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
delayline_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(delayline_t), 1, 1, 0);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, delayline_get_state);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, delayline_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, delayline_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, delayline_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  return fts_ok;
}

/************************************************************
 *
 *  tapin~ and tapout~
 *
 */

typedef struct 
{
  fts_object_t o;
  ftl_data_t delay;
  delayline_t *delayline;
  fts_dspstage_t *stage;
  enum interpolation_mode {mode_none, mode_cubic} mode;
} tap_t;

static void
tap_set_delay(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  double time = fts_get_number_float(at);
	
  if(time <= 0.0)
    time = 0;

  switch(this->mode)
    {
    case mode_none:
      {
	int *delay = ftl_data_get_ptr(this->delay);

	*delay = (int)(0.001 * delayline_get_sr(this->delayline) * time + 0.5);
      }
      break;

    case mode_cubic:
      {
	int *delay = ftl_data_get_ptr(this->delay);

	*delay = 0.001 * delayline_get_sr(this->delayline) * time;
      }
      break;

    default:
      break;
    }
}

static void
tap_set_delay_double(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  double *delay = ftl_data_get_ptr(this->delay);
  double time = fts_get_number_float(at);
  
  if(time <= 0.0)
    *delay = 0.0;
  else
    *delay = 0.001 * delayline_get_sr(this->delayline) * time;
}

static void
tap_set_delayline(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  delayline_t *delayline = (delayline_t *)fts_get_object(at);

  if(delayline != this->delayline)
    {
      if(this->delayline == NULL || this->stage == NULL || delayline_get_stage(delayline) == this->stage)
	{
	  if(this->delayline != NULL)
	    {
	      fts_object_release((fts_object_t *)this->delayline);

	      if(this->stage == NULL)
		this->stage = delayline_get_stage(delayline);
	    }

	  this->delayline = delayline;

	  fts_object_refer((fts_object_t *)delayline);
	}
      else
	fts_object_signal_runtime_error(o, "delay line must have same stage");
    }
}

static void
tapin_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tap_t *this = (tap_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[4];
  
  fts_set_pointer(a + 0, this->delayline);
  fts_set_ftl_data(a + 1, this->delay);
  fts_set_symbol(a + 2, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(a + 3, n_tick);
  fts_dsp_add_function(sym_tapin, 4, a);
}

static void 
tapin_ftl(fts_word_t *argv)
{
  delayline_t *delayline = (delayline_t *)fts_word_get_pointer(argv);
  int delay = *((int *)fts_word_get_pointer(argv + 1));
  float * restrict in = (float *) fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float * restrict samples = delayline->samples;
  int phase = delayline->phase;
  int size = delayline->drain_size;
  int ring_size = delayline->ring_size;
  int onset, tail;
  int i, k;

  if(delay > size)
    delay = size;
  
  onset = phase + delay;
  if(onset >= ring_size)
    onset -= ring_size;

  tail = ring_size - onset;

  /* add input to current tick */
  if(tail > n_tick)
    {
      for(i=0; i<n_tick; i++)
	samples[onset + i] += in[i];
    }
  else
    {
      int k;

      for(i=0; i<tail; i++)
	samples[onset + i] += in[i];

      for(k=0; i<n_tick; k++, i++)
	samples[k] += in[i];
    }
}

static void
tapout_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tap_t *this = (tap_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[4];
  
  fts_set_pointer(a + 0, this->delayline);
  fts_set_ftl_data(a + 1, this->delay);
  fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 3, n_tick);
  fts_dsp_add_function(sym_tapout, 4, a);
}

static void 
tapout_ftl(fts_word_t *argv)
{
  delayline_t *delayline = (delayline_t *)fts_word_get_pointer(argv);
  int delay = *((int *)fts_word_get_pointer(argv + 1));
  float * restrict out = (float *) fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float * restrict samples = delayline->samples;
  int size = delayline->delay_size;
  int ring_size = delayline->ring_size;
  int onset, tail;
  int i, k;
  
  if(delay > size)
    delay = size;

  onset = delayline->phase - n_tick - delay;
  if(onset < 0)
    onset += ring_size; /* ring buffer wrap around */

  tail = ring_size - onset;

  if(tail > n_tick)
    tail = n_tick;

  for(i=0; i<tail; i++)
    out[i] = samples[onset + i];

  for(k=0; i<n_tick; k++, i++)
    out[i] = samples[k];
}

static void
tapin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  ftl_data_t delay = ftl_data_alloc(sizeof(int));

  fts_dsp_add_object(o);

  this->delayline = NULL;
  this->delay = delay;
  this->stage = NULL;
  this->mode = mode_none;

  if(ac > 0 && fts_is_a(at, delayline_metaclass))
    tap_set_delayline(o, 0, 0, 1, at);
  else
    {
      fts_object_set_error(o, "First argument must be a delay line");
      return;
    }

  if(ac > 1 && fts_is_number(at + 1))
    tap_set_delay(o, 0, 0, 1, at + 1);
  
  fts_dsp_force_order((fts_object_t *)this, fts_dspstage_get_front(delayline_get_stage(this->delayline)));
}

static void
tapout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  ftl_data_t delay = ftl_data_alloc(sizeof(int));

  fts_dsp_add_object(o);

  this->delayline = NULL;
  this->delay = delay;
  this->stage = NULL;
  this->mode = mode_none;

  if(ac > 0 && fts_is_a(at, delayline_metaclass))
    tap_set_delayline(o, 0, 0, 1, at);
  else
    {
      fts_object_set_error(o, "First argument must be a delay line");
      return;
    }

  if(ac > 1 && fts_is_number(at + 1))
    tap_set_delay(o, 0, 0, 1, at + 1);
  
  fts_dsp_force_order(fts_dspstage_get_back(delayline_get_stage(this->delayline)), (fts_object_t *)this);
}

static void
vtap_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;

  fts_dsp_add_object(o);

  this->delayline = NULL;
  this->delay = NULL;
  this->stage = NULL;
  this->mode = mode_cubic;

  if(ac > 0 && fts_is_a(at, delayline_metaclass))
    tap_set_delayline(o, 0, 0, 1, at);
  else
    {
      fts_object_set_error(o, "First argument must be a delay line");
      return;
    }

  fts_dsp_force_order(fts_dspstage_get_back(delayline_get_stage(this->delayline)), (fts_object_t *)this);
}

static void
tap_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;

  if(this->delayline != NULL)
    fts_object_release((fts_object_t *)this->delayline);

  ftl_data_free(this->delay);
  fts_dsp_remove_object(o);
}

static fts_status_t
tapin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(tap_t), 3, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, tapin_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, tap_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, tapin_put);

  fts_method_define_varargs(cl, 1, fts_s_int, tap_set_delay);
  fts_method_define_varargs(cl, 1, fts_s_float, tap_set_delay);
  fts_method_define_varargs(cl, 2, sym_delayline, tap_set_delayline);  

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0); /* hidden orderforcing connection */

  return fts_ok;
}

static fts_status_t
tapout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(tap_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, tapout_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, tap_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, tapout_put);

  fts_method_define_varargs(cl, 0, fts_s_int, tap_set_delay);
  fts_method_define_varargs(cl, 0, fts_s_float, tap_set_delay);
  fts_method_define_varargs(cl, 1, sym_delayline, tap_set_delayline);  

  fts_dsp_declare_inlet(cl, 0); /* hidden orderforcing connection */
  fts_dsp_declare_outlet(cl, 0);

  return fts_ok;
}

/************************************************************
 *
 *  variable delay tap
 *
 */

static void
vtap_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tap_t *this = (tap_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[4];

  fts_set_pointer(a + 0, this->delayline);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 3, n_tick);
  fts_dsp_add_function(sym_vtap, 4, a);
}

static void 
vtap_ftl(fts_word_t *argv)
{
  delayline_t *delayline = (delayline_t *)fts_word_get_pointer(argv);
  float *in = (float *)fts_word_get_pointer(argv + 1);
  float *out = (float *)fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float * restrict samples = delayline->samples;
  double conv = 0.001 * delayline->sr;
  int delay_size = delayline->delay_size;
  int ring_size = delayline->ring_size;
  int phase = delayline->phase - n_tick;
  double f_ring_size = (double)ring_size;
  double f_phase = (double)phase;
  double f_max_delay =(double)delay_size;
  int i;

  for(i=0; i<n_tick; i++)
    {
      double f_delay = in[i] * conv;

      if(f_delay <= 0.5)
	{
	  int index = phase + i;
	  
	  if(index < 0)
	    index += ring_size;

	  out[i] = samples[index];
	}
      else if(f_delay < 1.0)
	{
	  int index = phase + i - 1;
	  
	  if(index < 0)
	    index += ring_size;
	  
	  out[i] = samples[index];
	}
      else if(f_delay > f_max_delay)
	{
	  int onset = phase - delay_size + i;

	  if(onset < 0)
	    onset += ring_size;

	  out[i] = samples[onset];
	}
      else
	{
	  double f_onset = f_phase - f_delay;

	  if(f_onset < 0.0)
	    f_onset += f_ring_size;
	  
	  fts_cubic_interpolate(samples, (int)f_onset, f_onset, out + i);
	}

      f_phase += 1.0;
    }
}

static fts_status_t
vtap_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(tap_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, vtap_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, tap_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, vtap_put);

  fts_method_define_varargs(cl, 1, sym_delayline, tap_set_delayline);  

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  return fts_ok;
}

/************************************************************
 *
 *  all delay config
 *
 */

void
signal_delay_config(void)
{
  sym_delayline = fts_new_symbol("delayline~");
  sym_delayline_with_input_and_drain = fts_new_symbol("delayline~ (input and drain)");
  sym_delayline_with_input = fts_new_symbol("delayline~ (drain, no input)");
  sym_delayline_with_drain = fts_new_symbol("delayline~ (input, no drain)");
  sym_tapin = fts_new_symbol("tapin~");
  sym_tapout = fts_new_symbol("tapout~");
  sym_vtap = fts_new_symbol("vtap~");

  delayline_metaclass = fts_class_install(sym_delayline, delayline_instantiate);
  fts_alias_install(fts_new_symbol("dline~"), sym_delayline);
  fts_metaclass_install(sym_tapin, tapin_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(sym_tapout, tapout_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(sym_vtap, vtap_instantiate, fts_arg_type_equiv);

  fts_dsp_declare_function(sym_delayline_with_input_and_drain, delayline_ftl_with_input_and_drain);
  fts_dsp_declare_function(sym_delayline_with_input, delayline_ftl_with_input);
  fts_dsp_declare_function(sym_delayline_with_drain, delayline_ftl_with_drain);
  fts_dsp_declare_function(sym_delayline, delayline_ftl);
  fts_dsp_declare_function(sym_tapin, tapin_ftl);
  fts_dsp_declare_function(sym_tapout, tapout_ftl);
  fts_dsp_declare_function(sym_vtap, vtap_ftl);
}
