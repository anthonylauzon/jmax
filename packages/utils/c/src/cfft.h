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
 */

#ifndef _CFFT_H_
#define _CFFT_H_

#include "utils.h"

/*********************************************************************************
 *
 *    complex FFT computation and utillities
 *
 */

void cfft_inplc(complex * restrict buf, float *coef_re, float *coef_im, int size);
void cifft_inplc(complex * restrict buf, float *coef_re, float *coef_im, int size);

/* (I)FFT with double oversampled coefficient tables */
void cfft_inplc_over_coef(complex * restrict buf, float *coef_re, float *coef_im, int over, int size);
void cifft_inplc_over_coef(complex * restrict buf, float *coef_re, float *coef_im, int over, int size);

void cfft_bitreversal_inplc(complex *buf, int *bitrev, int size);
void cfft_bitreversal_outplc(complex *in, complex *out, int *bitrev, int size);

int *cfft_make_bitreversed_table(int size);

/* declarations Added by MDC */

void cfft_bitreversal_over_inplc(complex * restrict buf, int * restrict bitrev, int over, int size);
void cfft_bitreversal_over_outplc(complex * restrict in, complex * restrict out, int * restrict bitrev, int over, int size);

#endif
