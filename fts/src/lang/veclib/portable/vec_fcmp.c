#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VEC_OP(fcmp_eq, float, float, long, COMP_eq)
DEFINE_FTS_VEC_OP(fcmp_neq, float, float, long, COMP_neq)
DEFINE_FTS_VEC_OP(fcmp_leq, float, float, long, COMP_leq)
DEFINE_FTS_VEC_OP(fcmp_geq, float, float, long, COMP_geq)
DEFINE_FTS_VEC_OP(fcmp_lt, float, float, long, COMP_lt)
DEFINE_FTS_VEC_OP(fcmp_gt, float, float, long, COMP_gt)


DEFINE_FTL_VEC_OP(fcmp_eq, float, float, long, COMP_eq)
DEFINE_FTL_VEC_OP(fcmp_neq, float, float, long, COMP_neq)
DEFINE_FTL_VEC_OP(fcmp_leq, float, float, long, COMP_leq)
DEFINE_FTL_VEC_OP(fcmp_geq, float, float, long, COMP_geq)
DEFINE_FTL_VEC_OP(fcmp_lt, float, float, long, COMP_lt)
DEFINE_FTL_VEC_OP(fcmp_gt, float, float, long, COMP_gt)

