#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VEC_OP(iadd, long, long, long, OP_add)
DEFINE_FTS_VEC_OP(isub, long, long, long, OP_sub)
DEFINE_FTS_VEC_OP(imul, long, long, long, OP_mul)
DEFINE_FTS_VEC_OP(idiv, long, long, long, OP_div)
DEFINE_FTS_VEC_OP(ibus, long, long, long, OP_bus)
DEFINE_FTS_VEC_OP(ivid, long, long, long, OP_vid)


DEFINE_FTL_VEC_OP(iadd, long, long, long, OP_add)
DEFINE_FTL_VEC_OP(isub, long, long, long, OP_sub)
DEFINE_FTL_VEC_OP(imul, long, long, long, OP_mul)
DEFINE_FTL_VEC_OP(idiv, long, long, long, OP_div)
DEFINE_FTL_VEC_OP(ibus, long, long, long, OP_bus)
DEFINE_FTL_VEC_OP(ivid, long, long, long, OP_vid)
