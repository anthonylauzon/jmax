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

/* veclib.h */

#ifndef _VECLIB_H_
#define _VECLIB_H_

/****************************************************************************
 *
 *    the VecLib vector function library
 *
 *      plattform independent API for FTS fft and arithmetic vector functions
 *      handling real and complex vectors
 *
 ****************************************************************************/

#include "lang/veclib/include/declare_fts.h"
#include "lang/veclib/include/vec_fft.h"

#ifndef RHAPSODY

#include "lang/veclib/include/vec_fun.h"
#include "lang/veclib/include/vec_arith.h"
#include "lang/veclib/include/vec_cmp.h"
#include "lang/veclib/include/vec_cpy.h"
#include "lang/veclib/include/vec_bit.h"
#include "lang/veclib/include/vec_misc.h"

#else

extern void fts_vec_sin(float *in, float *out, int size); extern void fts_vecx_sin(float *in, float *out, int size);
extern void fts_vec_cos(float *in, float *out, int size); extern void fts_vecx_cos(float *in, float *out, int size);
extern void fts_vec_tan(float *in, float *out, int size); extern void fts_vecx_tan(float *in, float *out, int size);
extern void fts_vec_asin(float *in, float *out, int size); extern void fts_vecx_asin(float *in, float *out, int size);
extern void fts_vec_acos(float *in, float *out, int size); extern void fts_vecx_acos(float *in, float *out, int size);
extern void fts_vec_atan(float *in, float *out, int size); extern void fts_vecx_atan(float *in, float *out, int size);
extern void fts_vec_sinh(float *in, float *out, int size); extern void fts_vecx_sinh(float *in, float *out, int size);
extern void fts_vec_cosh(float *in, float *out, int size); extern void fts_vecx_cosh(float *in, float *out, int size);
extern void fts_vec_tanh(float *in, float *out, int size); extern void fts_vecx_tanh(float *in, float *out, int size);

extern void fts_vec_exp(float *in, float *out, int size); extern void fts_vecx_exp(float *in, float *out, int size);
extern void fts_vec_log(float *in, float *out, int size); extern void fts_vecx_log(float *in, float *out, int size);
extern void fts_vec_log10(float *in, float *out, int size); extern void fts_vecx_log10(float *in, float *out, int size);
extern void fts_vec_expb(float *in, float scl, float *out, int size); extern void fts_vecx_expb(float *in, float scl, float *out, int size);
extern void fts_vec_logb(float *in, float scl, float *out, int size); extern void fts_vecx_logb(float *in, float scl, float *out, int size);

extern void fts_vec_sqrt(float *in, float *out, int size); extern void fts_vecx_sqrt(float *in, float *out, int size);
extern void fts_vec_rsqr(float *in, float *out, int size); extern void fts_vecx_rsqr(float *in, float *out, int size);






 

extern void fts_vec_conj(complex *in, complex *out, int size); extern void fts_vecx_conj(complex *in, complex *out, int size);
extern void fts_vec_csqr(complex *in, complex *out, int size); extern void fts_vecx_csqr(complex *in, complex *out, int size);
extern void fts_vec_cpolar(complex *in, complex *out, int size); extern void fts_vecx_cpolar(complex *in, complex *out, int size);
extern void fts_vec_crect(complex *in, complex *out, int size); extern void fts_vecx_crect(complex *in, complex *out, int size);

extern void fts_vec_cabsf(complex *in, float *out, int size); extern void fts_vecx_cabsf(complex *in, float *out, int size);
extern void fts_vec_csqrf(complex *in, float *out, int size); extern void fts_vecx_csqrf(complex *in, float *out, int size);






 

extern void fts_vec_round(float *in, int *out, int size); extern void fts_vecx_round(float *in, int *out, int size);






 

