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
 * Authors: Francois Dechelle, Norbert Schnell
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
#define DELAYLINE_ALLOC_TAIL 4

static void 
delayline_clear(delayline_t *this)
{
  int i;
      
  for(i=0; i<this->ring_size + this->n_tick; i++)
    this->samples[i] = 0.0;

  this->phase = this->ring_size;
}

static void
delayline_reset(delayline_t *this, double sr, int n_tick)
{
  int size = 0.001 * this->length * sr;
  int ring_size;
  
  if(size < n_tick) 
    size = n_tick;
  
  /* check if new size matches old size */
  if(size > this->alloc_size) /* need to reallocate */
    {
      /* allocate delayline */
      ring_size = size + n_tick + DELAYLINE_ALLOC_TAIL;
      ring_size += (-ring_size) & (n_tick - 1);
      this->samples = (float *)fts_realloc(this->samples, (ring_size + n_tick) * sizeof(float));

      this->ring_size = ring_size;
      this->size = size;
      this->alloc_size = size;
    }
  else if(size < this->alloc_size) /* no allocation */
    {
      ring_size = size + n_tick + DELAYLINE_ALLOC_TAIL;
      ring_size += (-ring_size) & (n_tick - 1);

      this->ring_size = ring_size;
      this->size = size;
    }

  delayline_clear(this);
    
  this->n_tick = n_tick;
  this->sr = sr;
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
  int phase = this->phase;
  int i;

  for (i=0; i<n_tick; i++)
    this->samples[phase + i] = in[i];

  if(this->phase >= this->ring_size) /* ring buffer wrap around */
    {
      this->phase = n_tick;

      for(i=0; i<n_tick; i++)
	this->samples[i] = in[i];
    }
  else
    this->phase += n_tick;
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
  int phase = this->phase;
  int i;

  for(i=0; i<n_tick; i++)
    {
      out[i] = this->samples[phase + i];
      this->samples[phase + i] = 0.0;
    }

  this->phase += n_tick;
}

static void
delayline_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delayline_t *this = (delayline_t *)o;

  ac--;
  at++;

  this->samples = NULL;
  this->phase = 0;
  this->ring_size = 0;
  this->size = 0;
  this->length = DELAYLINE_DEFAULT_LENGTH;
  this->alloc_size = 0;
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

  if(this->samples != NULL)
    fts_free(this->samples);

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
  int delay = data->delay;
  int onset;
  int i;
  
  if(delay > delayline->size)
    delay = delayline->size;

  onset = delayline->phase - delay - n_tick;

  if(onset < 0)
    onset += delayline->ring_size; /* ring buffer wrap around */

  for(i=0; i<n_tick; i++)
    out[i] = delayline->samples[onset + i];
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
  double max_delay = delayline->size;
  double conv = 0.001 * delayline->sr;
  double phase = delayline->phase - n_tick;
  int i;

  for(i=0; i<n_tick; i++)
    {
      double delay = in[i] * conv;
      double onset;

      if(delay < 1.0f)
	out[i] = samples[delayline->phase - n_tick + i];
      else
	{
	  if(delay > max_delay)
	    delay = max_delay;
	  
	  onset = phase - delay;
	  if(onset < 1.0)
	    onset += (double)delayline->ring_size;
	  
	  fts_cubic_interpolate(samples, (int)onset, onset, out + i);
	}

      phase += 1.0;
    }
}

static void
tap_set_delayline(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;
  tap_ftl_t *data = ftl_data_get_ptr(this->ftl_data);

  data->delayline = (delayline_t *)fts_get_object(at);
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
tap_set_tie(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tap_t *this = (tap_t *)o;
  tap_ftl_t *data = ftl_data_get_ptr(this->ftl_data);

  if(fts_is_symbol(at))
    {
      fts_symbol_t tie = fts_get_symbol(at);

      if(tie == fts_s_yes)
	fts_connection_new(FTS_HIDDEN, (fts_object_t *)data->delayline, 0, (fts_object_t *)this, 0);
      else if(tie != fts_s_no)
	fts_object_signal_runtime_error(o, "cannot set tie to %s", tie);
    }
  else if(fts_is_a(at, dline_metaclass))
    {
      fts_object_t *dline = fts_get_object(at);
      fts_connection_new(FTS_HIDDEN, dline, 0, (fts_object_t *)this, 0);
    }
  else if(fts_is_tuple(at))
    {
      fts_tuple_t *tup = fts_get_tuple(at);
      int tac = fts_tuple_get_size(tup);
      fts_atom_t *tat = fts_tuple_get_atoms(tup);
      int i;

      for(i=0; i<tac; i++)
	{
	  if(fts_is_a(tat + i, dline_metaclass))
	    {
	      fts_object_t *dline = fts_get_object(tat + i);
	      fts_connection_new(FTS_HIDDEN, dline, 0, (fts_object_t *)this, 0);
	    }
	}
    }
  else
    fts_object_signal_runtime_error(o, "unknown argument for tie");    
}

static void
tap_set_tie_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  tap_set_tie(o, 0, 0, 1, value);
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

  if(ac > 1)
    {
      if(fts_is_number(at + 1))
	tap_set_time(o, 0, 0, 1, at + 1);
      else
	fts_object_set_error(o, "Second argument (delay time) of number required");
    }
  
  if(data->delayline == NULL)
    fts_object_set_error(o, "First argument of delay~ required");
}

static void
tap_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tap_t *this = (tap_t *)o;

  ftl_data_free(this->ftl_data);
  fts_dsp_remove_object(o);
}

static fts_status_t
tap_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(tap_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, tap_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, tap_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, tap_put);

  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("tie"), tap_set_tie_prop);  

  if(ac > 1)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, tap_set_time);
      fts_method_define_varargs(cl, 0, fts_s_float, tap_set_time);
    }

  fts_dsp_declare_inlet(cl, 0);
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
  fts_dsp_declare_function(sym_dline, drain_ftl);
  fts_dsp_declare_function(sym_tap, tap_control_ftl);
  fts_dsp_declare_function(sym_tap_signal, tap_signal_ftl);
}
