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


#include "fts.h"
#include "osc.h"

typedef struct osc_data
{ 
  /* table stored as float vector or pointer to float */
  union
  {
    float *ptr;
    float_vector_t *fvec;
  } table;

  /* phase counter */
  fts_intphase_t phase;

  /* frequency controlled by signal or method */
  union
  {
    fts_intphase_t absolute;
    double factor;
  } incr;

} osc_data_t;

#define PHASE_FRAC_BITS 8
#define PHASE_FRAC_SIZE (1 << PHASE_FRAC_BITS)

#define PHASE_BITS (WAVE_TAB_BITS + PHASE_FRAC_BITS)
#define PHASE_RANGE (1 << PHASE_BITS)

static void
osc_ftl_control_input_ptr(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_ptr(argv + 0);
  float * restrict out = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = data->table.ptr;
  fts_intphase_t phi = data->phase;
  fts_intphase_t incr = data->incr.absolute;
  int i;

  for(i=0; i<n_tick; i++)
    {
      int index = fts_intphase_get_index(phi, OSC_TABLE_BITS);
      float frac = fts_intphase_get_frac(phi, OSC_TABLE_BITS);
      
      out[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
      
      phi = fts_intphase_wrap(phi + incr);
    }

  data->phase = phi;  
}

static void
osc_ftl_control_input_fvec(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_ptr(argv + 0);
  float * restrict out = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = float_vector_get_ptr(data->table.fvec);
  fts_intphase_t phi = data->phase;
  fts_intphase_t incr = data->incr.absolute;
  int i;

  for(i=0; i<n_tick; i++)
    {
      int index = fts_intphase_get_index(phi, OSC_TABLE_BITS);
      float frac = fts_intphase_get_frac(phi, OSC_TABLE_BITS);
      
      out[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
      
      phi = fts_intphase_wrap(phi + incr);
    }

  data->phase = phi;
}

static void
osc_ftl_signal_input_ptr(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_ptr(argv + 0);
  float * restrict in = (float *) fts_word_get_ptr(argv + 1);
  float * restrict out = (float *) fts_word_get_ptr(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float *buf = data->table.ptr;
  fts_intphase_t phi = data->phase;
  fts_intphase_t factor = data->incr.factor;
  int i;

  for(i=0; i<n_tick; i++)
    {
      int index = fts_intphase_get_index(phi, OSC_TABLE_BITS);
      float frac = fts_intphase_get_frac(phi, OSC_TABLE_BITS);
      fts_intphase_t incr = (fts_intphase_t)(factor * in[i]);
      
      out[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
      
      phi = fts_intphase_wrap(phi + incr);
    }

  data->phase = phi;  
}

static void
osc_ftl_signal_input_fvec(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_ptr(argv + 0);
  float * restrict in = (float *) fts_word_get_ptr(argv + 1);
  float * restrict out = (float *) fts_word_get_ptr(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float *buf = float_vector_get_ptr(data->table.fvec);
  fts_intphase_t phi = data->phase;
  fts_intphase_t factor = data->incr.factor;
  int i;

  for(i=0; i<n_tick; i++)
    {
      int index = fts_intphase_get_index(phi, OSC_TABLE_BITS);
      float frac = fts_intphase_get_frac(phi, OSC_TABLE_BITS);
      fts_intphase_t incr = (fts_intphase_t)(factor * in[i]);
      
      out[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
      
      phi = fts_intphase_wrap(phi + incr);
    }

  data->phase = phi;  
}

static void
osc_ftl_signal_input_inplace_ptr(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_ptr(argv + 0);
  float * restrict sig = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = data->table.ptr;
  fts_intphase_t phi = data->phase;
  fts_intphase_t factor = data->incr.factor;
  int i;

  for(i=0; i<n_tick; i++)
    {
      int index = fts_intphase_get_index(phi, OSC_TABLE_BITS);
      float frac = fts_intphase_get_frac(phi, OSC_TABLE_BITS);
      fts_intphase_t incr = (fts_intphase_t)(factor * sig[i]);
      
      sig[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
      
      phi = fts_intphase_wrap(phi + incr);
    }

  data->phase = phi;  
}

static void
osc_ftl_signal_input_inplace_fvec(fts_word_t *argv)
{
  osc_data_t *data = (osc_data_t *)fts_word_get_ptr(argv + 0);
  float * restrict sig = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = float_vector_get_ptr(data->table.fvec);
  fts_intphase_t phi = data->phase;
  fts_intphase_t factor = data->incr.factor;
  int i;

  for(i=0; i<n_tick; i++)
    {
      int index = fts_intphase_get_index(phi, OSC_TABLE_BITS);
      float frac = fts_intphase_get_frac(phi, OSC_TABLE_BITS);
      fts_intphase_t incr = (fts_intphase_t)(factor * sig[i]);
      
      sig[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
      
      phi = fts_intphase_wrap(phi + incr);
    }

  data->phase = phi;  
}

void
osc_declare_functions(void)
{
  dsp_declare_function(osc_ftl_symbols_ptr.control_input, osc_ftl_control_input_ptr);
  dsp_declare_function(osc_ftl_symbols_ptr.signal_input, osc_ftl_signal_input_ptr);
  dsp_declare_function(osc_ftl_symbols_ptr.signal_input_inplace, osc_ftl_signal_input_inplace_ptr);

  dsp_declare_function(osc_ftl_symbols_fvec.control_input, osc_ftl_control_input_fvec);
  dsp_declare_function(osc_ftl_symbols_fvec.signal_input, osc_ftl_signal_input_fvec);
  dsp_declare_function(osc_ftl_symbols_fvec.signal_input_inplace, osc_ftl_signal_input_inplace_fvec);
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
  data->phase = 0;
  data->incr.factor = 0.0;

  return ftl_data;
}

void
osc_data_set_factor(ftl_data_t ftl_data, double sr)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);

  data->incr.factor = FTS_INTPHASE_RANGE / sr;
}

void
osc_data_set_incr(ftl_data_t ftl_data, double incr)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);
  
  data->incr.absolute = (fts_intphase_t)(incr * FTS_INTPHASE_RANGE);
}

void
osc_data_set_phase(ftl_data_t ftl_data, double phase)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);
  
  data->phase = (fts_intphase_t)(phase * FTS_INTPHASE_RANGE);
}

void
osc_data_set_fvec(ftl_data_t ftl_data, float_vector_t *fvec)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);

  if(data->table.fvec)
    float_vector_release(data->table.fvec);
  
  data->table.fvec = fvec;
  
  if(fvec)
    float_vector_refer(fvec);
}

void
osc_data_set_ptr(ftl_data_t ftl_data, float *ptr)
{
  osc_data_t *data = (osc_data_t *)ftl_data_get_ptr(ftl_data);

  data->table.ptr = ptr;
}
