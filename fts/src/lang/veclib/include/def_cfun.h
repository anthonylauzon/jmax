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

/* define/cfun.h */

#include "lang/veclib/include/unroll.h"
#include "lang/veclib/include/unwrap.h"


/***************************************************
 *
 *    vector functions
 *
 *      COMPLEX -> COMPLEX / REAL
 *      prefetches complex elements of input vector to temporary variables
 *      --> unrolls just by a half of VECLIB_LOOP_UNROLL
 *
 */
 
/***************************************************
 *
 *    loop unrolling
 *
 */
#define NO_UNROLL_CFUN(fun) fun(in_0, out[i]);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_CFUN_H(fun)\
  fun(in_0, out[i]);\
  fun(in_1, out[i+1]);
  
#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_CFUN_H(fun)\
  fun(in_0, out[i]);\
  fun(in_1, out[i+1]);\
  fun(in_2, out[i+2]);\
  fun(in_3, out[i+3]);
  
#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_CFUN_H(fun)\
  fun(in_0, out[i]);\
  fun(in_1, out[i+1]);\
  fun(in_2, out[i+2]);\
  fun(in_3, out[i+3]);\
  fun(in_4, out[i+4]);\
  fun(in_5, out[i+5]);\
  fun(in_6, out[i+6]);\
  fun(in_7, out[i+7]);
 
#else

#define UNROLL_CFUN_H(fun) NO_UNROLL_CFUN(fun)

#endif


/***************************************************
 *
 *    function bodies
 *
 */
 
#define FTS_VEC_CFUN(t_in, t_out, fun)\
 NO_UNROLL_LOOP(\
  NO_UNROLL_REGS(t_in, in)\
  NO_UNROLL_CIN(in)\
  NO_UNROLL_CFUN(fun)\
 )

#define FTS_VECX_CFUN(t_in, t_out, fun)\
 UNROLL_LOOP_H(\
  UNROLL_REGS_H(t_in, in)\
  UNROLL_CIN_H(in)\
  UNROLL_CFUN_H(fun)\
 )

#define FTL_UNWRAP_CFUN(t_in, t_out)\
 UNWRAP_VEC(0, t_in, in)\
 UNWRAP_VEC(1, t_out, out)\
 UNWRAP_SIZE(2)


/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_CFUN(name, t_in, t_out, fun)\
 void fts_vec_ ## name(t_in *in, t_out *out, int size)\
 {FTS_VEC_CFUN(t_in, t_out, fun)}
  
#define DEFINE_FTS_VECX_CFUN(name, t_in, t_out, fun)\
 void fts_vecx_ ## name(t_in *in, t_out *out, int size)\
 {FTS_VECX_CFUN(t_in, t_out, fun)}

#define DEFINE_FTL_VEC_CFUN(name, t_in, t_out, fun)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_CFUN(t_in, t_out)\
  FTS_VEC_CFUN(t_in, t_out, fun)\
 }

#define DEFINE_FTL_VECX_CFUN(name, t_in, t_out, fun)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_CFUN(t_in, t_out)\
  FTS_VECX_CFUN(t_in, t_out, fun)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_CFUN(name, t_in, t_out, fun)\
 extern void fts_vec_ ## name(t_in *in, t_out *out, int size);
 
#define DECLARE_FTS_VECX_CFUN(name, t_in, t_out, fun)\
 extern void fts_vecx_ ## name(t_in *in, t_out *out, int size);
 
#define DECLARE_FTL_VEC_CFUN(name, t_in, t_out, fun)\
 extern void ftl_vec_ ## name(FTL_ARG);

#define DECLARE_FTL_VECX_CFUN(name, t_in, t_out, fun)\
 extern void ftl_vecx_ ## name(FTL_ARG);

