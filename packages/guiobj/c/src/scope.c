/*
 * Jmax
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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#include <fts/fts.h>
#include <ftsconfig.h>

fts_symbol_t scope_symbol = 0;
fts_symbol_t sym_display = 0;
fts_symbol_t sym_auto = 0;
fts_symbol_t sym_off = 0;
fts_symbol_t sym_set_period = 0;
fts_symbol_t sym_set_threshold = 0;

#define MIN_FLOAT -68719476736.
#define SCOPE_BUFFER_SIZE 1024
#define SCOPE_READY -1

typedef struct _scope_ftl_
{
  fts_object_t *object;
  float buffer[SCOPE_BUFFER_SIZE];
  enum scope_trigger {scope_period, scope_auto, scope_threshold} trigger;
  float threshold;
  float max;
  float last;
  int size;
  int period;
  int pre;
  int index;
  int count;
  int start;
  int send;
} scope_ftl_t;

typedef struct _scope_
{
  fts_object_t o;
  ftl_data_t data;
  fts_atom_t a[SCOPE_BUFFER_SIZE];
  int range;
  double period_msec;
  double sr_in_KHz;
} scope_t;

/***************************************************************************************
 *
 *  user methods
 *
 */

static void 
scope_reset(scope_ftl_t *data)
{
  data->count = 0;
  data->max = MIN_FLOAT;

  data->send = 0;

  fts_timebase_remove_object(fts_get_timebase(), (fts_object_t *)data);
}

static void 
scope_set_period(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  double duration = data->size / this->sr_in_KHz;
  double period_msec = fts_get_number_float(at);
  fts_atom_t a[1];

  if(period_msec < duration)
    period_msec = duration;

  this->period_msec = period_msec;
  data->period = period_msec * this->sr_in_KHz;

  scope_reset(data);

  fts_set_float(a, this->period_msec);

  if(fts_object_has_id(o))
    fts_client_send_message(o, sym_set_period, 1, a);
}

static void
scope_set_threshold(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  fts_atom_t a[1];
      
  if(fts_is_number(at))
    {
      float f = fts_get_number_float(at);
      data->trigger = scope_threshold;
      data->threshold = f;

      fts_set_float(a, f);
    }
  else if(fts_is_symbol(at))
    {
      fts_symbol_t s = fts_get_symbol(at);
      if(s == sym_auto)
	{
	  data->trigger = scope_auto;

	  fts_set_symbol(a, sym_auto);
	}
      else if(s == sym_off)
	{
	  data->trigger = scope_period;

	  fts_set_symbol(a, sym_off);
	}
    }

  scope_reset(data);
  
  if(fts_object_has_id(o))
    fts_client_send_message(o, sym_set_threshold, 1, a);
}

static void 
scope_set_threshold_auto(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);

  data->trigger = scope_auto;
  scope_reset(data);
}

static void 
scope_set_threshold_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);

  data->trigger = scope_period;
  scope_reset(data);
}

static void 
scope_set_pre_delay(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  int size = data->size;
  int n = fts_get_number_int(at);

  if(n < 0)
    n = 0;
  else if(n >= size)
    n = size - 1;

  data->pre = n;
}

static void
scope_set_size_by_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t * this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  int n = fts_get_int(at);
  
  data->size = n;

  if(data->pre >= n)
    data->pre = n - 1;
  
  scope_reset(data);
}

static void
scope_set_range_by_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t * this = (scope_t *)o;
  
  this->range = fts_get_int(at);
}

static void 
scope_send_to_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);

  if(patcher && fts_patcher_is_open(patcher))
    {
      fts_atom_t *a = this->a;  
      float range = (float)this->range;
      scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
      
      if(data->send)
	{
	  float *buf = data->buffer;
	  int index = data->start;
	  int size = data->size;
	  int tail = index + size - SCOPE_BUFFER_SIZE;
	  int i;
	  
	  if(tail < 0)
	    tail = 0;
	  
	  for(i=0; i<size-tail; i++)
	    {
	      float value = buf[index];
	      int display = (int)((range - 1.0) * (value + 1.0) / 2.0 + 0.5);
	      
	      if(display < 0)
		display = 0;
	      else if(display >= range)
		display = range - 1;
	      
	      fts_set_int(a + i, display);
	      index++;
	    }
	  
	  index = 0;
	  
	  for(; i<size; i++)
	    {
	      float value = buf[index];
	      int display = (int)((range - 1.0) * (value + 1.0) / 2.0 + 0.5);
	      
	      if(display < 0)
		display = 0;
	      else if(display >= range)
		display = range - 1;
	      
	      fts_set_int(a + i, display);
	      index++;
	    }
	  
	  fts_client_send_message(o, sym_display, data->size, this->a);
	}
      else
	fts_client_send_message(o, sym_display, 0, 0);
    }
}

/***************************************************************************************
 *
 *  dsp
 *
 */

static void 
scope_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr_in_KHz = 0.001 * fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[3];

  data->index = 0;
  scope_reset(data);

  data->period = this->period_msec * sr_in_KHz;
  this->sr_in_KHz = sr_in_KHz;

  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  fts_dsp_add_function(scope_symbol, 3, a);
}

