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
#include "sys.h"
#include "lang/veclib/include/vecdef.h"

/***************************************************
 *
 *    function body
 *
 */

#define FTS_VEC_CSPLIT(in, out0, out1) \
{\
  int i;\
  for(i=0; i<size; i++){\
    out0[i] = in[i].re;\
    out1[i] = in[i].im;\
  }\
}

/***************************************************
 *
 *    function definition
 *
 */
 
void fts_vec_csplit(complex *in, float *out0, float *out1, int size)
{
  FTS_VEC_CSPLIT(in, out0, out1)
}
  
void ftl_vec_csplit(FTL_ARG)
{
  UNWRAP_VEC(0, complex, in)
  UNWRAP_VEC(1, float, out0)
  UNWRAP_VEC(2, float, out1)
  UNWRAP_SIZE(3)
  FTS_VEC_CSPLIT(in, out0, out1)
}
