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

static fts_class_t *bus_class = 0;

static fts_symbol_t bus_tilda_symbol = 0;
static fts_symbol_t throw_tilda_symbol = 0;
static fts_symbol_t catch_tilda_symbol = 0;

typedef struct
{
  fts_dsp_object_t o;
  fts_dsp_edge_t *edge; /* DSP edge */
  int n_channels;
  float *buf[2]; /* two buffers */
  int toggle; /* toggle (0/1) indicating current write buffer (swapping buffers each tick) */
  int n_tick;  
} bus_t;

bus_t *
bus_get_or_create(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_atom_t *value = fts_name_get_value(patcher, name);

  if(fts_is_object(value))
    {
      fts_object_t *obj = fts_get_object(value);
      
      if(fts_object_get_class(obj) == bus_class)
	return (bus_t *)obj;
    }
  else if(fts_is_void(value))
    {
      /* create new bus */
      bus_t *bus = (bus_t *)fts_object_create(bus_class, NULL, 0, 0);
      
      fts_object_set_patcher((fts_object_t *)bus, patcher);
      fts_object_set_name((fts_object_t *)bus, name);
      
      return bus;
    }

  return NULL;
}

/*****************************************************************************
 *
 *  bus~
 *
 */

static void
bus_reset(bus_t *this, int n_tick, double sr)
{
  int n = n_tick * this->n_channels;
  int i;

  if(n_tick != this->n_tick)
    {
      this->buf[0] = fts_realloc(this->buf[0], n * sizeof(float));
      this->buf[1] = fts_realloc(this->buf[1], n * sizeof(float));

      this->n_tick = n_tick;
    }

  /* zero buffers */
  for(i=0; i<n; i++)
    (this->buf[0])[i] = (this->buf[1])[i] = 0.0;

  /* reset toggle */
  this->toggle = 0;
}

static void
bus_put_epilogue(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *)o;
  int n_tick = fts_dsp_get_tick_size();
  double sr = fts_dsp_get_sample_rate();
  fts_atom_t a;

  bus_reset(this, n_tick, sr);

  fts_set_object(&a, this);
  fts_dsp_add_function(bus_tilda_symbol, 1, &a);
}

static void
bus_ftl(fts_word_t *a)
{
  bus_t *this = (bus_t *)fts_word_get_pointer(a);
  int toggle = this->toggle;
  float *buf = this->buf[toggle];
  int n = this->n_channels * this->n_tick;
  int i;

  /* clear current buffer */
  for(i=0; i<n; i++)
    buf[i] = 0.0;

  /* swap */
  this->toggle = 1 - toggle;
}

static void
bus_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *)o;
  int n_channels = 0;

  if(ac > 0 && fts_is_a(at, fts_dsp_edge_class))
    {
      this->edge = (fts_dsp_edge_t *)fts_get_object(at);
      fts_object_refer((fts_object_t *)this->edge);
      ac--;
      at++;
    }
  else
    {
      this->edge = (fts_dsp_edge_t *)fts_object_create(fts_dsp_edge_class, NULL, 0, 0);
      fts_object_refer((fts_object_t *)this->edge);
    }

  if(ac > 0 && fts_is_number(at))
    n_channels = fts_get_number_int(at);

  if(n_channels < 1)
    n_channels = 1;

  this->buf[0] = NULL;
  this->buf[1] = NULL;
  this->toggle = 0;
  this->n_channels = n_channels;

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
bus_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *)o;

  fts_free(this->buf[0]);
  fts_free(this->buf[1]);

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
bus_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(bus_t), bus_init, bus_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_dump, fts_name_dump_method);
  fts_class_message_varargs(cl, fts_s_update_gui, fts_name_gui_method);

  fts_class_message_varargs(cl, fts_s_put_epilogue, bus_put_epilogue);
}

/*****************************************************************************
 *
 *  throw~/catch~
 *
 */

typedef struct access_tilda
{
  fts_dsp_object_t o;
  ftl_data_t bus;
  ftl_data_t index;
} access_t;

static fts_dsp_edge_t *
access_get_edge(access_t *this)
{
  if(this->bus)
    {
      bus_t *bus = *((bus_t **)ftl_data_get_ptr(this->bus));

      if(bus != NULL)
	return bus->edge;
    }
  
  return NULL;
}

static void
access_set_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  fts_dsp_edge_t *edge = access_get_edge(this);
  bus_t *bus = (bus_t *)fts_get_object(at);

  if(edge == bus->edge)
    {
      bus_t **ptr = ftl_data_get_ptr(this->bus);

      fts_object_release((fts_object_t *)*ptr);
      
      *ptr = bus;
      fts_object_refer((fts_object_t *)bus);
    }
  else
    fts_object_signal_runtime_error(o, "bus~ doesn't belong to the same DSP edge");
}

static void
access_set_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  int *ptr = ftl_data_get_ptr(this->index);
  bus_t *bus = *((bus_t **)ftl_data_get_ptr(this->bus));
  int index = fts_get_number_int(at);

  if(index < 0)
    *ptr = 0;
  else if(index >= bus->n_channels)
    *ptr = bus->n_channels - 1;
  else
    *ptr = index;
}

static void
access_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 1 && fts_is_number(at + 1))
    access_set_index(o, 0, 0, 1, at + 1);

  if(ac > 0 && fts_is_a(at, bus_class))
    access_set_bus(o, 0, 0, 1, at);	
}

