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

/* vecftl.h */

#ifndef _VECFTL_H_
#define _VECFTL_H_

#include <fts/lang/mess.h>

#if 0

/* (fd) This gives a lot of warning when compiling on redhat 7.0 (gcc version 2.96),
   so I've commented this out and replaced it by the macro expanded prototypes.
*/
#include <fts/lang/veclib/include/declare_ftl.h>
#include <fts/lang/veclib/include/vec_fun.h>
#include <fts/lang/veclib/include/vec_arith.h>
#include <fts/lang/veclib/include/vec_cmp.h>
#include <fts/lang/veclib/include/vec_cpy.h>
#include <fts/lang/veclib/include/vec_bit.h>
#include <fts/lang/veclib/include/vec_misc.h>

#else

extern void ftl_vec_sin(fts_word_t *argv); extern void ftl_vecx_sin(fts_word_t *argv);
extern void ftl_vec_cos(fts_word_t *argv); extern void ftl_vecx_cos(fts_word_t *argv);
extern void ftl_vec_tan(fts_word_t *argv); extern void ftl_vecx_tan(fts_word_t *argv);
extern void ftl_vec_asin(fts_word_t *argv); extern void ftl_vecx_asin(fts_word_t *argv);
extern void ftl_vec_acos(fts_word_t *argv); extern void ftl_vecx_acos(fts_word_t *argv);
extern void ftl_vec_atan(fts_word_t *argv); extern void ftl_vecx_atan(fts_word_t *argv);
extern void ftl_vec_sinh(fts_word_t *argv); extern void ftl_vecx_sinh(fts_word_t *argv);
extern void ftl_vec_cosh(fts_word_t *argv); extern void ftl_vecx_cosh(fts_word_t *argv);
extern void ftl_vec_tanh(fts_word_t *argv); extern void ftl_vecx_tanh(fts_word_t *argv);

extern void ftl_vec_exp(fts_word_t *argv); extern void ftl_vecx_exp(fts_word_t *argv);
extern void ftl_vec_log(fts_word_t *argv); extern void ftl_vecx_log(fts_word_t *argv);
extern void ftl_vec_log10(fts_word_t *argv); extern void ftl_vecx_log10(fts_word_t *argv);
extern void ftl_vec_expb(fts_word_t *argv); extern void ftl_vecx_expb(fts_word_t *argv);
extern void ftl_vec_logb(fts_word_t *argv); extern void ftl_vecx_logb(fts_word_t *argv);

extern void ftl_vec_sqrt(fts_word_t *argv); extern void ftl_vecx_sqrt(fts_word_t *argv);
extern void ftl_vec_rsqr(fts_word_t *argv); extern void ftl_vecx_rsqr(fts_word_t *argv);

extern void ftl_vec_conj(fts_word_t *argv); extern void ftl_vecx_conj(fts_word_t *argv);
extern void ftl_vec_csqr(fts_word_t *argv); extern void ftl_vecx_csqr(fts_word_t *argv);
extern void ftl_vec_cpolar(fts_word_t *argv); extern void ftl_vecx_cpolar(fts_word_t *argv);
extern void ftl_vec_crect(fts_word_t *argv); extern void ftl_vecx_crect(fts_word_t *argv);

extern void ftl_vec_cabsf(fts_word_t *argv); extern void ftl_vecx_cabsf(fts_word_t *argv);
extern void ftl_vec_csqrf(fts_word_t *argv); extern void ftl_vecx_csqrf(fts_word_t *argv);

extern void ftl_vec_round(fts_word_t *argv); extern void ftl_vecx_round(fts_word_t *argv);

extern void ftl_vec_fabs(fts_word_t *argv); extern void ftl_vecx_fabs(fts_word_t *argv);
extern void ftl_vec_iabs(fts_word_t *argv); extern void ftl_vecx_iabs(fts_word_t *argv);
extern void ftl_vec_cabs(fts_word_t *argv); extern void ftl_vecx_cabs(fts_word_t *argv);
 
extern void ftl_vec_fadd(fts_word_t *argv); extern void ftl_vecx_fadd(fts_word_t *argv);
extern void ftl_vec_fsub(fts_word_t *argv); extern void ftl_vecx_fsub(fts_word_t *argv);
extern void ftl_vec_fmul(fts_word_t *argv); extern void ftl_vecx_fmul(fts_word_t *argv);
extern void ftl_vec_fdiv(fts_word_t *argv); extern void ftl_vecx_fdiv(fts_word_t *argv);
extern void ftl_vec_fbus(fts_word_t *argv); extern void ftl_vecx_fbus(fts_word_t *argv);
extern void ftl_vec_fvid(fts_word_t *argv); extern void ftl_vecx_fvid(fts_word_t *argv);
 
