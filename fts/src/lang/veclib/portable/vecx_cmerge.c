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

#include <fts/sys.h>
#include <fts/lang/veclib/include/vecdef.h>

/***************************************************
 *
 *    loop unrolling
 *
 */
 
#define NO_UNROLL_CMERGE\
  (out_0.re = in0[i+0], out_0.im = in1[i+0]);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_CMERGE_H\
  (out_0.re = in0[i+0], out_0.im = in1[i+0]);\
  (out_1.re = in0[i+1], out_1.im = in1[i+1]);
  
#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_CMERGE_H\
  (out_0.re = in0[i+0], out_0.im = in1[i+0]);\
  (out_1.re = in0[i+1], out_1.im = in1[i+1]);\
  (out_2.re = in0[i+2], out_2.im = in1[i+2]);\
  (out_3.re = in0[i+3], out_3.im = in1[i+3]);
  
#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_CMERGE_H\
  (out_0.re = in0[i+0], out_0.im = in1[i+0]);\
  (out_1.re = in0[i+1], out_1.im = in1[i+1]);\
  (out_2.re = in0[i+2], out_2.im = in1[i+2]);\
  (out_3.re = in0[i+3], out_3.im = in1[i+3]);\
  (out_4.re = in0[i+4], out_4.im = in1[i+4]);\
  (out_5.re = in0[i+5], out_5.im = in1[i+5]);\
  (out_6.re = in0[i+6], out_6.im = in1[i+6]);\
  (out_7.re = in0[i+7], out_7.im = in1[i+7]);
 
#else

#define UNROLL_CMERGE_H NO_UNROLL_CMERGE

#endif

/***************************************************
 *
 *    function body
 *
 */
 
#define FTS_VECX_CMERGE(in0, in1, out) \
 UNROLL_LOOP_H(\
  UNROLL_REGS_H(complex, out)\
  UNROLL_CMERGE_H\
  UNROLL_COUT_H(out)\
 )\

/***************************************************
 *
 *    function definition
 *
 */
 
void fts_vecx_cmerge(float *in0, float *in1, complex *out, int size)
{
  FTS_VECX_CMERGE(in0, in1, out)
}

void ftl_vecx_cmerge(FTL_ARG)
{
  UNWRAP_VEC(0, float, in0)
  UNWRAP_VEC(1, float, in1)
  UNWRAP_VEC(2, complex, out)
  UNWRAP_SIZE(3)
  FTS_VECX_CMERGE(in0, in1, out)
}
