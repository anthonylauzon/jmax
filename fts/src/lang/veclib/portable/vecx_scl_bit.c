#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VECX_OPSCL(scl_bitand, int, int, int, OP_bitand)
DEFINE_FTS_VECX_OPSCL(scl_bitor, int, int, int, OP_bitor)
DEFINE_FTS_VECX_OPSCL(scl_bitxor, int, int, int, OP_bitxor)

DEFINE_FTS_VECX_OPSCL(scl_bitshl, int, int, int, OP_bitshl)
DEFINE_FTS_VECX_OPSCL(scl_bitshr, int, int, int, OP_bitshr)



DEFINE_FTL_VECX_OPSCL(scl_bitand, int, int, int, OP_bitand)
DEFINE_FTL_VECX_OPSCL(scl_bitor, int, int, int, OP_bitor)
DEFINE_FTL_VECX_OPSCL(scl_bitxor, int, int, int, OP_bitxor)

DEFINE_FTL_VECX_OPSCL(scl_bitshl, int, int, int, OP_bitshl)
DEFINE_FTL_VECX_OPSCL(scl_bitshr, int, int, int, OP_bitshr)
