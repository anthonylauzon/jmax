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
 */

#include "fir.h"

/****************************
 *  
 *  FIR 1st order:
 *
 *     y(n) = a0 * x(n) + a1 * x(n-1)
 *
 */

void
ftl_fir_1(fts_word_t *argv)
{
  float *state;
  float *coefs;
  long n_tick;
  float *x, *y;
  float a0, a1;
  float xnm1, xnp0, xnp1, xnp2, xnp3;
  long n;

  x = (float *)fts_word_get_pointer(argv); /* in0 */
  y = (float *)fts_word_get_pointer(argv+1); /* out0 */
  state = (float *)fts_word_get_pointer(argv+2);
  coefs = (float *)fts_word_get_pointer(argv+3);
  n_tick = fts_word_get_int(argv+4);

  xnm1 = state[0]; /* x(n-1) */
  a0 = coefs[0]; 
  a1 = coefs[1]; 

  xnp0 = a0 * x[0] + a1 * xnm1; /* x(n) */
  xnp1 = a0 * x[1] + a1 * xnp0; /* x(n+1) */
  xnp2 = a0 * x[2] + a1 * xnp1; /* x(n+2) */
  xnp3 = a0 * x[3] + a1 * xnp2; /* x(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = xnp0;
    xnp0 = a0 * x[n+0] + a1 * xnp3; /* x(n) */
    y[n-3] = xnp1;
    xnp1 = a0 * x[n+1] + a1 * xnp0; /* x(n+1) */
    y[n-2] = xnp2;
    xnp2 = a0 * x[n+2] + a1 * xnp1; /* x(n+2) */
    y[n-1] = xnp3;
    xnp3 = a0 * x[n+3] + a1 * xnp2; /* x(n+3) */
  }

  state[0] = xnp3; /* x(n-1) */

  y[n_tick-4] = xnp0;
  y[n_tick-3] = xnp1;
  y[n_tick-2] = xnp2;
  y[n_tick-1] = xnp3;
}


/****************************
 *  
 *  FIR 2nd order:
 *
 *     y(n) = a0 * x(n) + a1 * x(n-1) + a2 * x(n-2)
 *
 */

void
ftl_fir_2(fts_word_t *argv)
{
  float *state;
  float *coefs;
  long n_tick;
  float *x, *y;
  float xnm2, xnm1;
  float a0, a1, a2;
  float xnp0, xnp1, xnp2, xnp3;
  long n;

  x = (float *)fts_word_get_pointer(argv); /* in0 */
  y = (float *)fts_word_get_pointer(argv+1); /* out0 */
  state = (float *)fts_word_get_pointer(argv+2);
  coefs = (float *)fts_word_get_pointer(argv+3);
  n_tick = fts_word_get_int(argv+4);

  xnm2 = state[0]; /* x(n-2) */
  xnm1 = state[1]; /* x(n-1) */
  a0 = coefs[0]; 
  a1 = coefs[1]; 
  a2 = coefs[2]; 

  xnp0 = a0 * x[0] + a1 * xnm1 + a2 * xnm2; /* x(n) */
  xnp1 = a0 * x[1] + a1 * xnp0 + a2 * xnm1; /* x(n+1) */
  xnp2 = a0 * x[2] + a1 * xnp1 + a2 * xnp0; /* x(n+2) */
  xnp3 = a0 * x[3] + a1 * xnp2 + a2 * xnp1; /* x(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = xnp0;
    xnp0 = a0 * x[n+0] + a1 * xnp3 + a2 * xnp2; /* x(n) */
    y[n-3] = xnp1;
    xnp1 = a0 * x[n+1] + a1 * xnp0 + a2 * xnp3; /* x(n+1) */
    y[n-2] = xnp2;
    xnp2 = a0 * x[n+2] + a1 * xnp1 + a2 * xnp0; /* x(n+2) */
    y[n-1] = xnp3;
    xnp3 = a0 * x[n+3] + a1 * xnp2 + a2 * xnp1; /* x(n+3) */
  }

  state[0] = xnp2; /* x(n-2) */
  state[1] = xnp3; /* x(n-1) */

  y[n_tick-4] = xnp0;
  y[n_tick-3] = xnp1;
  y[n_tick-2] = xnp2;
  y[n_tick-1] = xnp3;
}

/****************************
 *  
 *  Nth order
 *
 *     y(n) = a0 * x(n) + a1 * x(n-1) + a2 * x(n-2) + a3 * x(n-3) - ... + aN * x(n-N)
 *
 */


void
ftl_fir_n(fts_word_t *argv)
{
}

void
signal_fir_config(void)
{
}
