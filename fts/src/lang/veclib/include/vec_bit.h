/* vecbit.h */

#include "lang/veclib/include/vecdef.h"


/**********************************************************
 *
 *		logic inversion
 *
 */

DECLARE_VEC_FUN(FUN, (bitnot, long, long, FUN_bitnot))


/**********************************************************
 *
 *		vector/vector logic and shift
 *
 */

DECLARE_VEC_FUN(OP, (bitand, long, long, long, OP_bitand))
DECLARE_VEC_FUN(OP, (bitor, long, long, long, OP_bitor))
DECLARE_VEC_FUN(OP, (bitxor, long, long, long, OP_bitxor))

DECLARE_VEC_FUN(OP, (bitshl, long, long, long, OP_bitshl))
DECLARE_VEC_FUN(OP, (bitshr, long, long, long, OP_bitshr))


/**********************************************************
 *
 *		vector/scalar logic and shift
 *
 */

DECLARE_VEC_FUN(OPSCL, (scl_bitand, long, long, long, OP_bitand))
DECLARE_VEC_FUN(OPSCL, (scl_bitor, long, long, long, OP_bitor))
DECLARE_VEC_FUN(OPSCL, (scl_bitxor, long, long, long, OP_bitxor))

DECLARE_VEC_FUN(OPSCL, (scl_bitshl, long, long, long, OP_bitshl))
DECLARE_VEC_FUN(OPSCL, (scl_bitshr, long, long, long, OP_bitshr))
