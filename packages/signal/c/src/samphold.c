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

static fts_symbol_t sym_samphold_tilda = 0;

#define INITVAL 1.e30

typedef struct
{
  float val;
  float last;
} samphold_data_t;

typedef struct
{
  fts_object_t _o;
  float val;
  float last;
  ftl_data_t data;
} samphold_t;

static void
ftl_samphold(fts_word_t *argv)
{
  float *in0 = (float *)fts_word_get_pointer(argv + 0);
  float *in1 = (float *)fts_word_get_pointer(argv + 1);
  float *out = (float *)fts_word_get_pointer(argv + 2);
  samphold_data_t *data = (samphold_data_t *)fts_word_get_pointer(argv + 3);
  long int n_tick = fts_word_get_int(argv + 4);
  float last = data->last;
  float val = data->val;
  int i;

  for(i=0; i<n_tick; i++)
    {
      float next = in0[i];

      if (next < last)
	val = in1[i];

      out[i] = val;

      last = next;
    }

  data->last = last;
  data->val = val;
}


static void
samphold_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  samphold_t *this = (samphold_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[5];

  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv + 3, this->data);
  fts_set_int(argv + 4, fts_dsp_get_input_size(dsp, 0));
  fts_dsp_add_function(sym_samphold_tilda, 5, argv);
}


static void
samphold_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  samphold_t *this = (samphold_t *)o;
  samphold_data_t *data = (samphold_data_t *)ftl_data_get_ptr(this->data);

  data->val = fts_get_number_float(at);
}


static void
samphold_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  samphold_t *this = (samphold_t *)o;
  samphold_data_t *data = (samphold_data_t *)ftl_data_get_ptr(this->data);

  data->last = INITVAL;
}


static void
samphold_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  samphold_t *this = (samphold_t *)o;
  samphold_data_t *data;

  this->data = ftl_data_new(samphold_data_t);
  data = (samphold_data_t *)ftl_data_get_ptr(this->data);

  data->last = INITVAL;
  data->val = 0.0;

  fts_dsp_add_object(o);
}

static void
samphold_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  samphold_t *this = (samphold_t *)o;

  ftl_data_free(this->data);
  fts_dsp_remove_object(o);
}

static fts_status_t
samphold_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(samphold_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, samphold_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, samphold_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, samphold_put);
  
  fts_method_define_varargs(cl, 0, fts_s_float, samphold_number);
  fts_method_define_varargs(cl, 0, fts_s_int, samphold_number);
  fts_method_define_varargs(cl, 0, fts_new_symbol("reset"), samphold_reset);
  
  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1);
  fts_dsp_declare_outlet(cl, 0);
  
  return fts_Success;
}

void
signal_samphold_config(void)
{
  sym_samphold_tilda = fts_new_symbol("samphold~");
  fts_dsp_declare_function(sym_samphold_tilda, ftl_samphold);
  
  fts_class_install(sym_samphold_tilda, samphold_instantiate);
}


