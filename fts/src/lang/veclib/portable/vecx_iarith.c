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

DEFINE_FTS_VECX_OP(iadd, int, int, int, OP_add)
DEFINE_FTS_VECX_OP(isub, int, int, int, OP_sub)
DEFINE_FTS_VECX_OP(imul, int, int, int, OP_mul)
DEFINE_FTS_VECX_OP(idiv, int, int, int, OP_div)
DEFINE_FTS_VECX_OP(ibus, int, int, int, OP_bus)
DEFINE_FTS_VECX_OP(ivid, int, int, int, OP_vid)



DEFINE_FTL_VECX_OP(iadd, int, int, int, OP_add)
DEFINE_FTL_VECX_OP(isub, int, int, int, OP_sub)
DEFINE_FTL_VECX_OP(imul, int, int, int, OP_mul)
DEFINE_FTL_VECX_OP(idiv, int, int, int, OP_div)
DEFINE_FTL_VECX_OP(ibus, int, int, int, OP_bus)
DEFINE_FTL_VECX_OP(ivid, int, int, int, OP_vid)
