/* rfft_shuffle.c */

#include "lang/veclib/portable/rfft_shuffle.h"

void shuffle_after_rfft_inplc(complex * restrict buf, complex * restrict coef, long n_over, long n_points)
{
  long idx, xdi, i;
  int n_step = n_over>>1;
  long n_half = n_points>>1;
    
  buf[0].re = buf[0].re + buf[0].im;
  buf[0].im = 0;
  for(idx=1, xdi=n_points-1, i=n_step; idx<n_half; idx++, xdi--, i+=n_step){
    float x1_re = 0.5f * (buf[idx].re + buf[xdi].re);
    float x1_im = 0.5f * (buf[idx].im - buf[xdi].im);
    float x2_re = 0.5f * (buf[xdi].im + buf[idx].im);
    float x2_im = 0.5f * (buf[xdi].re - buf[idx].re);
    float x2Ej_re = x2_re * coef[i].re - x2_im * coef[i].im; /* real of x2[idx] * exp(-j*PI*i/(2*n_points)) */
    float x2Ejim = x2_re * coef[i].im + x2_im * coef[i].re; /* imaginary of x2[idx] * exp(-j*PI*i/(2*n_points)) */
    buf[idx].re = x1_re + x2Ej_re;
    buf[idx].im = x1_im + x2Ejim;
    buf[xdi].re = x1_re - x2Ej_re;
    buf[xdi].im = x2Ejim - x1_im;
  }
  /* buf[n_half].re = buf[n_half].re; */
  buf[n_half].im = -buf[n_half].im;
}

void shuffle_after_rfft_outplc(complex * restrict in, complex * restrict out, complex * restrict coef, long n_over, long n_points)
{
  long idx, xdi, i;
  int n_step = n_over>>1;
  long n_half = n_points>>1;

  out[0].re = in[0].re + in[0].im;
  out[0].im = 0;
  for(idx=1, xdi=n_points-1, i=n_step; idx<n_half; idx++, xdi--, i+=n_step){
    float x1_re = 0.5f * (in[idx].re + in[xdi].re);
    float x1_im = 0.5f * (in[idx].im - in[xdi].im);
    float x2_re = 0.5f * (in[xdi].im + in[idx].im);
    float x2_im = 0.5f * (in[xdi].re - in[idx].re);
    float x2Ej_re = x2_re * coef[i].re - x2_im * coef[i].im; /* real of x2[idx] * exp(-j*PI*i/(2*n_points)) */
    float x2Ejim = x2_re * coef[i].im + x2_im * coef[i].re; /* imaginary of x2[idx] * exp(-j*PI*i/(2*n_points)) */
    out[idx].re = x1_re + x2Ej_re;
    out[idx].im = x1_im + x2Ejim;
    out[xdi].re = x1_re - x2Ej_re;
    out[xdi].im = x2Ejim - x1_im;
  }
  out[n_half].re = in[n_half].re;
  out[n_half].im = -in[n_half].im;
}

void shuffle_before_rifft_inplc(complex * restrict buf, complex * restrict coef, long n_over, long n_points)
{
  long idx, xdi, i;
  int n_step = n_over>>1;
  long n_half = n_points>>1;

  buf[0].im = buf[0].re; /* PI-freq ignored!!! */
  for(idx=1, xdi=n_points-1, i=n_step; idx<n_half; idx++, xdi--, i+=n_step){
    float x1_re = buf[idx].re + buf[xdi].re;
    float x1_im = buf[idx].im - buf[xdi].im;
    float x2Ej_re = buf[idx].re - buf[xdi].re; /* real of x2[idx] * exp(-j*PI*i/(2*n_points)) */
    float x2Ejim = buf[idx].im + buf[xdi].im; /* imaginary of x2[idx] * exp(-j*PI*i/(2*n_points)) */
    float x2_re = x2Ej_re * coef[i].re - x2Ejim * coef[i].im; /* real of x2 */
    float x2_im = x2Ej_re * coef[i].im + x2Ejim * coef[i].re; /* imaginary of x2 */
    buf[idx].re = x1_re - x2_im;
    buf[idx].im = x1_im + x2_re;
    buf[xdi].re = x1_re + x2_im;
    buf[xdi].im = x2_re - x1_im;
  }
  buf[n_half].re *= 2;
  buf[n_half].im *= -2;
}

void shuffle_before_rifft_outplc(complex * restrict in, complex * restrict out, complex * restrict coef, long n_over, long n_points)
{
  long idx, xdi, i;
  int n_step = n_over>>1;
  long n_half = n_points>>1;

  out[0].re = out[0].im = in[0].re; /* PI-freq ignored!!! */
  for(idx=1, xdi=n_points-1, i=n_step; idx<n_half; idx++, xdi--, i+=n_step){
    float x1_re = in[idx].re + in[xdi].re;
    float x1_im = in[idx].im - in[xdi].im;
    float x2Ej_re = in[idx].re - in[xdi].re; /* real of x2[idx] * exp(-j*PI*i/(2*n_points)) */
    float x2Ejim = in[idx].im + in[xdi].im; /* imaginary of x2[idx] * exp(-j*PI*i/(2*n_points)) */
    float x2_re = x2Ej_re * coef[i].re - x2Ejim * coef[i].im; /* real of x2 */
    float x2_im = x2Ej_re * coef[i].im + x2Ejim * coef[i].re; /* imaginary of x2 */
    out[idx].re = x1_re - x2_im;
    out[idx].im = x1_im + x2_re;
    out[xdi].re = x1_re + x2_im;
    out[xdi].im = x2_re - x1_im;
  }
  out[n_half].re = 2 * in[n_half].re;
  out[n_half].im = -2 * in[n_half].im;
}
