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

typedef struct 
{
  fts_wrapper_t phase;
  double incr;
} phasor_ftl_t;

ftl_data_t
phasor_ftl_data_new(void)
{
  return ftl_data_new(phasor_ftl_t);
}

void 
phasor_ftl_data_init(ftl_data_t ftl_data, float sr)
{
  phasor_ftl_t *state = (phasor_ftl_t *)ftl_data_get_ptr(ftl_data);

  fts_wrapper_frac_set(&state->phase, 0.0);
  state->incr = (double)1.0 / (double)sr;
}

void
phasor_ftl_set_phase(ftl_data_t ftl_data, float phi)
{
  phasor_ftl_t *state = (phasor_ftl_t *)ftl_data_get_ptr(ftl_data);

  fts_wrapper_frac_set(&state->phase, phi);
}

void 
phasor_ftl(fts_word_t *argv)
{
  float * restrict freq = (float *)  fts_word_get_pointer(argv + 0);
  float * restrict out  = (float *)  fts_word_get_pointer(argv + 1);
  phasor_ftl_t * restrict state = (phasor_ftl_t *) fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  double incr = state->incr;
  fts_wrapper_t phi = state->phase;
  int i;

  for(i=0; i<n; i++)
    {
      float this_freq = freq[i];
      out[i] = fts_wrapper_frac_get_wrap(&phi);
      fts_wrapper_incr(&phi, this_freq * incr);
    }

  state->phase = phi;
}

void 
phasor_ftl_inplace(fts_word_t *argv)
{
  float * restrict sig = (float *)  fts_word_get_pointer(argv + 0);
  phasor_ftl_t * restrict state = (phasor_ftl_t *) fts_word_get_pointer(argv + 1);
  long int n = fts_word_get_int(argv + 2);
  double incr = state->incr;
  fts_wrapper_t phi = state->phase;
  int i;

  for(i=0; i<n; i++)
    {
      float this_freq = sig[i];
      sig[i] = fts_wrapper_frac_get_wrap(&phi);
      fts_wrapper_incr(&phi, this_freq * incr);
    }

  state->phase = phi;
}
