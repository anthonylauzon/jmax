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

#include <utils.h>
#include "wavetab.h"
#include "osc.h"

struct _osc_ftl
{ 
  wavetab_samp_t *table;
  double phase;
  double incr;
};

/************************************************************
 *
 *  FTL functions
 *
 */

/* The C version of the oscillator include now three version,
   one with frequency modulation only, one with phase modulation
   only, and one with both */

static fts_symbol_t osc_ftl_sym = 0;
static fts_symbol_t osc_ftl_sym_freq = 0;
static fts_symbol_t osc_ftl_sym_freq_inplace = 0;
static fts_symbol_t osc_ftl_sym_phase = 0;
static fts_symbol_t osc_ftl_sym_phase_inplace = 0;
static fts_symbol_t osc_ftl_sym_phase_64 = 0;
static fts_symbol_t osc_ftl_sym_phase_64_inplace = 0;

void
osc_ftl_dsp_put(fts_dsp_descr_t *dsp, ftl_data_t ftl_data)
{
  fts_atom_t argv[5];

  if (fts_dsp_is_input_null(dsp, 0))
    {
      /* no frequency input */
      if (fts_dsp_is_input_null(dsp, 1))
	{
	  /* no phase input, no frequency input */
	  fts_dsp_add_function_copy(fts_dsp_get_input_name(dsp, 0), fts_dsp_get_output_name(dsp, 0), fts_dsp_get_input_size(dsp, 0));
	}
      else
	{

	  /* Different code generated for inplace and not in place operations */
	  if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* Inplace phase input, no frequency input */
	      if (fts_dsp_get_input_size(dsp, 0) == 64)
		{
		  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
		  fts_set_ftl_data(argv + 1, ftl_data);

		  dsp_add_funcall(osc_ftl_sym_phase_64_inplace, 2, argv);
		}
	      else
		{
		  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 1));
		  fts_set_ftl_data(argv+ 1, ftl_data);
		  fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));
		  dsp_add_funcall(osc_ftl_sym_phase_inplace, 3, argv);
		}
	    }
	  else
	    {
	      /* Non inplace phase input, no frequency input */
	      if (fts_dsp_get_input_size(dsp, 0) == 64)
		{
		  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
		  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
		  fts_set_ftl_data(argv+ 2, ftl_data);

		  dsp_add_funcall(osc_ftl_sym_phase_64, 3, argv);
		}
	      else
		{
		  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
		  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
		  fts_set_ftl_data(argv+ 2, ftl_data);
		  fts_set_int(argv + 3, fts_dsp_get_input_size(dsp, 0));
		  dsp_add_funcall(osc_ftl_sym_phase, 4, argv);
		}

	    }
	}
    }
  else
    {
      if (fts_dsp_is_input_null(dsp, 1))
	{
	  /* no phase input, frequency input */
	  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* In place */
	      fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_ftl_data(argv+ 1, ftl_data);
	      fts_set_int   (argv + 2, fts_dsp_get_input_size(dsp, 0));

	      dsp_add_funcall(osc_ftl_sym_freq_inplace, 3, argv);
	    }
	  else
	    {
	      /* Not In place */
	      fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_ftl_data(argv+ 2, ftl_data);
	      fts_set_int   (argv + 3, fts_dsp_get_input_size(dsp, 0));

	      dsp_add_funcall(osc_ftl_sym_freq, 4, argv);
	    }


	}
      else
	{
	  /* phase input, frequency input */
	  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol (argv + 1, fts_dsp_get_input_name(dsp, 1));
	  fts_set_symbol (argv + 2, fts_dsp_get_output_name(dsp, 0));
	  fts_set_ftl_data(argv+ 3, ftl_data);
	  fts_set_int   (argv + 4, fts_dsp_get_input_size(dsp, 0));

	  dsp_add_funcall(osc_ftl_sym, 5, argv);
	}
    }
}

/* implementations */

static void
osc_ftl_fun_phase(fts_word_t *argv)
{
  float * restrict phase = (float *) fts_word_get_pointer(argv + 0);
  float * restrict out   = (float *) fts_word_get_pointer(argv + 1);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  wavetab_samp_t * restrict tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->table;

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      float f;

      fts_wrapper_frac_set(&frac, phase[i] * (double)WAVE_TAB_SIZE);
      idx = fts_wrapper_get_int(&frac, WAVE_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = tab[idx].value + f * tab[idx].slope;
    }
}


static void
osc_ftl_fun_phase_inplace(fts_word_t *argv)
{
  float * restrict sig = (float *) fts_word_get_pointer(argv + 0);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  wavetab_samp_t * restrict tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->table;

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      float f;

      fts_wrapper_frac_set(&frac, sig[i] * (double)WAVE_TAB_SIZE);
      idx = fts_wrapper_get_int(&frac, WAVE_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      sig[i] = tab[idx].value + f * tab[idx].slope;
    }
}


/* version with vs == 64 */