extern void fts_vec_fabs(float *in, float *out, int size); extern void fts_vecx_fabs(float *in, float *out, int size);
extern void fts_vec_iabs(int *in, int *out, int size); extern void fts_vecx_iabs(int *in, int *out, int size);
extern void fts_vec_cabs(complex *in, complex *out, int size); extern void fts_vecx_cabs(complex *in, complex *out, int size);
#line 18 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_arith.h"
 







 

 
extern void fts_vec_fadd(float *in0, float *in1, float *out, int size); extern void fts_vecx_fadd(float *in0, float *in1, float *out, int size);
extern void fts_vec_fsub(float *in0, float *in1, float *out, int size); extern void fts_vecx_fsub(float *in0, float *in1, float *out, int size);
extern void fts_vec_fmul(float *in0, float *in1, float *out, int size); extern void fts_vecx_fmul(float *in0, float *in1, float *out, int size);
extern void fts_vec_fdiv(float *in0, float *in1, float *out, int size); extern void fts_vecx_fdiv(float *in0, float *in1, float *out, int size);
extern void fts_vec_fbus(float *in0, float *in1, float *out, int size); extern void fts_vecx_fbus(float *in0, float *in1, float *out, int size);
extern void fts_vec_fvid(float *in0, float *in1, float *out, int size); extern void fts_vecx_fvid(float *in0, float *in1, float *out, int size);
 
extern void fts_vec_fdot(float *in0, float *in1, float *ptr, int size); extern void fts_vecx_fdot(float *in0, float *in1, float *ptr, int size);

 
extern void fts_vec_iadd(int *in0, int *in1, int *out, int size); extern void fts_vecx_iadd(int *in0, int *in1, int *out, int size);
extern void fts_vec_isub(int *in0, int *in1, int *out, int size); extern void fts_vecx_isub(int *in0, int *in1, int *out, int size);
extern void fts_vec_imul(int *in0, int *in1, int *out, int size); extern void fts_vecx_imul(int *in0, int *in1, int *out, int size);
extern void fts_vec_idiv(int *in0, int *in1, int *out, int size); extern void fts_vecx_idiv(int *in0, int *in1, int *out, int size);
extern void fts_vec_ibus(int *in0, int *in1, int *out, int size); extern void fts_vecx_ibus(int *in0, int *in1, int *out, int size);
extern void fts_vec_ivid(int *in0, int *in1, int *out, int size); extern void fts_vecx_ivid(int *in0, int *in1, int *out, int size);
 
 
extern void fts_vec_cadd(complex *in0, complex *in1, complex *out, int size); extern void fts_vecx_cadd(complex *in0, complex *in1, complex *out, int size);
extern void fts_vec_csub(complex *in0, complex *in1, complex *out, int size); extern void fts_vecx_csub(complex *in0, complex *in1, complex *out, int size);
extern void fts_vec_cmul(complex *in0, complex *in1, complex *out, int size); extern void fts_vecx_cmul(complex *in0, complex *in1, complex *out, int size);
extern void fts_vec_cdiv(complex *in0, complex *in1, complex *out, int size); extern void fts_vecx_cdiv(complex *in0, complex *in1, complex *out, int size);
extern void fts_vec_cbus(complex *in0, complex *in1, complex *out, int size); extern void fts_vecx_cbus(complex *in0, complex *in1, complex *out, int size);
extern void fts_vec_cvid(complex *in0, complex *in1, complex *out, int size); extern void fts_vecx_cvid(complex *in0, complex *in1, complex *out, int size);

