#include "lang/veclib/include/vecdef.h"


/***************************************************
 *
 *    fill/copy vector
 *
 */

DECLARE_VEC_FUN(FUN, (fcpy, float, float, FUN_cpy))
DECLARE_VEC_FUN(FUN, (icpy, int, int, FUN_cpy))
DECLARE_VEC_FUN(CFUN, (ccpy, complex, complex, FUN_ccpy))

DECLARE_VEC_FUN(FUN, (fcpyre, float, complex, FUN_fcpyre))
DECLARE_VEC_FUN(FUN, (fcpyim, float, complex, FUN_fcpyim))

DECLARE_VEC_FUN(FILL, (ffill, float, float, FUN_cpy))
DECLARE_VEC_FUN(FILL, (ifill, int, int, FUN_cpy))
DECLARE_VEC_FUN(FILL, (cfill, complex, complex, FUN_ccpy))
DECLARE_VEC_FUN(FILL, (ffillre, float, complex, FUN_fcpyre))
DECLARE_VEC_FUN(FILL, (ffillim, float, complex, FUN_fcpyim))

DECLARE_VEC_FUN(FUN, (re, complex, float, FUN_re))
DECLARE_VEC_FUN(FUN, (im, complex, float, FUN_im))

DECLARE_VEC_FUN(SET, (fzero, float, FUN_zero))
DECLARE_VEC_FUN(SET, (izero, int, FUN_zero))
DECLARE_VEC_FUN(SET, (czero, complex, FUN_czero))

/***************************************************
 *
 *		float/int conversion 
 *
 */

DECLARE_VEC_FUN(FUN, (fcasti, float, int, FUN_cpy))
DECLARE_VEC_FUN(FUN, (icastf, int, float, FUN_cpy))