extern void ftl_vec_fdot(fts_word_t *argv); extern void ftl_vecx_fdot(fts_word_t *argv);

 
extern void ftl_vec_iadd(fts_word_t *argv); extern void ftl_vecx_iadd(fts_word_t *argv);
extern void ftl_vec_isub(fts_word_t *argv); extern void ftl_vecx_isub(fts_word_t *argv);
extern void ftl_vec_imul(fts_word_t *argv); extern void ftl_vecx_imul(fts_word_t *argv);
extern void ftl_vec_idiv(fts_word_t *argv); extern void ftl_vecx_idiv(fts_word_t *argv);
extern void ftl_vec_ibus(fts_word_t *argv); extern void ftl_vecx_ibus(fts_word_t *argv);
extern void ftl_vec_ivid(fts_word_t *argv); extern void ftl_vecx_ivid(fts_word_t *argv);
 
 
extern void ftl_vec_cadd(fts_word_t *argv); extern void ftl_vecx_cadd(fts_word_t *argv);
extern void ftl_vec_csub(fts_word_t *argv); extern void ftl_vecx_csub(fts_word_t *argv);
extern void ftl_vec_cmul(fts_word_t *argv); extern void ftl_vecx_cmul(fts_word_t *argv);
extern void ftl_vec_cdiv(fts_word_t *argv); extern void ftl_vecx_cdiv(fts_word_t *argv);
extern void ftl_vec_cbus(fts_word_t *argv); extern void ftl_vecx_cbus(fts_word_t *argv);
extern void ftl_vec_cvid(fts_word_t *argv); extern void ftl_vecx_cvid(fts_word_t *argv);

extern void ftl_vec_cfmul(fts_word_t *argv); extern void ftl_vecx_cfmul(fts_word_t *argv);
 
extern void ftl_vec_scl_fadd(fts_word_t *argv); extern void ftl_vecx_scl_fadd(fts_word_t *argv);
extern void ftl_vec_scl_fsub(fts_word_t *argv); extern void ftl_vecx_scl_fsub(fts_word_t *argv);
extern void ftl_vec_scl_fmul(fts_word_t *argv); extern void ftl_vecx_scl_fmul(fts_word_t *argv);
extern void ftl_vec_scl_fdiv(fts_word_t *argv); extern void ftl_vecx_scl_fdiv(fts_word_t *argv);
extern void ftl_vec_scl_fbus(fts_word_t *argv); extern void ftl_vecx_scl_fbus(fts_word_t *argv);
extern void ftl_vec_scl_fvid(fts_word_t *argv); extern void ftl_vecx_scl_fvid(fts_word_t *argv);
 
 
extern void ftl_vec_scl_iadd(fts_word_t *argv); extern void ftl_vecx_scl_iadd(fts_word_t *argv);
extern void ftl_vec_scl_isub(fts_word_t *argv); extern void ftl_vecx_scl_isub(fts_word_t *argv);
extern void ftl_vec_scl_imul(fts_word_t *argv); extern void ftl_vecx_scl_imul(fts_word_t *argv);
extern void ftl_vec_scl_idiv(fts_word_t *argv); extern void ftl_vecx_scl_idiv(fts_word_t *argv);
extern void ftl_vec_scl_ibus(fts_word_t *argv); extern void ftl_vecx_scl_ibus(fts_word_t *argv);
extern void ftl_vec_scl_ivid(fts_word_t *argv); extern void ftl_vecx_scl_ivid(fts_word_t *argv);
 
 
extern void ftl_vec_scl_cadd(fts_word_t *argv); extern void ftl_vecx_scl_cadd(fts_word_t *argv);
extern void ftl_vec_scl_csub(fts_word_t *argv); extern void ftl_vecx_scl_csub(fts_word_t *argv);
extern void ftl_vec_scl_cmul(fts_word_t *argv); extern void ftl_vecx_scl_cmul(fts_word_t *argv);
extern void ftl_vec_scl_cdiv(fts_word_t *argv); extern void ftl_vecx_scl_cdiv(fts_word_t *argv);
extern void ftl_vec_scl_cbus(fts_word_t *argv); extern void ftl_vecx_scl_cbus(fts_word_t *argv);
extern void ftl_vec_scl_cvid(fts_word_t *argv); extern void ftl_vecx_scl_cvid(fts_word_t *argv);
extern void ftl_vec_scl_cfmul(fts_word_t *argv); extern void ftl_vecx_scl_cfmul(fts_word_t *argv);

