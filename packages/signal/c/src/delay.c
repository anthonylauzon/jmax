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
static fts_symbol_t sym_delay = 0;
static fts_symbol_t sym_tapin = 0;
static fts_symbol_t sym_tapout = 0;
static fts_symbol_t sym_retap = 0;
static fts_symbol_t sym_vtap = 0;

fts_class_t *delayline_class;

/************************************************************
 *
 *  delay line
 *
 */

#define DELAYLINE_DEFAULT_LENGTH 1000

static void
delayline_initialize(delayline_t *this)
{
  this->buffer = NULL;
  this->phase = 0;
  this->delay_size = 0;
  this->drain_size = 0;
  this->zero_onset = 0;
  this->ring_size = 0;
  this->alloc = 0;
  this->delay_length = DELAYLINE_DEFAULT_LENGTH;
  this->drain_length = 0.0;
  this->edge = NULL;
  this->sr = fts_dsp_get_sample_rate();
  this->n_tick = fts_dsp_get_tick_size();
}

static void 
delayline_clear(delayline_t *this)
{
  int i;
      
  for(i=-DELAYLINE_ALLOC_HEAD; i<this->ring_size+DELAYLINE_ALLOC_TAIL; i++)
    this->buffer[i] = 0.0;

  this->phase = 0;
}

static void
delayline_reset(delayline_t *this, int n_tick, double sr)
{
  int delay_size = (0.001 * this->delay_length * sr + 0.5);
  int drain_size = (0.001 * this->drain_length * sr + 0.5);
  int ring_size;
  int size;

  /* align drain size to tick */
  size = (int)ceil((double)drain_size / (double)n_tick) * n_tick;

  /* set onset for zeroing */
  this->zero_onset = size;

  /* add delay size and exta points for cubic interpolation */
  size += delay_size + DELAYLINE_ALLOC_HEAD + DELAYLINE_ALLOC_TAIL;

  /* align size to tick and add extra ticks for input and zeroing */
  ring_size = (int)ceil((double)size / (double)n_tick) * n_tick + 2 * n_tick;

  /* check if new size matches old size */
  if(ring_size > this->alloc) /* need to reallocate */
    {
      /* allocate delayline (with head and tail for interpolation) */
      this->buffer = (float *)fts_realloc(this->buffer, (ring_size + DELAYLINE_ALLOC_HEAD + DELAYLINE_ALLOC_TAIL) * sizeof(float));
      this->buffer += DELAYLINE_ALLOC_HEAD;

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
  if(this->edge != NULL)
    fts_object_release((fts_object_t *)this->edge);

  if(this->buffer != NULL)
    fts_free(this->buffer - DELAYLINE_ALLOC_HEAD);
}

static void
delayline_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delayline_t *this = (delayline_t *)o;
  int n_tick = fts_dsp_edge_get_n_tick(this->edge);
  int sr = fts_dsp_edge_get_sr(this->edge);
  fts_atom_t a;
  
  delayline_reset(this, n_tick, sr);

  fts_set_object(&a, o);
  fts_dsp_add_function(sym_delayline, 1, &a);
}

static void 
delayline_ftl(fts_word_t *argv)
{
  delayline_t * restrict this = (delayline_t *)fts_word_get_pointer(argv + 0);
  int onset;
  int i;

  /* increment phase */
  this->phase += this->n_tick;
  
  if(this->phase == this->ring_size)
    this->phase = 0;  
  
  onset = this->phase + this->zero_onset;

  if(onset >= this->ring_size)
    onset -= this->ring_size;

  /* zero buffer */
  for(i=0; i<this->n_tick; i++)
    this->buffer[onset + i] = 0.0;
}

static void
delayline_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delayline_t *this = (delayline_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)o);

  delayline_initialize(this);

  if(ac > 0 && fts_is_a(at, fts_dsp_edge_class))
    {
      this->edge = (fts_dsp_edge_t *)fts_get_object(at);
      fts_object_refer((fts_object_t *)this->edge);
      ac--;
      at++;
    }
  else
    {
      this->edge = (fts_dsp_edge_t *)fts_object_create(fts_dsp_edge_class, 0, 0);
      fts_object_refer((fts_object_t *)this->edge);
    }

  if(ac > 0 && fts_is_number(at))
    this->delay_length = fts_get_number_float(at);
  
  if(ac > 1 && fts_is_number(at + 1))
    this->drain_length = fts_get_number_float(at + 1);
}

