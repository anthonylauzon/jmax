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

DEFINE_FTS_VEC_OPSCL(scl_fadd, float, float, float, OP_add)
DEFINE_FTS_VEC_OPSCL(scl_fsub, float, float, float, OP_sub)
DEFINE_FTS_VEC_OPSCL(scl_fmul, float, float, float, OP_mul)
DEFINE_FTS_VEC_OPSCL(scl_fdiv, float, float, float, OP_div)
DEFINE_FTS_VEC_OPSCL(scl_fbus, float, float, float, OP_bus)
DEFINE_FTS_VEC_OPSCL(scl_fvid, float, float, float, OP_vid)


DEFINE_FTL_VEC_OPSCL(scl_fadd, float, float, float, OP_add)
DEFINE_FTL_VEC_OPSCL(scl_fsub, float, float, float, OP_sub)
DEFINE_FTL_VEC_OPSCL(scl_fmul, float, float, float, OP_mul)
DEFINE_FTL_VEC_OPSCL(scl_fdiv, float, float, float, OP_div)
DEFINE_FTL_VEC_OPSCL(scl_fbus, float, float, float, OP_bus)
DEFINE_FTL_VEC_OPSCL(scl_fvid, float, float, float, OP_vid)
