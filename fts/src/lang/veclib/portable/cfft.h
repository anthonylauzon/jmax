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

/* cfft.h */

#ifndef _CFFT_H_
#define _CFFT_H_

#include "lang/veclib/complex.h"

/*********************************************************************************
 *
 *    complex FFT computation and utillities
 *
 */

extern void cfft_inplc(complex * restrict buf, float *coef_re, float *coef_im, int size);
extern void cifft_inplc(complex * restrict buf, float *coef_re, float *coef_im, int size);

/* (I)FFT with double oversampled coefficient tables */
extern void cfft_inplc_over_coef(complex * restrict buf, float *coef_re, float *coef_im, int over, int size);
extern void cifft_inplc_over_coef(complex * restrict buf, float *coef_re, float *coef_im, int over, int size);

extern void cfft_bitreversal_inplc(complex *buf, int *bitrev, int size);
extern void cfft_bitreversal_outplc(complex *in, complex *out, int *bitrev, int size);

extern int *cfft_make_bitreversed_table(int size);

/* declarations Added by MDC */

extern void cfft_bitreversal_over_inplc(complex * restrict buf, int * restrict bitrev, int over, int size);
extern void cfft_bitreversal_over_outplc(complex * restrict in, complex * restrict out, int * restrict bitrev, int over, int size);

#endif

