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

static fts_symbol_t sym_zerocross_tilda = 0;

typedef struct
{
  int count; /* zero crossing counter*/
  float last; /* last stored samples */
} zerocross_data_t;

typedef struct
{
  fts_object_t o;
  ftl_data_t data;
} zerocross_t;

static void
ftl_zerocross(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv + 0);
  zerocross_data_t *data = (zerocross_data_t *)fts_word_get_pointer(argv + 1);
  long n_tick = (long)fts_word_get_int(argv + 2);
  float last = data->last;
  int count = data->count;
  int last_sign = (last <= 0.0f);
  int i;

  for(i=0; i<n_tick; i++)
    {
      int current_sign = (in[i] <= 0.0f);

      count += last_sign ^ current_sign;
      last_sign = current_sign;
    }

  data->last = in[n_tick - 1];
  data->count = count;
}

static void
zerocross_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[3];

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_ftl_data(argv + 3, this->data);
  fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function(sym_zerocross_tilda, 3, argv);
}

static void
zerocross_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;
  zerocross_data_t *data = (zerocross_data_t *)ftl_data_get_ptr(this->data);

  fts_outlet_int(o, 0, data->count);
  data->count = 0;
}

static void
zerocross_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;
  zerocross_data_t *data;

  this->data = ftl_data_new(zerocross_data_t);
  data = (zerocross_data_t *)ftl_data_get_ptr(this->data);

  data->count = 0;
  data->last = 0.0;

  fts_dsp_add_object(o); 
}


static void
zerocross_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}


static void
zerocross_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(zerocross_t), zerocross_init, zerocross_delete);

  fts_class_message_varargs(cl, fts_s_put, zerocross_put);
  fts_class_message_varargs(cl, fts_s_bang, zerocross_bang);

  fts_dsp_declare_inlet(cl, 0);
  fts_class_outlet_int(cl, 0);
}

void
signal_zerocross_config(void)
{
  sym_zerocross_tilda = fts_new_symbol("zerocross~");
  fts_dsp_declare_function(sym_zerocross_tilda, ftl_zerocross);

  fts_class_install(sym_zerocross_tilda, zerocross_instantiate);
}
