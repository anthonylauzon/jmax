/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"


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
  float *in0 = (float *)fts_word_get_ptr(argv + 0);
  float *in1 = (float *)fts_word_get_ptr(argv + 1);
  float *out = (float *)fts_word_get_ptr(argv + 2);
  samphold_state_t *samphold = (samphold_state_t *)fts_word_get_ptr(argv + 3);
  long int n = fts_word_get_long(argv + 4);
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
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv+1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv+2, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv+3, this->samphold_ftl_data);
  fts_set_long  (argv+4, fts_dsp_get_input_size(dsp, 0));

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
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(sigsamphold_t), 2, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_float;
  a[2] = fts_s_float;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigsamphold_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigsamphold_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigsamphold_put, 1, a);
  
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, sigsamphold_number, 1, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sigsamphold_number, 1, a);
  
  fts_method_define(cl, 0, fts_new_symbol("reset"), sigsamphold_reset, 0, a);
  
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