static void
delayline_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delayline_t *this = (delayline_t *)o;

  delayline_destroy(this);
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
delayline_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(delayline_t), delayline_init, delayline_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_dump, fts_name_dump_method);
  fts_class_message_varargs(cl, fts_s_update_gui, fts_name_gui_method);

  fts_class_message_varargs(cl, fts_s_put_epilogue, delayline_put);
}

/************************************************************
 *
 *  delay~
 *
 */

typedef struct 
{
  fts_dsp_object_t o;
  ftl_data_t samples;
  delayline_t *line;
  enum interpolation_mode {mode_none, mode_cubic} mode;
  enum read_write_order {order_straight, order_loop} order;
} delay_t;

static void
delay_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  fts_atom_t a[5];
  
  delayline_reset(this->line, n_tick, sr);

  fts_set_pointer(a + 0, this->line);
  fts_set_ftl_data(a + 1, this->samples);
  fts_set_symbol(a + 2, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(a + 3, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 4, n_tick);  
  fts_dsp_add_function(sym_delay, 5, a);
}

static void 
delay_ftl(fts_word_t *argv)
{
  delayline_t *delayline = (delayline_t *)fts_word_get_pointer(argv + 0);
  int samples = *((int *)fts_word_get_pointer(argv + 1));
  float *in = (float *)fts_word_get_pointer(argv + 2);
  float *out = (float *)fts_word_get_pointer(argv + 3);
  int n_tick = fts_word_get_int(argv + 4);
  int phase = delayline->phase;
  int onset, tail;
  int i, k;

  if(samples > delayline->delay_size)
    samples = delayline->delay_size;
  
  onset = phase - samples;
  if(onset < 0)
    onset += delayline->ring_size; /* ring buffer wrap around */
  
  tail = delayline->ring_size - onset;
  
  if(tail > n_tick)
    tail = n_tick;
  
  for(i=0; i<tail; i++)
    {
      delayline->buffer[phase + i] = in[i];
      out[i] = delayline->buffer[onset + i];
    }
  
  for(k=0; i<n_tick; k++, i++)
    {
      delayline->buffer[phase + i] = in[i];
      out[i] = delayline->buffer[k];
    }
  
  phase += n_tick;
  
  if(phase == delayline->ring_size)
    delayline->phase = 0;
  else
    delayline->phase = phase;    
}

static void
delay_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;
  double time = fts_get_number_float(at);
	
   if(time <= 0.0)
    time = 0;

  switch(this->mode)
    {
    case mode_none:
      {
	int *samples = ftl_data_get_ptr(this->samples);

	*samples = (int)(0.001 * delayline_get_sr(this->line) * time + 0.5);
      }
      break;

    case mode_cubic:
      {
	double *samples = ftl_data_get_ptr(this->samples);

	*samples = 0.001 * delayline_get_sr(this->line) * time;
      }
      break;

    default:
      break;
    }
}

static void
delay_set_line(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;
  delayline_t *line = (delayline_t *)fts_get_object(at);

  if(line != this->line)
    {
      if(this->line == NULL || delayline_get_edge(line) == delayline_get_edge(this->line))
	{
	  if(this->line != NULL)
	    fts_object_release((fts_object_t *)this->line);

	  this->line = line;
	  fts_object_refer((fts_object_t *)line);
	}
      else
	fts_object_error(o, "delay line doesn't belong to the same DSP edge");
    }
}

static void
delay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;
  ftl_data_t samples = ftl_data_alloc(sizeof(int));

  fts_dsp_object_init((fts_dsp_object_t *)o);

  this->line = (delayline_t *)fts_malloc(sizeof(delayline_t));
  this->samples = samples;
  this->mode = mode_none;
  this->order = order_straight;

  delayline_initialize(this->line);

  if(ac > 0 && fts_is_number(at))
    {
      this->line->delay_length = fts_get_number_float(at);
      delay_set_time(o, 0, 0, 1, at);
    }
  
  if(ac > 1 && fts_is_number(at + 1))
    delay_set_time(o, 0, 0, 1, at + 1);
}

static void
delay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;

  delayline_destroy(this->line);
  fts_free(this->line);
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
delay_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(delay_t), delay_init, delay_delete);

  fts_class_message_varargs(cl, fts_s_put, delay_put);

  fts_class_inlet_int(cl, 1, delay_set_time);
  fts_class_inlet_float(cl, 1, delay_set_time);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

