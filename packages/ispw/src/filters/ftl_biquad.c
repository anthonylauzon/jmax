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
#include "biquad.h"

/****************************
 *  
 *  biquad filter in direct form I:
 *
 *     y(n) = a0 * x(n) + a1 * x(n-1) + a2 * x(n-2) - b1 * y(n-1) - b2 * y(n-2) 
 *  
 */

#if 0

void 
ftl_biquad_df1(fts_word_t *argv)
{
  biquad_state_df1_t *state;
  biquad_coefs_t *coefs;
  int n_tick;
  float *x, *y;
  float xnm1, xnm2, ynm2, ynm1;
  float a0, a1, a2, b1, b2;
  float ynp0, ynp1, ynp2, ynp3;
  int n;

  x = (float *)fts_word_get_ptr(argv); /* in0 */
  y = (float *)fts_word_get_ptr(argv+1); /* out0 */
  state = (biquad_state_df1_t *)fts_word_get_ptr(argv+2);
  coefs = (biquad_coefs_t *)fts_word_get_ptr(argv+3);
  n_tick = fts_word_get_long(argv+4);

  xnm1 = state->x1; /* x(n-1) */
  xnm2 = state->x2; /* x(n-2) */
  ynm1 = state->y1; /* y(n-1) */
  ynm2 = state->y2; /* y(n-2) */

  a0 = coefs->a0; 
  a1 = coefs->a1; 
  a2 = coefs->a2; 
  b1 = coefs->b1; 
  b2 = coefs->b2; 

  ynp0 = a0 * x[0] + a1 * xnm1 + a2 * xnm2 - b1 * ynm1 - b2 * ynm2; /* y(n) */
  ynp1 = a0 * x[1] + a1 * x[0] + a2 * xnm1 - b1 * ynp0 - b2 * ynm1; /* y(n+1) */
  ynp2 = a0 * x[2] + a1 * x[1] + a2 * x[0] - b1 * ynp1 - b2 * ynp0; /* y(n+2) */
  ynp3 = a0 * x[3] + a1 * x[2] + a2 * x[1] - b1 * ynp2 - b2 * ynp1; /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = a0 * x[n+0] + a1 * x[n-1] + a2 * x[n-2] - b1 * ynp3 - b2 * ynp2; /* y(n) */
    y[n-3] = ynp1;
    ynp1 = a0 * x[n+1] + a1 * x[n+0] + a2 * x[n-1] - b1 * ynp0 - b2 * ynp3; /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = a0 * x[n+2] + a1 * x[n+1] + a2 * x[n+0] - b1 * ynp1 - b2 * ynp0; /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = a0 * x[n+3] + a1 * x[n+2] + a2 * x[n+1] - b1 * ynp2 - b2 * ynp1; /* y(n+3) */
  }

  state->y1 = ynp3;
  state->y2 = ynp2;
  state->x1 = x[n_tick-1];
  state->x2 = x[n_tick-2];

  y[n_tick-4] = ynp0;
  y[n_tick-3] = ynp1;
  y[n_tick-2] = ynp2;
  y[n_tick-1] = ynp3;
}

#endif

