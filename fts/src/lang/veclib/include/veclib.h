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

extern void fts_vec_sin(float *in, float *out, long size); extern void fts_vecx_sin(float *in, float *out, long size);
extern void fts_vec_cos(float *in, float *out, long size); extern void fts_vecx_cos(float *in, float *out, long size);
extern void fts_vec_tan(float *in, float *out, long size); extern void fts_vecx_tan(float *in, float *out, long size);
extern void fts_vec_asin(float *in, float *out, long size); extern void fts_vecx_asin(float *in, float *out, long size);
extern void fts_vec_acos(float *in, float *out, long size); extern void fts_vecx_acos(float *in, float *out, long size);
extern void fts_vec_atan(float *in, float *out, long size); extern void fts_vecx_atan(float *in, float *out, long size);
extern void fts_vec_sinh(float *in, float *out, long size); extern void fts_vecx_sinh(float *in, float *out, long size);
extern void fts_vec_cosh(float *in, float *out, long size); extern void fts_vecx_cosh(float *in, float *out, long size);
extern void fts_vec_tanh(float *in, float *out, long size); extern void fts_vecx_tanh(float *in, float *out, long size);

extern void fts_vec_exp(float *in, float *out, long size); extern void fts_vecx_exp(float *in, float *out, long size);
extern void fts_vec_log(float *in, float *out, long size); extern void fts_vecx_log(float *in, float *out, long size);
extern void fts_vec_log10(float *in, float *out, long size); extern void fts_vecx_log10(float *in, float *out, long size);
extern void fts_vec_expb(float *in, float scl, float *out, long size); extern void fts_vecx_expb(float *in, float scl, float *out, long size);
extern void fts_vec_logb(float *in, float scl, float *out, long size); extern void fts_vecx_logb(float *in, float scl, float *out, long size);

extern void fts_vec_sqrt(float *in, float *out, long size); extern void fts_vecx_sqrt(float *in, float *out, long size);
extern void fts_vec_rsqr(float *in, float *out, long size); extern void fts_vecx_rsqr(float *in, float *out, long size);






 

extern void fts_vec_conj(complex *in, complex *out, long size); extern void fts_vecx_conj(complex *in, complex *out, long size);
extern void fts_vec_csqr(complex *in, complex *out, long size); extern void fts_vecx_csqr(complex *in, complex *out, long size);
extern void fts_vec_cpolar(complex *in, complex *out, long size); extern void fts_vecx_cpolar(complex *in, complex *out, long size);
extern void fts_vec_crect(complex *in, complex *out, long size); extern void fts_vecx_crect(complex *in, complex *out, long size);

extern void fts_vec_cabsf(complex *in, float *out, long size); extern void fts_vecx_cabsf(complex *in, float *out, long size);
extern void fts_vec_csqrf(complex *in, float *out, long size); extern void fts_vecx_csqrf(complex *in, float *out, long size);






 

extern void fts_vec_round(float *in, long *out, long size); extern void fts_vecx_round(float *in, long *out, long size);






 

extern void fts_vec_fabs(float *in, float *out, long size); extern void fts_vecx_fabs(float *in, float *out, long size);
extern void fts_vec_iabs(long *in, long *out, long size); extern void fts_vecx_iabs(long *in, long *out, long size);
extern void fts_vec_cabs(complex *in, complex *out, long size); extern void fts_vecx_cabs(complex *in, complex *out, long size);
#line 18 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_arith.h"
 







 

 
extern void fts_vec_fadd(float *in0, float *in1, float *out, long size); extern void fts_vecx_fadd(float *in0, float *in1, float *out, long size);
extern void fts_vec_fsub(float *in0, float *in1, float *out, long size); extern void fts_vecx_fsub(float *in0, float *in1, float *out, long size);
extern void fts_vec_fmul(float *in0, float *in1, float *out, long size); extern void fts_vecx_fmul(float *in0, float *in1, float *out, long size);
extern void fts_vec_fdiv(float *in0, float *in1, float *out, long size); extern void fts_vecx_fdiv(float *in0, float *in1, float *out, long size);
extern void fts_vec_fbus(float *in0, float *in1, float *out, long size); extern void fts_vecx_fbus(float *in0, float *in1, float *out, long size);
extern void fts_vec_fvid(float *in0, float *in1, float *out, long size); extern void fts_vecx_fvid(float *in0, float *in1, float *out, long size);
 