/************************************************************
 *
 *  tapin~ and tapout~
 *
 */

static void
tapin_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[4];
  
  fts_set_pointer(a + 0, this->line);
  fts_set_ftl_data(a + 1, this->samples);
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
  float * restrict buffer = delayline->buffer;
  int phase = delayline->phase;
  int size = delayline->drain_size;
  int ring_size = delayline->ring_size;
  int onset, tail;
  int i;

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
	buffer[onset + i] += in[i];
    }
  else
    {
      int k;

      for(i=0; i<tail; i++)
	buffer[onset + i] += in[i];

      for(k=0; i<n_tick; k++, i++)
	buffer[k] += in[i];
    }
}

static void
tapout_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[4];
  
  fts_set_pointer(a + 0, this->line);
  fts_set_ftl_data(a + 1, this->samples);
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
  float * restrict buffer = delayline->buffer;
  int size = delayline->delay_size;
  int ring_size = delayline->ring_size;
  int onset, tail;
  int i, k;
  
  if(delay > size)
    delay = size;

  onset = delayline->phase - delay;
  if(onset < 0)
    onset += ring_size; /* ring buffer wrap around */

  tail = ring_size - onset;

  if(tail > n_tick)
    tail = n_tick;

  for(i=0; i<tail; i++)
    out[i] = buffer[onset + i];

  for(k=0; i<n_tick; k++, i++)
    out[i] = buffer[k];
}

static void
tapin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;
  ftl_data_t samples = ftl_data_alloc(sizeof(int));

  fts_dsp_object_init((fts_dsp_object_t *)o);

  this->line = NULL;
  this->samples = samples;
  this->mode = mode_none;

  if(ac > 0 && fts_is_a(at, delayline_class))
    delay_set_line(o, 0, 0, 1, at);
  else
    {
      fts_object_error(o, "first argument must be a delay line");
      return;
    }

  if(ac > 1 && fts_is_number(at + 1))
    delay_set_time(o, 0, 0, 1, at + 1);
  
  fts_dsp_before_edge(o, delayline_get_edge(this->line));
}

static void
tapout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;
  ftl_data_t samples = ftl_data_alloc(sizeof(int));

  fts_dsp_object_init((fts_dsp_object_t *)o);

  this->line = NULL;
  this->samples = samples;
  this->mode = mode_none;

  if(ac > 0 && fts_is_a(at, delayline_class))
    delay_set_line(o, 0, 0, 1, at);
  else
    {
      fts_object_error(o, "first argument must be a delay line");
      return;
    }

  if(ac > 1 && fts_is_number(at + 1))
    delay_set_time(o, 0, 0, 1, at + 1);
  
  fts_dsp_after_edge(o, delayline_get_edge(this->line));
}

static void
tap_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;

  if(this->line != NULL)
    fts_object_release((fts_object_t *)this->line);

  if(this->samples != NULL)
    ftl_data_free(this->samples);

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
tapin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(delay_t), tapin_init, tap_delete);

  fts_class_message_varargs(cl, fts_s_put, tapin_put);

  fts_class_inlet_int(cl, 1, delay_set_time);
  fts_class_inlet_float(cl, 1, delay_set_time);
  fts_class_inlet(cl, 2, delayline_class, delay_set_line);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0); /* hidden orderforcing connection */
}

static void
tapout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(delay_t), tapout_init, tap_delete);

  fts_class_message_varargs(cl, fts_s_put, tapout_put);

  fts_class_inlet_int(cl, 0, delay_set_time);
  fts_class_inlet_float(cl, 0, delay_set_time);

  fts_class_inlet(cl, 1, delayline_class, delay_set_line);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

/************************************************************
 *
 *  feedback input tap
 *
 */

static void
retap_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;
  double time = fts_get_number_float(at);
  int *samples = ftl_data_get_ptr(this->samples);
  int n_tick = delayline_get_n_tick(this->line);
	
  if(time <= 0.0)
    time = 0;
  
  *samples = (int)(0.001 * delayline_get_sr(this->line) * time + 0.5);
  
  if(*samples < n_tick)
    *samples = n_tick;
}

