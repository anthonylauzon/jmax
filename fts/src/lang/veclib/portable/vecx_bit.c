#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VECX_FUN(bitnot, long, long, FUN_bitnot)

DEFINE_FTS_VECX_OP(bitand, long, long, long, OP_bitand)
DEFINE_FTS_VECX_OP(bitor, long, long, long, OP_bitor)
DEFINE_FTS_VECX_OP(bitxor, long, long, long, OP_bitxor)

DEFINE_FTS_VECX_OP(bitshl, long, long, long, OP_bitshl)
DEFINE_FTS_VECX_OP(bitshr, long, long, long, OP_bitshr)



DEFINE_FTL_VECX_FUN(bitnot, long, long, FUN_bitnot)

DEFINE_FTL_VECX_OP(bitand, long, long, long, OP_bitand)
DEFINE_FTL_VECX_OP(bitor, long, long, long, OP_bitor)
DEFINE_FTL_VECX_OP(bitxor, long, long, long, OP_bitxor)

DEFINE_FTL_VECX_OP(bitshl, long, long, long, OP_bitshl)
DEFINE_FTL_VECX_OP(bitshr, long, long, long, OP_bitshr)
