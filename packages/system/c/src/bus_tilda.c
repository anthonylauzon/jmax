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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "sigbus.h"

static fts_class_t *bus_tilda_class = 0;
static fts_symbol_t bus_tilda_symbol = 0;
static fts_symbol_t bus_tilda_write_symbol = 0;
static fts_symbol_t bus_tilda_read_symbol = 0;
static fts_symbol_t bus_tilda_write_read_symbol = 0;

static fts_symbol_t throw_tilda_symbol = 0;
static fts_symbol_t throw_tilda_channel_symbol = 0;
static fts_symbol_t catch_tilda_symbol = 0;
static fts_symbol_t catch_tilda_channel_symbol = 0;

typedef struct _bus_tilda_
{
  fts_object_t _o;
  fts_signal_bus_t *bus;
} bus_tilda_t;

typedef struct _access_tilda_bus_
{
  fts_object_t o;
  ftl_data_t bus;
  int n_channels;
} access_tilda_bus_t;

typedef struct _access_tilda_channel_
{
  fts_object_t o;
  fts_signal_bus_t *bus;
  ftl_data_t channel;
} access_tilda_channel_t;

/*****************************************************************************
 *
 *  throw~
 *
 */

#define NOTHING_CONNECTED -1

static void
access_tilda_set_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_tilda_bus_t *this = (access_tilda_bus_t *)o;
  fts_signal_bus_t **bus = ftl_data_get_ptr(this->bus);

  if(*bus)
    fts_object_release((fts_object_t *)*bus);

  *bus = (fts_signal_bus_t *)fts_get_object(at + 0);
  fts_object_refer((fts_object_t *)*bus);
}

static void
access_tilda_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_tilda_channel_t *this = (access_tilda_channel_t *)o;
  int *channel = ftl_data_get_ptr(this->channel);

  *channel = fts_get_number_int(at + 0);
}

static void
access_tilda_bus_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_tilda_bus_t *this = (access_tilda_bus_t *)o;
  fts_signal_bus_t **bus;

  ac--;
  at++;
  
  this->bus = ftl_data_alloc(sizeof(fts_signal_bus_t *));
  bus = ftl_data_get_ptr(this->bus);
  *bus = (fts_signal_bus_t *)fts_get_object(at + 0);

  fts_object_refer((fts_object_t *)*bus);

  this->n_channels = fts_signal_bus_get_size(*bus);

  fts_dsp_add_object(o);
}	

static void
access_tilda_channel_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_tilda_channel_t *this = (access_tilda_channel_t *)o;
  int *channel;

  ac--;
  at++;
  
  this->bus = (fts_signal_bus_t *)fts_get_object(at + 0);
  fts_object_refer((fts_object_t *)this->bus);

  this->channel = ftl_data_alloc(sizeof(int));
  channel = ftl_data_get_ptr(this->channel);      
  *channel = fts_get_int(at + 1);

  fts_dsp_add_object(o);
}	

static void
access_tilda_bus_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_tilda_bus_t *this = (access_tilda_bus_t *)o;
  fts_signal_bus_t **bus = ftl_data_get_ptr(this->bus);

  fts_dsp_remove_object(o);

  fts_object_release((fts_object_t *)*bus);
  ftl_data_free(this->bus);
}

static void
access_tilda_channel_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_tilda_channel_t *this = (access_tilda_channel_t *)o;

  fts_dsp_remove_object(o);

  fts_object_release((fts_object_t *)this->bus);
  ftl_data_free(this->channel);
}

static void
throw_tilda_bus_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_tilda_bus_t *this = (access_tilda_bus_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_signal_bus_t *bus = *((fts_signal_bus_t **)ftl_data_get_ptr(this->bus));
  int n_channels = this->n_channels;
  int first_connection = NOTHING_CONNECTED;
  int last_connection = NOTHING_CONNECTED;
  fts_atom_t a[FTS_SIGNAL_BUS_MAX_CHANNELS + 5];
  int i;

  last_connection = NOTHING_CONNECTED;
  
  for(i=0; i<n_channels; i++)
    {
      if(!fts_dsp_is_input_null(dsp, i))
	{
	  if(last_connection == NOTHING_CONNECTED)
	    first_connection = i;
	  
	  last_connection = i;
	}
    }
  
  if(first_connection != NOTHING_CONNECTED)
    {
      int n = last_connection - first_connection + 1;

      /* put data */
      fts_set_ftl_data(a + 0, this->bus);
      fts_set_ftl_data(a + 1, fts_signal_bus_get_toggle_data(bus));
      fts_set_int(a + 2, first_connection);
      fts_set_int(a + 3, last_connection);
      fts_set_int(a + 4, n_tick);
      
      /* put inputs */
      for(i=0; i<n; i++)
	fts_set_symbol(a + 5 + i, fts_dsp_get_input_name(dsp, first_connection + i));
      
      fts_dsp_add_function(throw_tilda_symbol, 5 + n, a);
    }
}

