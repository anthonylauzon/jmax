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

DEFINE_FTS_VECX_FUN(fcpy, float, float, FUN_cpy)
DEFINE_FTS_VECX_FUN(icpy, int, int, FUN_cpy)
DEFINE_FTS_VECX_CFUN(ccpy, complex, complex, FUN_ccpy)
DEFINE_FTS_VECX_FUN(fcpyre, float, complex, FUN_fcpyre)
DEFINE_FTS_VECX_FUN(fcpyim, float, complex, FUN_fcpyim)

DEFINE_FTS_VECX_FILL(ffill, float, float, FUN_cpy)
DEFINE_FTS_VECX_FILL(ifill, int, int, FUN_cpy)
DEFINE_FTS_VECX_FILL(cfill, complex, complex, FUN_ccpy)
DEFINE_FTS_VECX_FILL(ffillre, float, complex, FUN_fcpyre)
DEFINE_FTS_VECX_FILL(ffillim, float, complex, FUN_fcpyim)

DEFINE_FTS_VECX_FUN(re, complex, float, FUN_re)
DEFINE_FTS_VECX_FUN(im, complex, float, FUN_im)

DEFINE_FTS_VECX_SET(fzero, float, FUN_zero)
DEFINE_FTS_VECX_SET(izero, int, FUN_zero)
DEFINE_FTS_VECX_SET(czero, complex, FUN_czero)

DEFINE_FTS_VECX_FUN(fcasti, float, int, FUN_cpy)
DEFINE_FTS_VECX_FUN(icastf, int, float, FUN_cpy)




DEFINE_FTL_VECX_FUN(fcpy, float, float, FUN_cpy)
DEFINE_FTL_VECX_FUN(icpy, int, int, FUN_cpy)
DEFINE_FTL_VECX_CFUN(ccpy, complex, complex, FUN_ccpy) /* _CFUN: prefetches re and im! */
DEFINE_FTL_VECX_FUN(fcpyre, float, complex, FUN_fcpyre)
DEFINE_FTL_VECX_FUN(fcpyim, float, complex, FUN_fcpyim)

DEFINE_FTL_VECX_FILL(ffill, float, float, FUN_cpy)
DEFINE_FTL_VECX_FILL(ifill, int, int, FUN_cpy)
DEFINE_FTL_VECX_FILL(cfill, complex, complex, FUN_ccpy)
DEFINE_FTL_VECX_FILL(ffillre, float, complex, FUN_fcpyre)
DEFINE_FTL_VECX_FILL(ffillim, float, complex, FUN_fcpyim)

DEFINE_FTL_VECX_FUN(re, complex, float, FUN_re)
DEFINE_FTL_VECX_FUN(im, complex, float, FUN_im)

DEFINE_FTL_VECX_SET(fzero, float, FUN_zero)
DEFINE_FTL_VECX_SET(izero, int, FUN_zero)
DEFINE_FTL_VECX_SET(czero, complex, FUN_czero)

DEFINE_FTL_VECX_FUN(fcasti, float, int, FUN_cpy)
DEFINE_FTL_VECX_FUN(icastf, int, float, FUN_cpy)