static void
retap_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;
  ftl_data_t samples = ftl_data_alloc(sizeof(int));

  fts_dsp_object_init((fts_dsp_object_t *)o);

  this->line = NULL;
  this->samples = samples;
  this->mode = mode_none;

  if(ac > 0 && fts_is_a(at, delayline_class))
    delay_set_line(o, 0, 0, 1, at);
  else
    {
      fts_object_error(o, "first argument must be a delay line");
      return;
    }

  if(ac > 1 && fts_is_number(at + 1))
    retap_set_time(o, 0, 0, 1, at + 1);
  
  fts_dsp_after_edge(o, delayline_get_edge(this->line));
}

static void
retap_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(delay_t), retap_init, tap_delete);

  fts_class_message_varargs(cl, fts_s_put, tapin_put);

  fts_class_inlet_int(cl, 1, retap_set_time);
  fts_class_inlet_float(cl, 1, retap_set_time);
  fts_class_inlet(cl, 2, delayline_class, delay_set_line);

  fts_dsp_declare_inlet(cl, 0);
}

/************************************************************
 *
 *  variable delay tap
 *
 */

static void
vtap_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  delay_t *this = (delay_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[4];

  fts_set_pointer(a + 0, this->line);
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
  float * restrict buffer = delayline->buffer;
  double conv = 0.001 * delayline->sr;
  int delay_size = delayline->delay_size;
  int ring_size = delayline->ring_size;
  int phase = delayline->phase - n_tick;
  double f_ring_size = (double)ring_size;
  double f_phase = (double)phase;
  double f_max_delay =(double)delay_size;
  int i;

  /* set head tail for cubic interpolation */
  if(phase == 0)
    {
      delayline->buffer[-1] = delayline->buffer[delayline->ring_size - 1];
      delayline->buffer[delayline->ring_size] = delayline->buffer[0];
      delayline->buffer[delayline->ring_size + 1] = delayline->buffer[1];
    }

  for(i=0; i<n_tick; i++)
    {
      double f_delay = in[i] * conv;

      if(f_delay <= 0.5)
	{
	  int index = phase + i;
	  
	  if(index < 0)
	    index += ring_size;

	  out[i] = buffer[index];
	}
      else if(f_delay < 1.0)
	{
	  int index = phase + i - 1;
	  
	  if(index < 0)
	    index += ring_size;
	  
	  out[i] = buffer[index];
	}
      else if(f_delay > f_max_delay)
	{
	  int onset = phase - delay_size + i;

	  if(onset < 0)
	    onset += ring_size;

	  out[i] = buffer[onset];
	}
      else
	{
	  double f_onset = f_phase - f_delay;

	  if(f_onset < 0.0)
	    f_onset += f_ring_size;
	  
	  fts_cubic_interpolate(buffer, (int)f_onset, f_onset, out + i);
	}

      f_phase += 1.0;
    }
}

static void
vtap_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  delay_t *this = (delay_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)o);

  this->line = NULL;
  this->samples = NULL;
  this->mode = mode_cubic;

  if(ac > 0 && fts_is_a(at, delayline_class))
    delay_set_line(o, 0, 0, 1, at);
  else
    {
      fts_object_error(o, "first argument must be a delay line");
      return;
    }

  fts_dsp_after_edge(o, delayline_get_edge(this->line));
}

static void
vtap_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(delay_t), vtap_init, tap_delete);

  fts_class_message_varargs(cl, fts_s_put, vtap_put);

  fts_class_inlet(cl, 1, delayline_class, delay_set_line);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
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
  sym_delay = fts_new_symbol("delay~");
  sym_tapin = fts_new_symbol("tapin~");
  sym_tapout = fts_new_symbol("tapout~");
  sym_retap = fts_new_symbol("retap~");
  sym_vtap = fts_new_symbol("vtap~");

  delayline_class = fts_class_install(sym_delayline, delayline_instantiate);
  fts_class_alias(delayline_class, fts_new_symbol("dline~"));
  fts_class_install(sym_delay, delay_instantiate);
  fts_class_install(sym_tapin, tapin_instantiate);
  fts_class_install(sym_tapout, tapout_instantiate);
  fts_class_install(sym_retap, retap_instantiate);
  fts_class_install(sym_vtap, vtap_instantiate);

  fts_dsp_declare_function(sym_delayline, delayline_ftl);
  fts_dsp_declare_function(sym_delay, delay_ftl);
  fts_dsp_declare_function(sym_tapin, tapin_ftl);
  fts_dsp_declare_function(sym_tapout, tapout_ftl);
  fts_dsp_declare_function(sym_vtap, vtap_ftl);
}
