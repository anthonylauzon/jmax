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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#include <fts/fts.h>
#include "fvec.h"

fts_symbol_t scope_symbol = 0;

typedef struct _scope_ftl_
{
  fts_object_t o;
  fvec_t *fvec;
  enum scope_state {scope_ready, scope_triggered} state;
  float threshold;
  float last;
  int alloc;
  int index;
  int pre;
  int step;
  fts_alarm_t alarm;
} scope_ftl_t;

typedef struct _scope_
{
  fts_object_t o;
  ftl_data_t data;
} scope_t;

/***************************************************************************************
 *
 *  user methods
 *
 */

static void 
scope_reset(scope_ftl_t *data)
{
  data->state = scope_ready;
  data->index = 0;

  fts_alarm_unarm(&data->alarm);
}

static void 
scope_output(fts_alarm_t *alarm, void *o)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  fts_atom_t a[1];

  fts_alarm_unarm(alarm);

  fvec_atom_set(a, data->fvec);
  fts_outlet_send((fts_object_t *)o, 0, fvec_symbol, 1, a);
}

static void 
scope_set_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  int n = fts_get_number_int(at);

  if(n < 16)
    n = 16;
  else if(n > 1024)
    n = 1024;

  data->alloc = n + 64;

  fvec_set_size(data->fvec, data->alloc);
  fvec_set_size(data->fvec, n);

  if(data->pre > n)
    data->pre = n;

  scope_reset(data);
}

static void 
scope_set_threshold(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  float f = fts_get_number_float(at);

  data->threshold = f;
}

static void 
scope_set_pre_delay(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  int size = fvec_get_size(data->fvec);
  int n = fts_get_number_int(at);

  if(n < 0)
    n = 0;
  else if (n > size)
    n = size;

  data->pre = n;
}


static void 
scope_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;

  switch (ac)
    {
    case 3:
      if(fts_is_number(at + 2))
	scope_set_pre_delay(o, 0, 0, 1, at + 2);      
    case 2:
      if(fts_is_number(at + 1))
	scope_set_threshold(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	scope_set_size(o, 0, 0, 1, at);
    default:
      break;
    }
}

/***************************************************************************************
 *
 *  put
 *
 */

static void 
scope_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data = (scope_ftl_t *)ftl_data_get_ptr(this->data);
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[3];

  data->state = scope_ready;
  data->index = 0;
  
  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  fts_dsp_add_function(scope_symbol, 3, a);
}

static void
scope_ftl(fts_word_t *argv)
{
  scope_ftl_t *data = (scope_ftl_t *)fts_word_get_ptr(argv + 0);
  float * restrict in = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = fvec_get_ptr(data->fvec);
  int index = data->index;
  int step = data->step;
  int i;

  if(data->state == scope_triggered)
    {
      int size = fvec_get_size(data->fvec);
      
      if(size - index <= n_tick)
	{
	  fts_alarm_set_delay(&(data->alarm), 0.01);
	  fts_alarm_arm(&(data->alarm));

	  data->state = scope_ready;
	}
      
      /* there must be enough place in the buffer beyond size in order to put this all there behind */
      for(i=0; i<n_tick; i+=step)
	{
	  buf[index] = in[i];
	  index++;
	}
    }
  else /* if(data->state == scope_ready) */
    {
      int pre = data->pre;
      int alloc = data->alloc;
      float threshold = data->threshold;
      float last = data->last;
      
      /* just record all incomming data for pre-delay */
      for(i=0; i<n_tick; i+=step)
	{
	  float f = in[i];
	  
	  if(last < threshold &&  f >= threshold)
	    {
	      /* trigger */
	      data->state = scope_triggered;

	      if(index < pre)
		{
		  int tail = pre - index;
		  int j;
		  
		  /* copy pre-delay signal to beginning of vector */
		  for(j=1; j<=index; j++)
		    buf[pre - j] = buf[index - j];
		  
		  for(j=1; j<=tail; j++)
		    buf[tail - j] = buf[alloc - j];
		}
	      else if(index > pre)
		{
		  int head = index - pre;
		  int j;
		  
		  for(j=0; j<pre; j++)
		    buf[j] = buf[j + head];
		}
	      
	      index = pre;
	      
	      break;
	    }
	  else
	    {
	      buf[index] = f;
	      index++;
	      
	      if(index >= alloc)
		index -= alloc;

	      last = f;
	    }
	}

      for(; i<n_tick; i+=step)
	{
	  buf[index] = in[i];
	  index++;
	}
    }
  
  data->last = in[n_tick - 1];
  data->index = index;
}

/***************************************************************************************
 *
 *  class
 *
 */

static void
scope_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  scope_t *this = (scope_t *)o;
  scope_ftl_t *data;
  fvec_t *fvec;
  int i;

  ac--;
  at++;

  this->data = 0;

  if(ac > 3)
    {
      fts_object_set_error(o, "Extra argument");
      return;
    }

  for(i=0; i<ac; i++)
    {
      if(!fts_is_number(at + i))
	{
	  fts_object_set_error(o, "Wrong arguments");
	  return;
	}
    }

  this->data = ftl_data_alloc(sizeof(scope_ftl_t));
  data = ftl_data_get_ptr(this->data);
  
  data->fvec = (fvec_t *)fts_object_create(fvec_class, 0, 0);
  fts_object_refer((fts_object_t *)data->fvec);

  data->state = scope_ready;
  data->threshold = 0.5;
  data->index = 0;
  data->pre = 0;
  data->step = 1;
  fts_alarm_init(&(data->alarm), 0, scope_output, (void *)this);

  scope_set(o, 0, 0, ac, at);
  
  /* set default size */
  if(fvec_get_size(data->fvec) == 0)
    {
      fts_atom_t a[1];

      fts_set_int(a, 256);
      scope_set_size(o, 0, 0, 1, a);
    }

  fts_dsp_add_object((fts_object_t *)this);
}

static void
scope_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scope_t *this = (scope_t *)o;
  
  if(this->data)
    {
      scope_ftl_t *data = ftl_data_get_ptr(this->data);
      
      if(data->fvec)
	fts_object_release((fts_object_t *)data->fvec);    
      
      ftl_data_free(this->data);
      
      fts_dsp_remove_object(o);
    }
}

static fts_status_t
scope_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{  
  ac--;
  at++;

  fts_class_init(cl, sizeof(scope_t), 4, 2, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, scope_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, scope_delete);      
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("put"), scope_put);
  
  fts_method_define_varargs(cl, 1, fts_s_int, scope_set_size);
  fts_method_define_varargs(cl, 1, fts_s_float, scope_set_size);

  fts_method_define_varargs(cl, 2, fts_s_int, scope_set_threshold);
  fts_method_define_varargs(cl, 2, fts_s_float, scope_set_threshold);

  fts_method_define_varargs(cl, 3, fts_s_int, scope_set_pre_delay);
  fts_method_define_varargs(cl, 3, fts_s_float, scope_set_pre_delay);

  dsp_sig_inlet(cl, 0);

  return fts_Success;
}

void
signal_scope_config(void)
{
  scope_symbol = fts_new_symbol("scope~");
  dsp_declare_function(scope_symbol, scope_ftl);  

  fts_class_install(scope_symbol, scope_instantiate);
}
