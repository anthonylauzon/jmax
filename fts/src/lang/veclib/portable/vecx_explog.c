#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VECX_FUN(exp, float, float, FUN_exp)
DEFINE_FTS_VECX_FUN(log, float, float, FUN_log)
DEFINE_FTS_VECX_FUN(log10, float, float, FUN_log10)

DEFINE_FTS_VECX_OPSCL(expb, float, float, float, FUN_expb)
DEFINE_FTS_VECX_OPSCL(logb, float, float, float, FUN_logb)



DEFINE_FTL_VECX_FUN(exp, float, float, FUN_exp)
DEFINE_FTL_VECX_FUN(log, float, float, FUN_log)
DEFINE_FTL_VECX_FUN(log10, float, float, FUN_log10)

DEFINE_FTL_VECX_OPSCL(expb, float, float, float, FUN_expb)
DEFINE_FTL_VECX_OPSCL(logb, float, float, float, FUN_logb)

