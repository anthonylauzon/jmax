/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* sigapass3 - 2ns order all-pass style (like wahwah) ---   
zack settel 11/93  Ported to the new fts by mdc */

#include <math.h> 

#include "fts.h"

#include "biquad.h"

static fts_symbol_t sigapass3_function = 0;

/****************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t obj;
  ftl_data_t biquad_state;
  ftl_data_t biquad_coefs;
} sigapass3_t;

static void
sigapass3_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigapass3_t *this = (sigapass3_t *)o;
  float zero = 0.0f;
  float one = 1.0f;

  this->biquad_state = ftl_data_new(biquad_df1_state_t);
  this->biquad_coefs = ftl_data_new(biquad_coefs_t);

  ftl_data_set(biquad_coefs_t, this->biquad_coefs, a0, &zero);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, a1, &zero);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, a2, &one);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, b1, &zero);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, b2, &zero);

  dsp_list_insert(o); /* just put object in list */
}


static void
sigapass3_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigapass3_t *this = (sigapass3_t *)o;

  ftl_data_free(this->biquad_coefs);
  ftl_data_free(this->biquad_state);

  dsp_list_remove(o);
}

/****************************************
 *
 *  dsp
 *
 */

/* radian frequency clips to from 0 to 2/3.14159  */
/* updates coefs twice per vector */

static void
sigapass3_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigapass3_t *this = (sigapass3_t *)o;
  fts_atom_t argv[8];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  float conv;
  float zero = 0.0;

  ftl_data_set(biquad_df1_state_t, this->biquad_state, xnm1, &zero);
  ftl_data_set(biquad_df1_state_t, this->biquad_state, xnm2, &zero);
  ftl_data_set(biquad_df1_state_t, this->biquad_state, ynm1, &zero);
  ftl_data_set(biquad_df1_state_t, this->biquad_state, ynm2, &zero);

  conv = (2.0f * 3.14159265f) / fts_dsp_get_input_srate(dsp, 0);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv + 2, fts_dsp_get_input_name(dsp, 2));
  fts_set_symbol(argv + 3, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv + 4, this->biquad_state);
  fts_set_ftl_data(argv + 5, this->biquad_coefs);
  fts_set_float(argv + 6, conv);
  fts_set_long(argv + 7, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(sigapass3_function, 8, argv);
}

static void
ftl_apass3(fts_word_t *argv)
{
  fts_word_t biquad_args[5];
  float *in_sig = (float *)fts_word_get_ptr(argv + 0);
  float *in_freq = (float *)fts_word_get_ptr(argv + 1);
  float *in_q = (float *)fts_word_get_ptr(argv + 2);
  float *out_sig = (float *)fts_word_get_ptr(argv + 3);
  biquad_df1_state_t *biquad_state = (biquad_df1_state_t *)fts_word_get_ptr(argv + 4);
  biquad_coefs_t *biquad_coefs = (biquad_coefs_t *)fts_word_get_ptr(argv + 5);
  float conv = fts_word_get_float(argv + 6);
  long n_tick = fts_word_get_long(argv + 7);

  long n_tick_half = n_tick / 2;
  float theta, cos_theta, exp_theta, Q;

  /* calculate values for first half of vector */
  theta = conv * in_freq[0];
  if (theta > 2.0f)
    theta = 2.0f;
  else if (theta < 0.0001f)
    theta = 0.0001f; /* safety */

  Q = (in_q[0] <= 0.00001f)? 0.00001f: in_q[0];
  exp_theta = exp(-0.5f * theta / Q);  
  cos_theta = 1.0f - 0.5f * theta * theta;

  /* set up filter for first half of vector */
  biquad_coefs->a0 = biquad_coefs->b2 = exp_theta * exp_theta;
  biquad_coefs->a1 = biquad_coefs->b1 = -2.0f * cos_theta * exp_theta;
	
  /* compute first half of vector */
  fts_word_set_ptr(biquad_args + 0, in_sig);
  fts_word_set_ptr(biquad_args + 1, out_sig);
  fts_word_set_ptr(biquad_args + 2, biquad_state);
  fts_word_set_ptr(biquad_args + 3, biquad_coefs);
  fts_word_set_long(biquad_args + 4, n_tick_half);
  ftl_biquad_df1(biquad_args);
 
  /* calculate values for second half of vector */
  theta = conv * in_freq[n_tick_half];
  if (theta > 2.0f) 
    theta = 2.0f;
  else if (theta < 0.0001f) 
    theta = 0.0001f; /* safety */

  Q = (in_q[n_tick_half] <= .00001f)? 0.00001f: in_q[n_tick_half];
  exp_theta = exp(-0.5f * theta / Q);  
  cos_theta = 1.0f - 0.5f * theta * theta;

  /* set up filter for second half of vector */
  biquad_coefs->a0 = biquad_coefs->b2 = exp_theta * exp_theta;
  biquad_coefs->a1 = biquad_coefs->b1 = -2.0f * cos_theta * exp_theta;
	
  /* compute second half of vector */
  fts_word_set_ptr(biquad_args + 0, in_sig + n_tick_half);
  fts_word_set_ptr(biquad_args + 1, out_sig + n_tick_half);
  /* fts_word_set_ptr(biquad_args + 2, biquad_state); */ /* already set */
  /* fts_word_set_ptr(biquad_args + 3, biquad_coefs); */
  /* fts_word_set_long(biquad_args + 4, n_tick_half); */
  ftl_biquad_df1(biquad_args);
}		

static fts_status_t
sigapass3_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  /* initialize the class */
  fts_class_init(cl, sizeof(sigapass3_t), 3, 1, 0); 

  /* define the system methods */
  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigapass3_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigapass3_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigapass3_put);

  /* signal inlets/outlets */
  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1);
  dsp_sig_inlet(cl, 2);
  dsp_sig_outlet(cl, 0);

  sigapass3_function = fts_new_symbol("apass3");
  dsp_declare_function(sigapass3_function, ftl_apass3);

  return fts_Success;
}

void
sigapass3_config(void)
{
  fts_class_install(fts_new_symbol("apass3~"), sigapass3_instantiate);
}

