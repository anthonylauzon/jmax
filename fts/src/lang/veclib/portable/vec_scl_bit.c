#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VEC_OPSCL(scl_bitand, long, long, long, OP_bitand)
DEFINE_FTS_VEC_OPSCL(scl_bitor, long, long, long, OP_bitor)
DEFINE_FTS_VEC_OPSCL(scl_bitxor, long, long, long, OP_bitxor)
DEFINE_FTS_VEC_OPSCL(scl_bitshl, long, long, long, OP_bitshl)
DEFINE_FTS_VEC_OPSCL(scl_bitshr, long, long, long, OP_bitshr)


DEFINE_FTL_VEC_OPSCL(scl_bitand, long, long, long, OP_bitand)
DEFINE_FTL_VEC_OPSCL(scl_bitor, long, long, long, OP_bitor)
DEFINE_FTL_VEC_OPSCL(scl_bitxor, long, long, long, OP_bitxor)
DEFINE_FTL_VEC_OPSCL(scl_bitshl, long, long, long, OP_bitshl)
DEFINE_FTL_VEC_OPSCL(scl_bitshr, long, long, long, OP_bitshr)
