/* veclib.h */

#ifndef _VECLIB_H_
#define _VECLIB_H_

/****************************************************************************
 *
 *    the VecLib vector function library
 *
 *      plattform independent API for FTS fft and arithmetic vector functions
 *      handling real and complex vectors
 *
 ****************************************************************************/

#include "lang/veclib/include/declare_fts.h"
#include "lang/veclib/include/vec_fft.h"
#include "lang/veclib/include/vec_fun.h"
#include "lang/veclib/include/vec_arith.h"
#include "lang/veclib/include/vec_cmp.h"
#include "lang/veclib/include/vec_cpy.h"
#include "lang/veclib/include/vec_bit.h"
#include "lang/veclib/include/vec_misc.h"

/* hand coded */
extern void fts_vec_csplit(complex *in, float *out0, float *out1, long size);
extern void fts_vecx_csplit(complex *in, float *out0, float *out1, long size);
extern void fts_vec_cmerge(float *in0, float *in1, complex *out, long size);
extern void fts_vecx_cmerge(float *in0, float *in1, complex *out, long size);


#endif /* _VECLIB_H_ */




