/* veccomp.h */

#include "lang/veclib/include/vecdef.h"


/**********************************************************
 *
 *		vector/vector comparisons
 *
 */

/* veccomp_float.c */
DECLARE_VEC_FUN(OP, (fcmp_eq, float, float, int, COMP_eq))
DECLARE_VEC_FUN(OP, (fcmp_neq, float, float, int, COMP_neq))
DECLARE_VEC_FUN(OP, (fcmp_leq, float, float, int, COMP_leq))
DECLARE_VEC_FUN(OP, (fcmp_geq, float, float, int, COMP_geq))
DECLARE_VEC_FUN(OP, (fcmp_le, float, float, int, COMP_le))
DECLARE_VEC_FUN(OP, (fcmp_gr, float, float, int, COMP_gr))
 
/* veccomp_int.c */
DECLARE_VEC_FUN(OP, (icmp_eq, int, int, int, COMP_eq))
DECLARE_VEC_FUN(OP, (icmp_neq, int, int, int, COMP_neq))
DECLARE_VEC_FUN(OP, (icmp_leq, int, int, int, COMP_leq))
DECLARE_VEC_FUN(OP, (icmp_geq, int, int, int, COMP_geq))
DECLARE_VEC_FUN(OP, (icmp_le, int, int, int, COMP_le))
DECLARE_VEC_FUN(OP, (icmp_gr, int, int, int, COMP_gr))


/**********************************************************
 *
 *		vector/scalar comparisons
 *
 */

/* veccomp_float.c */
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_eq, float, float, int, COMP_eq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_neq, float, float, int, COMP_neq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_leq, float, float, int, COMP_leq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_geq, float, float, int, COMP_geq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_le, float, float, int, COMP_le))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_gr, float, float, int, COMP_gr))
 
/* veccomp_int.c */
DECLARE_VEC_FUN(OPSCL, (scl_icmp_eq, int, int, int, COMP_eq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_neq, int, int, int, COMP_neq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_leq, int, int, int, COMP_leq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_geq, int, int, int, COMP_geq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_le, int, int, int, COMP_le))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_gr, int, int, int, COMP_gr))