static void
throw_tilda_write_to_bus(fts_word_t *a)
{
  fts_signal_bus_t *bus = *((fts_signal_bus_t **)fts_word_get_pointer(a + 0));
  int toggle = *((int *)fts_word_get_pointer(a + 1));
  int first_connection = fts_word_get_int(a + 2);
  int last_connection = fts_word_get_int(a + 3);
  int n_tick = fts_word_get_int(a + 4);
  float *buffer = fts_signal_bus_get_buffer(bus, toggle);
  int n_channels = fts_signal_bus_get_size(bus);
  int i, j;

  if(last_connection > n_channels - 1)
    last_connection = n_channels - 1;
  
  for(i=first_connection; i<=last_connection; i++)
    {
      float *input = (float *)fts_word_get_pointer(a + 5 + i);
      float *write = buffer + i * n_tick;

      for(j=0; j<n_tick; j++)
	write[j] += input[j];
    }
}

static void
throw_tilda_channel_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_tilda_channel_t *this = (access_tilda_channel_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[7];

  if(!fts_dsp_is_input_null(dsp, 0))
    {
      fts_set_ftl_data(a + 0, fts_signal_bus_get_buffer_data(this->bus, 0));
      fts_set_ftl_data(a + 1, fts_signal_bus_get_buffer_data(this->bus, 1));
      fts_set_ftl_data(a + 2, fts_signal_bus_get_toggle_data(this->bus));
      fts_set_ftl_data(a + 3, this->channel);
      fts_set_int(a + 4, fts_signal_bus_get_size(this->bus));
      fts_set_int(a + 5, n_tick);
      fts_set_symbol(a + 6, fts_dsp_get_input_name(dsp, 0));

      fts_dsp_add_function(throw_tilda_channel_symbol, 7, a);
    }
}

static void
throw_tilda_write_to_channel(fts_word_t *a)
{
  int toggle = *((int *)fts_word_get_pointer(a + 2));
  int channel = *((int *)fts_word_get_pointer(a + 3));
  int n_channels = fts_word_get_int(a + 4);
  int n_tick = fts_word_get_int(a + 5);
  float *input = (float *)fts_word_get_pointer(a + 6);
  float *buffer = (float *)fts_word_get_pointer(a + toggle);
  float *write = buffer + channel * n_tick;
  int i;

  if(channel >= 0 && channel < n_channels)
    {
      for(i=0; i<n_tick; i++)
	write[i] += input[i];
    }
}

static fts_status_t
throw_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_a(at, fts_signal_bus_type) && (ac == 1 || (ac == 2 && fts_is_int(at + 1))))
    {
      if(ac == 1) 
	{
	  /* bus */
	  fts_signal_bus_t *bus = (fts_signal_bus_t *)fts_get_object(at);
	  int n_channels = fts_signal_bus_get_size(bus);
	  int i;

	  fts_class_init(cl, sizeof(access_tilda_bus_t), n_channels + 1, 0, 0);
	  
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, access_tilda_bus_init);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, access_tilda_bus_delete);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, throw_tilda_bus_put);
	  
	  for(i=0; i<n_channels; i++)
	    fts_dsp_declare_inlet(cl, i);
	  
	  fts_method_define_varargs(cl, n_channels, fts_signal_bus_symbol, access_tilda_set_bus);
	}
      else
	{
	  /* channel */
	  fts_class_init(cl, sizeof(access_tilda_channel_t), 2, 0, 0);
	  
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, access_tilda_channel_init);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, access_tilda_channel_delete);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, throw_tilda_channel_put);
	  
	  fts_dsp_declare_inlet(cl, 0);
	  fts_method_define_varargs(cl, 1, fts_s_int, access_tilda_set_channel);
	}

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

