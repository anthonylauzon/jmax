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
 * Authors: Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "iir.h"

#if defined(USE_FP_ONSET)
#define FILTERS_FP_ONSET(c) (1e-37 + (c))
#else
#define FILTERS_FP_ONSET(c) (c)
#endif

static fts_symbol_t sym_iir[5] = {0, 0, 0, 0, 0};

typedef struct
{
  fts_dsp_object_t head;
  ftl_data_t state;
  ftl_data_t coefs;  
  long n_order;
} iir_t;

static void
iir_clear(fts_object_t *o, int inlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  float *state = (float *)ftl_data_get_ptr(this->state);
  int i;

  for(i=0; i<this->n_order; i++)
    state[i] = 0.0;
}

static void
iir_set_coef(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  float *coefs = (float *)ftl_data_get_ptr(this->coefs);

  coefs[i-1] = fts_get_float_arg(ac, at, 0, 0.0f);
}

/****************************************
 *
 *  dsp
 *
 */

static void
iir_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  fts_atom_t argv[6];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  iir_clear(o, 0, 0, 0, 0);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv + 2, this->state);
  fts_set_ftl_data(argv + 3, this->coefs);
  fts_set_int(argv + 4, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function(sym_iir[this->n_order], 5, argv);
}

/* y(n) = x(n) - b1 * y(n-1) */
void
ftl_iir_1(fts_word_t *argv)
{
  float *state;
  float *coefs;
  int n_tick;
  float *x, *y;
  float b1;
  float ynm1;
  float ynp0, ynp1, ynp2, ynp3;
  int n;

  x = (float *)fts_word_get_pointer(argv); /* in0 */
  y = (float *)fts_word_get_pointer(argv+1); /* out0 */
  state = (float *)fts_word_get_pointer(argv+2);
  coefs = (float *)fts_word_get_pointer(argv+3);
  n_tick = fts_word_get_int(argv+4);

  ynm1 = state[0]; /* y(n-1) */
  b1 = coefs[0]; 

  ynp0 = FILTERS_FP_ONSET(x[0] - b1 * ynm1); /* y(n) */
  ynp1 = FILTERS_FP_ONSET(x[1] - b1 * ynp0); /* y(n+1) */
  ynp2 = FILTERS_FP_ONSET(x[2] - b1 * ynp1); /* y(n+2) */
  ynp3 = FILTERS_FP_ONSET(x[3] - b1 * ynp2); /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = FILTERS_FP_ONSET(x[n+0] - b1 * ynp3); /* y(n) */
    y[n-3] = ynp1;
    ynp1 = FILTERS_FP_ONSET(x[n+1] - b1 * ynp0); /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = FILTERS_FP_ONSET(x[n+2] - b1 * ynp1); /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = FILTERS_FP_ONSET(x[n+3] - b1 * ynp2); /* y(n+3) */
  }

  state[0] = ynp3; /* y(n-1) */

  y[n_tick-4] = ynp0;
  y[n_tick-3] = ynp1;
  y[n_tick-2] = ynp2;
  y[n_tick-1] = ynp3;
}


/* y(n) = x(n) - b1 * y(n-1) - b2 * y(n-2) */
void
ftl_iir_2(fts_word_t *argv)
{
  float *state;
  float *coefs;
  int n_tick;
  float *x, *y;
  float ynm2, ynm1;
  float b1, b2;
  float ynp0, ynp1, ynp2, ynp3;
  int n;

  x = (float *)fts_word_get_pointer(argv); /* in0 */
  y = (float *)fts_word_get_pointer(argv+1); /* out0 */
  state = (float *)fts_word_get_pointer(argv+2);
  coefs = (float *)fts_word_get_pointer(argv+3);
  n_tick = fts_word_get_int(argv+4);

  ynm2 = state[0]; /* y(n-2) */
  ynm1 = state[1]; /* y(n-1) */
  b1 = coefs[0]; 
  b2 = coefs[1]; 

  ynp0 = FILTERS_FP_ONSET(x[0] - b1 * ynm1 - b2 * ynm2); /* y(n) */
  ynp1 = FILTERS_FP_ONSET(x[1] - b1 * ynp0 - b2 * ynm1); /* y(n+1) */
  ynp2 = FILTERS_FP_ONSET(x[2] - b1 * ynp1 - b2 * ynp0); /* y(n+2) */
  ynp3 = FILTERS_FP_ONSET(x[3] - b1 * ynp2 - b2 * ynp1); /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = FILTERS_FP_ONSET(x[n+0] - b1 * ynp3 - b2 * ynp2); /* y(n) */
    y[n-3] = ynp1;
    ynp1 = FILTERS_FP_ONSET(x[n+1] - b1 * ynp0 - b2 * ynp3); /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = FILTERS_FP_ONSET(x[n+2] - b1 * ynp1 - b2 * ynp0); /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = FILTERS_FP_ONSET(x[n+3] - b1 * ynp2 - b2 * ynp1); /* y(n+3) */
  }

  y[n_tick-4] = ynp0;
  y[n_tick-3] = ynp1;
  y[n_tick-2] = state[0] = ynp2; /* y(n-2) */
  y[n_tick-1] = state[1] = ynp3; /* y(n-1) */
}