static void
scope_ftl(fts_word_t *argv)
{
  scope_ftl_t *data = (scope_ftl_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float last = data->last;
  float max = data->max;
  float *buf = data->buffer;
  int index = data->index;
  int count = data->count;
  int pre = data->pre;
  int period = data->period;
  int size = data->size;
  int i;

  if(data->trigger == scope_period)
    {
      for(i=0; i<n_tick; i++)
	{
	  float f = in[i];
	  
	  buf[index] = f;
	  
	  if(count >= period)
	    {
	      /* trigger each period */
	      count = 0;
	      data->start = index;
	      max = MIN_FLOAT;
	    }
	  else if(count == size)
	    {
	      /* send recorded data */
	      data->send = size;
	      fts_timebase_add_call(fts_get_timebase(), data->object, scope_send_to_client, 0, 0.0);
	    }

	  count++;

	  index++;
	  if(index >= SCOPE_BUFFER_SIZE)
	    index = 0;
	}      
    }
  else /* trigger == scope_auto || trigger == scope_threshold */
    {
      float threshold = data->threshold;

      for(i=0; i<n_tick; i++)
	{
	  float f = in[i];
	  
	  buf[index] = f;
	  
	  if(count <= 0)
	    {
	      /* waiting for trigger */
	      if(last <= f && f >= threshold)
		{
		  /* trigger */
		  data->start = index - pre;

		  if(data->start < 0)
		    data->start += SCOPE_BUFFER_SIZE;

		  /* count upwards from pre delay */
		  count = pre + 1;
		}
	      else
		{
		  if(count <= -period)
		    {
		      /* end of period without triggered */
		      count = 0;

		      data->send = 0;
		      fts_timebase_add_call(fts_get_timebase(), data->object, scope_send_to_client, 0, 0.0);

		      /* reset threshold for auto trigger */
		      if(data->trigger == scope_auto)
			data->threshold = 0.75 * max;
		      
		      max = MIN_FLOAT;
		    }

		  count--;
		}
	    }
	  else 
	    {
	      if(count >= period)
		{
		  /* reset threshold for auto trigger */
		  if(data->trigger == scope_auto)
		    data->threshold = 0.75 * max;
		  
		  max = MIN_FLOAT;

		  /* start new period looking for trigger */
		  count = -1;
		}
	      else if(count == size)
		{
		  /* send recorded data */
		  data->send = size;
		  fts_timebase_add_call(fts_get_timebase(), data->object, scope_send_to_client, 0, 0.0);
		  
		  count++;
		}
	      else
		count++;
	    }

	  if(f > max)
	    max = f;

	  last = f;

	  index++;	  
	  if(index >= SCOPE_BUFFER_SIZE)
	    index = 0;
	}
    }
  
  data->max = max;
  data->last = last;
  data->index = index;
  data->count = count;
}

/***************************************************************************************
 *
 *  class
 *
 */

static void 
scope_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  fts_atom_t a;

  fts_set_float(&a, this->period_msec);
  fts_dumper_send(dumper, sym_set_period, 1, &a);

  switch(data->trigger)
    {
    case scope_period:
      fts_set_symbol(&a, sym_off);
      fts_dumper_send(dumper, sym_set_threshold, 1, &a);
      break;
    case scope_auto:
      fts_set_symbol(&a, sym_auto);
      fts_dumper_send(dumper, sym_set_threshold, 1, &a);
      break;
    case scope_threshold:
      fts_set_float(&a, data->threshold);
      fts_dumper_send(dumper, sym_set_threshold, 1, &a);
      break;
    default:
      break;
    }
}

static void 
scope_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  fts_atom_t a[1];

  fts_set_float(a, this->period_msec);
  fts_client_send_message(o, sym_set_period, 1, a);

  fts_set_float(a, data->threshold);
  fts_client_send_message(o, sym_set_threshold, 1, a);
}

static void
scope_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data;
  fts_atom_t a[1];
  int i;

  ac--;
  at++;

  this->data = 0;
  this->sr_in_KHz = 0.001 * fts_dsp_get_sample_rate();
  this->period_msec = 100.0;

  this->data = ftl_data_alloc(sizeof(scope_ftl_t));
  data = ftl_data_get_ptr(this->data);
  
  data->object = o;
  data->trigger = scope_auto;
  data->threshold = 0.5;
  data->last = 0.0;

  data->size = 128;
  this->range = 128;
  data->period = 0;
  data->pre = 0;
  data->start = 0;

  scope_reset(data);
  
  fts_dsp_add_object((fts_object_t *)this);
}

static void
scope_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  
  if(this->data)
    {
      ftl_data_free(this->data);      
      fts_dsp_remove_object(o);
    }
}

static fts_status_t
scope_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{  
  fts_class_init(cl, sizeof(scope_t), 1, 0, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, scope_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, scope_delete);      

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, scope_put);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_size, scope_set_size_by_client);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("range"), scope_set_range_by_client);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_set_period, scope_set_period);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_set_threshold, scope_set_threshold);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("onset"), scope_set_pre_delay);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, scope_dump); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload, scope_upload); 

  fts_dsp_declare_inlet(cl, 0);

  return fts_Success;
}

void
scope_config(void)
{
  scope_symbol = fts_new_symbol("scope~");
  sym_display = fts_new_symbol("display");
  sym_auto = fts_new_symbol("auto");
  sym_off = fts_new_symbol("off");
  sym_set_period = fts_new_symbol("setPeriod");
  sym_set_threshold = fts_new_symbol("setThreshold");

  fts_dsp_declare_function(scope_symbol, scope_ftl);  
  fts_class_install(scope_symbol, scope_instantiate);
  fts_alias_install(fts_new_symbol("scope"), scope_symbol);
}
