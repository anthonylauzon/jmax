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


#include "fts.h"
#include "wavetab.h"
#include "osc_ftl.h"

struct _osc_ftl
{ 
  wavetab_samp_t *table;
  fts_intphase_t phase;
  double incr;
};

#define PHASE_FRAC_BITS 8
#define PHASE_FRAC_SIZE (1 << PHASE_FRAC_BITS)

#define PHASE_BITS (WAVE_TAB_BITS + PHASE_FRAC_BITS)
#define PHASE_RANGE (1 << PHASE_BITS)

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

	  /* Special case: always output zero */
	  fts_set_symbol(argv + 0,   fts_dsp_get_input_name(dsp, 0)); /* input is zero, anyway */
	  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));

	  dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
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
  float * restrict phase = (float *) fts_word_get_ptr(argv + 0);
  float * restrict out = (float *) fts_word_get_ptr(argv + 1);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_ptr(argv + 2);
  int n = fts_word_get_int(argv + 3);
  wavetab_samp_t * restrict tab = this->table;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  for(i=0; i<n; i++)
    {
      fts_intphase_t phi = (fts_intphase_t)(phase[i] * (float)PHASE_RANGE) & (fts_intphase_t)(PHASE_RANGE - 1);
      int index = phi >> PHASE_FRAC_BITS;
      float frac = ((fts_intphase_t)phi & (PHASE_FRAC_SIZE - 1)) * (1.0f / (float)PHASE_FRAC_SIZE);
      
      out[i] = tab[index].value + tab[index].slope * frac;
    }
}

static void
osc_ftl_fun_phase_inplace(fts_word_t *argv)
{
  float * restrict sig = (float *) fts_word_get_ptr(argv + 0);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_ptr(argv + 1);
  int n = fts_word_get_int(argv + 2);
  wavetab_samp_t * restrict tab = this->table;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  for(i=0; i<n; i++)
    {
      fts_intphase_t phi = (fts_intphase_t)(sig[i] * (float)PHASE_RANGE) & (fts_intphase_t)(PHASE_RANGE - 1);
      int index = phi >> PHASE_FRAC_BITS;
      float frac = ((fts_intphase_t)phi & (PHASE_FRAC_SIZE - 1)) * (1.0f / (float)PHASE_FRAC_SIZE);
      
      sig[i] = tab[index].value + tab[index].slope * frac;
    }
}


/* version with vs == 64 */

static void
osc_ftl_fun_phase_64(fts_word_t *argv)
{
  float * restrict phase = (float *) fts_word_get_ptr(argv + 0);
  float * restrict out = (float *) fts_word_get_ptr(argv + 1);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_ptr(argv + 2);
  wavetab_samp_t * restrict tab = this->table;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  for(i=0; i<64; i++)
    {
      fts_intphase_t phi = (fts_intphase_t)(phase[i] * (float)PHASE_RANGE) & (fts_intphase_t)(PHASE_RANGE - 1);
      int index = phi >> PHASE_FRAC_BITS;
      float frac = ((fts_intphase_t)phi & (PHASE_FRAC_SIZE - 1)) * (1.0f / (float)PHASE_FRAC_SIZE);
      
      out[i] = tab[index].value + tab[index].slope * frac;
    }
}

static void
osc_ftl_fun_phase_64_inplace(fts_word_t *argv)
{
  float * restrict sig = (float *) fts_word_get_ptr(argv + 0);
  osc_ftl_t * restrict this = (osc_ftl_t *)fts_word_get_ptr(argv + 1);
  wavetab_samp_t * restrict tab = this->table;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  for(i=0; i<64; i++)
    {
      fts_intphase_t phi = (fts_intphase_t)(sig[i] * (float)PHASE_RANGE) & (fts_intphase_t)(PHASE_RANGE - 1);
      int index = phi >> PHASE_FRAC_BITS;
      float frac = ((fts_intphase_t)phi & (PHASE_FRAC_SIZE - 1)) * (1.0f / (float)PHASE_FRAC_SIZE);
      
      sig[i] = tab[index].value + tab[index].slope * frac;
    }
}