extern void fts_vec_fdot(float *in0, float *in1, float *ptr, long size); extern void fts_vecx_fdot(float *in0, float *in1, float *ptr, long size);

 
extern void fts_vec_iadd(long *in0, long *in1, long *out, long size); extern void fts_vecx_iadd(long *in0, long *in1, long *out, long size);
extern void fts_vec_isub(long *in0, long *in1, long *out, long size); extern void fts_vecx_isub(long *in0, long *in1, long *out, long size);
extern void fts_vec_imul(long *in0, long *in1, long *out, long size); extern void fts_vecx_imul(long *in0, long *in1, long *out, long size);
extern void fts_vec_idiv(long *in0, long *in1, long *out, long size); extern void fts_vecx_idiv(long *in0, long *in1, long *out, long size);
extern void fts_vec_ibus(long *in0, long *in1, long *out, long size); extern void fts_vecx_ibus(long *in0, long *in1, long *out, long size);
extern void fts_vec_ivid(long *in0, long *in1, long *out, long size); extern void fts_vecx_ivid(long *in0, long *in1, long *out, long size);
 
 
extern void fts_vec_cadd(complex *in0, complex *in1, complex *out, long size); extern void fts_vecx_cadd(complex *in0, complex *in1, complex *out, long size);
extern void fts_vec_csub(complex *in0, complex *in1, complex *out, long size); extern void fts_vecx_csub(complex *in0, complex *in1, complex *out, long size);
extern void fts_vec_cmul(complex *in0, complex *in1, complex *out, long size); extern void fts_vecx_cmul(complex *in0, complex *in1, complex *out, long size);
extern void fts_vec_cdiv(complex *in0, complex *in1, complex *out, long size); extern void fts_vecx_cdiv(complex *in0, complex *in1, complex *out, long size);
extern void fts_vec_cbus(complex *in0, complex *in1, complex *out, long size); extern void fts_vecx_cbus(complex *in0, complex *in1, complex *out, long size);
extern void fts_vec_cvid(complex *in0, complex *in1, complex *out, long size); extern void fts_vecx_cvid(complex *in0, complex *in1, complex *out, long size);