/*****************************************************************************
 *
 *  catch~
 *
 */

static void
catch_tilda_bus_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_tilda_bus_t *this = (access_tilda_bus_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_signal_bus_t *bus = *((fts_signal_bus_t **)ftl_data_get_ptr(this->bus));
  int n_channels = this->n_channels;
  fts_atom_t a[FTS_SIGNAL_BUS_MAX_CHANNELS + 4];
  int i;

  /* put data */
  fts_set_ftl_data(a + 0, this->bus);
  fts_set_ftl_data(a + 1, fts_signal_bus_get_toggle_data(bus));
  fts_set_int(a + 2, n_channels);
  fts_set_int(a + 3, n_tick);
  
  /* put outputs */
  for(i=0; i<n_channels; i++)
    fts_set_symbol(a + 4 + i, fts_dsp_get_output_name(dsp, i));
  
  fts_dsp_add_function(catch_tilda_symbol, 4 + n_channels, a);
}

static void
catch_tilda_read_from_bus(fts_word_t *a)
{
  fts_signal_bus_t *bus = *((fts_signal_bus_t **)fts_word_get_pointer(a + 0));
  int toggle = *((int *)fts_word_get_pointer(a + 1));
  int n_channels = fts_word_get_int(a + 2);
  int n_tick = fts_word_get_int(a + 3);
  float *buffer = fts_signal_bus_get_buffer(bus, 1 - toggle);
  int bus_channels = fts_signal_bus_get_size(bus);
  int i, j;

  if(n_channels <= bus_channels)
    {
      for(i=0; i<n_channels; i++)
	{
	  float *output = (float *)fts_word_get_pointer(a + 4 + i);
	  float *read = buffer + i * n_tick;
	  
	  for(j=0; j<n_tick; j++)
	    output[j] = read[j];
	}
    }
  else
    {
      for(i=0; i<bus_channels; i++)
	{
	  float *output = (float *)fts_word_get_pointer(a + 4 + i);
	  float *read = buffer + i * n_tick;
	  
	  for(j=0; j<n_tick; j++)
	    output[j] = read[j];
	}

      for(; i<n_channels; i++)
	{
	  float *output = (float *)fts_word_get_pointer(a + 4 + i);
	  
	  for(j=0; j<n_tick; j++)
	    output[j] = 0.0;
	}
    }
}

static void
catch_tilda_channel_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_tilda_channel_t *this = (access_tilda_channel_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  int n_channels = fts_signal_bus_get_size(this->bus);
  fts_atom_t a[7];

  fts_set_ftl_data(a + 0, fts_signal_bus_get_buffer_data(this->bus, 0));
  fts_set_ftl_data(a + 1, fts_signal_bus_get_buffer_data(this->bus, 1));
  fts_set_ftl_data(a + 2, fts_signal_bus_get_toggle_data(this->bus));
  fts_set_ftl_data(a + 3, this->channel);
  fts_set_int(a + 4, fts_signal_bus_get_size(this->bus));
  fts_set_int(a + 5, n_tick);
  fts_set_symbol(a + 6, fts_dsp_get_output_name(dsp, 0));
  
  fts_dsp_add_function(catch_tilda_channel_symbol, 7, a);
}

static void
catch_tilda_read_from_channel(fts_word_t *a)
{
  int toggle = *((int *)fts_word_get_pointer(a + 2));
  int channel = *((int *)fts_word_get_pointer(a + 3));
  int n_channels = fts_word_get_int(a + 4);
  int n_tick = fts_word_get_int(a + 5);
  float *output = (float *)fts_word_get_pointer(a + 6);
  float *buffer = (float *)fts_word_get_pointer(a + 1 - toggle);
  float *read = buffer + channel * n_tick;
  int i;

  if(channel >= 0 && channel < n_channels)
    {
      for(i=0; i<n_tick; i++)
	output[i] = read[i];
    }
  else
    {
      for(i=0; i<n_tick; i++)
	output[i] = 0.0;      
    }
}

