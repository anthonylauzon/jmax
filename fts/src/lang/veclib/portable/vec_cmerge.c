#include "sys.h"
#include "lang/veclib/include/vecdef.h"

/***********************************************
 *
 *    plain loop version
 *
 */
 
#define FTS_VEC_CMERGE(in0, in1, out) \
{\
  long i;\
  for(i=0; i<size; i++){\
    (out[i+0].re = in0[i+0], out[i+0].im = in1[i+0]);\
  }\
}


/***************************************************
 *
 *    function definition
 *
 */
 
void fts_vec_cmerge(float *in0, float *in1, complex *out, long size)
{
  FTS_VEC_CMERGE(in0, in1, out)
}
  
void ftl_vec_cmerge(FTL_ARG)
{
  UNWRAP_VEC(0, float, in0)
  UNWRAP_VEC(1, float, in1)
  UNWRAP_VEC(2, complex, out)
  UNWRAP_SIZE(3)
  FTS_VEC_CMERGE(in0, in1, out)
}