extern void fts_vec_cfmul(complex *in0, float *in1, complex *out, long size); extern void fts_vecx_cfmul(complex *in0, float *in1, complex *out, long size);






 

 
extern void fts_vec_scl_fadd(float *in, float scl, float *out, long size); extern void fts_vecx_scl_fadd(float *in, float scl, float *out, long size);
extern void fts_vec_scl_fsub(float *in, float scl, float *out, long size); extern void fts_vecx_scl_fsub(float *in, float scl, float *out, long size);
extern void fts_vec_scl_fmul(float *in, float scl, float *out, long size); extern void fts_vecx_scl_fmul(float *in, float scl, float *out, long size);
extern void fts_vec_scl_fdiv(float *in, float scl, float *out, long size); extern void fts_vecx_scl_fdiv(float *in, float scl, float *out, long size);
extern void fts_vec_scl_fbus(float *in, float scl, float *out, long size); extern void fts_vecx_scl_fbus(float *in, float scl, float *out, long size);
extern void fts_vec_scl_fvid(float *in, float scl, float *out, long size); extern void fts_vecx_scl_fvid(float *in, float scl, float *out, long size);
 
 
extern void fts_vec_scl_iadd(long *in, long scl, long *out, long size); extern void fts_vecx_scl_iadd(long *in, long scl, long *out, long size);
extern void fts_vec_scl_isub(long *in, long scl, long *out, long size); extern void fts_vecx_scl_isub(long *in, long scl, long *out, long size);
extern void fts_vec_scl_imul(long *in, long scl, long *out, long size); extern void fts_vecx_scl_imul(long *in, long scl, long *out, long size);
extern void fts_vec_scl_idiv(long *in, long scl, long *out, long size); extern void fts_vecx_scl_idiv(long *in, long scl, long *out, long size);
extern void fts_vec_scl_ibus(long *in, long scl, long *out, long size); extern void fts_vecx_scl_ibus(long *in, long scl, long *out, long size);
extern void fts_vec_scl_ivid(long *in, long scl, long *out, long size); extern void fts_vecx_scl_ivid(long *in, long scl, long *out, long size);
 
 
extern void fts_vec_scl_cadd(complex *in, complex scl, complex *out, long size); extern void fts_vecx_scl_cadd(complex *in, complex scl, complex *out, long size);
extern void fts_vec_scl_csub(complex *in, complex scl, complex *out, long size); extern void fts_vecx_scl_csub(complex *in, complex scl, complex *out, long size);
extern void fts_vec_scl_cmul(complex *in, complex scl, complex *out, long size); extern void fts_vecx_scl_cmul(complex *in, complex scl, complex *out, long size);
extern void fts_vec_scl_cdiv(complex *in, complex scl, complex *out, long size); extern void fts_vecx_scl_cdiv(complex *in, complex scl, complex *out, long size);
extern void fts_vec_scl_cbus(complex *in, complex scl, complex *out, long size); extern void fts_vecx_scl_cbus(complex *in, complex scl, complex *out, long size);
extern void fts_vec_scl_cvid(complex *in, complex scl, complex *out, long size); extern void fts_vecx_scl_cvid(complex *in, complex scl, complex *out, long size);
extern void fts_vec_scl_cfmul(complex *in, float scl, complex *out, long size); extern void fts_vecx_scl_cfmul(complex *in, float scl, complex *out, long size);

