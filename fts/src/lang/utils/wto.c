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
