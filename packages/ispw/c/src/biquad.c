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

#include "filters.h"
#include "biquad.h"

static fts_symbol_t biquad_dsp_function = 0;
static fts_symbol_t biquad_inplace_dsp_function = 0;

typedef struct
{
  fts_object_t head;
  ftl_data_t biquad_state;
  ftl_data_t biquad_coefs;
} biquad_t;

/****************************************
 *
 *  methods
 *
 */

static void
biquad_state_clear(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  biquad_state_t *state = (biquad_state_t *)ftl_data_get_ptr(this->biquad_state);

  state->xnm1 = 0.0;
  state->xnm2 = 0.0;
  state->ynm1 = 0.0;
  state->ynm2 = 0.0;
}

static void
biquad_set_coef_a0(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  biquad_coefs_t *coefs = (biquad_coefs_t *)ftl_data_get_ptr(this->biquad_coefs);

  coefs->a0 = fts_get_number_float(at);
}

static void
biquad_set_coef_a1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  biquad_coefs_t *coefs = (biquad_coefs_t *)ftl_data_get_ptr(this->biquad_coefs);

  coefs->a1 = fts_get_number_float(at);
}

static void
biquad_set_coef_a2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  biquad_coefs_t *coefs = (biquad_coefs_t *)ftl_data_get_ptr(this->biquad_coefs);

  coefs->a2 = fts_get_number_float(at);
}

static void
biquad_set_coef_b1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  biquad_coefs_t *coefs = (biquad_coefs_t *)ftl_data_get_ptr(this->biquad_coefs);

  coefs->b1 = fts_get_number_float(at);
}

static void
biquad_set_coef_b2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  biquad_coefs_t *coefs = (biquad_coefs_t *)ftl_data_get_ptr(this->biquad_coefs);

  coefs->b2 = fts_get_number_float(at);
}

static void
biquad_set_coefs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;

  switch(ac)
    {
    default:
    case 5:
      if(fts_is_number(at + 4))
	biquad_set_coef_b2(o, 0, 0, 1, at + 4);
    case 4:
      if(fts_is_number(at + 3))
	biquad_set_coef_b1(o, 0, 0, 1, at + 3);
    case 3:
      if(fts_is_number(at + 2))
	biquad_set_coef_a2(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	biquad_set_coef_a1(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	biquad_set_coef_a0(o, 0, 0, 1, at + 0);
    case 0:
      break;
    }
}

/****************************************
 *
 *  dsp
 *
 */

void 
ftl_biquad(fts_word_t *argv)
{
  float *x = (float *)fts_word_get_pointer(argv); /* input */
  float *y = (float *)fts_word_get_pointer(argv + 1); /* output */
  biquad_state_t *state = (biquad_state_t *)fts_word_get_pointer(argv + 2);
  biquad_coefs_t *coefs = (biquad_coefs_t *)fts_word_get_pointer(argv + 3);
  int n_tick = fts_word_get_int(argv + 4);
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

  for (i=0; i<n_tick; i++)
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


void 
ftl_biquad_inplace(fts_word_t *argv)
{
  float *xy = (float *)fts_word_get_pointer(argv + 0);
  biquad_state_t *state = (biquad_state_t *)fts_word_get_pointer(argv + 1);
  biquad_coefs_t *coefs = (biquad_coefs_t *)fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv+3);
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

  for (i=0; i<n_tick; i++)
    {
      float xn = xy[i];
      float yn = FILTERS_FP_ONSET(a0 * xn + a1 * xnm1 + a2 * xnm2 - b1 * ynm1 - b2 * ynm2);

      xy[i] = yn;

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

static void
biquad_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[5];


  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
    {
      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_ftl_data(argv+1, this->biquad_state);
      fts_set_ftl_data(argv+2, this->biquad_coefs);
      fts_set_int(argv+3, fts_dsp_get_input_size(dsp, 0));

      biquad_state_clear(o, 0, 0, 0, 0);
      fts_dsp_add_function(biquad_inplace_dsp_function, 4, argv);
    }
  else
    {
      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv+2, this->biquad_state);
      fts_set_ftl_data(argv+3, this->biquad_coefs);
      fts_set_int(argv+4, fts_dsp_get_input_size(dsp, 0));

      biquad_state_clear(o, 0, 0, 0, 0);
      fts_dsp_add_function(biquad_dsp_function, 5, argv);
    }
}

/****************************************
 *
 *  class
 *
 */

static void
biquad_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;

  this->biquad_state = ftl_data_alloc(sizeof(biquad_state_t));
  this->biquad_coefs = ftl_data_alloc(sizeof(biquad_coefs_t));

  biquad_state_clear(o, 0, 0, 0, 0);
  biquad_set_coefs(o, 0, 0, ac, at);

  fts_dsp_add_object(o);
}

static void
biquad_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;

  ftl_data_free(this->biquad_coefs);
  ftl_data_free(this->biquad_state);

  fts_dsp_remove_object(o);
}


static fts_status_t
biquad_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;

  fts_class_init(cl, sizeof(biquad_t), 6, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, biquad_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, biquad_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, biquad_put);
  
  fts_method_define_varargs(cl, 1, fts_s_float, biquad_set_coef_a0);
  fts_method_define_varargs(cl, 2, fts_s_float, biquad_set_coef_a1);
  fts_method_define_varargs(cl, 3, fts_s_float, biquad_set_coef_a2);
  fts_method_define_varargs(cl, 4, fts_s_float, biquad_set_coef_b1);
  fts_method_define_varargs(cl, 5, fts_s_float, biquad_set_coef_b2);
  
  fts_method_define_varargs(cl, 1, fts_s_int, biquad_set_coef_a0);
  fts_method_define_varargs(cl, 2, fts_s_int, biquad_set_coef_a1);
  fts_method_define_varargs(cl, 3, fts_s_int, biquad_set_coef_a2);
  fts_method_define_varargs(cl, 4, fts_s_int, biquad_set_coef_b1);
  fts_method_define_varargs(cl, 5, fts_s_int, biquad_set_coef_b2);
  
  fts_method_define_varargs(cl, 0, fts_s_set, biquad_set_coefs);
  fts_method_define_varargs(cl, 0, fts_s_clear, biquad_state_clear);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  
  return fts_Success;
}

void
biquad_config(void)
{
  biquad_dsp_function = fts_new_symbol("biquad");
  biquad_inplace_dsp_function = fts_new_symbol("biquad_inplace");

  fts_dsp_declare_function(biquad_dsp_function, ftl_biquad);
  fts_dsp_declare_function(biquad_inplace_dsp_function, ftl_biquad_inplace);

  fts_class_install(fts_new_symbol("biquad~"), biquad_instantiate);
}
