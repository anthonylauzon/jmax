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
#include "sys.h"
#include "lang/veclib/include/vecdef.h"

/***************************************************
 *
 *    loop unrolling
 *
 */
 
#define NO_UNROLL_CSPLIT\
  (out0_0 = in[i+0].re, out1_0 = in[i+0].im);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_CSPLIT_H\
  (out0_0 = in[i+0].re, out1_0 = in[i+0].im);\
  (out0_1 = in[i+1].re, out1_1 = in[i+1].im);
  
#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_CSPLIT_H\
  (out0_0 = in[i+0].re, out1_0 = in[i+0].im);\
  (out0_1 = in[i+1].re, out1_1 = in[i+1].im);\
  (out0_2 = in[i+2].re, out1_2 = in[i+2].im);\
  (out0_3 = in[i+3].re, out1_3 = in[i+3].im);
  
#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_CSPLIT_H\
  (out0_0 = in[i+0].re, out1_0 = in[i+0].im);\
  (out0_1 = in[i+1].re, out1_1 = in[i+1].im);\
  (out0_2 = in[i+2].re, out1_2 = in[i+2].im);\
  (out0_3 = in[i+3].re, out1_3 = in[i+3].im);\
  (out0_4 = in[i+4].re, out1_4 = in[i+4].im);\
  (out0_5 = in[i+5].re, out1_5 = in[i+5].im);\
  (out0_6 = in[i+6].re, out1_6 = in[i+6].im);\
  (out0_7 = in[i+7].re, out1_7 = in[i+7].im);
 
#else

#define UNROLL_CSPLIT_H NO_UNROLL_CSPLIT

#endif

/***************************************************
 *
 *    function body
 *
 */
 
#define FTS_VECX_CSPLIT(in, out0, out1) \
 UNROLL_LOOP_H(\
  UNROLL_REGS_H(float, out0)\
  UNROLL_REGS_H(float, out1)\
  UNROLL_CSPLIT_H\
  UNROLL_2OUT_H(out0, out1)\
 )\

/***************************************************
 *
 *    function definition
 *
 */
 
void fts_vecx_csplit(complex *in, float *out0, float *out1, int size)
{
  FTS_VECX_CSPLIT(in, out0, out1)
}

void ftl_vecx_csplit(FTL_ARG)
{
  UNWRAP_VEC(0, complex, in)
  UNWRAP_VEC(1, float, out0)
  UNWRAP_VEC(2, float, out1)
  UNWRAP_SIZE(3)
  FTS_VECX_CSPLIT(in, out0, out1)
}

