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

/* kernels/fun.h */

#ifndef _KERNELSFUN_H_
#define _KERNELSFUN_H_

#include "lang/veclib/complex.h"

/***************************************************
 *
 *    VECTOR -> VECTOR functions
 *
 *   math functions (real -> real):
 *     FUN_sin ... (ANSI C function)
 *     FUN_cos ... (ANSI C function)
 *     FUN_tan ... (ANSI C function)
 *     FUN_asin ... (ANSI C function)
 *     FUN_acos ... (ANSI C function)
 *     FUN_atan ... (ANSI C function)
 *     FUN_sinh ... (ANSI C function)
 *     FUN_cosh ... (ANSI C function)
 *     FUN_tanh ... (ANSI C function)
 *
 *     FUN_exp ... (ANSI C function)
 *     FUN_log ... (ANSI C function)
 *     FUN_log10 ... (ANSI C function)
 *     FUN_expb ... exponential function of a scaled vector (for all bases different from e)
 *     FUN_logb ... scaled logarithmic function (for all bases different from e)
 *
 *     FUN_sqrt ... (ANSI C function)
 *     FUN_fabs ... (ANSI C function)
 *
 *   integer only functions (integral -> integral):
 *      FUN_bitnot ... (ANSI C operator ~)
 *      FUN_iabs ... integer absolut value
 *
 *    copy / type conversion:
 *      FUN_round ... round float to int: out = in + 0.5 (casts implicit: float -> int)
 *      FUN_cpy ... cast real type: out = in (casts implicit: int -> float, float -> int)
 *      FUN_ccpy ... copies complex
 *
 *      FUN_re ... get real part of complex
 *      FUN_im ... get imaginary part of complex
 *      FUN_fcpyre ... copy float to real part of complex
 *      FUN_fcpyim ... copy float to imaginary part of complex
 *      FUN_fcpyc ... copy float to complex (im = 0)
 *
 *    convert complex / real (complex <-> real):
 *      FUN_cabsf ... absolute value of complex argument as real result
 *      FUN_csqrf ... square of complex argument as real result
 *
 *   complex functions (complex -> complex):
 *      FUN_conj ... conjugation of complex argument
 *      FUN_csqr ... square of complex argument
 *      FUN_cpolar ... converts rectangular to polar representation (using atan2 ANSI C function)
 *      FUN_crect ... converts polar to rectangular representation (using sin and cos ANSI C functions)
 *      FUN_cabs ... absolute value of complex argument
 *
 */


/***************************************************
 *
 *    math functions
 *
 */
 
#define FUN_sin(in, out) ((out) = sin(in))
#define FUN_cos(in, out) ((out) = cos(in))
#define FUN_tan(in, out) ((out) = tan(in))
#define FUN_asin(in, out) ((out) = asin(in))
#define FUN_acos(in, out) ((out) = acos(in))
#define FUN_atan(in, out) ((out) = atan(in))
#define FUN_sinh(in, out) ((out) = sinh(in))
#define FUN_cosh(in, out) ((out) = cosh(in))
#define FUN_tanh(in, out) ((out) = tanh(in))

#define FUN_exp(in, out) ((out) = exp(in))
#define FUN_log(in, out) ((out) = log(in))
#define FUN_log10(in, out) ((out) = log10(in))
#define FUN_expb(in, scl, out) ((out) = exp((scl) * (in)))
#define FUN_logb(in, scl, out) ((out) = (scl) * log((in)))

#define FUN_sqrt(in, out) ((out) = sqrt(in))
#define FUN_rsqr(in, out) ((out) = 1./sqrt(in))


/***************************************************
 *
 *   logical operators (integral -> integral)
 *
 */
 
#define FUN_bitnot(in, out) ((out) = ~(in))
 
  
/***************************************************
 *
 *    absolute vals
 *
 */

#define FUN_fabs(in, out) ((out) = fabs(in))
#define FUN_iabs(in, out) ((out) = ((in) >= 0)? (in): -(in))


/*******************************************************************
 *
 *    copy / type conversion
 *
 */
 
#define FUN_ccpy(in, out) ((out).re = (in).re, (out).im = (in).im)
#define FUN_cpy(in, out) ((out) = (in))
#define FUN_round(in, out) ((out) = (in) + 0.5)

#define FUN_re(in, out) ((out) = (in).re)
#define FUN_im(in, out) ((out) = (in).im)
#define FUN_fcpyre(in, out) ((out).re = (in))
#define FUN_fcpyim(in, out) ((out).im = (in))
#define FUN_fcpyc(in, out) ((out).re = (in), (out).im = 0)


/***************************************************
 *
 *    convert complex to real (complex -> real)
 *
 */

#define FUN_cabsf(in, out) ((out) = sqrt((in).re * (in).re + (in).im * (in).im))
#define FUN_csqrf(in, out) ((out) = (in).re * (in).re + (in).im * (in).im)


/***************************************************
 *
 *    complex functions
 *
 */

#define FUN_cabs(in, out)\
 ((out).re = sqrt((in).re * (in).re + (in).im * (in).im), (out).im = 0)
#define FUN_conj(in, out)\
 ((out).re = (in).re, (out).im = -(in).im)
#define FUN_csqr(in, out)\
  ((out).re = (in).re * (in).re + (in).im * (in).im, (out).im = 0)
#define FUN_cpolar(in, out)\
  ((out).re = sqrt((in).re * (in).re + (in).im * (in).im), (out).im = atan2((in).im, (in).re))
#define FUN_crect(in, out)\
 ((out).re = (in).re * cos((in).im), (out).im = (in).re * sin((in).im))


#endif /* _KERNELSFUN_H_ */