/* y(n) = x(n) - b1 * y(n-1) - b2 * y(n-2) - b3 * y(n-3) */
void
ftl_iir_3(fts_word_t *argv)
{
  float *state;
  float *coefs;
  int n_tick;
  float *x, *y;
  float ynm3, ynm2, ynm1;
  float b1, b2, b3;
  float ynp0, ynp1, ynp2, ynp3;
  int n;

  x = (float *)fts_word_get_pointer(argv); /* in0 */
  y = (float *)fts_word_get_pointer(argv+1); /* out0 */
  state = (float *)fts_word_get_pointer(argv+2);
  coefs = (float *)fts_word_get_pointer(argv+3);
  n_tick = fts_word_get_int(argv+4);

  ynm3 = state[0]; /* y(n-3) */
  ynm2 = state[1]; /* y(n-2) */
  ynm1 = state[2]; /* y(n-1) */
  b1 = coefs[0]; 
  b2 = coefs[1]; 
  b3 = coefs[2]; 

  ynp0 = FILTERS_FP_ONSET(x[0] - b1 * ynm1 - b2 * ynm2 - b3 * ynm3); /* y(n) */
  ynp1 = FILTERS_FP_ONSET(x[1] - b1 * ynp0 - b2 * ynm1 - b3 * ynm2); /* y(n+1) */
  ynp2 = FILTERS_FP_ONSET(x[2] - b1 * ynp1 - b2 * ynp0 - b3 * ynm1); /* y(n+2) */
  ynp3 = FILTERS_FP_ONSET(x[3] - b1 * ynp2 - b2 * ynp1 - b3 * ynp0); /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = FILTERS_FP_ONSET(x[n+0] - b1 * ynp3 - b2 * ynp2 - b3 * ynp1); /* y(n) */
    y[n-3] = ynp1;
    ynp1 = FILTERS_FP_ONSET(x[n+1] - b1 * ynp0 - b2 * ynp3 - b3 * ynp2); /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = FILTERS_FP_ONSET(x[n+2] - b1 * ynp1 - b2 * ynp0 - b3 * ynp3); /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = FILTERS_FP_ONSET(x[n+3] - b1 * ynp2 - b2 * ynp1 - b3 * ynp0); /* y(n+3) */
  }

  y[n_tick-4] = ynp0;
  y[n_tick-3] = state[0] = ynp1; /* y(n-3) */
  y[n_tick-2] = state[1] = ynp2; /* y(n-2) */
  y[n_tick-1] = state[2] = ynp3; /* y(n-1) */
}

/* y(n) = x(n) - b1 * y(n-1) - b2 * y(n-2) - b3 * y(n-3) - b4 * y(n-4) */
void
ftl_iir_4(fts_word_t *argv)
{
  float *state;
  float *coefs;
  int n_tick;
  float *x, *y;
  float ynm4, ynm3, ynm2, ynm1;
  float b1, b2, b3, b4;
  float ynp0, ynp1, ynp2, ynp3;
  int n;

  x = (float *)fts_word_get_pointer(argv); /* in0 */
  y = (float *)fts_word_get_pointer(argv+1); /* out0 */
  state = (float *)fts_word_get_pointer(argv+2);
  coefs = (float *)fts_word_get_pointer(argv+3);
  n_tick = fts_word_get_int(argv+4);

  ynm4 = state[0]; /* y(n-4) */
  ynm3 = state[1]; /* y(n-3) */
  ynm2 = state[2]; /* y(n-2) */
  ynm1 = state[3]; /* y(n-1) */
  b1 = coefs[0]; 
  b2 = coefs[1]; 
  b3 = coefs[2]; 
  b4 = coefs[3]; 

  ynp0 = FILTERS_FP_ONSET(x[0] - b1 * ynm1 - b2 * ynm2 - b3 * ynm3 - b4 * ynm4); /* y(n) */
  ynp1 = FILTERS_FP_ONSET(x[1] - b1 * ynp0 - b2 * ynm1 - b3 * ynm2 - b4 * ynm3); /* y(n+1) */
  ynp2 = FILTERS_FP_ONSET(x[2] - b1 * ynp1 - b2 * ynp0 - b3 * ynm1 - b4 * ynm2); /* y(n+2) */
  ynp3 = FILTERS_FP_ONSET(x[3] - b1 * ynp2 - b2 * ynp1 - b3 * ynp0 - b4 * ynm1); /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = FILTERS_FP_ONSET(x[n+0] - b1 * ynp3 - b2 * ynp2 - b3 * ynp1 - b4 * ynp0); /* y(n) */
    y[n-3] = ynp1;
    ynp1 = FILTERS_FP_ONSET(x[n+1] - b1 * ynp0 - b2 * ynp3 - b3 * ynp2 - b4 * ynp1); /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = FILTERS_FP_ONSET(x[n+2] - b1 * ynp1 - b2 * ynp0 - b3 * ynp3 - b4 * ynp2); /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = FILTERS_FP_ONSET(x[n+3] - b1 * ynp2 - b2 * ynp1 - b3 * ynp0 - b4 * ynp3); /* y(n+3) */
  }

  y[n_tick-4] = state[0] = ynp0; /* y(n-4) */
  y[n_tick-3] = state[1] = ynp1; /* y(n-3) */
  y[n_tick-2] = state[2] = ynp2; /* y(n-2) */
  y[n_tick-1] = state[3] = ynp3; /* y(n-1) */
}