extern void ftl_vec_fcmp_eq(fts_word_t *argv); extern void ftl_vecx_fcmp_eq(fts_word_t *argv);
extern void ftl_vec_fcmp_neq(fts_word_t *argv); extern void ftl_vecx_fcmp_neq(fts_word_t *argv);
extern void ftl_vec_fcmp_leq(fts_word_t *argv); extern void ftl_vecx_fcmp_leq(fts_word_t *argv);
extern void ftl_vec_fcmp_geq(fts_word_t *argv); extern void ftl_vecx_fcmp_geq(fts_word_t *argv);
extern void ftl_vec_fcmp_le(fts_word_t *argv); extern void ftl_vecx_fcmp_le(fts_word_t *argv);
extern void ftl_vec_fcmp_gr(fts_word_t *argv); extern void ftl_vecx_fcmp_gr(fts_word_t *argv);
 
 
extern void ftl_vec_icmp_eq(fts_word_t *argv); extern void ftl_vecx_icmp_eq(fts_word_t *argv);
extern void ftl_vec_icmp_neq(fts_word_t *argv); extern void ftl_vecx_icmp_neq(fts_word_t *argv);
extern void ftl_vec_icmp_leq(fts_word_t *argv); extern void ftl_vecx_icmp_leq(fts_word_t *argv);
extern void ftl_vec_icmp_geq(fts_word_t *argv); extern void ftl_vecx_icmp_geq(fts_word_t *argv);
extern void ftl_vec_icmp_le(fts_word_t *argv); extern void ftl_vecx_icmp_le(fts_word_t *argv);
extern void ftl_vec_icmp_gr(fts_word_t *argv); extern void ftl_vecx_icmp_gr(fts_word_t *argv);
 
extern void ftl_vec_scl_fcmp_eq(fts_word_t *argv); extern void ftl_vecx_scl_fcmp_eq(fts_word_t *argv);
extern void ftl_vec_scl_fcmp_neq(fts_word_t *argv); extern void ftl_vecx_scl_fcmp_neq(fts_word_t *argv);
extern void ftl_vec_scl_fcmp_leq(fts_word_t *argv); extern void ftl_vecx_scl_fcmp_leq(fts_word_t *argv);
extern void ftl_vec_scl_fcmp_geq(fts_word_t *argv); extern void ftl_vecx_scl_fcmp_geq(fts_word_t *argv);
extern void ftl_vec_scl_fcmp_le(fts_word_t *argv); extern void ftl_vecx_scl_fcmp_le(fts_word_t *argv);
extern void ftl_vec_scl_fcmp_gr(fts_word_t *argv); extern void ftl_vecx_scl_fcmp_gr(fts_word_t *argv);
 
 
extern void ftl_vec_scl_icmp_eq(fts_word_t *argv); extern void ftl_vecx_scl_icmp_eq(fts_word_t *argv);
extern void ftl_vec_scl_icmp_neq(fts_word_t *argv); extern void ftl_vecx_scl_icmp_neq(fts_word_t *argv);
extern void ftl_vec_scl_icmp_leq(fts_word_t *argv); extern void ftl_vecx_scl_icmp_leq(fts_word_t *argv);
extern void ftl_vec_scl_icmp_geq(fts_word_t *argv); extern void ftl_vecx_scl_icmp_geq(fts_word_t *argv);
extern void ftl_vec_scl_icmp_le(fts_word_t *argv); extern void ftl_vecx_scl_icmp_le(fts_word_t *argv);
extern void ftl_vec_scl_icmp_gr(fts_word_t *argv); extern void ftl_vecx_scl_icmp_gr(fts_word_t *argv);

extern void ftl_vec_fcpy(fts_word_t *argv); extern void ftl_vecx_fcpy(fts_word_t *argv);
extern void ftl_vec_icpy(fts_word_t *argv); extern void ftl_vecx_icpy(fts_word_t *argv);
extern void ftl_vec_ccpy(fts_word_t *argv); extern void ftl_vecx_ccpy(fts_word_t *argv);

extern void ftl_vec_fcpyre(fts_word_t *argv); extern void ftl_vecx_fcpyre(fts_word_t *argv);
extern void ftl_vec_fcpyim(fts_word_t *argv); extern void ftl_vecx_fcpyim(fts_word_t *argv);

