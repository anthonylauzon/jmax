/* cfft.h */

#ifndef _CFFT_H_
#define _CFFT_H_

#include "lang/veclib/complex.h"

/**************************************************************************************
 *
 *    stucture and root pointer for fft lookups
 */
 
typedef struct{
  complex *fftcoef;          /* fft coefficients */
  complex *ifftcoef;         /* ifft coefficients */
  long *bitrev;              /* lookup for bitreversing */ 
  long nperiod;              /* period of the sine/cosine (in most cases same as fftsize) */
} fft_lookup_t;
 
extern fft_lookup_t *the_fft_lookups;

/*********************************************************************************
 *
 *    complex FFT computation and utillities
 *
 */

extern void cfft_inplc(complex * restrict buf, complex * restrict coef, long nstep, long npoints);

extern void bitreversal_inplc(complex *buf, long *bitrev, long nstep, long npoints);
extern void bitreversal(complex *in, complex *out, long *bitrev, long nstep, long npoints);

extern void generate_fft_coefficients(complex *coef, complex *icoef, long npoints);
extern void generate_bitreversed_indices(long *bitrev, long npoints);


#endif

