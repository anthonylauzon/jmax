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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */


#include <fts/fts.h>
#include <utils.h>
#include "osc.h"

typedef struct osc_data
{ 
  /* table stored as float vector or pointer to float */
  union
  {
    float *ptr;
    fvec_t *fvec;
  } table;

  /* phase */
  double phase;

  /* frequency controlled by signal or method */
  union
  {
    double absolute;
    double factor;
  } incr;

} osc_data_t;

static void
osc_ftl_control_input_ptr(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float * restrict buf = data->table.ptr;
  double incr = data->incr.absolute;
  fts_wrapper_t phi;
  int i;

  fts_wrapper_frac_set(&phi, data->phase);

  for(i=0; i<n_tick; i++)
    {
      fts_wrapper_t frac;
      int index;
      double f;

      fts_wrapper_copy(&phi, &frac);
      index = fts_wrapper_get_int(&frac, OSC_TABLE_BITS);
      fts_wrapper_incr(&phi, incr);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = buf[index] + f * (buf[index + 1] - buf[index]);
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  data->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TABLE_BITS);
}

static void
osc_ftl_control_input_fvec(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float * restrict buf = fvec_get_ptr(data->table.fvec);
  double incr = data->incr.absolute;
  fts_wrapper_t phi;
  int i;

  fts_wrapper_frac_set(&phi, data->phase);

  for(i=0; i<n_tick; i++)
    {
      fts_wrapper_t frac;
      int index;
      double f;

      fts_wrapper_copy(&phi, &frac);
      index = fts_wrapper_get_int(&frac, OSC_TABLE_BITS);
      fts_wrapper_incr(&phi, incr);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = buf[index] + f * (buf[index + 1] - buf[index]);
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  data->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TABLE_BITS);
}

static void
osc_ftl_signal_input_ptr(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *) fts_word_get_pointer(argv + 1);
  float * restrict out  = (float *) fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float * restrict buf = data->table.ptr;
  double factor = data->incr.factor;
  fts_wrapper_t phi;
  int i;

  fts_wrapper_frac_set(&phi, data->phase);

  for(i=0; i<n_tick; i++)
    {
      fts_wrapper_t frac;
      int index;
      double f;

      fts_wrapper_copy(&phi, &frac);
      index = fts_wrapper_get_int(&frac, OSC_TABLE_BITS);
      fts_wrapper_incr(&phi, in[i] * factor);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = buf[index] + f * (buf[index + 1] - buf[index]);
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  data->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TABLE_BITS);
}

static void
osc_ftl_signal_input_fvec(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *) fts_word_get_pointer(argv + 1);
  float * restrict out  = (float *) fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float * restrict buf = fvec_get_ptr(data->table.fvec);
  double factor = data->incr.factor;
  fts_wrapper_t phi;
  int i;

  fts_wrapper_frac_set(&phi, data->phase);

  for(i=0; i<n_tick; i++)
    {
      fts_wrapper_t frac;
      int index;
      double f;

      fts_wrapper_copy(&phi, &frac);
      index = fts_wrapper_get_int(&frac, OSC_TABLE_BITS);
      fts_wrapper_incr(&phi, in[i] * factor);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = buf[index] + f * (buf[index + 1] - buf[index]);
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  data->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TABLE_BITS);
}

static void
osc_ftl_signal_input_inplace_ptr(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict sig = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float * restrict buf = data->table.ptr;
  double factor = data->incr.factor;
  fts_wrapper_t phi;
  int i;

  fts_wrapper_frac_set(&phi, data->phase);

  for(i=0; i<n_tick; i++)
    {
      fts_wrapper_t frac;
      int index;
      double f;

      fts_wrapper_copy(&phi, &frac);
      index = fts_wrapper_get_int(&frac, OSC_TABLE_BITS);
      fts_wrapper_incr(&phi, sig[i] * factor);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      sig[i] = buf[index] + f * (buf[index + 1] - buf[index]);
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  data->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TABLE_BITS);
}

static void
osc_ftl_signal_input_inplace_fvec(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict sig = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float * restrict buf = fvec_get_ptr(data->table.fvec);
  double factor = data->incr.factor;
  fts_wrapper_t phi;
  int i;

  fts_wrapper_frac_set(&phi, data->phase);

  for(i=0; i<n_tick; i++)
    {
      fts_wrapper_t frac;
      int index;
      double f;

      fts_wrapper_copy(&phi, &frac);
      index = fts_wrapper_get_int(&frac, OSC_TABLE_BITS);
      fts_wrapper_incr(&phi, sig[i] * factor);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      sig[i] = buf[index] + f * (buf[index + 1] - buf[index]);
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  data->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TABLE_BITS);
}

void
osc_declare_functions(void)
{
  fts_dsp_declare_function(osc_ftl_symbols_ptr.control_input, osc_ftl_control_input_ptr);
  fts_dsp_declare_function(osc_ftl_symbols_ptr.signal_input, osc_ftl_signal_input_ptr);
  fts_dsp_declare_function(osc_ftl_symbols_ptr.signal_input_inplace, osc_ftl_signal_input_inplace_ptr);

  fts_dsp_declare_function(osc_ftl_symbols_fvec.control_input, osc_ftl_control_input_fvec);
  fts_dsp_declare_function(osc_ftl_symbols_fvec.signal_input, osc_ftl_signal_input_fvec);
  fts_dsp_declare_function(osc_ftl_symbols_fvec.signal_input_inplace, osc_ftl_signal_input_inplace_fvec);
}

/***************************************************************************
 *
 *  FTL data
 *
 */

ftl_data_t 
osc_data_new(void)
{
  ftl_data_t ftl_data = ftl_data_new(osc_data_t);
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);

  data->table.ptr = 0;
  data->table.fvec = 0;
  data->phase = 0.0;
  data->incr.factor = 0.0;
  data->incr.absolute = 0.0;

  return ftl_data;
}

void
osc_data_set_factor(ftl_data_t ftl_data, double sr)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);

  data->incr.factor = (double)OSC_TABLE_SIZE / sr;
}

void
osc_data_set_incr(ftl_data_t ftl_data, double incr)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);
  
  data->incr.absolute = (double)OSC_TABLE_SIZE * incr;
}

void
osc_data_set_phase(ftl_data_t ftl_data, double phase)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);
  
  data->phase = (double)OSC_TABLE_SIZE * phase;
}

void
osc_data_set_fvec(ftl_data_t ftl_data, fvec_t *fvec)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);

  if(data->table.fvec)
    fts_object_release((fts_object_t *)data->table.fvec);
  
  data->table.fvec = fvec;
  
  if(fvec)
    fts_object_refer((fts_object_t *)fvec);
}

void
osc_data_set_ptr(ftl_data_t ftl_data, float *ptr)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);

  data->table.ptr = ptr;
}