extern void ftl_vec_ffill(fts_word_t *argv); extern void ftl_vecx_ffill(fts_word_t *argv);
extern void ftl_vec_ifill(fts_word_t *argv); extern void ftl_vecx_ifill(fts_word_t *argv);
extern void ftl_vec_cfill(fts_word_t *argv); extern void ftl_vecx_cfill(fts_word_t *argv);
extern void ftl_vec_ffillre(fts_word_t *argv); extern void ftl_vecx_ffillre(fts_word_t *argv);
extern void ftl_vec_ffillim(fts_word_t *argv); extern void ftl_vecx_ffillim(fts_word_t *argv);

extern void ftl_vec_re(fts_word_t *argv); extern void ftl_vecx_re(fts_word_t *argv);
extern void ftl_vec_im(fts_word_t *argv); extern void ftl_vecx_im(fts_word_t *argv);

extern void ftl_vec_fzero(fts_word_t *argv); extern void ftl_vecx_fzero(fts_word_t *argv);
extern void ftl_vec_izero(fts_word_t *argv); extern void ftl_vecx_izero(fts_word_t *argv);
extern void ftl_vec_czero(fts_word_t *argv); extern void ftl_vecx_czero(fts_word_t *argv);

extern void ftl_vec_fcasti(fts_word_t *argv); extern void ftl_vecx_fcasti(fts_word_t *argv);
extern void ftl_vec_icastf(fts_word_t *argv); extern void ftl_vecx_icastf(fts_word_t *argv);

extern void ftl_vec_bitnot(fts_word_t *argv); extern void ftl_vecx_bitnot(fts_word_t *argv);

extern void ftl_vec_bitand(fts_word_t *argv); extern void ftl_vecx_bitand(fts_word_t *argv);
extern void ftl_vec_bitor(fts_word_t *argv); extern void ftl_vecx_bitor(fts_word_t *argv);
extern void ftl_vec_bitxor(fts_word_t *argv); extern void ftl_vecx_bitxor(fts_word_t *argv);

extern void ftl_vec_bitshl(fts_word_t *argv); extern void ftl_vecx_bitshl(fts_word_t *argv);
extern void ftl_vec_bitshr(fts_word_t *argv); extern void ftl_vecx_bitshr(fts_word_t *argv);

extern void ftl_vec_scl_bitand(fts_word_t *argv); extern void ftl_vecx_scl_bitand(fts_word_t *argv);
extern void ftl_vec_scl_bitor(fts_word_t *argv); extern void ftl_vecx_scl_bitor(fts_word_t *argv);
extern void ftl_vec_scl_bitxor(fts_word_t *argv); extern void ftl_vecx_scl_bitxor(fts_word_t *argv);

extern void ftl_vec_scl_bitshl(fts_word_t *argv); extern void ftl_vecx_scl_bitshl(fts_word_t *argv);
extern void ftl_vec_scl_bitshr(fts_word_t *argv); extern void ftl_vecx_scl_bitshr(fts_word_t *argv);

extern void ftl_vec_fmin(fts_word_t *argv); extern void ftl_vecx_fmin(fts_word_t *argv);
extern void ftl_vec_imin(fts_word_t *argv); extern void ftl_vecx_imin(fts_word_t *argv);

extern void ftl_vec_fmax(fts_word_t *argv); extern void ftl_vecx_fmax(fts_word_t *argv);
extern void ftl_vec_imax(fts_word_t *argv); extern void ftl_vecx_imax(fts_word_t *argv);

extern void ftl_vec_fsum(fts_word_t *argv); extern void ftl_vecx_fsum(fts_word_t *argv);
extern void ftl_vec_isum(fts_word_t *argv); extern void ftl_vecx_isum(fts_word_t *argv);
extern void ftl_vec_csum(fts_word_t *argv); extern void ftl_vecx_csum(fts_word_t *argv);

extern void ftl_vec_fclip(fts_word_t *argv); extern void ftl_vecx_fclip(fts_word_t *argv);
extern void ftl_vec_iclip(fts_word_t *argv); extern void ftl_vecx_iclip(fts_word_t *argv);

#endif

/* hand coded */
extern void ftl_vec_csplit(fts_word_t *a);
extern void ftl_vecx_csplit(fts_word_t *a);
extern void ftl_vec_cmerge(fts_word_t *a);
extern void ftl_vecx_cmerge(fts_word_t *a);

#endif /* _VECFTL_H_ */
