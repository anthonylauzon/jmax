/* vecarith.h */

#include "lang/veclib/include/vecdef.h"

/**********************************************************
 *
 *		vector/vector arithmetic
 *
 */

/* vecarith_float.c */
DECLARE_VEC_FUN(OP, (fadd, float, float, float, OP_add))
DECLARE_VEC_FUN(OP, (fsub, float, float, float, OP_sub))
DECLARE_VEC_FUN(OP, (fmul, float, float, float, OP_mul))
DECLARE_VEC_FUN(OP, (fdiv, float, float, float, OP_div))
DECLARE_VEC_FUN(OP, (fbus, float, float, float, OP_bus))
DECLARE_VEC_FUN(OP, (fvid, float, float, float, OP_vid))
 
DECLARE_VEC_FUN(DOT, (fdot, float, float, float, OP_dot, 0))

/* vecarith_long.c */
DECLARE_VEC_FUN(OP, (iadd, long, long, long, OP_add))
DECLARE_VEC_FUN(OP, (isub, long, long, long, OP_sub))
DECLARE_VEC_FUN(OP, (imul, long, long, long, OP_mul))
DECLARE_VEC_FUN(OP, (idiv, long, long, long, OP_div))
DECLARE_VEC_FUN(OP, (ibus, long, long, long, OP_bus))
DECLARE_VEC_FUN(OP, (ivid, long, long, long, OP_vid))
 
/* vecarith_complex.c */
DECLARE_VEC_FUN(COP, (cadd, complex, complex, complex, OP_cadd))
DECLARE_VEC_FUN(COP, (csub, complex, complex, complex, OP_csub))
DECLARE_VEC_FUN(COP, (cmul, complex, complex, complex, OP_cmul))
DECLARE_VEC_FUN(COP, (cdiv, complex, complex, complex, OP_cdiv))
DECLARE_VEC_FUN(COP, (cbus, complex, complex, complex, OP_cbus))
DECLARE_VEC_FUN(COP, (cvid, complex, complex, complex, OP_cvid))

DECLARE_VEC_FUN(OP, (cfmul, complex, float, complex, OP_cfmul))


/**********************************************************
 *
 *		vector/scalar arithmetic
 *
 */

/* vecarith_float.c */
DECLARE_VEC_FUN(OPSCL, (scl_fadd, float, float, float, OP_add))
DECLARE_VEC_FUN(OPSCL, (scl_fsub, float, float, float, OP_sub))
DECLARE_VEC_FUN(OPSCL, (scl_fmul, float, float, float, OP_mul))
DECLARE_VEC_FUN(OPSCL, (scl_fdiv, float, float, float, OP_div))
DECLARE_VEC_FUN(OPSCL, (scl_fbus, float, float, float, OP_bus))
DECLARE_VEC_FUN(OPSCL, (scl_fvid, float, float, float, OP_vid))
 
/* vecarith_long.c */
DECLARE_VEC_FUN(OPSCL, (scl_iadd, long, long, long, OP_add))
DECLARE_VEC_FUN(OPSCL, (scl_isub, long, long, long, OP_sub))
DECLARE_VEC_FUN(OPSCL, (scl_imul, long, long, long, OP_mul))
DECLARE_VEC_FUN(OPSCL, (scl_idiv, long, long, long, OP_div))
DECLARE_VEC_FUN(OPSCL, (scl_ibus, long, long, long, OP_bus))
DECLARE_VEC_FUN(OPSCL, (scl_ivid, long, long, long, OP_vid))
 
/* vecarith_complex.c */
DECLARE_VEC_FUN(OPSCL, (scl_cadd, complex, complex, complex, OP_cadd))
DECLARE_VEC_FUN(OPSCL, (scl_csub, complex, complex, complex, OP_scl_sub))
DECLARE_VEC_FUN(OPSCL, (scl_cmul, complex, complex, complex, OP_cmul))
DECLARE_VEC_FUN(OPSCL, (scl_cdiv, complex, complex, complex, OP_cdiv))
DECLARE_VEC_FUN(OPSCL, (scl_cbus, complex, complex, complex, OP_cbus))
DECLARE_VEC_FUN(OPSCL, (scl_cvid, complex, complex, complex, OP_cvid))
DECLARE_VEC_FUN(OPSCL, (scl_cfmul, complex, float, complex, OP_cfmul))

