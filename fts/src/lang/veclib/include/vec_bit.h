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
/* vecbit.h */

#include "lang/veclib/include/vecdef.h"


/**********************************************************
 *
 *		logic inversion
 *
 */

DECLARE_VEC_FUN(FUN, (bitnot, int, int, FUN_bitnot))


/**********************************************************
 *
 *		vector/vector logic and shift
 *
 */

DECLARE_VEC_FUN(OP, (bitand, int, int, int, OP_bitand))
DECLARE_VEC_FUN(OP, (bitor, int, int, int, OP_bitor))
DECLARE_VEC_FUN(OP, (bitxor, int, int, int, OP_bitxor))

DECLARE_VEC_FUN(OP, (bitshl, int, int, int, OP_bitshl))
DECLARE_VEC_FUN(OP, (bitshr, int, int, int, OP_bitshr))


/**********************************************************
 *
 *		vector/scalar logic and shift
 *
 */

DECLARE_VEC_FUN(OPSCL, (scl_bitand, int, int, int, OP_bitand))
DECLARE_VEC_FUN(OPSCL, (scl_bitor, int, int, int, OP_bitor))
DECLARE_VEC_FUN(OPSCL, (scl_bitxor, int, int, int, OP_bitxor))

DECLARE_VEC_FUN(OPSCL, (scl_bitshl, int, int, int, OP_bitshl))
DECLARE_VEC_FUN(OPSCL, (scl_bitshr, int, int, int, OP_bitshr))
