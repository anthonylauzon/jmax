#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VEC_FUN(fcpy, float, float, FUN_cpy)
DEFINE_FTS_VEC_FUN(icpy, long, long, FUN_cpy)
DEFINE_FTS_VEC_CFUN(ccpy, complex, complex, FUN_ccpy) /* _CFUN: prefetches re and im! */
DEFINE_FTS_VEC_FUN(fcpyre, float, complex, FUN_fcpyre)
DEFINE_FTS_VEC_FUN(fcpyim, float, complex, FUN_fcpyim)

DEFINE_FTS_VEC_FILL(ffill, float, float, FUN_cpy)
DEFINE_FTS_VEC_FILL(ifill, long, long, FUN_cpy)
DEFINE_FTS_VEC_FILL(cfill, complex, complex, FUN_ccpy)
DEFINE_FTS_VEC_FILL(ffillre, float, complex, FUN_fcpyre)
DEFINE_FTS_VEC_FILL(ffillim, float, complex, FUN_fcpyim)

DEFINE_FTS_VEC_FUN(re, complex, float, FUN_re)
DEFINE_FTS_VEC_FUN(im, complex, float, FUN_im)

DEFINE_FTS_VEC_SET(fzero, float, FUN_zero)
DEFINE_FTS_VEC_SET(izero, long, FUN_zero)
DEFINE_FTS_VEC_SET(czero, complex, FUN_czero)

DEFINE_FTS_VEC_FUN(fcasti, float, long, FUN_cpy)
DEFINE_FTS_VEC_FUN(icastf, long, float, FUN_cpy)



DEFINE_FTL_VEC_FUN(fcpy, float, float, FUN_cpy)
DEFINE_FTL_VEC_FUN(icpy, long, long, FUN_cpy)
DEFINE_FTL_VEC_CFUN(ccpy, complex, complex, FUN_ccpy) /* _CFUN: prefetches re and im! */
DEFINE_FTL_VEC_FUN(fcpyre, float, complex, FUN_fcpyre)
DEFINE_FTL_VEC_FUN(fcpyim, float, complex, FUN_fcpyim)

DEFINE_FTL_VEC_FILL(ffill, float, float, FUN_cpy)
DEFINE_FTL_VEC_FILL(ifill, long, long, FUN_cpy)
DEFINE_FTL_VEC_FILL(cfill, complex, complex, FUN_ccpy)
DEFINE_FTL_VEC_FILL(ffillre, float, complex, FUN_fcpyre)
DEFINE_FTL_VEC_FILL(ffillim, float, complex, FUN_fcpyim)

DEFINE_FTL_VEC_FUN(re, complex, float, FUN_re)
DEFINE_FTL_VEC_FUN(im, complex, float, FUN_im)

DEFINE_FTL_VEC_SET(fzero, float, FUN_zero)
DEFINE_FTL_VEC_SET(izero, long, FUN_zero)
DEFINE_FTL_VEC_SET(czero, complex, FUN_czero)

DEFINE_FTL_VEC_FUN(fcasti, float, long, FUN_cpy)
DEFINE_FTL_VEC_FUN(icastf, long, float, FUN_cpy)