static void
access_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  bus_t **bus;

  this->bus = ftl_data_alloc(sizeof(bus_t *));
  this->index = ftl_data_alloc(sizeof(int));

  bus = (bus_t **)ftl_data_get_ptr(this->bus);
  *bus = NULL;

  if(ac > 1 && fts_is_number(at + 1))
    access_set_index(o, 0, 0, 1, at + 1);

  if(ac > 0)
    {

      if(fts_is_symbol(at))
	{
	  fts_symbol_t name = fts_get_symbol(at);
	  
	  *bus = bus_get_or_create(fts_object_get_patcher(o), name);
	  
	  if(*bus == NULL)
	    {
	      fts_object_set_error(o, "%s is not a bus~", name);
	      return;
	    }

	  fts_name_add_listener(fts_object_get_patcher(o), name, o);
	}
      else if(fts_is_a(at, bus_class))
	*bus = (bus_t *)fts_get_object(at);
      else
	{
	  fts_object_set_error(o, "bad argument");
	  return;
	}
      
      fts_object_refer((fts_object_t *)*bus);
    }
  else
    fts_object_set_error(o, "bus~ required");

  fts_dsp_object_init((fts_dsp_object_t *)o);
}	

static void
access_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  fts_dsp_object_delete((fts_dsp_object_t *)o);

  if(this->bus)
    {
      bus_t **bus = ftl_data_get_ptr(this->bus);

      if(*bus != NULL)
	fts_object_release((fts_object_t *)*bus);

      ftl_data_free(this->bus);
    }

  if(this->index)
    ftl_data_free(this->index);
}

/*****************************************************************************
 *
 *  throw~
 *
 */

static void
throw_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[4];

  if(!fts_dsp_is_input_null(dsp, 0))
    {
      fts_set_ftl_data(a + 0, this->bus);
      fts_set_ftl_data(a + 1, this->index);
      fts_set_symbol(a + 2, fts_dsp_get_input_name(dsp, 0));
      fts_set_int(a + 3, n_tick);
      fts_dsp_add_function(throw_tilda_symbol, 4, a);
    }
}

static void
throw_ftl(fts_word_t *a)
{
  bus_t *bus = *((bus_t **)fts_word_get_pointer(a + 0));
  int index = *((int *)fts_word_get_pointer(a + 1));
  float *input = (float *)fts_word_get_pointer(a + 2);
  int n_tick = fts_word_get_int(a + 3);
  float *buffer = bus->buf[bus->toggle];
  float *write = buffer + index * n_tick;
  int i;

  for(i=0; i<n_tick; i++)
    write[i] += input[i];
}

static void
throw_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  fts_dsp_edge_t *edge;

  access_init(o, 0, 0, ac, at);

  edge = access_get_edge(this);

  if(edge)
    fts_dsp_before_edge(o, edge);
}

static void
throw_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(access_t), throw_init, access_delete);

  fts_class_message_varargs(cl, fts_s_put, throw_put);

  fts_class_inlet(cl, 1, bus_class, access_set_bus);
  fts_class_inlet_int(cl, 1, access_set_index);
  fts_class_inlet_varargs(cl, 1, access_varargs);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

/*****************************************************************************
 *
 *  catch~
 *
 */

static void
catch_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t a[4];

  fts_set_ftl_data(a + 0, this->bus);
  fts_set_ftl_data(a + 1, this->index);
  fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 3, n_tick);
  fts_dsp_add_function(catch_tilda_symbol, 4, a);
}

static void
catch_ftl(fts_word_t *a)
{
  bus_t *bus = *((bus_t **)fts_word_get_pointer(a + 0));
  int index = *((int *)fts_word_get_int(a + 1));
  float *output = (float *)fts_word_get_pointer(a + 2);
  int n_tick = fts_word_get_int(a + 3);
  float *buffer = bus->buf[bus->toggle];
  float *read = buffer + index * n_tick;
  int i;

  for(i=0; i<n_tick; i++)
    output[i] = read[i];
}

static void
catch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  fts_dsp_edge_t *edge;

  access_init(o, 0, 0, ac, at);

  edge = access_get_edge(this);

  if(edge)
    fts_dsp_after_edge(o, edge);
}

static void
catch_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(access_t), catch_init, access_delete);

  fts_class_message_varargs(cl, fts_s_put, catch_put);

  fts_class_inlet_int(cl, 0, access_set_index);
  fts_class_inlet(cl, 0, bus_class, access_set_bus);
  fts_class_inlet_varargs(cl, 0, access_varargs);
      
  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

void
signal_bus_config(void)
{
  bus_tilda_symbol = fts_new_symbol("bus~");
  throw_tilda_symbol = fts_new_symbol("throw~");
  catch_tilda_symbol = fts_new_symbol("catch~");

  fts_dsp_declare_function(bus_tilda_symbol, bus_ftl);
  fts_dsp_declare_function(throw_tilda_symbol, throw_ftl);
  fts_dsp_declare_function(catch_tilda_symbol, catch_ftl);

  bus_class = fts_class_install(bus_tilda_symbol, bus_instantiate);
  fts_class_install(throw_tilda_symbol, throw_instantiate);
  fts_class_install(catch_tilda_symbol, catch_instantiate);
}
