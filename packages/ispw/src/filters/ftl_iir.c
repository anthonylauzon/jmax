#include "iir.h"

/****************************
 *  
 *  one pole filter:
 *
 *     y(n) = x(n) - b1 * y(n-1)
 *
 */

void
ftl_iir_1(fts_word_t *argv)
{
  float *state;
  float *coefs;
  long n_tick;
  float *x, *y;
  float b1;
  float ynm1;
  float ynp0, ynp1, ynp2, ynp3;
  long n;

  x = (float *)fts_word_get_ptr(argv); /* in0 */
  y = (float *)fts_word_get_ptr(argv+1); /* out0 */
  state = (float *)fts_word_get_ptr(argv+2);
  coefs = (float *)fts_word_get_ptr(argv+3);
  n_tick = fts_word_get_long(argv+4);

  ynm1 = state[0]; /* y(n-1) */
  b1 = coefs[0]; 

  ynp0 = x[0] - b1 * ynm1; /* y(n) */
  ynp1 = x[1] - b1 * ynp0; /* y(n+1) */
  ynp2 = x[2] - b1 * ynp1; /* y(n+2) */
  ynp3 = x[3] - b1 * ynp2; /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = x[n+0] - b1 * ynp3; /* y(n) */
    y[n-3] = ynp1;
    ynp1 = x[n+1] - b1 * ynp0; /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = x[n+2] - b1 * ynp1; /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = x[n+3] - b1 * ynp2; /* y(n+3) */
  }

  state[0] = ynp3; /* y(n-1) */

  y[n_tick-4] = ynp0;
  y[n_tick-3] = ynp1;
  y[n_tick-2] = ynp2;
  y[n_tick-1] = ynp3;
}


/****************************
 *  
 *  two pole filter:
 *
 *     y(n) = x(n) - b1 * y(n-1) - b2 * y(n-2)
 *
 */

void
ftl_iir_2(fts_word_t *argv)
{
  float *state;
  float *coefs;
  long n_tick;
  float *x, *y;
  float ynm2, ynm1;
  float b1, b2;
  float ynp0, ynp1, ynp2, ynp3;
  long n;

  x = (float *)fts_word_get_ptr(argv); /* in0 */
  y = (float *)fts_word_get_ptr(argv+1); /* out0 */
  state = (float *)fts_word_get_ptr(argv+2);
  coefs = (float *)fts_word_get_ptr(argv+3);
  n_tick = fts_word_get_long(argv+4);

  ynm2 = state[0]; /* y(n-2) */
  ynm1 = state[1]; /* y(n-1) */
  b1 = coefs[0]; 
  b2 = coefs[1]; 

  ynp0 = x[0] - b1 * ynm1 - b2 * ynm2; /* y(n) */
  ynp1 = x[1] - b1 * ynp0 - b2 * ynm1; /* y(n+1) */
  ynp2 = x[2] - b1 * ynp1 - b2 * ynp0; /* y(n+2) */
  ynp3 = x[3] - b1 * ynp2 - b2 * ynp1; /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = x[n+0] - b1 * ynp3 - b2 * ynp2; /* y(n) */
    y[n-3] = ynp1;
    ynp1 = x[n+1] - b1 * ynp0 - b2 * ynp3; /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = x[n+2] - b1 * ynp1 - b2 * ynp0; /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = x[n+3] - b1 * ynp2 - b2 * ynp1; /* y(n+3) */
  }

  y[n_tick-4] = ynp0;
  y[n_tick-3] = ynp1;
  y[n_tick-2] = state[0] = ynp2; /* y(n-2) */
  y[n_tick-1] = state[1] = ynp3; /* y(n-1) */
}

/****************************
 *  
 *  three pole filter:
 *
 *     y(n) = x(n) - b1 * y(n-1) - b2 * y(n-2) - b3 * y(n-3)
 *
 */

