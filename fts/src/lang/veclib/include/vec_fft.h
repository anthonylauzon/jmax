/* vecfft.h */

#ifndef _VECFFT_H_
#define _VECFFT_H_

#include "lang/veclib/vecstat.h" /* status messages (errors) */
#include "lang/veclib/complex.h"

#define FTS_MIN_FFT_SIZE 16
#define FTS_MAX_FFT_SIZE 134217728 /* enough? */

extern fts_status_t fts_fft_declaresize(long n_points);
extern int fts_is_fft_size(long n_points);

extern fts_status_t fts_cfft_inplc(complex *buf, long n_points);
extern fts_status_t fts_cifft_inplc(complex *buf, long n_points);
extern fts_status_t fts_rfft_inplc(float *buf, long n_points); /* takes real buffer of n_points! */
extern fts_status_t fts_rifft_inplc(float *buf, long n_points); /* "returns" real buffer of n_points! */

extern fts_status_t fts_cfft(complex *in, complex *out, long n_points);
extern fts_status_t fts_cifft(complex *in, complex *out, long n_points);
extern fts_status_t fts_rfft(float *in, complex *out, long n_points);
extern fts_status_t fts_rifft(complex *in, float *out, long n_points);

#endif /* _VECFFT_H_ */