static void
osc_ftl_fun_phase_64(fts_word_t *argv)
{
  float * restrict phase = (float *) fts_word_get_pointer(argv + 0);
  float * restrict out   = (float *) fts_word_get_pointer(argv + 1);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_pointer(argv + 2);
  wavetab_samp_t * restrict tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->table;

  for(i=0; i<64; i++)
    {
      fts_wrapper_t frac;
      int idx;
      float f;

      fts_wrapper_frac_set(&frac, phase[i] * (double)WAVE_TAB_SIZE);
      idx = fts_wrapper_get_int(&frac, WAVE_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = tab[idx].value + f * tab[idx].slope;
    }
}


static void
osc_ftl_fun_phase_64_inplace(fts_word_t *argv)
{
  float * restrict sig = (float *) fts_word_get_pointer(argv + 0);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_pointer(argv + 1);
  wavetab_samp_t * restrict tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->table;

  for(i=0; i<64; i++)
    {
      fts_wrapper_t frac;
      int idx;
      float f;

      fts_wrapper_frac_set(&frac, sig[i] * (double)WAVE_TAB_SIZE);
      idx = fts_wrapper_get_int(&frac, WAVE_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      sig[i] = tab[idx].value + f * tab[idx].slope;
    }
}

static void
osc_ftl_fun_freq(fts_word_t *argv)
{
  float * restrict freq = (float *) fts_word_get_pointer(argv + 0);
  float * restrict out  = (float *) fts_word_get_pointer(argv + 1);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  wavetab_samp_t * restrict tab = this->table;
  fts_wrapper_t phi;
  double incr = this->incr;
  int i;

  fts_wrapper_frac_set(&phi, this->phase);

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      double f;

      fts_wrapper_copy(&phi, &frac);
      idx = fts_wrapper_get_int(&frac, WAVE_TAB_BITS);
      fts_wrapper_incr(&phi, freq[i] * incr);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = tab[idx].value + f * tab[idx].slope;
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  this->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), WAVE_TAB_BITS);
}

static void
osc_ftl_fun_freq_inplace(fts_word_t *argv)
{
  float * restrict inout = (float *) fts_word_get_pointer(argv + 0);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  wavetab_samp_t * restrict  tab = this->table;
  fts_wrapper_t phi;
  double incr = this->incr;
  int i;

  fts_wrapper_frac_set(&phi, this->phase);

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      double f;

      fts_wrapper_copy(&phi, &frac);
      idx = fts_wrapper_get_int(&frac, WAVE_TAB_BITS);
      fts_wrapper_incr(&phi, inout[i] * incr);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      inout[i] = tab[idx].value + f * tab[idx].slope;
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  this->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), WAVE_TAB_BITS);
}

static void
osc_ftl_fun(fts_word_t *argv)
{
  float *freq  = (float *) fts_word_get_pointer(argv + 0);
  float *phase = (float *) fts_word_get_pointer(argv + 1);
  float *out   = (float *) fts_word_get_pointer(argv + 2);
  osc_ftl_t *this = (osc_ftl_t *)fts_word_get_pointer(argv + 3);
  int n = fts_word_get_int(argv + 4);
  wavetab_samp_t *tab = this->table;
  fts_wrapper_t phi;
  double incr = this->incr;
  int i;

  /* get running phase from last time */
  fts_wrapper_frac_set(&phi, this->phase);

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      double f;
      double a, b;

      /* copy running phase into fractional wrapper */
      fts_wrapper_copy(&phi, &frac);

      /* add offset phase to running phase */
      fts_wrapper_incr(&frac, phase[i] * (double)WAVE_TAB_SIZE);

      /* increment running phase */
      fts_wrapper_incr(&phi, freq[i] * incr);

      /* get integer and (after wrapping) fractional part */
      idx = fts_wrapper_get_int(&frac, WAVE_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      /* interpolate */
      out[i] = tab[idx].value + f * tab[idx].slope;
    }

  /* use wrapper phi to wrap running phase into oscillator table range */
  this->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), WAVE_TAB_BITS);
}

void
osc_ftl_declare_functions(void)
{
  osc_ftl_sym_phase = fts_new_symbol("osc_phase");
  dsp_declare_function(osc_ftl_sym_phase, osc_ftl_fun_phase);

  osc_ftl_sym_phase_64 = fts_new_symbol("osc_phase_64");
  dsp_declare_function(osc_ftl_sym_phase_64, osc_ftl_fun_phase_64);

  osc_ftl_sym_phase_inplace = fts_new_symbol("osc_phase_inplace");
  dsp_declare_function(osc_ftl_sym_phase_inplace, osc_ftl_fun_phase_inplace);

  osc_ftl_sym_phase_64_inplace = fts_new_symbol("osc_phase_64_inplace");
  dsp_declare_function(osc_ftl_sym_phase_64_inplace, osc_ftl_fun_phase_64_inplace);

  osc_ftl_sym_freq = fts_new_symbol("osc_freq");
  dsp_declare_function(osc_ftl_sym_freq, osc_ftl_fun_freq);

  osc_ftl_sym_freq_inplace = fts_new_symbol("osc_freq_inplace");
  dsp_declare_function(osc_ftl_sym_freq_inplace, osc_ftl_fun_freq_inplace);

  osc_ftl_sym = fts_new_symbol("osc");
  dsp_declare_function(osc_ftl_sym, osc_ftl_fun);
}

/***************************************************************************
 *
 *  FTL data
 *
 */

ftl_data_t osc_ftl_data_new(void)
{
  return ftl_data_new(osc_ftl_t);
}

void
osc_ftl_data_init(ftl_data_t ftl_data, float sr)
{
  osc_ftl_t *data = (osc_ftl_t *)ftl_data_get_ptr(ftl_data);

  data->incr = (double)WAVE_TAB_SIZE / (double)sr;
  data->phase = 0.0;
}

void
osc_ftl_data_set_phase(ftl_data_t ftl_data, float phase)
{
  osc_ftl_t *data = (osc_ftl_t *)ftl_data_get_ptr(ftl_data);

  data->phase = (double)phase;
}

void
osc_ftl_data_set_table(ftl_data_t ftl_data, void *table)
{
  osc_ftl_t *data = (osc_ftl_t *)ftl_data_get_ptr(ftl_data);

  data->table = (wavetab_samp_t *)table;
}
