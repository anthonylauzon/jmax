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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Zack Settel.
 *
 */

#include <math.h> 
#include <fts/fts.h>
#include <filters.h>

/* radian frequency clips to from 0 to 2/3.14159  */
/* updates coefs twice per vector */

static fts_symbol_t sigapass3_function = 0;

typedef struct 
{
  fts_object_t obj;
  ftl_data_t biquad_state;
  ftl_data_t biquad_coefs;
} sigapass3_t;

static void
sigapass3_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigapass3_t *this = (sigapass3_t *)o;
  fts_atom_t argv[8];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float conv;
  float zero = 0.0;

  ftl_data_set(biquad_state_t, this->biquad_state, xnm1, &zero);
  ftl_data_set(biquad_state_t, this->biquad_state, xnm2, &zero);
  ftl_data_set(biquad_state_t, this->biquad_state, ynm1, &zero);
  ftl_data_set(biquad_state_t, this->biquad_state, ynm2, &zero);

  conv = (2.0f * 3.14159265f) / fts_dsp_get_input_srate(dsp, 0);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv + 2, fts_dsp_get_input_name(dsp, 2));
  fts_set_symbol(argv + 3, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv + 4, this->biquad_state);
  fts_set_ftl_data(argv + 5, this->biquad_coefs);
  fts_set_float(argv + 6, conv);
  fts_set_int(argv + 7, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function(sigapass3_function, 8, argv);
}

static void
ftl_apass3(fts_word_t *argv)
{
  fts_word_t biquad_args[5];
  float *in_sig = (float *)fts_word_get_pointer(argv + 0);
  float *in_freq = (float *)fts_word_get_pointer(argv + 1);
  float *in_q = (float *)fts_word_get_pointer(argv + 2);
  float *out_sig = (float *)fts_word_get_pointer(argv + 3);
  biquad_state_t *biquad_state = (biquad_state_t *)fts_word_get_pointer(argv + 4);
  biquad_coefs_t *biquad_coefs = (biquad_coefs_t *)fts_word_get_pointer(argv + 5);
  float conv = fts_word_get_float(argv + 6);
  long n_tick = fts_word_get_int(argv + 7);

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
	
  compute_biquad(in_sig, out_sig, biquad_state, biquad_coefs, n_tick_half);

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
	
  compute_biquad(in_sig + n_tick_half, out_sig + n_tick_half, biquad_state, biquad_coefs, n_tick_half);
}		

static void
sigapass3_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigapass3_t *this = (sigapass3_t *)o;
  biquad_coefs_t *data;
  float zero = 0.0f;
  float one = 1.0f;

  this->biquad_state = ftl_data_new(biquad_state_t);
  this->biquad_coefs = ftl_data_new(biquad_coefs_t);

  data = ftl_data_get_ptr(this->biquad_coefs);
  data->a0 = 0.0;
  data->a1 = 0.0;
  data->a2 = 1.0;
  data->b1 = 0.0;
  data->b2 = 0.0;

  fts_dsp_add_object(o); /* just put object in list */
}

static void
sigapass3_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigapass3_t *this = (sigapass3_t *)o;

  ftl_data_free(this->biquad_coefs);
  ftl_data_free(this->biquad_state);

  fts_dsp_remove_object(o);
}

static fts_status_t
sigapass3_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sigapass3_t), 3, 1, 0); 

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, sigapass3_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, sigapass3_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, sigapass3_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1);
  fts_dsp_declare_inlet(cl, 2);
  fts_dsp_declare_outlet(cl, 0);

  sigapass3_function = fts_new_symbol("apass3");
  fts_dsp_declare_function(sigapass3_function, ftl_apass3);

  return fts_ok;
}

void
sigapass3_config(void)
{
  fts_class_install(fts_new_symbol("apass3~"), sigapass3_instantiate);
}

