/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
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

/* vecarith_int.c */
DECLARE_VEC_FUN(OP, (iadd, int, int, int, OP_add))
DECLARE_VEC_FUN(OP, (isub, int, int, int, OP_sub))
DECLARE_VEC_FUN(OP, (imul, int, int, int, OP_mul))
DECLARE_VEC_FUN(OP, (idiv, int, int, int, OP_div))
DECLARE_VEC_FUN(OP, (ibus, int, int, int, OP_bus))
DECLARE_VEC_FUN(OP, (ivid, int, int, int, OP_vid))
 
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
 
/* vecarith_int.c */
DECLARE_VEC_FUN(OPSCL, (scl_iadd, int, int, int, OP_add))
DECLARE_VEC_FUN(OPSCL, (scl_isub, int, int, int, OP_sub))
DECLARE_VEC_FUN(OPSCL, (scl_imul, int, int, int, OP_mul))
DECLARE_VEC_FUN(OPSCL, (scl_idiv, int, int, int, OP_div))
DECLARE_VEC_FUN(OPSCL, (scl_ibus, int, int, int, OP_bus))
DECLARE_VEC_FUN(OPSCL, (scl_ivid, int, int, int, OP_vid))
 
/* vecarith_complex.c */
DECLARE_VEC_FUN(OPSCL, (scl_cadd, complex, complex, complex, OP_cadd))
DECLARE_VEC_FUN(OPSCL, (scl_csub, complex, complex, complex, OP_scl_sub))
DECLARE_VEC_FUN(OPSCL, (scl_cmul, complex, complex, complex, OP_cmul))
DECLARE_VEC_FUN(OPSCL, (scl_cdiv, complex, complex, complex, OP_cdiv))
DECLARE_VEC_FUN(OPSCL, (scl_cbus, complex, complex, complex, OP_cbus))
DECLARE_VEC_FUN(OPSCL, (scl_cvid, complex, complex, complex, OP_cvid))
DECLARE_VEC_FUN(OPSCL, (scl_cfmul, complex, float, complex, OP_cfmul))