static void
osc_ftl_fun_freq(fts_word_t *argv)
{
  float *freq = (float *) fts_word_get_ptr(argv + 0);
  float *out  = (float *) fts_word_get_ptr(argv + 1);
  osc_ftl_t *this = (osc_ftl_t *)fts_word_get_ptr(argv + 2);
  int n = fts_word_get_int(argv + 3);
  fts_intphase_t phi = this->phase;
  double incr = this->incr;
  wavetab_samp_t *tab = this->table;
  int i;

  for(i=0; i<n; i++)
    {
      int index = fts_intphase_get_index(phi, WAVE_TAB_BITS);
      float frac = fts_intphase_get_frac(phi, WAVE_TAB_BITS);

      out[i] = tab[index].value + tab[index].slope * frac;

      phi = fts_intphase_wrap(phi + (fts_intphase_t)(incr * freq[i]));
    }

  this->phase = phi;
}

static void
osc_ftl_fun_freq_inplace(fts_word_t *argv)
{
  float *sig = (float *) fts_word_get_ptr(argv + 0);
  osc_ftl_t *this = (osc_ftl_t *)fts_word_get_ptr(argv + 1);
  int n = fts_word_get_int(argv + 2);
  fts_intphase_t phi = this->phase;
  double incr = this->incr;
  wavetab_samp_t *tab = this->table;
  int i;

  for(i=0; i<n; i++)
    {
      int index = fts_intphase_get_index(phi, WAVE_TAB_BITS);
      float frac = fts_intphase_get_frac(phi, WAVE_TAB_BITS);
      float this_incr = incr * sig[i];

      sig[i] = tab[index].value + tab[index].slope * frac;

      phi = fts_intphase_wrap(phi + (fts_intphase_t)(this_incr));
    }

  this->phase = phi;
}

static void
osc_ftl_fun(fts_word_t *argv)
{
  float *freq  = (float *) fts_word_get_ptr(argv + 0);
  float *phase = (float *) fts_word_get_ptr(argv + 1);
  float *out   = (float *) fts_word_get_ptr(argv + 2);
  osc_ftl_t *this = (osc_ftl_t *)fts_word_get_ptr(argv + 3);
  int n = fts_word_get_int(argv + 4);
  fts_intphase_t phi_freq = this->phase;
  double incr = this->incr;
  wavetab_samp_t *tab = this->table;
  int i;

  for(i = 0; i < n; i++)
    {
      fts_intphase_t phi_offset = phase[i] * (float)PHASE_RANGE;
      fts_intphase_t phi_sum = (fts_intphase_t)(phi_offset + (phi_freq >> (FTS_INTPHASE_BITS - PHASE_BITS)));
      int index = ((phi_sum & (PHASE_RANGE - 1)) >> PHASE_FRAC_BITS);
      float frac = (float)(phi_sum & (PHASE_FRAC_SIZE - 1)) * (float)(1.0f / PHASE_FRAC_SIZE);

      out[i] = tab[index].value + tab[index].slope * frac;

      phi_freq = fts_intphase_wrap(phi_freq + (fts_intphase_t)(incr * freq[i]));
    }

  this->phase = phi_freq;
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

ftl_data_t 
osc_ftl_data_new(void)
{
  return ftl_data_new(osc_ftl_t);
}

void
osc_ftl_data_init(ftl_data_t ftl_data, float sr)
{
  osc_ftl_t *data = (osc_ftl_t *)ftl_data_get_ptr(ftl_data);

  data->incr = (double)FTS_INTPHASE_RANGE / (double)sr;
  data->phase = 0;
}

void
osc_ftl_data_set_phase(ftl_data_t ftl_data, float phase)
{
  osc_ftl_t *data = (osc_ftl_t *)ftl_data_get_ptr(ftl_data);
  
  data->phase = phase * FTS_INTPHASE_RANGE;
}

void
osc_ftl_data_set_table(ftl_data_t ftl_data, void *table)
{
  osc_ftl_t *data = (osc_ftl_t *)ftl_data_get_ptr(ftl_data);

  data->table = (wavetab_samp_t *)table;
}