void
ftl_biquad_df1(fts_word_t *argv)
{
  biquad_df1_state_t *state;
  biquad_coefs_t *coefs;
  int n_tick;
  float *x, *y;
  float xnm1, xnm2, ynm2, ynm1;
  float a0, a1, a2, b1, b2;
  float ynp0, ynp1, ynp2, ynp3;
  int n;

  x = (float *)fts_word_get_ptr(argv); /* in0 */
  y = (float *)fts_word_get_ptr(argv+1); /* out0 */
  state = (biquad_df1_state_t *)fts_word_get_ptr(argv+2);
  coefs = (biquad_coefs_t *)fts_word_get_ptr(argv+3);
  n_tick = fts_word_get_long(argv+4);

  xnm1 = state->xnm1; /* x(n-1) */
  xnm2 = state->xnm2; /* x(n-2) */
  ynm1 = state->ynm1; /* y(n-1) */
  ynm2 = state->ynm2; /* y(n-2) */

  a0 = coefs->a0; 
  a1 = coefs->a1; 
  a2 = coefs->a2; 
  b1 = coefs->b1; 
  b2 = coefs->b2; 

  ynp0 = a0 * x[0] + a1 * xnm1 + a2 * xnm2 - b1 * ynm1 - b2 * ynm2; /* y(n) */
  ynp1 = a0 * x[1] + a1 * x[0] + a2 * xnm1 - b1 * ynp0 - b2 * ynm1; /* y(n+1) */
  ynp2 = a0 * x[2] + a1 * x[1] + a2 * x[0] - b1 * ynp1 - b2 * ynp0; /* y(n+2) */
  ynp3 = a0 * x[3] + a1 * x[2] + a2 * x[1] - b1 * ynp2 - b2 * ynp1; /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    xnm2 = x[n-2];
    xnm1 = x[n-1];

    y[n-4] = ynp0;
    y[n-3] = ynp1;
    y[n-2] = ynp2;
    y[n-1] = ynp3;

    ynp0 = a0 * x[n+0] + a1 * xnm1   + a2 * xnm2   - b1 * ynp3 - b2 * ynp2; /* y(n) */
    ynp1 = a0 * x[n+1] + a1 * x[n+0] + a2 * xnm1   - b1 * ynp0 - b2 * ynp3; /* y(n+1) */
    ynp2 = a0 * x[n+2] + a1 * x[n+1] + a2 * x[n+0] - b1 * ynp1 - b2 * ynp0; /* y(n+2) */
    ynp3 = a0 * x[n+3] + a1 * x[n+2] + a2 * x[n+1] - b1 * ynp2 - b2 * ynp1; /* y(n+3) */
  }

  state->xnm2 = x[n_tick-2];
  state->xnm1 = x[n_tick-1];

  y[n_tick-4] = ynp0;
  y[n_tick-3] = ynp1;
  state->ynm2 = y[n_tick-2] = ynp2;
  state->ynm1 = y[n_tick-1] = ynp3;
}

/****************************
 *  
 *  biquad filter in direct form II:
 *
 *     w(n) = x(n) - b1 * w(n-1) - b2 * w(n-2)
 *     y(n) = a0 * w(n) + a1 * w(n-1) + a2 * w(n-2)
 *
 */

void
ftl_biquad_df2(fts_word_t *argv)
{
  biquad_df2_state_t *state;
  biquad_coefs_t *coefs;
  int n_tick;
  float *x, *y;
  float wnm1, wnm2;
  float a0, a1, a2, b1, b2;
  float wnp0, wnp1, wnp2, wnp3;
  int n;

  x = (float *)fts_word_get_ptr(argv); /* in0 */
  y = (float *)fts_word_get_ptr(argv+1); /* out0 */
  state = (biquad_df2_state_t *)fts_word_get_ptr(argv+2);
  coefs = (biquad_coefs_t *)fts_word_get_ptr(argv+3);
  n_tick = fts_word_get_long(argv+4);

  wnm1 = state->wnm1; /* w(n-1) */
  wnm2 = state->wnm2; /* w(n-2) */

  a0 = coefs->a0; 
  a1 = coefs->a1; 
  a2 = coefs->a2; 
  b1 = coefs->b1; 
  b2 = coefs->b2; 

  wnp0 = x[0] - b1 * wnm1 - b2 * wnm2; /* w(n) */
  wnp1 = x[1] - b1 * wnp0 - b2 * wnm1; /* w(n+1) */
  wnp2 = x[2] - b1 * wnp1 - b2 * wnp0; /* w(n+2) */
  wnp3 = x[3] - b1 * wnp2 - b2 * wnp1; /* w(n+3) */

  y[0] = a0 * wnp0 + a1 * wnm1 + a2 * wnm2;
  y[1] = a0 * wnp1 + a1 * wnp0 + a2 * wnm1;
  y[2] = a0 * wnp2 + a1 * wnp1 + a2 * wnp0;
  y[3] = a0 * wnp3 + a1 * wnp2 + a2 * wnp1;

  for(n=4; n<n_tick; n+=4)
  {
    wnm2 = wnp2;
    wnm1 = wnp3;

    wnp0 = x[n+0] - b1 * wnm1 - b2 * wnm2; /* w(n) */
    wnp1 = x[n+1] - b1 * wnp0 - b2 * wnm1; /* w(n+1) */
    wnp2 = x[n+2] - b1 * wnp1 - b2 * wnp0; /* w(n+2) */
    wnp3 = x[n+3] - b1 * wnp2 - b2 * wnp1; /* w(n+3) */

    y[n+0] = a0 * wnp0 + a1 * wnm1 + a2 * wnm2;
    y[n+1] = a0 * wnp1 + a1 * wnp0 + a2 * wnm1;
    y[n+2] = a0 * wnp2 + a1 * wnp1 + a2 * wnp0;
    y[n+3] = a0 * wnp3 + a1 * wnp2 + a2 * wnp1;
  }

  state->wnm2 = wnp2;
  state->wnm1 = wnp3;
}
