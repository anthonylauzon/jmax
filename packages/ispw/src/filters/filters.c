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

extern void ftl_2p2z(fts_word_t *argv);
static void sig2p2z_state_clear(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);

static fts_symbol_t sig2p2z_function = 0;
static fts_symbol_t sig2p2z_64_function = 0;
static fts_symbol_t sig2p2z_64_1ops_function = 0;

typedef struct
{
  fts_object_t _o;

  ftl_data_t ftl_data;
} sig2p2z_t;


/*************************************
 *
 *  dsp
 *
 */

void
ftl_2p2z(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  float *out = (float *)fts_word_get_ptr(argv + 1);
  ctlf2p2z_t *x = (ctlf2p2z_t *)fts_word_get_ptr(argv + 2);
  long int n = fts_word_get_long(argv + 3);
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


static void ftl_64_2p2z(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv + 0);
  float * restrict out = (float *)fts_word_get_ptr(argv + 1);
  ctlf2p2z_t *x = (ctlf2p2z_t *)fts_word_get_ptr(argv + 2);
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

static void ftl_64_1ops_2p2z(fts_word_t *argv)
{
  float * restrict vec = (float *)fts_word_get_ptr(argv + 0);
  ctlf2p2z_t * restrict x = (ctlf2p2z_t *)fts_word_get_ptr(argv + 1);
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
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  sig2p2z_state_clear(o, 0, 0, 0, 0);

  if (fts_dsp_get_input_size(dsp, 0) == 64)
    {
      if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	{
	  fts_set_symbol(argv + 0,   fts_dsp_get_input_name(dsp, 0));
	  fts_set_ftl_data(argv + 1, this->ftl_data);
	  dsp_add_funcall(sig2p2z_64_1ops_function, 2, argv);
	}
      else
	{
	  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_ftl_data(argv+2, this->ftl_data);
	  dsp_add_funcall(sig2p2z_64_function, 3, argv);
	}
    }
  else
    {
      fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv+2, this->ftl_data);

      fts_set_long(argv+3, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(sig2p2z_function, 4, argv);
    }
}

/*************************************
 *
 *  user methods
 *
 */

static void
sig2p2z_state_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;
  const float zero = 0.0;

  ftl_data_set(ctlf2p2z_t, this->ftl_data, state1, &zero);
  ftl_data_set(ctlf2p2z_t, this->ftl_data, state2, &zero);
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

/*************************************
 *
 *  object
 *
 */

static void
sig2p2z_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;

  this->ftl_data = ftl_data_new(ctlf2p2z_t);

  sig2p2z_set(o, winlet, s, ac-1, at+1);

  dsp_list_insert(o);
}

static void
sig2p2z_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sig2p2z_t *this = (sig2p2z_t *)o;

  ftl_data_free(this->ftl_data);
  dsp_list_remove(o);
}

/*************************************
 *
 *  class
 *
 */

static fts_status_t
sig2p2z_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[6];

  fts_class_init(cl, sizeof(sig2p2z_t), 7, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sig2p2z_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sig2p2z_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sig2p2z_put, 1, a);
  
  fts_method_define(cl, 0, fts_new_symbol("clear"), sig2p2z_state_clear, 0, 0);

  a[0] = fts_s_number;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  a[3] = fts_s_number;
  a[4] = fts_s_number;
  a[5] = fts_s_number;
  fts_method_define(cl, 0, fts_s_set, sig2p2z_set, 6, a);
  
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, sig2p2z_gain_c0, 1, a);
  fts_method_define(cl, 2, fts_s_float, sig2p2z_fbcoef1_c1, 1, a);
  fts_method_define(cl, 3, fts_s_float, sig2p2z_fbcoef2_c2, 1, a);
  fts_method_define(cl, 4, fts_s_float, sig2p2z_ffcoef0_c3, 1, a);
  fts_method_define(cl, 5, fts_s_float, sig2p2z_ffcoef1_c4, 1, a);
  fts_method_define(cl, 6, fts_s_float, sig2p2z_ffcoef2_c5, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, sig2p2z_gain_c0, 1, a);
  fts_method_define(cl, 2, fts_s_int, sig2p2z_fbcoef1_c1, 1, a);
  fts_method_define(cl, 3, fts_s_int, sig2p2z_fbcoef2_c2, 1, a);
  fts_method_define(cl, 4, fts_s_int, sig2p2z_ffcoef0_c3, 1, a);
  fts_method_define(cl, 5, fts_s_int, sig2p2z_ffcoef1_c4, 1, a);
  fts_method_define(cl, 6, fts_s_int, sig2p2z_ffcoef2_c5, 1, a);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

static void sig2p2z_config(void)
{
  sig2p2z_function = fts_new_symbol("2p2z");
  dsp_declare_function(sig2p2z_function, ftl_2p2z);

  sig2p2z_64_function = fts_new_symbol("2p2z_64");
  dsp_declare_function(sig2p2z_64_function, ftl_64_2p2z);

  sig2p2z_64_1ops_function = fts_new_symbol("2p2z_64_1ops");
  dsp_declare_function(sig2p2z_64_1ops_function, ftl_64_1ops_2p2z);
}




