/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* define/cop.h */

#include <fts/lang/veclib/include/unroll.h>
#include <fts/lang/veclib/include/unwrap.h>



/***************************************************
 *
 *    vector operations
 *
 *      COMPLEX (x) COMPLEX -> COMPLEX / REAL
 *      prefetches complex elements of both input vectors to temporary variables (hopefully registers)
 *      --> unrolls just by a quarter of VECLIB_LOOP_UNROLL
 *
 */
 
#define NO_UNROLL_COP(op) op(in0_0, in1_0, out[i]);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_COP_Q(op)\
  op(in0_0, in1_0, out[i]);

#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_COP_Q(op)\
  op(in0_0, in1_0, out[i]);\
  op(in0_1, in1_1, out[i+1]);

#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_COP_Q(op)\
  op(in0_0, in1_0, out[i]);\
  op(in0_1, in1_1, out[i+1]);\
  op(in0_2, in1_2, out[i+2]);\
  op(in0_3, in1_3, out[i+3]);
 
#else

#define UNROLL_COP_Q(op) NO_UNROLL_COP(op)

#endif


/***************************************************
 *
 *    function bodies (complex/complex)
 *
 */
 
#define FTS_VEC_COP(t_in0, t_in1, t_out, op)\
 NO_UNROLL_LOOP(\
  NO_UNROLL_REGS(t_in0, in0)\
  NO_UNROLL_REGS(t_in1, in1)\
  NO_UNROLL_2CIN(in0, in1)\
  NO_UNROLL_COP(op)\
 )

#define FTS_VECX_COP(t_in0, t_in1, t_out, op)\
 UNROLL_LOOP_Q(\
  UNROLL_REGS_Q(t_in0, in0)\
  UNROLL_REGS_Q(t_in1, in1)\
  UNROLL_2CIN_Q(in0, in1)\
  UNROLL_COP_Q(op)\
 )

#define FTL_UNWRAP_COP(t_in0, t_in1, t_out)\
 UNWRAP_VEC(0, t_in0, in0)\
 UNWRAP_VEC(1, t_in1, in1)\
 UNWRAP_VEC(2, t_out, out)\
 UNWRAP_SIZE(3)


/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_COP(name, t_in0, t_in1, t_out, op)\
 void fts_vec_ ## name(t_in0 *in0, t_in1 *in1, t_out *out, int size)\
 {FTS_VEC_COP(t_in0, t_in1, t_out, op)}
  
#define DEFINE_FTS_VECX_COP(name, t_in0, t_in1, t_out, op)\
 void fts_vecx_ ## name(t_in0 *in0, t_in1 *in1, t_out *out, int size)\
 {FTS_VECX_COP(t_in0, t_in1, t_out, op)}

#define DEFINE_FTL_VEC_COP(name, t_in0, t_in1, t_out, op)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_COP(t_in0, t_in1, t_out)\
  FTS_VEC_COP(t_in0, t_in1, t_out, op)\
 }

#define DEFINE_FTL_VECX_COP(name, t_in0, t_in1, t_out, op)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_COP(t_in0, t_in1, t_out)\
  FTS_VECX_COP(t_in0, t_in1, t_out, op)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_COP(name, t_in0, t_in1, t_out, op)\
 FTS_API void fts_vec_ ## name(t_in0 *in0, t_in1 *in1, t_out *out, int size);
 
#define DECLARE_FTS_VECX_COP(name, t_in0, t_in1, t_out, op)\
 FTS_API void fts_vecx_ ## name(t_in0 *in0, t_in1 *in1, t_out *out, int size);
 
#define DECLARE_FTL_VEC_COP(name, t_in0, t_in1, t_out, op)\
 FTS_API void ftl_vec_ ## name(FTL_ARG);

#define DECLARE_FTL_VECX_COP(name, t_in0, t_in1, t_out, op)\
 FTS_API void ftl_vecx_ ## name(FTL_ARG);


