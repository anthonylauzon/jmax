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

#include <fts/sys.h>
#include <fts/runtime.h>
#include <fts/lang/mess.h>
#include <fts/lang/ftl.h>
#include <fts/lang/dsp.h>

fts_symbol_t fts_signal_bus_symbol = 0;
fts_class_t *fts_signal_bus_class = 0;

/*****************************************************************************
 *
 *  signal bus
 *
 */

static void
fts_signal_bus_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_signal_bus_t *this = (fts_signal_bus_t *)o;
  int n = this->n_tick * this->n_channels;
  float *buf0, *buf1;
  int *toggle;
  int i;

  buf0 = (float *)ftl_data_get_ptr(this->buf[0]);
  buf1 = (float *)ftl_data_get_ptr(this->buf[1]);
  toggle = (int *)ftl_data_get_ptr(this->toggle);
  
  for(i=0; i<n; i++)
    buf0[i] = buf1[i] = 0.0;

  *toggle = 0;
}

static void
fts_signal_bus_put_prologue(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_signal_bus_t *this = (fts_signal_bus_t *)o;
  int n_tick = fts_get_tick_size();
  fts_atom_t a[4];
  int i;

  this->n_tick = n_tick;

  fts_signal_bus_reset(o, 0, 0, ac, at);

  fts_set_ftl_data(a + 0, this->buf[0]);
  fts_set_ftl_data(a + 1, this->buf[1]);
  fts_set_ftl_data(a + 2, this->toggle);
  fts_set_int(a + 3, this->n_channels * n_tick);
  fts_dsp_add_function(fts_signal_bus_symbol, 4, a);
}

static void
fts_signal_bus_toggle_and_zero(fts_word_t *a)
{
  int *toggle = (int *)fts_word_get_ptr(a + 2);
  int n = fts_word_get_int(a + 3);
  float *write;
  int i;

  *toggle = 1 - *toggle;

  write = (float *)fts_word_get_ptr(a + *toggle);

  for(i=0; i<n; i++)
    write[i] = 0.0;
}

static void
fts_signal_bus_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fts_signal_bus_t *this = (fts_signal_bus_t *)o;
  int n_channels;
  int i, n;

  ac--;
  at++;

  if(ac > 0)
    {
      n_channels = fts_get_int(at);

      if(n_channels < 1 || n_channels > FTS_SIGNAL_BUS_MAX_CHANNELS)
	{
	  fts_object_set_error(o, "Wrong argument for # of channels (1 to 64)");
	  return;
	}
    }
  else
    n_channels = 1;

  n = fts_get_tick_size() * n_channels;

  this->buf[0] = ftl_data_alloc(n * sizeof(float));
  this->buf[1] = ftl_data_alloc(n * sizeof(float));
  this->toggle = ftl_data_alloc(sizeof(int));

  fts_signal_bus_reset(o, 0, 0, 0, 0);

  this->n_channels = n_channels;

  fts_dsp_add_object(o);
}

static void
fts_signal_bus_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fts_signal_bus_t *this = (fts_signal_bus_t *)o;

  ftl_data_free(this->buf[0]);
  ftl_data_free(this->buf[1]);
  ftl_data_free(this->toggle);

  fts_dsp_remove_object(o);
}

static fts_status_t
fts_signal_bus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_signal_bus_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_signal_bus_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fts_signal_bus_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put_prologue, fts_signal_bus_put_prologue);

  return fts_Success;
}

void
fts_signal_bus_config(void)
{
  fts_signal_bus_symbol = fts_new_symbol("signal_bus");
  fts_class_install(fts_signal_bus_symbol, fts_signal_bus_instantiate);
  fts_signal_bus_class = fts_class_get_by_name(fts_signal_bus_symbol);

  fts_dsp_declare_function(fts_signal_bus_symbol, fts_signal_bus_toggle_and_zero);
}