void
ftl_iir_3(fts_word_t *argv)
{
  float *state;
  float *coefs;
  long n_tick;
  float *x, *y;
  float ynm3, ynm2, ynm1;
  float b1, b2, b3;
  float ynp0, ynp1, ynp2, ynp3;
  long n;

  x = (float *)fts_word_get_ptr(argv); /* in0 */
  y = (float *)fts_word_get_ptr(argv+1); /* out0 */
  state = (float *)fts_word_get_ptr(argv+2);
  coefs = (float *)fts_word_get_ptr(argv+3);
  n_tick = fts_word_get_long(argv+4);

  ynm3 = state[0]; /* y(n-3) */
  ynm2 = state[1]; /* y(n-2) */
  ynm1 = state[2]; /* y(n-1) */
  b1 = coefs[0]; 
  b2 = coefs[1]; 
  b3 = coefs[2]; 

  ynp0 = x[0] - b1 * ynm1 - b2 * ynm2 - b3 * ynm3; /* y(n) */
  ynp1 = x[1] - b1 * ynp0 - b2 * ynm1 - b3 * ynm2; /* y(n+1) */
  ynp2 = x[2] - b1 * ynp1 - b2 * ynp0 - b3 * ynm1; /* y(n+2) */
  ynp3 = x[3] - b1 * ynp2 - b2 * ynp1 - b3 * ynp0; /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = x[n+0] - b1 * ynp3 - b2 * ynp2 - b3 * ynp1; /* y(n) */
    y[n-3] = ynp1;
    ynp1 = x[n+1] - b1 * ynp0 - b2 * ynp3 - b3 * ynp2; /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = x[n+2] - b1 * ynp1 - b2 * ynp0 - b3 * ynp3; /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = x[n+3] - b1 * ynp2 - b2 * ynp1 - b3 * ynp0; /* y(n+3) */
  }

  y[n_tick-4] = ynp0;
  y[n_tick-3] = state[0] = ynp1; /* y(n-3) */
  y[n_tick-2] = state[1] = ynp2; /* y(n-2) */
  y[n_tick-1] = state[2] = ynp3; /* y(n-1) */
}

/****************************
 *  
 *  four pole filter:
 *
 *     y(n) = x(n) - b1 * y(n-1) - b2 * y(n-2) - b3 * y(n-3) - b4 * y(n-4)
 *
 */

void
ftl_iir_4(fts_word_t *argv)
{
  float *state;
  float *coefs;
  long n_tick;
  float *x, *y;
  float ynm4, ynm3, ynm2, ynm1;
  float b1, b2, b3, b4;
  float ynp0, ynp1, ynp2, ynp3;
  long n;

  x = (float *)fts_word_get_ptr(argv); /* in0 */
  y = (float *)fts_word_get_ptr(argv+1); /* out0 */
  state = (float *)fts_word_get_ptr(argv+2);
  coefs = (float *)fts_word_get_ptr(argv+3);
  n_tick = fts_word_get_long(argv+4);

  ynm4 = state[0]; /* y(n-4) */
  ynm3 = state[1]; /* y(n-3) */
  ynm2 = state[2]; /* y(n-2) */
  ynm1 = state[3]; /* y(n-1) */
  b1 = coefs[0]; 
  b2 = coefs[1]; 
  b3 = coefs[2]; 
  b4 = coefs[3]; 

  ynp0 = x[0] - b1 * ynm1 - b2 * ynm2 - b3 * ynm3 - b4 * ynm4; /* y(n) */
  ynp1 = x[1] - b1 * ynp0 - b2 * ynm1 - b3 * ynm2 - b4 * ynm3; /* y(n+1) */
  ynp2 = x[2] - b1 * ynp1 - b2 * ynp0 - b3 * ynm1 - b4 * ynm2; /* y(n+2) */
  ynp3 = x[3] - b1 * ynp2 - b2 * ynp1 - b3 * ynp0 - b4 * ynm1; /* y(n+3) */

  for(n=4; n<n_tick; n+=4)
  {
    y[n-4] = ynp0;
    ynp0 = x[n+0] - b1 * ynp3 - b2 * ynp2 - b3 * ynp1 - b4 * ynp0; /* y(n) */
    y[n-3] = ynp1;
    ynp1 = x[n+1] - b1 * ynp0 - b2 * ynp3 - b3 * ynp2 - b4 * ynp1; /* y(n+1) */
    y[n-2] = ynp2;
    ynp2 = x[n+2] - b1 * ynp1 - b2 * ynp0 - b3 * ynp3 - b4 * ynp2; /* y(n+2) */
    y[n-1] = ynp3;
    ynp3 = x[n+3] - b1 * ynp2 - b2 * ynp1 - b3 * ynp0 - b4 * ynp3; /* y(n+3) */
  }

  y[n_tick-4] = state[0] = ynp0; /* y(n-4) */
  y[n_tick-3] = state[1] = ynp1; /* y(n-3) */
  y[n_tick-2] = state[2] = ynp2; /* y(n-2) */
  y[n_tick-1] = state[3] = ynp3; /* y(n-1) */
}
