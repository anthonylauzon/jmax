/* vecfft.c */

#include "sys.h"
#include "lang/mess.h"

#include "lang/veclib/include/vec_fft.h"
#include "lang/veclib/complex.h"
#include "lang/veclib/portable/rfft_shuffle.h"
#include "lang/veclib/portable/cfft.h"

#define CHECK_LOOKUPS(p, n) ((p)? ((p)->nperiod / (n)): (0))

/**************************************************************************************
 *
 *    fts_is_fft_size()
 */
 
int fts_is_fft_size(long n)
{
  if(n < FTS_MIN_FFT_SIZE || n > FTS_MAX_FFT_SIZE) return(0);

  /* power of 2? */
  while((n >>= 1) && !(n & 1))
    ;

  return(n == 1);
}

/**************************************************************************************\
 *
 *    fts_fft_declaresize()
 *
 */
 
fts_status_t fts_fft_declaresize(long n_points)
{
  complex *coef, *icoef;
  long *bitrev;
    
  if(!fts_is_fft_size(n_points)) return &fts_vec_NoFftSize;
  
  if(!the_fft_lookups){
    the_fft_lookups = (fft_lookup_t *)fts_malloc(sizeof(fft_lookup_t));
  }else if(the_fft_lookups->nperiod >= n_points){
    return(fts_Success);
  }else{
    fts_free((void *)the_fft_lookups->fftcoef);
    fts_free((void *)the_fft_lookups->ifftcoef);
    fts_free((void *)the_fft_lookups->bitrev);
  }
  
  /* allocate space for lookup tables */
  if(!(coef = (complex *)fts_malloc(n_points * sizeof(complex)))){
    fts_free((void *)the_fft_lookups);
    the_fft_lookups = (fft_lookup_t *)0;
    return &fts_vec_NoLookups;
  }else if(!(icoef = (complex *)fts_malloc(n_points * sizeof(complex)))){
    fts_free((void *)the_fft_lookups);
    the_fft_lookups = (fft_lookup_t *)0;
    fts_free((void *)coef);
    return &fts_vec_NoLookups;
  }else if(!(bitrev = (long *)fts_malloc(n_points * sizeof(long)))){
    fts_free((void *)the_fft_lookups);
    the_fft_lookups = (fft_lookup_t *)0;
    fts_free((void *)coef);
    fts_free((void *)icoef);
    return &fts_vec_NoLookups;
  }
  
  generate_fft_coefficients(coef, icoef, n_points);
  generate_bitreversed_indices(bitrev, n_points);

  the_fft_lookups->fftcoef = coef;
  the_fft_lookups->ifftcoef = icoef;
  the_fft_lookups->bitrev = bitrev;
  the_fft_lookups->nperiod = n_points;
    
  return(fts_Success);
}

/**************************************************************************************\
 *
 *    the complex fft and ifft
 */

fts_status_t fts_cfft_inplc(complex *buf, long n_points)
{
  long n_over = CHECK_LOOKUPS(the_fft_lookups, n_points);
  if(!n_over) return &fts_vec_NoLookups;
    
  bitreversal_inplc(buf, the_fft_lookups->bitrev, n_over, n_points);
  cfft_inplc(buf, the_fft_lookups->fftcoef, n_over, n_points);
  
  return(fts_Success);
}

fts_status_t fts_cfft(complex *in, complex *out, long n_points)
{
  long n_over = CHECK_LOOKUPS(the_fft_lookups, n_points);
  if(!n_over) return &fts_vec_NoLookups;
    
  if(in == out)
    bitreversal_inplc(out, the_fft_lookups->bitrev, n_over, n_points);
  else
    bitreversal(in, out, the_fft_lookups->bitrev, n_over, n_points);
  cfft_inplc(out, the_fft_lookups->fftcoef, n_over, n_points);
  
  return(fts_Success);
}

fts_status_t fts_cifft_inplc(complex *buf, long n_points)
{
  long n_over = CHECK_LOOKUPS(the_fft_lookups, n_points);
  if(!n_over) return &fts_vec_NoLookups;
    
  bitreversal_inplc(buf, the_fft_lookups->bitrev, n_over, n_points);
  cfft_inplc(buf, the_fft_lookups->ifftcoef, n_over, n_points);
  
  return(fts_Success);
}

fts_status_t fts_cifft(complex *in, complex *out, long n_points)
{
  long n_over = CHECK_LOOKUPS(the_fft_lookups, n_points);
  if(!n_over) return &fts_vec_NoLookups;
    
  if(in == out)
    bitreversal_inplc(out, the_fft_lookups->bitrev, n_over, n_points);
  else
    bitreversal(in, out, the_fft_lookups->bitrev, n_over, n_points);
  cfft_inplc(out, the_fft_lookups->ifftcoef, n_over, n_points);
  
  return(fts_Success);
}

/**************************************************************************************\
 *
 *    the "real" fft
 */

fts_status_t fts_rfft_inplc(float *buf, long n_points)
{
  long n_cplx = n_points >> 1;
  long n_over = CHECK_LOOKUPS(the_fft_lookups, n_cplx);
  if(n_over < 2) return &fts_vec_NoLookups;
    
  bitreversal_inplc((complex *)buf, the_fft_lookups->bitrev, n_over, n_cplx);
  cfft_inplc((complex *)buf, the_fft_lookups->fftcoef, n_over, n_cplx);
  shuffle_after_rfft_inplc((complex *)buf, the_fft_lookups->fftcoef, n_over, n_cplx);
  
  return(fts_Success);
}

fts_status_t fts_rfft(float *in, complex *out, long n_points)
{
  long n_cplx = n_points >> 1;
  long n_over = CHECK_LOOKUPS(the_fft_lookups, n_cplx);
  if(n_over < 2) return &fts_vec_NoLookups;
    
  if(in == (float *)out)
    bitreversal_inplc(out, the_fft_lookups->bitrev, n_over, n_cplx);
  else
    bitreversal((complex *)in, out, the_fft_lookups->bitrev, n_over, n_cplx);
  cfft_inplc(out, the_fft_lookups->fftcoef, n_over, n_cplx);
  shuffle_after_rfft_inplc(out, the_fft_lookups->fftcoef, n_over, n_cplx);
  
  return(fts_Success);
}

fts_status_t fts_rifft_inplc(float *buf, long n_points)
{
  long n_cplx = n_points >> 1;
  long n_over = CHECK_LOOKUPS(the_fft_lookups, n_cplx);
  if(n_over < 2) return &fts_vec_NoLookups;
    
  shuffle_before_rifft_inplc((complex *)buf, the_fft_lookups->ifftcoef, n_over, n_cplx);
  bitreversal_inplc((complex *)buf, the_fft_lookups->bitrev, n_over, n_cplx);
  cfft_inplc((complex *)buf, the_fft_lookups->ifftcoef, n_over, n_cplx);
  
  return(fts_Success);
}

fts_status_t fts_rifft(complex *in, float *out, long n_points)
{
  long n_cplx = n_points >> 1;
  long n_over = CHECK_LOOKUPS(the_fft_lookups, n_cplx);
  if(n_over < 2) return &fts_vec_NoLookups;
    
  shuffle_before_rifft(in, (complex *)out, the_fft_lookups->ifftcoef, n_over, n_cplx);
  bitreversal_inplc((complex *)out, the_fft_lookups->bitrev, n_over, n_cplx);
  cfft_inplc((complex *)out, the_fft_lookups->ifftcoef, n_over, n_cplx);
  
  return(fts_Success);
}
