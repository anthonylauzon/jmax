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


static fts_symbol_t sigsamphold_function = 0;

#define INITVAL 1.e30

typedef struct
{
  float val;
  float last;
} samphold_state_t;

typedef struct
{
  fts_object_t _o;

  float val;
  float last;
  ftl_data_t samphold_ftl_data;
} sigsamphold_t;

static void
ftl_samphold(fts_word_t *argv)
{
  float *in0 = (float *)fts_word_get_pointer(argv + 0);
  float *in1 = (float *)fts_word_get_pointer(argv + 1);
  float *out = (float *)fts_word_get_pointer(argv + 2);
  samphold_state_t *samphold = (samphold_state_t *)fts_word_get_pointer(argv + 3);
  long int n = fts_word_get_int(argv + 4);
  float last = samphold->last;
  float val = samphold->val;
  float next;

  while (n--)
    {
      next = *(in0++);
      if (next < last) val = *in1;
      in1++;
      last = next;
      *out++ = val;
    }

  samphold->last = last;
  samphold->val = val;
}


static void
sigsamphold_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigsamphold_t *this = (sigsamphold_t *)o;
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer_arg(ac, at, 0, 0);

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv+1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv+2, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv+3, this->samphold_ftl_data);
  fts_set_int  (argv+4, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(sigsamphold_function, 5, argv);
}


static void
sigsamphold_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigsamphold_t *this = (sigsamphold_t *)o;
  float f = (float) fts_get_number_float(at);

  ftl_data_set(samphold_state_t, this->samphold_ftl_data, val, &f);
}


static void
sigsamphold_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigsamphold_t *this = (sigsamphold_t *)o;
  const float f = INITVAL;

  ftl_data_set(samphold_state_t, this->samphold_ftl_data, last, &f);
}


static void
sigsamphold_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigsamphold_t *this = (sigsamphold_t *)o;
  float f;

  this->samphold_ftl_data = ftl_data_new(samphold_state_t);

  f = INITVAL;
  ftl_data_set(samphold_state_t, this->samphold_ftl_data, last, &f);

  f = 0.0f;
  ftl_data_set(samphold_state_t, this->samphold_ftl_data, val, &f);

  dsp_list_insert(o);
}

static void
sigsamphold_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sigsamphold_t *this = (sigsamphold_t *)o;

  ftl_data_free(this->samphold_ftl_data);
  dsp_list_remove(o);
}

static fts_status_t
sigsamphold_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sigsamphold_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sigsamphold_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sigsamphold_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigsamphold_put);
  
  fts_method_define_varargs(cl, 0, fts_s_float, sigsamphold_number);
  fts_method_define_varargs(cl, 0, fts_s_int, sigsamphold_number);
  fts_method_define_varargs(cl, 0, fts_new_symbol("reset"), sigsamphold_reset);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1);
  dsp_sig_outlet(cl, 0);
  
  sigsamphold_function = fts_new_symbol("sigsamphold");
  dsp_declare_function(sigsamphold_function, ftl_samphold);
  
  return fts_Success;
}

void
sigsamphold_config(void)
{
  fts_class_install(fts_new_symbol("samphold~"),sigsamphold_instantiate);
}


