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
#include "wave.h"

#define PHASE_FRAC_BITS 8
#define PHASE_FRAC_SIZE (1 << PHASE_FRAC_BITS)

#define PHASE_BITS (WAVE_TABLE_BITS + PHASE_FRAC_BITS)
#define PHASE_RANGE (1 << PHASE_BITS)

static void
wave_ftl_outplace_ptr(fts_word_t *argv)
{
  wave_data_t *data = (wave_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *) fts_word_get_pointer(argv + 1);
  float * restrict out = (float *) fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float *buf = data->table.ptr;
  int i;

  for(i=0; i<n_tick; i++)
    {
      fts_intphase_t phi = (fts_intphase_t)(in[i] * (float)PHASE_RANGE) & (fts_intphase_t)(PHASE_RANGE - 1);
      int index = phi >> PHASE_FRAC_BITS;
      float frac = ((fts_intphase_t)phi & (PHASE_FRAC_SIZE - 1)) * (1.0f / (float)PHASE_FRAC_SIZE);
      
      out[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
    }
}

static void
wave_ftl_outplace_fvec(fts_word_t *argv)
{
  wave_data_t *data = (wave_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *) fts_word_get_pointer(argv + 1);
  float * restrict out = (float *) fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  float *buf = fvec_get_ptr(data->table.fvec);
  int i;

  for(i=0; i<n_tick; i++)
    {
      fts_intphase_t phi = (fts_intphase_t)(in[i] * (float)PHASE_RANGE) & (fts_intphase_t)(PHASE_RANGE - 1);
      int index = phi >> PHASE_FRAC_BITS;
      float frac = ((fts_intphase_t)phi & (PHASE_FRAC_SIZE - 1)) * (1.0f / (float)PHASE_FRAC_SIZE);
      
      out[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
    }
}

static void
wave_ftl_inplace_ptr(fts_word_t *argv)
{
  wave_data_t *data = (wave_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict sig = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = data->table.ptr;
  int i;

  for(i=0; i<n_tick; i++)
    {
      fts_intphase_t phi = (fts_intphase_t)(sig[i] * (float)PHASE_RANGE) & (fts_intphase_t)(PHASE_RANGE - 1);
      int index = phi >> PHASE_FRAC_BITS;
      float frac = ((fts_intphase_t)phi & (PHASE_FRAC_SIZE - 1)) * (1.0f / (float)PHASE_FRAC_SIZE);
      
      sig[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
    }
}

static void
wave_ftl_inplace_fvec(fts_word_t *argv)
{
  wave_data_t *data = (wave_data_t *)fts_word_get_pointer(argv + 0);
  float * restrict sig = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = fvec_get_ptr(data->table.fvec);
  int i;

  for(i=0; i<n_tick; i++)
    {
      fts_intphase_t phi = (fts_intphase_t)(sig[i] * (float)PHASE_RANGE) & (fts_intphase_t)(PHASE_RANGE - 1);
      int index = phi >> PHASE_FRAC_BITS;
      float frac = ((fts_intphase_t)phi & (PHASE_FRAC_SIZE - 1)) * (1.0f / (float)PHASE_FRAC_SIZE);
      
      sig[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
    }
}

void
wave_declare_functions(void)
{
  fts_dsp_declare_function(wave_ftl_symbols_ptr.outplace, wave_ftl_outplace_ptr);
  fts_dsp_declare_function(wave_ftl_symbols_ptr.inplace, wave_ftl_inplace_ptr);

  fts_dsp_declare_function(wave_ftl_symbols_fvec.outplace, wave_ftl_outplace_fvec);
  fts_dsp_declare_function(wave_ftl_symbols_fvec.inplace, wave_ftl_inplace_fvec);
}
