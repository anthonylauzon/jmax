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

/* define/opscl.h */

#include <fts/lang/veclib/include/unroll.h>
#include <fts/lang/veclib/include/unwrap.h>


/***************************************************
 *
 *    vector/scalar operations
 *
 *      VECTOR (x) SCALAR -> VECTOR
 *      prestores elements of output vector in temporary variables (hopefully registers)
 *
 */
 
/***************************************************
 *
 *    unrolling
 *
 */

#define NO_UNROLL_OPSCL(op) op(in[i+0], scl, out_0);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_OPSCL(op)\
  op(in[i+0], scl, out_0);\
  op(in[i+1], scl, out_1);\
  op(in[i+2], scl, out_2);\
  op(in[i+3], scl, out_3);

#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_OPSCL(op)\
  op(in[i+0], scl, out_0);\
  op(in[i+1], scl, out_1);\
  op(in[i+2], scl, out_2);\
  op(in[i+3], scl, out_3);\
  op(in[i+4], scl, out_4);\
  op(in[i+5], scl, out_5);\
  op(in[i+6], scl, out_6);\
  op(in[i+7], scl, out_7);

#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_OPSCL(op)\
  op(in[i+0], scl, out_0);\
  op(in[i+1], scl, out_1);\
  op(in[i+2], scl, out_2);\
  op(in[i+3], scl, out_3);\
  op(in[i+4], scl, out_4);\
  op(in[i+5], scl, out_5);\
  op(in[i+6], scl, out_6);\
  op(in[i+7], scl, out_7);\
  op(in[i+8], scl, out_8);\
  op(in[i+9], scl, out_9);\
  op(in[i+10], scl, out_10);\
  op(in[i+11], scl, out_11);\
  op(in[i+12], scl, out_12);\
  op(in[i+13], scl, out_13);\
  op(in[i+14], scl, out_14);\
  op(in[i+15], scl, out_15);
 
#else

#define UNROLL_OPSCL(op) NO_UNROLL_OPSCL(op)

#endif


/***************************************************
 *
 *              function bodies
 *
 */
 
#define FTS_VEC_OPSCL(t_in, t_scl, t_out, op)\
 NO_UNROLL_LOOP(\
  NO_UNROLL_REGS(t_out, out)\
  NO_UNROLL_OPSCL(op)\
  NO_UNROLL_OUT(out)\
 )

#define FTS_VECX_OPSCL(t_in, t_scl, t_out, op)\
 UNROLL_LOOP(\
  UNROLL_REGS(t_out, out)\
  UNROLL_OPSCL(op)\
  UNROLL_OUT(out)\
 )

#define FTL_UNWRAP_OPSCL(t_in, t_scl, t_out)\
 UNWRAP_VEC(0, t_in, in)\
 UNWRAP_SCL(1, t_scl, scl)\
 UNWRAP_VEC(2, t_out, out)\
 UNWRAP_SIZE(3)
 
 
/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_OPSCL(name, t_in, t_scl, t_out, op)\
 void fts_vec_ ## name(t_in *in, t_scl scl, t_out *out, int size)\
 {FTS_VEC_OPSCL(t_in, t_scl, t_out, op)}
  
#define DEFINE_FTS_VECX_OPSCL(name, t_in, t_scl, t_out, op)\
 void fts_vecx_ ## name(t_in *in, t_scl scl, t_out *out, int size)\
 {FTS_VECX_OPSCL(t_in, t_scl, t_out, op)}

#define DEFINE_FTL_VEC_OPSCL(name, t_in, t_scl, t_out, op)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_OPSCL(t_in, t_scl, t_out)\
  FTS_VEC_OPSCL(t_in, t_scl, t_out, op)\
 }

#define DEFINE_FTL_VECX_OPSCL(name, t_in, t_scl, t_out, op)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_OPSCL(t_in, t_scl, t_out)\
  FTS_VECX_OPSCL(t_in, t_scl, t_out, op)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_OPSCL(name, t_in, t_scl, t_out, op)\
 FTS_API void fts_vec_ ## name(t_in *in, t_scl scl, t_out *out, int size);
 
#define DECLARE_FTS_VECX_OPSCL(name, t_in, t_scl, t_out, op)\
 FTS_API void fts_vecx_ ## name(t_in *in, t_scl scl, t_out *out, int size);
 
#define DECLARE_FTL_VEC_OPSCL(name, t_in, t_scl, t_out, op)\
 FTS_API void ftl_vec_ ## name(FTL_ARG);
 
#define DECLARE_FTL_VECX_OPSCL(name, t_in, t_scl, t_out, op)\
 FTS_API void ftl_vecx_ ## name(FTL_ARG);