extern void fts_vec_cfmul(complex *in0, float *in1, complex *out, int size); extern void fts_vecx_cfmul(complex *in0, float *in1, complex *out, int size);






 

 
extern void fts_vec_scl_fadd(float *in, float scl, float *out, int size); extern void fts_vecx_scl_fadd(float *in, float scl, float *out, int size);
extern void fts_vec_scl_fsub(float *in, float scl, float *out, int size); extern void fts_vecx_scl_fsub(float *in, float scl, float *out, int size);
extern void fts_vec_scl_fmul(float *in, float scl, float *out, int size); extern void fts_vecx_scl_fmul(float *in, float scl, float *out, int size);
extern void fts_vec_scl_fdiv(float *in, float scl, float *out, int size); extern void fts_vecx_scl_fdiv(float *in, float scl, float *out, int size);
extern void fts_vec_scl_fbus(float *in, float scl, float *out, int size); extern void fts_vecx_scl_fbus(float *in, float scl, float *out, int size);
extern void fts_vec_scl_fvid(float *in, float scl, float *out, int size); extern void fts_vecx_scl_fvid(float *in, float scl, float *out, int size);
 
 
extern void fts_vec_scl_iadd(int *in, int scl, int *out, int size); extern void fts_vecx_scl_iadd(int *in, int scl, int *out, int size);
extern void fts_vec_scl_isub(int *in, int scl, int *out, int size); extern void fts_vecx_scl_isub(int *in, int scl, int *out, int size);
extern void fts_vec_scl_imul(int *in, int scl, int *out, int size); extern void fts_vecx_scl_imul(int *in, int scl, int *out, int size);
extern void fts_vec_scl_idiv(int *in, int scl, int *out, int size); extern void fts_vecx_scl_idiv(int *in, int scl, int *out, int size);
extern void fts_vec_scl_ibus(int *in, int scl, int *out, int size); extern void fts_vecx_scl_ibus(int *in, int scl, int *out, int size);
extern void fts_vec_scl_ivid(int *in, int scl, int *out, int size); extern void fts_vecx_scl_ivid(int *in, int scl, int *out, int size);
 
 
extern void fts_vec_scl_cadd(complex *in, complex scl, complex *out, int size); extern void fts_vecx_scl_cadd(complex *in, complex scl, complex *out, int size);
extern void fts_vec_scl_csub(complex *in, complex scl, complex *out, int size); extern void fts_vecx_scl_csub(complex *in, complex scl, complex *out, int size);
extern void fts_vec_scl_cmul(complex *in, complex scl, complex *out, int size); extern void fts_vecx_scl_cmul(complex *in, complex scl, complex *out, int size);
extern void fts_vec_scl_cdiv(complex *in, complex scl, complex *out, int size); extern void fts_vecx_scl_cdiv(complex *in, complex scl, complex *out, int size);
extern void fts_vec_scl_cbus(complex *in, complex scl, complex *out, int size); extern void fts_vecx_scl_cbus(complex *in, complex scl, complex *out, int size);
extern void fts_vec_scl_cvid(complex *in, complex scl, complex *out, int size); extern void fts_vecx_scl_cvid(complex *in, complex scl, complex *out, int size);
extern void fts_vec_scl_cfmul(complex *in, float scl, complex *out, int size); extern void fts_vecx_scl_cfmul(complex *in, float scl, complex *out, int size);

