/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "ieeewrap.h"

void fts_wavetab_osc_init(fts_wto_t *wto, float* table, int size)
{
  wto->table = table;
  wto->size = size;
  wto->index = 0.0;
  wto->incr = 0.0;
}

/************************************************************************
 *
 *  run wave table oscillator
 *
 */

void
fts_wto_run(fts_wto_t *wto, float *out, long n)
{
  fts_wrap_value_t wv;
  int i;

  fts_wrap_value_init(&wv, wto->index);

  for(i=0; i<n; i++)
    {
      long index;
      double frac;
      float a, b;

      fts_wrap_value_set(&wv);
      index = fts_wrap_value_get_int(&wv, wto->size);
      frac = fts_wrap_value_get_frac(&wv);
      
      a = wto->table[index];
      b = wto->table[index+1];
      
      out[i] = a + frac * (b - a);

      fts_wrap_value_incr(&wv, wto->incr);
    }

  wto->index = fts_wrap_value_get_wrapped(&wv, wto->size);
}

void
fts_wto_run_freq(fts_wto_t *wto, float *freq, float *out, long n)
{
  fts_wrap_value_t wv;
  int i;

  fts_wrap_value_init(&wv, wto->index);

  for(i=0; i<n; i++)
    {
      long index;
      double frac;
      float a, b;

      fts_wrap_value_set(&wv);
      index = fts_wrap_value_get_int(&wv, wto->size);
      frac = fts_wrap_value_get_frac(&wv);
      
      a = wto->table[index];
      b = wto->table[index+1];
      
      out[i] = a + frac * (b - a);

      fts_wrap_value_incr(&wv, freq[0] * wto->incr);
    }

  wto->index = fts_wrap_value_get_wrapped(&wv, wto->size);
}
