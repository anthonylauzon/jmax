/* rfft_shuffle.h */

#include "lang/veclib/complex.h"

/*****************************************************************************************
 *
 *    shuffle_after_rfft() and shuffle_befor_rifft()
 *
 *      shuffling routines to compute the positive half of a spectra out of the FFT
 *      of a 2*N points real signal treated as real and imaginary part of a complex
 *      signal and vice versa:
 *
 *      with:
 *        X+ = shuffle_after_rfft(S, ...) ... use after complex FFT
 *      and:
 *        S = shuffle_befor_rifft(X+, ...) ... use befor complex IFFT
 *
 *      where:
 *        x[m], m = 0..2*N-1 ... real signal
 *        X+[k], k = 0..N-1 ... positive part of spectrum of x[m]
 *      and:
 *        s[n] = x[2n] + j x[2n+1], n = 0..N-1 ... real signal as complex vector
 *        S[k], k = 0..N-1 ... complex FFT of complex vector s[n]
 *
 *
 *    arguments:
 *      buf ... buffer for inplace shuffling
 *      in, out ... input vector, output vector for non inplace shuffling
 *      coef ... lookup table with e^(-j*2*PI*n), n = 0..n_coef_period/2-1
 *      n_over ... n_coef_period / n_points
 *      n_points ... # of complex points (cfft size)
 *        (this routines may use the FFT lookup tables, which can be shared by FFT's of different size)
 */
 
extern void shuffle_after_rfft_inplc(complex *buf, complex *coef, long n_over, long n_points);
extern void shuffle_before_rifft_inplc(complex *buf, complex *coef, long n_over, long n_points);

extern void shuffle_after_rfft(complex *in, complex *out, complex *coef, long n_over, long n_points);
extern void shuffle_before_rifft(complex *in, complex *out, complex *coef, long n_over, long n_points);
