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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>

#include "sampbuf.h"

static fts_symbol_t tabcycle_fun_symbol = 0;


/******************************************************************
 *
 *    object
 *
 */
 
typedef struct
{
  int size; /* cycle size */
  int offset; /* sample offset (grows by the vectorSize) */
  sampbuf_t *buf; /* samp tab buffer */
} tabcycle_ctl_t;

typedef struct
{
  fts_dsp_object_t obj; 
  ftl_data_t tabcycle_data;
  fts_symbol_t tab_name; /* symbol bound to table we'll use */
  float value; /* value to write ot samptab */
  int state; /* inlet state - table style */
  int size; /* cache info: cycle size */
} tabcycle_t;


static void
tabcycle_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;

  if(ac == 2 && fts_is_symbol(at) && fts_is_int(at + 1))
    {
      fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);
      int size = fts_get_int(at + 1);
      
      this->tab_name = tab_name;
      this->value = 0;
      this->state = 0;
      this->size = size;
      
      this->tabcycle_data = ftl_data_alloc(sizeof(tabcycle_ctl_t));
      
      fts_dsp_object_init((fts_dsp_object_t *)o);
    }
  else
    fts_object_set_error(o, "bad arguments");
}


static void
tabcycle_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;

  ftl_data_free(this->tabcycle_data);
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

/******************************************************************
 *
 *    dsp
 *
 */
 
static void
tabcycle_dsp_function(fts_word_t *argv)
{
  float *out = (float *)fts_word_get_pointer(argv);
  tabcycle_ctl_t *ctl = (tabcycle_ctl_t *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int offset;
  int i;

  offset = ctl->offset;
  
  for(i=0; i<n_tick; i++)
    out[i] = ctl->buf->samples[offset + i];

  ctl->offset = (ctl->offset + n_tick) % ctl->size;
}

static void
tabcycle_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  sampbuf_t *buf;
  int n_tick;
  int l;

  buf = sampbuf_get(this->tab_name);

  if (buf)
    {
      n_tick = fts_dsp_get_input_size(dsp, 0);

      if(this->size % n_tick || this->size <= 0)
	{
	  post("tabcycle~: %s: size must be a multiple of %d\n", this->tab_name, n_tick);
	  return;
	}

      if (buf->size < this->size)
	{
	  post("tabcycle~: %s: table to short\n", this->tab_name);
	  return;
	}
      
      l = this->size;
      ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, size, &l);

      ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, buf, &buf);

      l = 0;
      ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, offset, &l);

      fts_set_symbol(argv + 0, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv + 1, this->tabcycle_data);
      fts_set_int(argv + 2, n_tick);
      fts_dsp_add_function(tabcycle_fun_symbol, 3, argv);
    }
  else
    post("tabcycle~: %s: can't find table~\n", this->tab_name);
}

/******************************************************************
 *
 *    user methods
 *
 */

static void
tabcycle_bang(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;
  const int  l = 0;

  ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, offset, &l);
}

static void
tabcycle_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;

  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);
  sampbuf_t *buf = sampbuf_get(tab_name);
  
  if (buf)
    {
      if (buf->size >= this->size)
	{
	  const int l = 0;
	  
	  ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, buf, &buf);
	  ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, offset, &l);
	  this->tab_name = tab_name;
	}
    }
  else
    post("tabcycle~: %s: can't find table~\n", this->tab_name);
}

/******************************************************************
 *
 *    class
 *
 */
 
static void
class_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(tabcycle_t), tabcycle_init, tabcycle_delete);

  fts_class_message_varargs(cl, fts_s_put, tabcycle_put);

  fts_class_inlet_bang(cl, 0, tabcycle_bang);
  fts_class_message_varargs(cl, fts_s_set, tabcycle_set);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  tabcycle_fun_symbol = fts_new_symbol("tabcycle");
  fts_dsp_declare_function(tabcycle_fun_symbol, tabcycle_dsp_function);
}

void
tabcycle_config(void)
{
  fts_class_install(fts_new_symbol("tabcycle~"),class_instantiate);
}