static fts_status_t
catch_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_a(at, fts_signal_bus_type) && (ac == 1 || (ac == 2 && fts_is_int(at + 1))))
    {
      if(ac == 1) 
	{
	  /* bus */
	  fts_signal_bus_t *bus = (fts_signal_bus_t *)fts_get_object(at);
	  int n_channels = fts_signal_bus_get_size(bus);
	  int i;

	  fts_class_init(cl, sizeof(access_tilda_bus_t), 1, n_channels, 0);
	  
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, access_tilda_bus_init);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, access_tilda_bus_delete);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, catch_tilda_bus_put);
	  
	  fts_method_define_varargs(cl, 0, fts_signal_bus_symbol, access_tilda_set_bus);
	  
	  for(i=0; i<n_channels; i++)
	    fts_dsp_declare_outlet(cl, i);
	}
      else
	{
	  /* channel */
	  fts_class_init(cl, sizeof(access_tilda_channel_t), 1, 1, 0);
	  
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, access_tilda_channel_init);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, access_tilda_channel_delete);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, catch_tilda_channel_put);
	  
	  fts_method_define_varargs(cl, 0, fts_s_int, access_tilda_set_channel);
	  
	  fts_dsp_declare_outlet(cl, 0);
	}

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

/*****************************************************************************
 *
 *  bus~
 *
 */

#define BUS_TILDA_WRITE 1
#define BUS_TILDA_READ 2
#define BUS_TILDA_WRITE_READ 3

static void
bus_tilda_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bus_tilda_t *this = (bus_tilda_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  int n_channels = fts_signal_bus_get_size(this->bus);
  int write_read = 0;
  fts_atom_t a[2 * FTS_SIGNAL_BUS_MAX_CHANNELS + 5];
  int i;

  for(i=0; i<n_channels; i++)
    {
      if(!fts_dsp_is_input_null(dsp, i))
	write_read |= BUS_TILDA_WRITE;
      if(fts_object_outlet_is_connected(o, i))
	write_read |= BUS_TILDA_READ;
    }
  
  fts_set_ftl_data(a + 0, fts_signal_bus_get_buffer_data(this->bus, 0));
  fts_set_ftl_data(a + 1, fts_signal_bus_get_buffer_data(this->bus, 1));
  fts_set_ftl_data(a + 2, fts_signal_bus_get_toggle_data(this->bus));
  fts_set_int(a + 3, n_channels);
  fts_set_int(a + 4, n_tick);
	
  if(write_read == BUS_TILDA_WRITE)
    {
      for(i=0; i<n_channels; i++)
	fts_set_symbol(a + 5 + i, fts_dsp_get_input_name(dsp, i));
      
      fts_dsp_add_function(bus_tilda_write_symbol, 5 + n_channels, a);
    }
  else if(write_read == BUS_TILDA_READ)
    {
      for(i=0; i<n_channels; i++)
	fts_set_symbol(a + 5 + i, fts_dsp_get_output_name(dsp, i));
      
      fts_dsp_add_function(bus_tilda_read_symbol, 5 + n_channels, a);   
    }
  else if(write_read == BUS_TILDA_WRITE_READ)
    {
      for(i=0; i<n_channels; i++)
	{
	  fts_set_symbol(a + 5 + i, fts_dsp_get_input_name(dsp, i));
	  fts_set_symbol(a + 5 + n_channels + i, fts_dsp_get_output_name(dsp, i));
	}
      
      fts_dsp_add_function(bus_tilda_write_read_symbol, 5 + 2 * n_channels, a);
    }
}

static void
bus_tilda_write(fts_word_t *a)
{
  int toggle = *((int *)fts_word_get_pointer(a + 2));
  int n_channels = fts_word_get_int(a + 3);
  int n_tick = fts_word_get_int(a + 4);
  float *write_buffer = (float *)fts_word_get_pointer(a + toggle);
  fts_word_t *ins = a + 5;
  int i, j;

  for(i=0; i<n_channels; i++)
    {
      float *input = (float *)fts_word_get_pointer(ins + i);
      float *write = write_buffer + i * n_tick;

      for(j=0; j<n_tick; j++)
	write[j] += input[j];
    }
}

static void
bus_tilda_read(fts_word_t *a)
{
  int toggle = *((int *)fts_word_get_pointer(a + 2));
  int n_channels = fts_word_get_int(a + 3);
  int n_tick = fts_word_get_int(a + 4);
  float *read_buffer = (float *)fts_word_get_pointer(a + 1 - toggle);
  fts_word_t *outs = a + 5;
  int i, j;

  for(i=0; i<n_channels; i++)
    {
      float *output = (float *)fts_word_get_pointer(outs + i);
      float *read = read_buffer + i * n_tick;

      for(j=0; j<n_tick; j++)
	output[j] = read[j];
    }
}

