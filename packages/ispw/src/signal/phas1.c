/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"


static fts_symbol_t phasor_function = 0;
static fts_symbol_t phasor_inplace_function = 0;

typedef struct 
{
  double phase;
  float fconv;
} phasor_state_t;



static void ftl_phasor(fts_word_t *argv)
{
  float * restrict freq = (float *)  fts_word_get_ptr(argv + 0);
  float * restrict out  = (float *)  fts_word_get_ptr(argv + 1);
  phasor_state_t * restrict x = (phasor_state_t *) fts_word_get_ptr(argv + 2);
  int n = fts_word_get_long(argv + 3);
  int phase;
  float fconv;
  int i;

  phase = 0x7fffffffL * x->phase;
  fconv = x->fconv * 0x7fffffffL;

  for (i = 0; i < n ; i ++)
    {
      phase = (phase + (int)(fconv * freq[i])) & 0x7fffffffL;
      out[i] = (1.0f/0x7fffffffL) * phase;
    }

  x->phase = (1.0f/0x7fffffffL) * phase;
}

static void ftl_inplace_phasor(fts_word_t *argv)
{
  float * restrict sig = (float *)  fts_word_get_ptr(argv + 0);
  phasor_state_t * restrict x = (phasor_state_t *) fts_word_get_ptr(argv + 1);
  long int n = fts_word_get_long(argv + 2);
  int phase;
  float fconv;
  int i;

  phase = 0x7fffffffL * x->phase;
  fconv = x->fconv * 0x7fffffffL;

  for (i = 0; i < n ; i ++)
    {
      phase = (phase + (int)(fconv * sig[i])) & 0x7fffffffL;
      sig[i] = (1.0f/0x7fffffffL) * phase;
    }

  x->phase = (1.0f/0x7fffffffL) * phase;
}


typedef struct 
{
  fts_object_t _o;

  ftl_data_t phasor_ftl_data;
} phasor_t;


static void
phasor_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[4];
  float fconv;
  const double zero = 0.0f;

  fconv = 1.0f / fts_dsp_get_input_srate(dsp, 0);

  ftl_data_set(phasor_state_t, this->phasor_ftl_data, fconv, &fconv);

  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
    {
      /* Use the inplace version */

      fts_set_symbol( argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_ftl_data( argv+1, this->phasor_ftl_data);
      fts_set_long( argv+2, fts_dsp_get_input_size(dsp, 0));

      dsp_add_funcall(phasor_inplace_function, 3, argv);
    }
  else
    {
      /* standard code */
      fts_set_symbol( argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol( argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data( argv+2, this->phasor_ftl_data);
      fts_set_long( argv+3, fts_dsp_get_input_size(dsp, 0));

      dsp_add_funcall(phasor_function, 4, argv);
    }
}


static void
phasor_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  const double zero = 0.0f;

  this->phasor_ftl_data = ftl_data_new(phasor_state_t);
  ftl_data_set(phasor_state_t, this->phasor_ftl_data, phase, &zero);

  dsp_list_insert(o);		/* just put object in list */
}


static void
phasor_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post("phasor: phase print not implemented\n");
}


static void
phasor_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  double f = (double) fts_get_number(at);

  ftl_data_set(phasor_state_t, this->phasor_ftl_data, phase, &f);
}


static void
phasor_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;

  ftl_data_free(this->phasor_ftl_data);
  dsp_list_remove(o);
}


static fts_status_t
phasor_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(phasor_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, phasor_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, phasor_delete, 0, a);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, phasor_put, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, phasor_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, phasor_number, 1, a);

  fts_method_define(cl, 0, fts_s_print, phasor_print, 0, a);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  phasor_function = fts_new_symbol("phasor");
  dsp_declare_function(phasor_function, ftl_phasor);

  phasor_inplace_function = fts_new_symbol("phasor_inplace");
  dsp_declare_function(phasor_inplace_function, ftl_inplace_phasor);

  return fts_Success;
}

void
phasor_config(void)
{
  fts_metaclass_create(fts_new_symbol("phasor~"),phasor_instantiate, fts_always_equiv);
}
