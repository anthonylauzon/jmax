/* veccomp.h */

#include "lang/veclib/include/vecdef.h"


/**********************************************************
 *
 *		vector/vector comparisons
 *
 */

/* veccomp_float.c */
DECLARE_VEC_FUN(OP, (fcmp_eq, float, float, long, COMP_eq))
DECLARE_VEC_FUN(OP, (fcmp_neq, float, float, long, COMP_neq))
DECLARE_VEC_FUN(OP, (fcmp_leq, float, float, long, COMP_leq))
DECLARE_VEC_FUN(OP, (fcmp_geq, float, float, long, COMP_geq))
DECLARE_VEC_FUN(OP, (fcmp_le, float, float, long, COMP_le))
DECLARE_VEC_FUN(OP, (fcmp_gr, float, float, long, COMP_gr))
 
/* veccomp_long.c */
DECLARE_VEC_FUN(OP, (icmp_eq, long, long, long, COMP_eq))
DECLARE_VEC_FUN(OP, (icmp_neq, long, long, long, COMP_neq))
DECLARE_VEC_FUN(OP, (icmp_leq, long, long, long, COMP_leq))
DECLARE_VEC_FUN(OP, (icmp_geq, long, long, long, COMP_geq))
DECLARE_VEC_FUN(OP, (icmp_le, long, long, long, COMP_le))
DECLARE_VEC_FUN(OP, (icmp_gr, long, long, long, COMP_gr))


/**********************************************************
 *
 *		vector/scalar comparisons
 *
 */

/* veccomp_float.c */
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_eq, float, float, long, COMP_eq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_neq, float, float, long, COMP_neq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_leq, float, float, long, COMP_leq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_geq, float, float, long, COMP_geq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_le, float, float, long, COMP_le))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_gr, float, float, long, COMP_gr))
 
/* veccomp_long.c */
DECLARE_VEC_FUN(OPSCL, (scl_icmp_eq, long, long, long, COMP_eq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_neq, long, long, long, COMP_neq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_leq, long, long, long, COMP_leq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_geq, long, long, long, COMP_geq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_le, long, long, long, COMP_le))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_gr, long, long, long, COMP_gr))