#line 19 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_cmp.h"
 








 

 
extern void fts_vec_fcmp_eq(float *in0, float *in1, long *out, long size); extern void fts_vecx_fcmp_eq(float *in0, float *in1, long *out, long size);
extern void fts_vec_fcmp_neq(float *in0, float *in1, long *out, long size); extern void fts_vecx_fcmp_neq(float *in0, float *in1, long *out, long size);
extern void fts_vec_fcmp_leq(float *in0, float *in1, long *out, long size); extern void fts_vecx_fcmp_leq(float *in0, float *in1, long *out, long size);
extern void fts_vec_fcmp_geq(float *in0, float *in1, long *out, long size); extern void fts_vecx_fcmp_geq(float *in0, float *in1, long *out, long size);
extern void fts_vec_fcmp_le(float *in0, float *in1, long *out, long size); extern void fts_vecx_fcmp_le(float *in0, float *in1, long *out, long size);
extern void fts_vec_fcmp_gr(float *in0, float *in1, long *out, long size); extern void fts_vecx_fcmp_gr(float *in0, float *in1, long *out, long size);
 
 
extern void fts_vec_icmp_eq(long *in0, long *in1, long *out, long size); extern void fts_vecx_icmp_eq(long *in0, long *in1, long *out, long size);
extern void fts_vec_icmp_neq(long *in0, long *in1, long *out, long size); extern void fts_vecx_icmp_neq(long *in0, long *in1, long *out, long size);
extern void fts_vec_icmp_leq(long *in0, long *in1, long *out, long size); extern void fts_vecx_icmp_leq(long *in0, long *in1, long *out, long size);
extern void fts_vec_icmp_geq(long *in0, long *in1, long *out, long size); extern void fts_vecx_icmp_geq(long *in0, long *in1, long *out, long size);
extern void fts_vec_icmp_le(long *in0, long *in1, long *out, long size); extern void fts_vecx_icmp_le(long *in0, long *in1, long *out, long size);
extern void fts_vec_icmp_gr(long *in0, long *in1, long *out, long size); extern void fts_vecx_icmp_gr(long *in0, long *in1, long *out, long size);






 

 
extern void fts_vec_scl_fcmp_eq(float *in, float scl, long *out, long size); extern void fts_vecx_scl_fcmp_eq(float *in, float scl, long *out, long size);
extern void fts_vec_scl_fcmp_neq(float *in, float scl, long *out, long size); extern void fts_vecx_scl_fcmp_neq(float *in, float scl, long *out, long size);
extern void fts_vec_scl_fcmp_leq(float *in, float scl, long *out, long size); extern void fts_vecx_scl_fcmp_leq(float *in, float scl, long *out, long size);
extern void fts_vec_scl_fcmp_geq(float *in, float scl, long *out, long size); extern void fts_vecx_scl_fcmp_geq(float *in, float scl, long *out, long size);
extern void fts_vec_scl_fcmp_le(float *in, float scl, long *out, long size); extern void fts_vecx_scl_fcmp_le(float *in, float scl, long *out, long size);
extern void fts_vec_scl_fcmp_gr(float *in, float scl, long *out, long size); extern void fts_vecx_scl_fcmp_gr(float *in, float scl, long *out, long size);
 
 
extern void fts_vec_scl_icmp_eq(long *in, long scl, long *out, long size); extern void fts_vecx_scl_icmp_eq(long *in, long scl, long *out, long size);
extern void fts_vec_scl_icmp_neq(long *in, long scl, long *out, long size); extern void fts_vecx_scl_icmp_neq(long *in, long scl, long *out, long size);
extern void fts_vec_scl_icmp_leq(long *in, long scl, long *out, long size); extern void fts_vecx_scl_icmp_leq(long *in, long scl, long *out, long size);
extern void fts_vec_scl_icmp_geq(long *in, long scl, long *out, long size); extern void fts_vecx_scl_icmp_geq(long *in, long scl, long *out, long size);
extern void fts_vec_scl_icmp_le(long *in, long scl, long *out, long size); extern void fts_vecx_scl_icmp_le(long *in, long scl, long *out, long size);
extern void fts_vec_scl_icmp_gr(long *in, long scl, long *out, long size); extern void fts_vecx_scl_icmp_gr(long *in, long scl, long *out, long size);

#line 20 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_cpy.h"







 

extern void fts_vec_fcpy(float *in, float *out, long size); extern void fts_vecx_fcpy(float *in, float *out, long size);
extern void fts_vec_icpy(long *in, long *out, long size); extern void fts_vecx_icpy(long *in, long *out, long size);
extern void fts_vec_ccpy(complex *in, complex *out, long size); extern void fts_vecx_ccpy(complex *in, complex *out, long size);

extern void fts_vec_fcpyre(float *in, complex *out, long size); extern void fts_vecx_fcpyre(float *in, complex *out, long size);
extern void fts_vec_fcpyim(float *in, complex *out, long size); extern void fts_vecx_fcpyim(float *in, complex *out, long size);

extern void fts_vec_ffill(float scl, float *out, long size); extern void fts_vecx_ffill(float scl, float *out, long size);
extern void fts_vec_ifill(long scl, long *out, long size); extern void fts_vecx_ifill(long scl, long *out, long size);
extern void fts_vec_cfill(complex scl, complex *out, long size); extern void fts_vecx_cfill(complex scl, complex *out, long size);
extern void fts_vec_ffillre(float scl, complex *out, long size); extern void fts_vecx_ffillre(float scl, complex *out, long size);
extern void fts_vec_ffillim(float scl, complex *out, long size); extern void fts_vecx_ffillim(float scl, complex *out, long size);

extern void fts_vec_re(complex *in, float *out, long size); extern void fts_vecx_re(complex *in, float *out, long size);
extern void fts_vec_im(complex *in, float *out, long size); extern void fts_vecx_im(complex *in, float *out, long size);

extern void fts_vec_fzero(float *out, long size); extern void fts_vecx_fzero(float *out, long size);
extern void fts_vec_izero(long *out, long size); extern void fts_vecx_izero(long *out, long size);
extern void fts_vec_czero(complex *out, long size); extern void fts_vecx_czero(complex *out, long size);





 

