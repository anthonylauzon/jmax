/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/* vecfft.h */

#ifndef _VECFFT_H_
#define _VECFFT_H_

#include "lang/veclib/complex.h"

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