/* y(n) = x(n) - b1 * y(n-1) - b2 * y(n-2) - b3 * y(n-3) - ... - bN * y(n-N) */
void
ftl_iir_n(fts_word_t *argv)
{
  float *state;
  float *coefs;
  int n_tick;
  int n_order;
  float *x, *y;
  int i, n;

  x = (float *)fts_word_get_pointer(argv); /* in0 */
  y = (float *)fts_word_get_pointer(argv + 1); /* out0 */
  state = (float *)fts_word_get_pointer(argv + 2);
  coefs = (float *)fts_word_get_pointer(argv + 3);
  n_order = fts_word_get_int(argv + 4); /* N */
  n_tick = fts_word_get_int(argv + 5);

  for(n=0; n<n_order; n++)
    {
      y[n] = FILTERS_FP_ONSET(x[n]);

      for(i=0; i<n_order-n; i++)
	y[n] -= coefs[i] * state[n_order-1-i];

      for( ; i<n_order; i++)
	y[n] -= coefs[i] * y[n-1-i];
    }

  for( ; n<n_tick; n++)
    {
      y[n] = FILTERS_FP_ONSET(x[n]);

      for(i=0; i<n_order; i++)
	y[n] -= coefs[i] * y[n-1-i];
    }

  for(i=0; i<n_order; i++)
    state[i] = y[i];
}

/****************************************
 *
 *  class
 *
 */

static void
iir_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;

  if(ac >= 0 && ac <= 4)
    {
      int n_order = ac;
      float *coefs;
      int i;
      
      if(n_order < 1)
	n_order = 1;

      this->n_order = n_order;
      this->coefs = ftl_data_alloc(sizeof(float) * n_order);
      this->state = ftl_data_alloc(sizeof(float) * n_order);
      
      coefs = (float *)ftl_data_get_ptr(this->coefs);

      for(i=0; i<n_order; i++)
	{
	  if(i < ac && fts_is_number(at + i))
	    coefs[i] = fts_get_float_arg(ac, at, i, 0.0f);
	  else
	    coefs[i] = 0.0;
	}
  
      fts_dsp_object_init((fts_dsp_object_t *)o);
      fts_object_set_inlets_number(o, n_order + 1);
    }
  else
    fts_object_error(o, "max 4th order");
}


static void
iir_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;

  if(this->n_order > 0)
    {
      ftl_data_free(this->coefs);
      ftl_data_free(this->state);
    }

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
iir_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(iir_t), iir_init, iir_delete);

  fts_class_message_varargs(cl, fts_s_put, iir_put);
  
  fts_class_message_varargs(cl, fts_s_clear, iir_clear);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  
  fts_class_inlet_int(cl, 1, iir_set_coef);
  fts_class_inlet_float(cl, 1, iir_set_coef);
}

void
signal_iir_config(void)
{ 
  sym_iir[0] = fts_new_symbol("iir~");
  sym_iir[1] = fts_new_symbol("iir~ (1 coef)");
  sym_iir[2] = fts_new_symbol("iir~ (2 coef)");
  sym_iir[3] = fts_new_symbol("iir~ (3 coef)");
  sym_iir[4] = fts_new_symbol("iir~ (4 coef)");

  fts_dsp_declare_function(sym_iir[1], ftl_iir_1);
  fts_dsp_declare_function(sym_iir[2], ftl_iir_2);
  fts_dsp_declare_function(sym_iir[3], ftl_iir_3);
  fts_dsp_declare_function(sym_iir[4], ftl_iir_4);

  fts_class_install(sym_iir[0], iir_instantiate);
}
