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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */


#include <fts/fts.h>

static fts_symbol_t sym_threshold = 0;

typedef struct
{
  fts_object_t *object;
  int status; /* true if waiting for the low threshold */
  float hi_thresh;
  float lo_thresh;
  int hi_dead_ticks;
  int lo_dead_ticks;
  int wait; /* ticks to wait before becoming active */
} threshold_data_t;

typedef struct
{
  fts_object_t _o;
  ftl_data_t data;
  double hi_dead_msec;
  double lo_dead_msec;
  double ticks_per_msec;
} threshold_t;

static void
threshold_output_low(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang(o, 0);
}

static void
threshold_output_high(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang(o, 1);
}

static void
set_times(threshold_t *this)
{
  threshold_data_t *data = (threshold_data_t *)ftl_data_get_ptr(this->data);
  int hi_dead_ticks = (int)(this->ticks_per_msec * this->hi_dead_msec) - 1;
  int lo_dead_ticks = (int)(this->ticks_per_msec * this->lo_dead_msec) - 1;
  
  data->hi_dead_ticks = (hi_dead_ticks > 0)? hi_dead_ticks: 0;
  data->lo_dead_ticks = (lo_dead_ticks > 0)? lo_dead_ticks: 0;
}

static void
threshold_status_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  threshold_data_t *data = (threshold_data_t *)ftl_data_get_ptr(this->data);

  data->status = fts_get_number_int(at);
  data->wait = 0;
}

static void
threshold_status_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  threshold_data_t *data = (threshold_data_t *)ftl_data_get_ptr(this->data);

  data->status = fts_get_number_int(at);
  data->wait = 0;
}

static void
threshold_hi_thresh(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  threshold_data_t *data = (threshold_data_t *)ftl_data_get_ptr(this->data);

  data->hi_thresh = fts_get_number_float(at);

  if(data->lo_thresh > data->hi_thresh)
    data->lo_thresh = data->hi_thresh;
}

static void
threshold_hi_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  threshold_data_t *data = (threshold_data_t *)ftl_data_get_ptr(this->data);
  int hi_dead_msec = fts_get_number_int(at);
  int hi_dead_ticks = (int)(this->ticks_per_msec * hi_dead_msec) - 1;
  
  data->hi_dead_ticks = (hi_dead_ticks > 0)? hi_dead_ticks: 0;
  this->hi_dead_msec = hi_dead_msec;
}
  
static void
threshold_lo_thresh(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  threshold_data_t *data = (threshold_data_t *)ftl_data_get_ptr(this->data);

  data->lo_thresh = fts_get_number_float(at);

  if (data->lo_thresh > data->hi_thresh)
    data->lo_thresh = data->hi_thresh;
}
  
static void
threshold_lo_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  threshold_data_t *data = (threshold_data_t *)ftl_data_get_ptr(this->data);
  int lo_dead_msec = fts_get_number_int(at);
  int lo_dead_ticks = (int)(this->ticks_per_msec * lo_dead_msec) - 1;
  
  data->lo_dead_ticks = (lo_dead_ticks > 0)? lo_dead_ticks: 0;
  this->lo_dead_msec = lo_dead_msec;
}
  
static void
threshold_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  threshold_data_t *data = (threshold_data_t *)ftl_data_get_ptr(this->data);

  switch (ac)
    {
    case 4:
      if(fts_is_number(at + 3))
	this->lo_dead_msec = fts_get_number_float(at + 3);
    case 3:
      if(fts_is_number(at + 2))
	data->lo_thresh = fts_get_number_float(at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->hi_dead_msec = fts_get_number_float(at + 1);
    case 1:
      if(fts_is_number(at))
	data->hi_thresh = fts_get_number_float(at);
    }
  
  if (data->lo_thresh > data->hi_thresh)
    data->lo_thresh = data->hi_thresh;

  set_times(this);
}

void
threshold_reset(threshold_t *this, double sr, int tick_size)
{
  this->ticks_per_msec =  sr / (1000.0f * tick_size);
  set_times(this);
}

static void
threshold_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  int size = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t argv[3];
  
  threshold_reset(this, sr, size);
  
  fts_set_ftl_data(argv + 0, this->data);
  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));
  fts_dsp_add_function(sym_threshold, 3, argv);
}

static void
ftl_threshold(fts_word_t *argv)
{
  threshold_data_t *data = (threshold_data_t *)fts_word_get_pointer(argv + 0);
  float *in0 = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  
  if(data->wait)
    data->wait--;
  else if (data->status)
    {
      if (in0[n_tick-1] < data->lo_thresh)
	{
	  data->status = 0;
	  data->wait = data->lo_dead_ticks;
	  fts_timebase_add_call(fts_get_timebase(), data->object, threshold_output_low, 0, 0.0);
	}
    }
  else if (in0[n_tick-1] >= data->hi_thresh)
    {
      data->status = 1;
      data->wait = data->hi_dead_ticks;
      fts_timebase_add_call(fts_get_timebase(), data->object, threshold_output_high, 0, 0.0);
    }
}

static void
threshold_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  threshold_t *this = (threshold_t *)o;
  threshold_data_t *data;
  
  this->data = ftl_data_new(threshold_data_t);
  data = (threshold_data_t *)ftl_data_get_ptr(this->data);

  data->object = o;
  data->status = 0;
  data->wait = 0;
  this->ticks_per_msec = 1.0;

  threshold_set(o, 0, 0, ac, at);
  
  fts_dsp_add_object(o);
}

static void
threshold_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}

static void
class_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(threshold_t), threshold_init, threshold_delete);

  fts_class_method_varargs(cl, fts_s_put, threshold_put);
  
  fts_class_method_varargs(cl, fts_s_set, threshold_set);
  
  fts_dsp_declare_inlet(cl, 0);

  fts_class_inlet_number(cl, 0, threshold_status_int);
  fts_class_inlet_number(cl, 1, threshold_hi_thresh);
  fts_class_inlet_number(cl, 2, threshold_hi_time);
  fts_class_inlet_number(cl, 3, threshold_lo_thresh);
  fts_class_inlet_number(cl, 4, threshold_lo_time);

  fts_class_outlet_bang(cl, 0);
  fts_class_outlet_bang(cl, 1);
  }

void
signal_threshold_config(void)
{
  sym_threshold = fts_new_symbol("threshold~");
  fts_dsp_declare_function(sym_threshold, ftl_threshold);
    
  fts_class_install(sym_threshold, class_instantiate);
}
