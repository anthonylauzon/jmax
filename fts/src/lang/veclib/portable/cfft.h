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


#endif

