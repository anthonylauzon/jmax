/* cfft.c */

#include <math.h>
#include "lang/veclib/complex.h"
#include "lang/veclib/portable/cfft.h"

fft_lookup_t *the_fft_lookups = (fft_lookup_t *)0;

/***************************************************************************
 *
 *    fft computation on bit reversed shuffled data
 *    (based on the UDI routine by P.Depalle)
 *      for fft: coef = exp(j*2*PI*n/N), n = 0..N-1 
 *      for ifft: coef = exp(-j*2*PI*n/N), n = 0..N-1
 *         see routine: generate_fft_coefficients()
 *
 */
void cfft_inplc(complex *buf, complex *coef, long nstep, long npoints)
{
  long log2;
  long m, n;
  long i, j, k, halfup, up, down;
  
  /* log2 = log2(npoints) */ 
  for(log2=-1, n=npoints; n; n>>=1, log2++)
    ; 

  for(i=1, halfup=1, up=2, down=npoints>>1; i<=log2; i++, halfup<<=1, up<<=1, down>>=1){

    for(j=0, k=0; j<halfup; j++, k+=(down*nstep)){
      float Wre = coef[k].re;
      float Wim = coef[k].im;
      
      for(m=j, n=j+halfup; m<npoints; m+=up, n+=up){
        float Are = buf[m].re;
        float Aim = buf[m].im;
        float Bre = buf[n].re;
        float Bim = buf[n].im;
        float Cre = Bre * Wre - Bim * Wim;
        float Cim = Bre * Wim + Bim * Wre;
        buf[m].re = Are + Cre;
        buf[m].im = Aim + Cim;
        buf[n].re = Are - Cre;
        buf[n].im = Aim - Cim;
      }
    }
  }  
}

/***************************************************************************
 *
 *    bitreversal buffer 
 *
 */
void bitreversal_inplc(complex *buf, long *bitrev, long nstep, long npoints)
{
  long idx, xdi;
  complex z;
  long nshift;

  /* nshift = log2(nstep) */
  for(nshift=-1; nstep; nstep>>=1, nshift++)
    ;

  for(idx=0; idx<npoints; idx++){
    if((xdi = (bitrev[idx]>>nshift)) > idx){
      z = buf[idx];    
      buf[idx] = buf[xdi];
      buf[xdi] = z;    
    }
  }
}

void bitreversal(complex *in, complex *out, long *bitrev, long nstep, long npoints)
{
  long idx, xdi;
  long nshift;

  /* nshift = log2(nstep) */
  for(nshift=-1; nstep; nstep>>=1, nshift++)
    ;

  for(idx=0; idx<npoints; idx++){
    xdi = bitrev[idx]>>nshift;
    out[xdi] = in[idx];    
    out[idx] = in[xdi];
  }
}

/***************************************************************************
 *
 *    compute weights for the fft
 *
 */
void generate_fft_coefficients(complex *coef, complex *icoef, long npoints)
{
  long i;
  double phase = 0;
  double phsinc = 6.283185307 / npoints;
  
  for(i=0; i<npoints; i++){
    coef[i].re = icoef[i].re = cos(phase);
    coef[i].im = -(icoef[i].im = sin(phase));
    phase += phsinc;
  }
}

/***************************************************************************
 *
 *    compute table of bitreversed indices
 *
 */
void generate_bitreversed_indices(long *bitrev, long npoints)
{
  long idx, xdi;
  long i, j;
  long log2;

  /* log2 = log2(npoints) */
  for(log2=-1, i=npoints; i; i>>=1, log2++)
    ;

  for(i=0; i<npoints; i++){
    idx = i;
    xdi = 0;
    for(j=1; j<log2; j++){
      xdi += (idx & 1);
      xdi <<= 1;
      idx >>= 1;
    }
    bitrev[i] = xdi + (idx & 1);
  }
}
