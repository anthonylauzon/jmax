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
/****************************
 *  
 *  biquad filter coefficients and states
 *
 */

typedef struct
{
  float a0;
  float a1;
  float a2;
  float b1;
  float b2;
} biquad_coefs_t;

/****************************
 *  
 *  filter state for direct form I:
 *
 *     y(n) = a0 * x(n) + a1 * x(n-1) + a2 * x(n-2) - b1 * y(n-1) - b2 * y(n-2) 
 *  
 */

typedef struct
{
  float ynm1; /* y(n-1) */
  float ynm2; /* y(n-2) */
  float xnm1; /* X(n-1) */
  float xnm2; /* x(n-2) */
} biquad_df1_state_t;



/****************************
 *  
 *  filter state for direct form II:
 *
 *     w(n) = x(n) - b1 * w(n-1) - b2 * w(n-2)
 *     y(n) = a0 * w(n) + a1 * w(n-1) + a2 * w(n-2)
 *
 */

typedef struct
{
  float wnm1; /* w(n-1) */
  float wnm2; /* w(n-2) */
} biquad_df2_state_t;


/****************************
 *  
 *  FTL biquad filter functions
 *
 *    ftl_biquad_df1
 *      float *in
 *      float *out
 *      biquad_state_df1_t *state
 *      biquad_coefs_t *coefs
 *      long n_tick
 *
 *    ftl_biquad_df2
 *      float *in
 *      float *out
 *      biquad_state_df2_t *state
 *      biquad_coefs_t *coefs
 *      long n_tick
 *
 *
 */

extern void ftl_biquad_df1(fts_word_t *argv); /* direct form I */
extern void ftl_biquad_df2(fts_word_t *argv); /* direct form II */

