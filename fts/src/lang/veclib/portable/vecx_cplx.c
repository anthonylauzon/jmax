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

DEFINE_FTS_VECX_CFUN(cabs, complex, complex, FUN_cabs)
DEFINE_FTS_VECX_CFUN(conj, complex, complex, FUN_conj)
DEFINE_FTS_VECX_CFUN(csqr, complex, complex, FUN_csqr)
DEFINE_FTS_VECX_CFUN(cpolar, complex, complex, FUN_cpolar)
DEFINE_FTS_VECX_CFUN(crect, complex, complex, FUN_crect)

DEFINE_FTS_VECX_CFUN(cabsf, complex, float, FUN_cabsf)
DEFINE_FTS_VECX_CFUN(csqrf, complex, float, FUN_csqrf)



DEFINE_FTL_VECX_CFUN(cabs, complex, complex, FUN_cabs)
DEFINE_FTL_VECX_CFUN(conj, complex, complex, FUN_conj)
DEFINE_FTL_VECX_CFUN(csqr, complex, complex, FUN_csqr)
DEFINE_FTL_VECX_CFUN(cpolar, complex, complex, FUN_cpolar)
DEFINE_FTL_VECX_CFUN(crect, complex, complex, FUN_crect)

DEFINE_FTL_VECX_CFUN(cabsf, complex, float, FUN_cabsf)
DEFINE_FTL_VECX_CFUN(csqrf, complex, float, FUN_csqrf)