extern void fts_vec_fcasti(float *in, long *out, long size); extern void fts_vecx_fcasti(float *in, long *out, long size);
extern void fts_vec_icastf(long *in, float *out, long size); extern void fts_vecx_icastf(long *in, float *out, long size);
#line 21 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_bit.h"
 








 

extern void fts_vec_bitnot(long *in, long *out, long size); extern void fts_vecx_bitnot(long *in, long *out, long size);






 

extern void fts_vec_bitand(long *in0, long *in1, long *out, long size); extern void fts_vecx_bitand(long *in0, long *in1, long *out, long size);
extern void fts_vec_bitor(long *in0, long *in1, long *out, long size); extern void fts_vecx_bitor(long *in0, long *in1, long *out, long size);
extern void fts_vec_bitxor(long *in0, long *in1, long *out, long size); extern void fts_vecx_bitxor(long *in0, long *in1, long *out, long size);

extern void fts_vec_bitshl(long *in0, long *in1, long *out, long size); extern void fts_vecx_bitshl(long *in0, long *in1, long *out, long size);
extern void fts_vec_bitshr(long *in0, long *in1, long *out, long size); extern void fts_vecx_bitshr(long *in0, long *in1, long *out, long size);






 

extern void fts_vec_scl_bitand(long *in, long scl, long *out, long size); extern void fts_vecx_scl_bitand(long *in, long scl, long *out, long size);
extern void fts_vec_scl_bitor(long *in, long scl, long *out, long size); extern void fts_vecx_scl_bitor(long *in, long scl, long *out, long size);
extern void fts_vec_scl_bitxor(long *in, long scl, long *out, long size); extern void fts_vecx_scl_bitxor(long *in, long scl, long *out, long size);

extern void fts_vec_scl_bitshl(long *in, long scl, long *out, long size); extern void fts_vecx_scl_bitshl(long *in, long scl, long *out, long size);
extern void fts_vec_scl_bitshr(long *in, long scl, long *out, long size); extern void fts_vecx_scl_bitshr(long *in, long scl, long *out, long size);
#line 22 "veclib.h"
#line 1 "../../../../src/lang/veclib/include/vec_misc.h"


 

extern void fts_vec_fmin(float *in, float *ptr, long size); extern void fts_vecx_fmin(float *in, float *ptr, long size);
extern void fts_vec_imin(long *in, long *ptr, long size); extern void fts_vecx_imin(long *in, long *ptr, long size);

extern void fts_vec_fmax(float *in, float *ptr, long size); extern void fts_vecx_fmax(float *in, float *ptr, long size);
extern void fts_vec_imax(long *in, long *ptr, long size); extern void fts_vecx_imax(long *in, long *ptr, long size);


 

extern void fts_vec_fsum(float *in, float *ptr, long size); extern void fts_vecx_fsum(float *in, float *ptr, long size);
extern void fts_vec_isum(long *in, long *ptr, long size); extern void fts_vecx_isum(long *in, long *ptr, long size);
extern void fts_vec_csum(complex *in, complex *ptr, long size); extern void fts_vecx_csum(complex *in, complex *ptr, long size);


 

extern void fts_vec_fclip(float *in, float scl0, float scl1, float *out, long size); extern void fts_vecx_fclip(float *in, float scl0, float scl1, float *out, long size);
extern void fts_vec_iclip(long *in, long scl0, long scl1, long *out, long size); extern void fts_vecx_iclip(long *in, long scl0, long scl1, long *out, long size);

#endif

/* hand coded */
extern void fts_vec_csplit(complex *in, float *out0, float *out1, long size);
extern void fts_vecx_csplit(complex *in, float *out0, float *out1, long size);
extern void fts_vec_cmerge(float *in0, float *in1, complex *out, long size);
extern void fts_vecx_cmerge(float *in0, float *in1, complex *out, long size);


#endif /* _VECLIB_H_ */