#line 19 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_cmp.h"
 








 

 
extern void fts_vec_fcmp_eq(float *in0, float *in1, int *out, int size); extern void fts_vecx_fcmp_eq(float *in0, float *in1, int *out, int size);
extern void fts_vec_fcmp_neq(float *in0, float *in1, int *out, int size); extern void fts_vecx_fcmp_neq(float *in0, float *in1, int *out, int size);
extern void fts_vec_fcmp_leq(float *in0, float *in1, int *out, int size); extern void fts_vecx_fcmp_leq(float *in0, float *in1, int *out, int size);
extern void fts_vec_fcmp_geq(float *in0, float *in1, int *out, int size); extern void fts_vecx_fcmp_geq(float *in0, float *in1, int *out, int size);
extern void fts_vec_fcmp_le(float *in0, float *in1, int *out, int size); extern void fts_vecx_fcmp_le(float *in0, float *in1, int *out, int size);
extern void fts_vec_fcmp_gr(float *in0, float *in1, int *out, int size); extern void fts_vecx_fcmp_gr(float *in0, float *in1, int *out, int size);
 
 
extern void fts_vec_icmp_eq(int *in0, int *in1, int *out, int size); extern void fts_vecx_icmp_eq(int *in0, int *in1, int *out, int size);
extern void fts_vec_icmp_neq(int *in0, int *in1, int *out, int size); extern void fts_vecx_icmp_neq(int *in0, int *in1, int *out, int size);
extern void fts_vec_icmp_leq(int *in0, int *in1, int *out, int size); extern void fts_vecx_icmp_leq(int *in0, int *in1, int *out, int size);
extern void fts_vec_icmp_geq(int *in0, int *in1, int *out, int size); extern void fts_vecx_icmp_geq(int *in0, int *in1, int *out, int size);
extern void fts_vec_icmp_le(int *in0, int *in1, int *out, int size); extern void fts_vecx_icmp_le(int *in0, int *in1, int *out, int size);
extern void fts_vec_icmp_gr(int *in0, int *in1, int *out, int size); extern void fts_vecx_icmp_gr(int *in0, int *in1, int *out, int size);






 

 
extern void fts_vec_scl_fcmp_eq(float *in, float scl, int *out, int size); extern void fts_vecx_scl_fcmp_eq(float *in, float scl, int *out, int size);
extern void fts_vec_scl_fcmp_neq(float *in, float scl, int *out, int size); extern void fts_vecx_scl_fcmp_neq(float *in, float scl, int *out, int size);
extern void fts_vec_scl_fcmp_leq(float *in, float scl, int *out, int size); extern void fts_vecx_scl_fcmp_leq(float *in, float scl, int *out, int size);
extern void fts_vec_scl_fcmp_geq(float *in, float scl, int *out, int size); extern void fts_vecx_scl_fcmp_geq(float *in, float scl, int *out, int size);
extern void fts_vec_scl_fcmp_le(float *in, float scl, int *out, int size); extern void fts_vecx_scl_fcmp_le(float *in, float scl, int *out, int size);
extern void fts_vec_scl_fcmp_gr(float *in, float scl, int *out, int size); extern void fts_vecx_scl_fcmp_gr(float *in, float scl, int *out, int size);
 
 
extern void fts_vec_scl_icmp_eq(int *in, int scl, int *out, int size); extern void fts_vecx_scl_icmp_eq(int *in, int scl, int *out, int size);
extern void fts_vec_scl_icmp_neq(int *in, int scl, int *out, int size); extern void fts_vecx_scl_icmp_neq(int *in, int scl, int *out, int size);
extern void fts_vec_scl_icmp_leq(int *in, int scl, int *out, int size); extern void fts_vecx_scl_icmp_leq(int *in, int scl, int *out, int size);
extern void fts_vec_scl_icmp_geq(int *in, int scl, int *out, int size); extern void fts_vecx_scl_icmp_geq(int *in, int scl, int *out, int size);
extern void fts_vec_scl_icmp_le(int *in, int scl, int *out, int size); extern void fts_vecx_scl_icmp_le(int *in, int scl, int *out, int size);
extern void fts_vec_scl_icmp_gr(int *in, int scl, int *out, int size); extern void fts_vecx_scl_icmp_gr(int *in, int scl, int *out, int size);

#line 20 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_cpy.h"







 

extern void fts_vec_fcpy(float *in, float *out, int size); extern void fts_vecx_fcpy(float *in, float *out, int size);
extern void fts_vec_icpy(int *in, int *out, int size); extern void fts_vecx_icpy(int *in, int *out, int size);
extern void fts_vec_ccpy(complex *in, complex *out, int size); extern void fts_vecx_ccpy(complex *in, complex *out, int size);

extern void fts_vec_fcpyre(float *in, complex *out, int size); extern void fts_vecx_fcpyre(float *in, complex *out, int size);
extern void fts_vec_fcpyim(float *in, complex *out, int size); extern void fts_vecx_fcpyim(float *in, complex *out, int size);

extern void fts_vec_ffill(float scl, float *out, int size); extern void fts_vecx_ffill(float scl, float *out, int size);
extern void fts_vec_ifill(int scl, int *out, int size); extern void fts_vecx_ifill(int scl, int *out, int size);
extern void fts_vec_cfill(complex scl, complex *out, int size); extern void fts_vecx_cfill(complex scl, complex *out, int size);
extern void fts_vec_ffillre(float scl, complex *out, int size); extern void fts_vecx_ffillre(float scl, complex *out, int size);
extern void fts_vec_ffillim(float scl, complex *out, int size); extern void fts_vecx_ffillim(float scl, complex *out, int size);

extern void fts_vec_re(complex *in, float *out, int size); extern void fts_vecx_re(complex *in, float *out, int size);
extern void fts_vec_im(complex *in, float *out, int size); extern void fts_vecx_im(complex *in, float *out, int size);

