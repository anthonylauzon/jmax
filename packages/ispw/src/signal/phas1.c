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

/******************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_wrapper_t phase;
  double incr;
} phasor_state_t;

typedef struct 
{
  fts_object_t _o;
  ftl_data_t state;
} phasor_t;

static void
phasor_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  phasor_state_t *state;

  this->state = ftl_data_new(phasor_state_t);
  state = ftl_data_get_ptr(this->state);

  fts_wrapper_frac_set(&state->phase, 0.0);

  dsp_list_insert(o);
}

static void
phasor_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;

  ftl_data_free(this->state);
  dsp_list_remove(o);
}

/******************************************************************
 *
 *  dsp
 *
 */

static void
phasor_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  phasor_state_t *state = ftl_data_get_ptr(this->state);
  fts_atom_t argv[4];
  double incr;

  state->incr = (double)1.0 / (double)fts_dsp_get_input_srate(dsp, 0);

  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
    {
      /* Use the inplace version */

      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_ftl_data(argv+1, this->state);
      fts_set_long(argv+2, fts_dsp_get_input_size(dsp, 0));

      dsp_add_funcall(phasor_inplace_function, 3, argv);
    }
  else
    {
      /* standard code */
      fts_set_symbol( argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol( argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data( argv+2, this->state);
      fts_set_long( argv+3, fts_dsp_get_input_size(dsp, 0));

      dsp_add_funcall(phasor_function, 4, argv);
    }
}

static void ftl_phasor(fts_word_t *argv)
{
  float * restrict freq = (float *)  fts_word_get_ptr(argv + 0);
  float * restrict out  = (float *)  fts_word_get_ptr(argv + 1);
  phasor_state_t * restrict state = (phasor_state_t *) fts_word_get_ptr(argv + 2);
  int n = fts_word_get_long(argv + 3);
  double incr = state->incr;
  fts_wrapper_t phi = state->phase;
  int i;

  for(i=0; i<n; i++)
    {
      float this_freq = freq[i];
      out[i] = fts_wrapper_frac_get_wrap(&phi);
      fts_wrapper_incr(&phi, this_freq * incr);
    }

  state->phase = phi;
}

static void ftl_inplace_phasor(fts_word_t *argv)
{
  float * restrict sig = (float *)  fts_word_get_ptr(argv + 0);
  phasor_state_t * restrict state = (phasor_state_t *) fts_word_get_ptr(argv + 1);
  long int n = fts_word_get_long(argv + 2);
  double incr = state->incr;
  fts_wrapper_t phi = state->phase;
  int i;

  for(i=0; i<n; i++)
    {
      float this_freq = sig[i];
      sig[i] = fts_wrapper_frac_get_wrap(&phi);
      fts_wrapper_incr(&phi, this_freq * incr);
    }

  state->phase = phi;
}

/******************************************************************
 *
 *  user methods
 *
 */

static void
phasor_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post("phasor: phase print not implemented\n");
}


static void
phasor_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  phasor_state_t *state = ftl_data_get_ptr(this->state);
  double f = (double) fts_get_number(at);

  fts_wrapper_frac_set(&state->phase, f);
}

/******************************************************************
 *
 *  class
 *
 */

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
  fts_metaclass_create(fts_new_symbol("phasor~"), phasor_instantiate, fts_always_equiv);
}
