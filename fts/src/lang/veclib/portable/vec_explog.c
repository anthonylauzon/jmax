#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VEC_FUN(exp, float, float, FUN_exp)
DEFINE_FTS_VEC_FUN(log, float, float, FUN_log)
DEFINE_FTS_VEC_FUN(log10, float, float, FUN_log10)
DEFINE_FTS_VEC_OPSCL(expb, float, float, float, FUN_expb)
DEFINE_FTS_VEC_OPSCL(logb, float, float, float, FUN_logb)


DEFINE_FTL_VEC_FUN(exp, float, float, FUN_exp)
DEFINE_FTL_VEC_FUN(log, float, float, FUN_log)
DEFINE_FTL_VEC_FUN(log10, float, float, FUN_log10)
DEFINE_FTL_VEC_OPSCL(expb, float, float, float, FUN_expb)
DEFINE_FTL_VEC_OPSCL(logb, float, float, float, FUN_logb)

