/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VEC_OP(icmp_eq, int, int, int, COMP_eq)
DEFINE_FTS_VEC_OP(icmp_neq, int, int, int, COMP_neq)
DEFINE_FTS_VEC_OP(icmp_leq, int, int, int, COMP_leq)
DEFINE_FTS_VEC_OP(icmp_geq, int, int, int, COMP_geq)
DEFINE_FTS_VEC_OP(icmp_lt, int, int, int, COMP_lt)
DEFINE_FTS_VEC_OP(icmp_gt, int, int, int, COMP_gt)


DEFINE_FTL_VEC_OP(icmp_eq, int, int, int, COMP_eq)
DEFINE_FTL_VEC_OP(icmp_neq, int, int, int, COMP_neq)
DEFINE_FTL_VEC_OP(icmp_leq, int, int, int, COMP_leq)
DEFINE_FTL_VEC_OP(icmp_geq, int, int, int, COMP_geq)
DEFINE_FTL_VEC_OP(icmp_lt, int, int, int, COMP_lt)
DEFINE_FTL_VEC_OP(icmp_gt, int, int, int, COMP_gt)