static void
bus_tilda_write_and_read(fts_word_t *a)
{
  int toggle = *((int *)fts_word_get_pointer(a + 2));
  int n_channels = fts_word_get_int(a + 3);
  int n_tick = fts_word_get_int(a + 4);
  float *write_buffer = (float *)fts_word_get_pointer(a + toggle);
  float *read_buffer = (float *)fts_word_get_pointer(a + 1 - toggle);
  fts_word_t *ins = a + 5;
  fts_word_t *outs = a + 5 + n_channels;
  int i, j;

  for(i=0; i<n_channels; i++)
    {
      float *input = (float *)fts_word_get_pointer(ins + i);
      float *write = write_buffer + i * n_tick;

      for(j=0; j<n_tick; j++)
	write[j] += input[j];
    }

  for(i=0; i<n_channels; i++)
    {
      float *output = (float *)fts_word_get_pointer(outs + i);
      float *read = read_buffer + i * n_tick;

      for(j=0; j<n_tick; j++)
	output[j] = read[j];
    }
}

static void
bus_tilda_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  bus_tilda_t *this = (bus_tilda_t *)o;
  fts_atom_t a;
  int n_channels = 0;
  
  ac--;
  at++;

  if(ac == 0)
    n_channels = 1;
  else if(ac == 1 && fts_is_int(at))
    n_channels = fts_get_int(at);

  fts_set_int(&a, n_channels);
  this->bus = (fts_signal_bus_t *)fts_object_create(fts_signal_bus_type, 1, &a);
  fts_object_refer((fts_object_t *)this->bus);

  fts_dsp_add_object(o);
}

static void
bus_tilda_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  bus_tilda_t *this = (bus_tilda_t *)o;

  fts_object_release((fts_object_t *)this->bus);

  fts_dsp_remove_object(o);
}

static void
bus_tilda_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  bus_tilda_t *this = (bus_tilda_t *)obj;

  fts_set_object(value, this->bus);
}

static fts_status_t
bus_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n_channels = 0;
  int i;

  if(ac == 0)
    n_channels = 1;
  else if(ac == 1 && fts_is_int(at))
    n_channels = fts_get_int(at);
  else
    return &fts_CannotInstantiate;

  fts_class_init(cl, sizeof(bus_tilda_t), n_channels, n_channels, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bus_tilda_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, bus_tilda_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, bus_tilda_put);
  
  for(i=0; i<n_channels; i++)
    {
      fts_dsp_declare_inlet(cl, i);
      fts_dsp_declare_outlet(cl, i);
    }
	  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, bus_tilda_get_state);
  
  return fts_Success;
}

void
bus_tilda_config(void)
{
  throw_tilda_symbol = fts_new_symbol("throw~");
  throw_tilda_channel_symbol = fts_new_symbol("throw~ channel");
  fts_dsp_declare_function(throw_tilda_symbol, throw_tilda_write_to_bus);
  fts_dsp_declare_function(throw_tilda_channel_symbol, throw_tilda_write_to_channel);
  
  catch_tilda_symbol = fts_new_symbol("catch~");
  catch_tilda_channel_symbol = fts_new_symbol("catch~ channel");
  fts_dsp_declare_function(catch_tilda_symbol, catch_tilda_read_from_bus);
  fts_dsp_declare_function(catch_tilda_channel_symbol, catch_tilda_read_from_channel);

  bus_tilda_symbol = fts_new_symbol("bus~");
  bus_tilda_write_symbol = fts_new_symbol("bus~ write");
  bus_tilda_read_symbol = fts_new_symbol("bus~ read");
  bus_tilda_write_read_symbol = fts_new_symbol("bus~ write&read");
  fts_dsp_declare_function(bus_tilda_write_symbol, bus_tilda_write);
  fts_dsp_declare_function(bus_tilda_read_symbol, bus_tilda_read);
  fts_dsp_declare_function(bus_tilda_write_read_symbol, bus_tilda_write_and_read);

  fts_metaclass_install(bus_tilda_symbol, bus_tilda_instantiate, fts_first_arg_equiv);
  fts_metaclass_install(throw_tilda_symbol, throw_tilda_instantiate, fts_never_equiv);
  fts_metaclass_install(catch_tilda_symbol, catch_tilda_instantiate, fts_never_equiv);
}
