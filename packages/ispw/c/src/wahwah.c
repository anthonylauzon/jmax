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
#include "filters.h"

static fts_symbol_t wahwah_function = 0;

/****************************************
 *
 *  object
 *
 */

typedef struct wahwah
{
  fts_object_t obj;
  ftl_data_t state;
} wahwah_t;

static void
wahwah_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wahwah_t *this = (wahwah_t *)o;

  this->state = ftl_data_alloc(sizeof(float) * 2);

  dsp_list_insert(o);
}

static void
wahwah_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wahwah_t *this = (wahwah_t *)o;

  ftl_data_free(this->state);

  dsp_list_remove(o);
}

static void
wahwah_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wahwah_t *this = (wahwah_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  float *state = (float *)ftl_data_get_ptr(this->state);
  float conv = 2.0 * 3.14159265 / sr;
  fts_atom_t argv[7];

  state[0] = 0.0;
  state[1] = 0.0;
  
  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv + 2, fts_dsp_get_input_name(dsp, 2));
  fts_set_symbol(argv + 3, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv + 4, this->state);
  fts_set_float(argv + 5, conv);
  fts_set_int(argv + 6, n_tick);

  dsp_add_funcall(wahwah_function, 7, argv);
}

static void
ftl_wahwah(fts_word_t *argv)
{
  float *x = (float *)fts_word_get_ptr(argv + 0);
  float *in_freq = (float *)fts_word_get_ptr(argv + 1);
  float *in_q = (float *)fts_word_get_ptr(argv + 2);
  float *y = (float *)fts_word_get_ptr(argv + 3);
  float *state = (float *)fts_word_get_ptr(argv + 4);
  float conv = fts_word_get_float(argv + 5);
  long n_tick = fts_word_get_int(argv + 6);
  float theta, cos_theta, sin_theta, q, r;
  float c, b1, b2;
  float ynm2, ynm1;
  float ynp0, ynp1, ynp2, ynp3;
  long n;
 
  theta = conv * in_freq[0];
  if(theta > 2.0f)
    theta = 2.0f;
  else if (theta < 0)
    theta = 0.0f;

  cos_theta = 1.0f - 0.5f * theta * theta;
  sin_theta = theta;

  if(sin_theta > 1.0f)
    sin_theta = 1.0f;
    
  q = in_q[0];
  if(q <= 0.00001f)
    r = 0.0f;
  else
    {
      r = 1.0f - theta / q;
      if (r < 0.0f)
	r = 0.0f;
    }
  
  ynm2 = state[0]; /* y(n-2) */
  ynm1 = state[1]; /* y(n-1) */

  b1 = -2.0f * r * cos_theta; 
  b2 = r * r;
  c = (1.0f - r) * ((1.0f - r) + r * sin_theta);

  ynp0 = FILTERS_FP_ONSET(x[0] - b1 * ynm1 - b2 * ynm2); /* y(n) */
  ynp1 = FILTERS_FP_ONSET(x[1] - b1 * ynp0 - b2 * ynm1); /* y(n+1) */
  ynp2 = FILTERS_FP_ONSET(x[2] - b1 * ynp1 - b2 * ynp0); /* y(n+2) */
  ynp3 = FILTERS_FP_ONSET(x[3] - b1 * ynp2 - b2 * ynp1); /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = c * ynp0;
    ynp0 = FILTERS_FP_ONSET(x[n+0] - b1 * ynp3 - b2 * ynp2); /* y(n) */
    y[n-3] = c * ynp1;
    ynp1 = FILTERS_FP_ONSET(x[n+1] - b1 * ynp0 - b2 * ynp3); /* y(n+1) */
    y[n-2] = c * ynp2;
    ynp2 = FILTERS_FP_ONSET(x[n+2] - b1 * ynp1 - b2 * ynp0); /* y(n+2) */
    y[n-1] = c * ynp3;
    ynp3 = FILTERS_FP_ONSET(x[n+3] - b1 * ynp2 - b2 * ynp1); /* y(n+3) */
  }

  y[n_tick-4] = c * ynp0;
  y[n_tick-3] = c * ynp1;
  y[n_tick-2] = c * ynp2;
  y[n_tick-1] = c * ynp3;

  state[0] = ynp2; /* y(n-2) */
  state[1] = ynp3; /* y(n-1) */
}

static fts_status_t
wahwah_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(wahwah_t), 3, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, wahwah_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, wahwah_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, wahwah_put, 1, a);
  
  /* signal inlets and outlets */
  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1);
  dsp_sig_inlet(cl, 2);
  dsp_sig_outlet(cl, 0);
  
  wahwah_function = fts_new_symbol("wahwah");
  dsp_declare_function(wahwah_function, ftl_wahwah);

  return fts_Success;
}

void
wahwah_config(void)
{
  fts_class_install(fts_new_symbol("wahwah~"), wahwah_instantiate);
}
