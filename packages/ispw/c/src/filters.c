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
 */

#include <fts/fts.h>
#include <ftsconfig.h>
#include "filters.h"

/*************************************
 *
 *  biquad
 *
 */

void 
compute_biquad(float *x, float *y, biquad_state_t *state, biquad_coefs_t *coefs, int n)
{
  float xnm1 = state->xnm1; /* x(n-1) */
  float xnm2 = state->xnm2; /* x(n-2) */
  float ynm1 = state->ynm1; /* y(n-1) */
  float ynm2 = state->ynm2; /* y(n-2) */
  float a0 = coefs->a0; 
  float a1 = coefs->a1; 
  float a2 = coefs->a2; 
  float b1 = coefs->b1; 
  float b2 = coefs->b2; 
  int i;

  for (i=0; i<n; i++)
    {
      float xn = x[i];
      float yn = FILTERS_FP_ONSET(a0 * xn + a1 * xnm1 + a2 * xnm2 - b1 * ynm1 - b2 * ynm2);

      y[i] = yn;

      xnm2 = xnm1;
      xnm1 = xn;
      ynm2 = ynm1;
      ynm1 = yn;
    }

  state->xnm2 = xnm2;
  state->xnm1 = xnm1;
  state->ynm2 = ynm2;
  state->ynm1 = ynm1;
}

/*************************************
 *
 *  2p2z
 *
 */

static void sig2p2z_state_clear(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);

static fts_symbol_t sig2p2z_function = 0;
static fts_symbol_t sig2p2z_64_function = 0;
static fts_symbol_t sig2p2z_64_1ops_function = 0;

typedef struct ctlf2p2z		/* control structure for 2p2z~ */
{
  float state1; /* previous output */
  float state2; /* previous previous output */
  float gain;   /* these three are c0, c1, c2 in help window */
  float fbcoef1;
  float fbcoef2;
  float ffcoef0; /* these are d0, d1, d2 in help window */
  float ffcoef1;
  float ffcoef2;

} ctlf2p2z_t;

typedef struct
{
  fts_object_t _o;
  ftl_data_t ftl_data;
} sig2p2z_t;

static void
sig2p2z_state_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  const float zero = 0.0;

  ftl_data_set(ctlf2p2z_t, this->ftl_data, state1, &zero);
  ftl_data_set(ctlf2p2z_t, this->ftl_data, state2, &zero);
}

static void
ftl_2p2z(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv + 0);
  float *out = (float *)fts_word_get_pointer(argv + 1);
  ctlf2p2z_t *x = (ctlf2p2z_t *)fts_word_get_pointer(argv + 2);
  long int n = fts_word_get_int(argv + 3);
  int i;
  float ym1, ym2, ym0;
  float a, b, g;
  float ff0, ff1, ff2;

  ym1 = x->state1;
  ym2 = x->state2;

  a = x->fbcoef1;
  b = x->fbcoef2;
  g = x->gain;

  ff0 = x->ffcoef0;
  ff1 = x->ffcoef1;
  ff2 = x->ffcoef2;

  for (i = 0; i < n; i++)
    {
      ym0 = g * in[i] + a * ym1 + b * ym2;
      out[i] = ff0 * ym0 + ff1 * ym1 + ff2 * ym2;
      ym2 = ym1;
      ym1 = ym0;
    }

  x->state1 = ym1;
  x->state2 = ym2;
}


static void 
ftl_64_2p2z(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  ctlf2p2z_t *x = (ctlf2p2z_t *)fts_word_get_pointer(argv + 2);
  int i;
  float ym1, ym2, ym0;
  float a, b, g;
  float ff0, ff1, ff2;

  ym1 = x->state1;
  ym2 = x->state2;

  a = x->fbcoef1;
  b = x->fbcoef2;
  g = x->gain;

  ff0 = x->ffcoef0;
  ff1 = x->ffcoef1;
  ff2 = x->ffcoef2;

  for (i = 0; i < 64; i ++)
    {
      ym0 = g * in[i] + a * ym1 + b * ym2;
      out[i] = ff0 * ym0 + ff1 * ym1 + ff2 * ym2;
      ym2 = ym1;
      ym1 = ym0;
    }

  x->state1 = ym1;
  x->state2 = ym2;
}

static void 
ftl_64_1ops_2p2z(fts_word_t *argv)
{
  float * restrict vec = (float *)fts_word_get_pointer(argv + 0);
  ctlf2p2z_t * restrict x = (ctlf2p2z_t *)fts_word_get_pointer(argv + 1);
  int i;
  float ym1, ym2, ym0;
  float a, b, g;
  float ff0, ff1, ff2;

  ym1 = x->state1;
  ym2 = x->state2;

  a = x->fbcoef1;
  b = x->fbcoef2;
  g = x->gain;

  ff0 = x->ffcoef0;
  ff1 = x->ffcoef1;
  ff2 = x->ffcoef2;

  for (i = 0; i < 64; i ++)
    {
      ym0 = g * vec[i] + a * ym1 + b * ym2;
      vec[i] = ff0 * ym0 + ff1 * ym1 + ff2 * ym2;
      ym2 = ym1;
      ym1 = ym0;
    }

  x->state1 = ym1;
  x->state2 = ym2;
}


