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

DEFINE_FTS_VEC_COP(cadd, complex, complex, complex, OP_cadd)
DEFINE_FTS_VEC_COP(csub, complex, complex, complex, OP_csub)
DEFINE_FTS_VEC_COP(cmul, complex, complex, complex, OP_cmul)
DEFINE_FTS_VEC_COP(cdiv, complex, complex, complex, OP_cdiv)
DEFINE_FTS_VEC_COP(cbus, complex, complex, complex, OP_cbus)
DEFINE_FTS_VEC_COP(cvid, complex, complex, complex, OP_cvid)

DEFINE_FTL_VEC_COP(cadd, complex, complex, complex, OP_cadd)
DEFINE_FTL_VEC_COP(csub, complex, complex, complex, OP_csub)
DEFINE_FTL_VEC_COP(cmul, complex, complex, complex, OP_cmul)
DEFINE_FTL_VEC_COP(cdiv, complex, complex, complex, OP_cdiv)
DEFINE_FTL_VEC_COP(cbus, complex, complex, complex, OP_cbus)
DEFINE_FTL_VEC_COP(cvid, complex, complex, complex, OP_cvid)
