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

/* kernels/op.h */

#ifndef _KERNELSOP_H_
#define _KERNELSOP_H_

#include <fts/lang/veclib/complex.h>


/***************************************************
 *
 *   real two operant operations and comparisons
 *
 *   real arithmetic (real real -> real):
 *      OP_add ... addition: (out) = in0 + in1
 *      OP_sub ... subtraction: (out) = in0 - in1
 *      OP_mul ... multiplication: (out) = in0 * in1
 *      OP_div ... division: (out) = in0 / in1
 *      OP_bus ... swapped subtraction: (out) = in1 - in0
 *      OP_vid ... swapped division: (out) = in1 / in0
 *
 *    dot product:
 *      OP_dot ... float vector dot product (real real -> real scalar)
 *
 *   logical operators (integral integral -> integral):
 *      OP_bitand ... (ANSI C operator &)
 *      OP_bitor ... (ANSI C operator |)
 *      OP_bitxor ... (ANSI C operator ^)
 *
 *   shifting operators (integral -> integral):
 *      OP_bitshr ... (ANSI C operator >>)
 *      OP_bitshl ... (ANSI C operator <<)
 *
 *   comparisons (real real -> real):
 *     COMP_eq ... equal: (out) = (in0 == in1)
 *     COMP_neq ... not equal: (out) = (in0 != in1)
 *     COMP_leq ... less or equal: (out) = (in0 <= in1)
 *     COMP_geq ... greater or equal: (out) = (in0 >= in1)
 *     COMP_lt ... less: (out) = (in0 < in1)
 *     COMP_gt ... greater: (out) = (in0 > in1)
 *
 *
 *   complex two operant operations
 *
 *   complex arithmetic (complex complex -> complex):
 *      OP_cadd ... complex addition: (out) = in0 + in1
 *      OP_csub ... complex subtraction: (out) = in0 - in1
 *      OP_cmul ... complex multiplication: (out) = in0 * in1
 *      OP_cdiv ... complex division: (out) = in0 / in1
 *      OP_cbus ... swapped complex subtraction: (out) = in1 - in0
 *      OP_cvid ... swapped complex division: (out) = in1 / in0
 *
 *   misc:
 *     OP_cfmul ... multiply complex by real (complex real -> complex)
 *
 */
 
 
/***************************************************
 *
 *    real arithmetic
 *
 */

#define OP_add(in0, in1, out) (out) = (in0) + (in1)
#define OP_sub(in0, in1, out) (out) = (in0) - (in1)
#define OP_mul(in0, in1, out) (out) = (in0) * (in1)
#define OP_div(in0, in1, out) (out) = (in0) / (in1)
#define OP_bus(in0, in1, out) (out) = (in1) - (in0)
#define OP_vid(in0, in1, out) (out) = (in1) / (in0)


/***************************************************
 *
 *    dot product
 *
 */

#define OP_dot(in0, in1, out) ((out) += (in0) * (in1))


/***************************************************
 *
 *   logical operators (integral integral -> integral):
 *
 */
 
#define OP_bitand(in0, in1, out) ((out) = (in1) & (in0))
#define OP_bitor(in0, in1, out) ((out) = (in1) | (in0))
#define OP_bitxor(in0, in1, out) ((out) = (in1) ^ (in0))


/***************************************************
 *
 *   shifting operators (integral -> integral)
 *
 */
 
#define OP_bitshr(in0, in1, out) ((out) = (in0) >> (in1))
#define OP_bitshl(in0, in1, out) ((out) = (in0) << (in1))

 
/***************************************************
 *
 *    comparison (real real -> real)
 *
 */

#define COMP_eq(in0, in1, out) ((out) = (in0) == (in1))
#define COMP_neq(in0, in1, out) ((out) = (in0) != (in1))
#define COMP_leq(in0, in1, out) ((out) = (in0) <= (in1))
#define COMP_geq(in0, in1, out) ((out) = (in0) >= (in1))
#define COMP_lt(in0, in1, out) ((out) = (in0) < (in1))
#define COMP_gt(in0, in1, out) ((out) = (in0) > (in1))


/***************************************************
 *
 *    complex arithmetic
 *
 */

#define OP_cadd(in0, in1, out) ((out).re = (in0).re + (in1).re, (out).im = (in0).im + (in1).im)
#define OP_csub(in0, in1, out) ((out).re = (in0).re - (in1).re, (out).im = (in0).im - (in1).im)
#define OP_cmul(in0, in1, out)\
 ((out).re = (in0).re * (in1).re - (in0).im * (in1).im, (out).im = (in0).re * (in1).im + (in0).re * (in1).im)
#define OP_cdiv(in0, in1, out)\
 {float c = 1. / (in1).re * (in1).re + (in1).im * (in1).im;\
  (out).re = c * ((in0).re * (in1).re + (in0).im * (in1).im);\
  (out).im = c * ((in0).im * (in1).re - (in0).re * (in1).im);}
#define OP_cbus(in0, in1, out) ((out).re = (in1).re - (in0).re, (out).im = (in1).im - (in0).im)
#define OP_cvid(in0, in1, out)\
 {float c = 1. / ((in0).re * (in0).re + (in0).im * (in0).im);\
  (out).re = c * ((in1).re * (in0).re + (in1).im * (in0).im);\
  (out).im = c * ((in1).im * (in0).re - (in1).re * (in0).im);}


/***************************************************
 *
 *    mixed operations (complex float -> complex)
 *
 */

#define OP_cfmul(in0, in1, out)\
 ((out).re = (in0).re * (in1), (out).im = (in0).im * (in1))


#endif /* _KERNELSOP_H_ */