static void
sig2p2z_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  fts_atom_t argv[4];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  sig2p2z_state_clear(o, 0, 0, 0, 0);

  if (fts_dsp_get_input_size(dsp, 0) == 64)
    {
      if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	{
	  fts_set_symbol(argv + 0,   fts_dsp_get_input_name(dsp, 0));
	  fts_set_ftl_data(argv + 1, this->ftl_data);
	  fts_dsp_add_function(sig2p2z_64_1ops_function, 2, argv);
	}
      else
	{
	  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_ftl_data(argv+2, this->ftl_data);
	  fts_dsp_add_function(sig2p2z_64_function, 3, argv);
	}
    }
  else
    {
      fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv+2, this->ftl_data);

      fts_set_int(argv+3, fts_dsp_get_input_size(dsp, 0));
      fts_dsp_add_function(sig2p2z_function, 4, argv);
    }
}

static void
sig2p2z_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  float f;

  f = fts_get_float_arg(ac, at, 0, 0.0f);
  ftl_data_set(ctlf2p2z_t, this->ftl_data, gain, &f);

  f = fts_get_float_arg(ac, at, 1, 0.0f);
  ftl_data_set(ctlf2p2z_t, this->ftl_data, fbcoef1, &f);

  f = fts_get_float_arg(ac, at, 2, 0.0f);
  ftl_data_set(ctlf2p2z_t, this->ftl_data, fbcoef2, &f);

  f = fts_get_float_arg(ac, at, 3, 0.0f);
  ftl_data_set(ctlf2p2z_t, this->ftl_data, ffcoef0, &f);

  f = fts_get_float_arg(ac, at, 4, 0.0f);
  ftl_data_set(ctlf2p2z_t, this->ftl_data, ffcoef1, &f);

  f = fts_get_float_arg(ac, at, 5, 0.0f);
  ftl_data_set(ctlf2p2z_t, this->ftl_data, ffcoef2, &f);
}

static void
sig2p2z_gain_c0(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  ftl_data_set(ctlf2p2z_t, this->ftl_data, gain, &f);
}

static void
sig2p2z_fbcoef1_c1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  float f =  fts_get_float_arg(ac, at, 0, 0.0f);

  ftl_data_set(ctlf2p2z_t, this->ftl_data, fbcoef1, &f);
}

static void
sig2p2z_fbcoef2_c2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  float f =  fts_get_float_arg(ac, at, 0, 0.0f);

  ftl_data_set(ctlf2p2z_t, this->ftl_data, fbcoef2, &f);
}

static void
sig2p2z_ffcoef0_c3(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  ftl_data_set(ctlf2p2z_t, this->ftl_data, ffcoef0, &f);
}

static void
sig2p2z_ffcoef1_c4(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  ftl_data_set(ctlf2p2z_t, this->ftl_data, ffcoef1, &f);
}

static void
sig2p2z_ffcoef2_c5(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  ftl_data_set(ctlf2p2z_t, this->ftl_data, ffcoef2, &f);
}

static void
sig2p2z_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;

  this->ftl_data = ftl_data_new(ctlf2p2z_t);

  sig2p2z_set(o, winlet, s, ac, at);

  fts_dsp_add_object(o);
}

static void
sig2p2z_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;

  ftl_data_free(this->ftl_data);
  fts_dsp_remove_object(o);
}

static void
sig2p2z_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sig2p2z_t), sig2p2z_init, sig2p2z_delete);

  fts_class_message_varargs(cl, fts_s_put, sig2p2z_put);
  
  fts_class_message_varargs(cl, fts_s_clear, sig2p2z_state_clear);

  fts_class_message_varargs(cl, fts_s_set, sig2p2z_set);
  
  fts_class_inlet_float(cl, 1, sig2p2z_gain_c0);
  fts_class_inlet_float(cl, 2, sig2p2z_fbcoef1_c1);
  fts_class_inlet_float(cl, 3, sig2p2z_fbcoef2_c2);
  fts_class_inlet_float(cl, 4, sig2p2z_ffcoef0_c3);
  fts_class_inlet_float(cl, 5, sig2p2z_ffcoef1_c4);
  fts_class_inlet_float(cl, 6, sig2p2z_ffcoef2_c5);

  fts_class_inlet_int(cl, 1, sig2p2z_gain_c0);
  fts_class_inlet_int(cl, 2, sig2p2z_fbcoef1_c1);
  fts_class_inlet_int(cl, 3, sig2p2z_fbcoef2_c2);
  fts_class_inlet_int(cl, 4, sig2p2z_ffcoef0_c3);
  fts_class_inlet_int(cl, 5, sig2p2z_ffcoef1_c4);
  fts_class_inlet_int(cl, 6, sig2p2z_ffcoef2_c5);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  }

static void 
sig2p2z_config(void)
{
  sig2p2z_function = fts_new_symbol("2p2z");
  fts_dsp_declare_function(sig2p2z_function, ftl_2p2z);

  sig2p2z_64_function = fts_new_symbol("2p2z_64");
  fts_dsp_declare_function(sig2p2z_64_function, ftl_64_2p2z);

  sig2p2z_64_1ops_function = fts_new_symbol("2p2z_64_1ops");
  fts_dsp_declare_function(sig2p2z_64_1ops_function, ftl_64_1ops_2p2z);
}
