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

DEFINE_FTS_VEC_FUN(bitnot, int, int, FUN_bitnot)
DEFINE_FTS_VEC_OP(bitand, int, int, int, OP_bitand)
DEFINE_FTS_VEC_OP(bitor, int, int, int, OP_bitor)
DEFINE_FTS_VEC_OP(bitxor, int, int, int, OP_bitxor)
DEFINE_FTS_VEC_OP(bitshl, int, int, int, OP_bitshl)
DEFINE_FTS_VEC_OP(bitshr, int, int, int, OP_bitshr)


DEFINE_FTL_VEC_FUN(bitnot, int, int, FUN_bitnot)
DEFINE_FTL_VEC_OP(bitand, int, int, int, OP_bitand)
DEFINE_FTL_VEC_OP(bitor, int, int, int, OP_bitor)
DEFINE_FTL_VEC_OP(bitxor, int, int, int, OP_bitxor)
DEFINE_FTL_VEC_OP(bitshl, int, int, int, OP_bitshl)
DEFINE_FTL_VEC_OP(bitshr, int, int, int, OP_bitshr)
