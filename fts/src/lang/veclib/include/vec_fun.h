/*
 * jMax
 * Copyright (C) 1999 by IRCAM
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* veclfun.h */

#include "lang/veclib/include/vecdef.h"


/**********************************************************
 *
 *    float functions
 *
 */

DECLARE_VEC_FUN(FUN, (sin, float, float, FUN_sin))
DECLARE_VEC_FUN(FUN, (cos, float, float, FUN_cos))
DECLARE_VEC_FUN(FUN, (tan, float, float, FUN_tan))
DECLARE_VEC_FUN(FUN, (asin, float, float, FUN_asin))
DECLARE_VEC_FUN(FUN, (acos, float, float, FUN_acos))
DECLARE_VEC_FUN(FUN, (atan, float, float, FUN_atan))
DECLARE_VEC_FUN(FUN, (sinh, float, float, FUN_sinh))
DECLARE_VEC_FUN(FUN, (cosh, float, float, FUN_cosh))
DECLARE_VEC_FUN(FUN, (tanh, float, float, FUN_tanh))

DECLARE_VEC_FUN(FUN, (exp, float, float, FUN_exp))
DECLARE_VEC_FUN(FUN, (log, float, float, FUN_log))
DECLARE_VEC_FUN(FUN, (log10, float, float, FUN_log10))
DECLARE_VEC_FUN(OPSCL, (expb, float, float, float, FUN_expb))
DECLARE_VEC_FUN(OPSCL, (logb, float, float, float, FUN_logb))

DECLARE_VEC_FUN(FUN, (sqrt, float, float, FUN_sqrt))
DECLARE_VEC_FUN(FUN, (rsqr, float, float, FUN_rsqr))


/**********************************************************
 *
 *    complex functions
 *
 */

DECLARE_VEC_FUN(CFUN, (conj, complex, complex, FUN_conj))
DECLARE_VEC_FUN(CFUN, (csqr, complex, complex, FUN_csqr))
DECLARE_VEC_FUN(CFUN, (cpolar, complex, complex, FUN_cpolar))
DECLARE_VEC_FUN(CFUN, (crect, complex, complex, FUN_crect))

DECLARE_VEC_FUN(CFUN, (cabsf, complex, float, FUN_cabsf))
DECLARE_VEC_FUN(CFUN, (csqrf, complex, float, FUN_csqrf))


/***************************************************
 *
 *    float/int conversion 
 *
 */

DECLARE_VEC_FUN(FUN, (round, float, int, FUN_round))


/***************************************************
 *
 *    absolute value
 *
 */

DECLARE_VEC_FUN(FUN, (fabs, float, float, FUN_fabs))
DECLARE_VEC_FUN(FUN, (iabs, int, int, FUN_iabs))
DECLARE_VEC_FUN(CFUN, (cabs, complex, complex, FUN_cabs))
