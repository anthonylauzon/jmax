/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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

void ftl_biquad_df1(fts_word_t *argv)
{
  biquad_df1_state_t * restrict state;
  biquad_coefs_t * restrict coefs;
  int n_tick;
  float * restrict x, * restrict y;
  float xnm1, xnm2, ynm2, ynm1;
  float a0, a1, a2, b1, b2;
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

  for ( n = 0; n < n_tick; n ++)
    {
      float xn, yn;

      xn = x[n];
      yn = a0 * xn + a1 * xnm1 + a2 * xnm2 - b1 * ynm1 - b2 * ynm2;

      y[n] = yn;

      xnm2 = xnm1;
      xnm1 = xn;
      ynm2 = ynm1;
      ynm1 = yn;
    }

  state->xnm2 = xnm2;
  state->xnm1 = xnm1;
  state->ynm2 = ynm2;
  state->ynm1 = ynm1;
}


void ftl_biquad_df1_inplace(fts_word_t *argv)
{
  biquad_df1_state_t * restrict state;
  biquad_coefs_t * restrict coefs;
  int n_tick;
  float * restrict xy;
  float xnm1, xnm2, ynm2, ynm1;
  float a0, a1, a2, b1, b2;
  int n;

  xy = (float *)fts_word_get_ptr(argv); /* in0 */
  state = (biquad_df1_state_t *)fts_word_get_ptr(argv+1);
  coefs = (biquad_coefs_t *)fts_word_get_ptr(argv+2);
  n_tick = fts_word_get_long(argv+3);

  xnm1 = state->xnm1; /* x(n-1) */
  xnm2 = state->xnm2; /* x(n-2) */
  ynm1 = state->ynm1; /* y(n-1) */
  ynm2 = state->ynm2; /* y(n-2) */

  a0 = coefs->a0; 
  a1 = coefs->a1; 
  a2 = coefs->a2; 
  b1 = coefs->b1; 
  b2 = coefs->b2; 

  for ( n = 0; n < n_tick; n ++)
    {
      float xn, yn;

      xn = xy[n];

      yn = a0 * xn + a1 * xnm1 + a2 * xnm2 - b1 * ynm1 - b2 * ynm2;

      xy[n] = yn;

      xnm2 = xnm1;
      xnm1 = xn;
      ynm2 = ynm1;
      ynm1 = yn;
    }

  state->xnm2 = xnm2;
  state->xnm1 = xnm1;
  state->ynm2 = ynm2;
  state->ynm1 = ynm1;
}


/****************************
 *  
 *  biquad filter in direct form II:
 *
 *     w(n) = x(n) - b1 * w(n-1) - b2 * w(n-2)
 *     y(n) = a0 * w(n) + a1 * w(n-1) + a2 * w(n-2)
 *
 */

void ftl_biquad_df2(fts_word_t *argv)
{
  biquad_df2_state_t * restrict state;
  biquad_coefs_t * restrict coefs;
  int n_tick;
  float * restrict x, * restrict y;
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



void ftl_biquad_df2_inplace(fts_word_t *argv)
{
  biquad_df2_state_t * restrict state;
  biquad_coefs_t * restrict coefs;
  int n_tick;
  float * restrict xy;
  float wnm1, wnm2;
  float a0, a1, a2, b1, b2;
  float wnp0, wnp1, wnp2, wnp3;
  int n;

  xy = (float *)fts_word_get_ptr(argv); /* in0 */
  state = (biquad_df2_state_t *)fts_word_get_ptr(argv+1);
  coefs = (biquad_coefs_t *)fts_word_get_ptr(argv+2);
  n_tick = fts_word_get_long(argv+3);

  wnm1 = state->wnm1; /* w(n-1) */
  wnm2 = state->wnm2; /* w(n-2) */

  a0 = coefs->a0; 
  a1 = coefs->a1; 
  a2 = coefs->a2; 
  b1 = coefs->b1; 
  b2 = coefs->b2; 

  wnp0 = xy[0] - b1 * wnm1 - b2 * wnm2; /* w(n) */
  wnp1 = xy[1] - b1 * wnp0 - b2 * wnm1; /* w(n+1) */
  wnp2 = xy[2] - b1 * wnp1 - b2 * wnp0; /* w(n+2) */
  wnp3 = xy[3] - b1 * wnp2 - b2 * wnp1; /* w(n+3) */

  xy[0] = a0 * wnp0 + a1 * wnm1 + a2 * wnm2;
  xy[1] = a0 * wnp1 + a1 * wnp0 + a2 * wnm1;
  xy[2] = a0 * wnp2 + a1 * wnp1 + a2 * wnp0;
  xy[3] = a0 * wnp3 + a1 * wnp2 + a2 * wnp1;

  for(n=4; n<n_tick; n+=4)
  {
    wnm2 = wnp2;
    wnm1 = wnp3;

    wnp0 = xy[n+0] - b1 * wnm1 - b2 * wnm2; /* w(n) */
    wnp1 = xy[n+1] - b1 * wnp0 - b2 * wnm1; /* w(n+1) */
    wnp2 = xy[n+2] - b1 * wnp1 - b2 * wnp0; /* w(n+2) */
    wnp3 = xy[n+3] - b1 * wnp2 - b2 * wnp1; /* w(n+3) */

    xy[n+0] = a0 * wnp0 + a1 * wnm1 + a2 * wnm2;
    xy[n+1] = a0 * wnp1 + a1 * wnp0 + a2 * wnm1;
    xy[n+2] = a0 * wnp2 + a1 * wnp1 + a2 * wnp0;
    xy[n+3] = a0 * wnp3 + a1 * wnp2 + a2 * wnp1;
  }

  state->wnm2 = wnp2;
  state->wnm1 = wnp3;
}










