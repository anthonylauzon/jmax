#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VECX_OP(icmp_eq, int, int, int, COMP_eq)
DEFINE_FTS_VECX_OP(icmp_neq, int, int, int, COMP_neq)
DEFINE_FTS_VECX_OP(icmp_leq, int, int, int, COMP_leq)
DEFINE_FTS_VECX_OP(icmp_geq, int, int, int, COMP_geq)
DEFINE_FTS_VECX_OP(icmp_lt, int, int, int, COMP_lt)
DEFINE_FTS_VECX_OP(icmp_gt, int, int, int, COMP_gt)



DEFINE_FTL_VECX_OP(icmp_eq, int, int, int, COMP_eq)
DEFINE_FTL_VECX_OP(icmp_neq, int, int, int, COMP_neq)
DEFINE_FTL_VECX_OP(icmp_leq, int, int, int, COMP_leq)
DEFINE_FTL_VECX_OP(icmp_geq, int, int, int, COMP_geq)
DEFINE_FTL_VECX_OP(icmp_lt, int, int, int, COMP_lt)
DEFINE_FTL_VECX_OP(icmp_gt, int, int, int, COMP_gt)

