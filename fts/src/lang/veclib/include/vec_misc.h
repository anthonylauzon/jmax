#include "lang/veclib/include/vecdef.h"

/* min, max element of vector */

DECLARE_VEC_FUN(VECSCL, (fmin, float, float, FUN_min, *ptr))
DECLARE_VEC_FUN(VECSCL, (imin, int, int, FUN_min, *ptr))

DECLARE_VEC_FUN(VECSCL, (fmax, float, float, FUN_max, *ptr))
DECLARE_VEC_FUN(VECSCL, (imax, int, int, FUN_max, *ptr))


/* sum vector elements */

DECLARE_VEC_FUN(VECSCL, (fsum, float, float, FUN_sum, 0.))
DECLARE_VEC_FUN(VECSCL, (isum, int, int, FUN_sum, 0))
DECLARE_VEC_FUN(VECSCL, (csum, complex, complex, FUN_csum, 0))


/* clip vector */

DECLARE_VEC_FUN(TWOSCL, (fclip, float, float, float, float, FUN_clip))
DECLARE_VEC_FUN(TWOSCL, (iclip, int, int, int, int, FUN_clip))

