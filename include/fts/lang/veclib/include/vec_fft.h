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

/* vecfft.h */

#ifndef _VECFFT_H_
#define _VECFFT_H_

#include <fts/lang/veclib/complex.h>

#define FTS_FFT_MIN_SIZE 16

#define FTS_FFT_MAX_SIZE_LOG2 16
#define FTS_FFT_MAX_SIZE (1 << FTS_FFT_MAX_SIZE_LOG2)

/* intitialize FFT/IFFT procedures for a specific size (returns non zero when successfull) */
extern int fts_fft_declaresize(int size);
/* check FFT/IFFT size (returns non zero for valid fft size) */
extern int fts_is_fft_size(int size);

extern void fts_cfft_inplc(complex *buf, int size);
extern void fts_cifft_inplc(complex *buf, int size);
extern void fts_rfft_inplc(float *buf, int size); /* takes real buffer of size size */
extern void fts_rifft_inplc(float *buf, int size); /* "returns" real buffer of size size */

extern void fts_cfft(complex *in, complex *out, int size);
extern void fts_cifft(complex *in, complex *out, int size);
extern void fts_rfft(float *in, complex *out, int size);
extern void fts_rifft(complex *in, float *out, int size);

#endif /* _VECFFT_H_ */
