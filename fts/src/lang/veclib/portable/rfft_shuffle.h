#include "lang/veclib/complex.h"

/*****************************************************************************************
 *
 *    rfft_shuffle_after_fft() and rfft_shuffle_befor_ifft()
 *
 *      shuffling routines to compute the positive half of a spectra out of the FFT
 *      of a 2*N points real signal treated as real and imaginary part of a complex
 *      signal and vice versa:
 *
 *      with:
 *        X+ = rfft_shuffle_after_fft(S, ...) ... use after complex FFT
 *      and:
 *        S = rfft_shuffle_befor_ifft(X+, ...) ... use befor complex IFFT
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
 *      coef_re ... lookup table with cos(2*PI*i/size), i = 0 ... size/2-1
 *      coef_im ... lookup table with sin(2*PI*i/size), i = 0 ... size/2-1
 *      size ... # of complex points (cfft size)
 *      (Note: the lookup tables just contain half of the sine/cosine period in size points)
 */
 
extern void rfft_shuffle_after_fft_inplc(complex *buf, float *coef_re, float *coef_im, int size);
extern void rfft_shuffle_after_fft_outplc(complex *in, complex *out, float *coef_re, float *coef_im, int size);

extern void rfft_shuffle_before_ifft_inplc(complex *buf, float *coef_re, float *coef_im, int size);
extern void rfft_shuffle_before_ifft_outplc(complex *in, complex *out, float *coef_re, float *coef_im, int size);