extern void fts_vec_fzero(float *out, int size); extern void fts_vecx_fzero(float *out, int size);
extern void fts_vec_izero(int *out, int size); extern void fts_vecx_izero(int *out, int size);
extern void fts_vec_czero(complex *out, int size); extern void fts_vecx_czero(complex *out, int size);





 

extern void fts_vec_fcasti(float *in, int *out, int size); extern void fts_vecx_fcasti(float *in, int *out, int size);
extern void fts_vec_icastf(int *in, float *out, int size); extern void fts_vecx_icastf(int *in, float *out, int size);
#line 21 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_bit.h"
 








 

extern void fts_vec_bitnot(int *in, int *out, int size); extern void fts_vecx_bitnot(int *in, int *out, int size);






 

extern void fts_vec_bitand(int *in0, int *in1, int *out, int size); extern void fts_vecx_bitand(int *in0, int *in1, int *out, int size);
extern void fts_vec_bitor(int *in0, int *in1, int *out, int size); extern void fts_vecx_bitor(int *in0, int *in1, int *out, int size);
extern void fts_vec_bitxor(int *in0, int *in1, int *out, int size); extern void fts_vecx_bitxor(int *in0, int *in1, int *out, int size);

extern void fts_vec_bitshl(int *in0, int *in1, int *out, int size); extern void fts_vecx_bitshl(int *in0, int *in1, int *out, int size);
extern void fts_vec_bitshr(int *in0, int *in1, int *out, int size); extern void fts_vecx_bitshr(int *in0, int *in1, int *out, int size);






 

extern void fts_vec_scl_bitand(int *in, int scl, int *out, int size); extern void fts_vecx_scl_bitand(int *in, int scl, int *out, int size);
extern void fts_vec_scl_bitor(int *in, int scl, int *out, int size); extern void fts_vecx_scl_bitor(int *in, int scl, int *out, int size);
extern void fts_vec_scl_bitxor(int *in, int scl, int *out, int size); extern void fts_vecx_scl_bitxor(int *in, int scl, int *out, int size);

extern void fts_vec_scl_bitshl(int *in, int scl, int *out, int size); extern void fts_vecx_scl_bitshl(int *in, int scl, int *out, int size);
extern void fts_vec_scl_bitshr(int *in, int scl, int *out, int size); extern void fts_vecx_scl_bitshr(int *in, int scl, int *out, int size);
#line 22 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_misc.h"


 

extern void fts_vec_fmin(float *in, float *ptr, int size); extern void fts_vecx_fmin(float *in, float *ptr, int size);
extern void fts_vec_imin(int *in, int *ptr, int size); extern void fts_vecx_imin(int *in, int *ptr, int size);

extern void fts_vec_fmax(float *in, float *ptr, int size); extern void fts_vecx_fmax(float *in, float *ptr, int size);
extern void fts_vec_imax(int *in, int *ptr, int size); extern void fts_vecx_imax(int *in, int *ptr, int size);


 

extern void fts_vec_fsum(float *in, float *ptr, int size); extern void fts_vecx_fsum(float *in, float *ptr, int size);
extern void fts_vec_isum(int *in, int *ptr, int size); extern void fts_vecx_isum(int *in, int *ptr, int size);
extern void fts_vec_csum(complex *in, complex *ptr, int size); extern void fts_vecx_csum(complex *in, complex *ptr, int size);


 

extern void fts_vec_fclip(float *in, float scl0, float scl1, float *out, int size); extern void fts_vecx_fclip(float *in, float scl0, float scl1, float *out, int size);
extern void fts_vec_iclip(int *in, int scl0, int scl1, int *out, int size); extern void fts_vecx_iclip(int *in, int scl0, int scl1, int *out, int size);

#endif

/* hand coded */
extern void fts_vec_csplit(complex *in, float *out0, float *out1, int size);
extern void fts_vecx_csplit(complex *in, float *out0, float *out1, int size);
extern void fts_vec_cmerge(float *in0, float *in1, complex *out, int size);
extern void fts_vecx_cmerge(float *in0, float *in1, complex *out, int size);


#endif /* _VECLIB_H_ */




