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

#include <fts/fts.h>

void rfft_shuffle_after_fft_inplc(complex * restrict buf, float *coef_re, float *coef_im, int size)
{
  int idx, xdi;
    
  buf[0].re = buf[0].re + buf[0].im;
  buf[0].im = 0;
  for(idx=1, xdi=size-1; idx<size/2; idx++, xdi--){
    float x1_re = 0.5f * (buf[idx].re + buf[xdi].re);
    float x1_im = 0.5f * (buf[idx].im - buf[xdi].im);
    float x2_re = 0.5f * (buf[xdi].im + buf[idx].im);
    float x2_im = 0.5f * (buf[xdi].re - buf[idx].re);
    float x2Ej_re = x2_im * coef_im[idx] + x2_re * coef_re[idx]; /* real of x2[idx] * exp(-j*PI*i/size) */
    float x2Ej_im = x2_im * coef_re[idx] - x2_re * coef_im[idx]; /* imaginary of x2[idx] * exp(-j*PI*i/size) */
    buf[idx].re = x1_re + x2Ej_re;
    buf[idx].im = x1_im + x2Ej_im;
    buf[xdi].re = x1_re - x2Ej_re;
    buf[xdi].im = x2Ej_im - x1_im;
  }
  /* buf[idx].re = buf[idx].re; */
  buf[idx].im = -buf[idx].im;
}

void rfft_shuffle_after_fft_outplc(complex * restrict in, complex * restrict out, float *coef_re, float *coef_im, int size)
{
  int idx, xdi;

  out[0].re = in[0].re + in[0].im;
  out[0].im = 0;
  for(idx=1, xdi=size-1; idx<size/2; idx++, xdi--){
    float x1_re = 0.5f * (in[idx].re + in[xdi].re);
    float x1_im = 0.5f * (in[idx].im - in[xdi].im);
    float x2_re = 0.5f * (in[xdi].im + in[idx].im);
    float x2_im = 0.5f * (in[xdi].re - in[idx].re);
    float x2Ej_re = x2_im * coef_im[idx] + x2_re * coef_re[idx]; /* real of x2[idx] * exp(-j*PI*i/size) */
    float x2Ej_im = x2_im * coef_re[idx] - x2_re * coef_im[idx]; /* imaginary of x2[idx] * exp(-j*PI*i/size) */
    out[idx].re = x1_re + x2Ej_re;
    out[idx].im = x1_im + x2Ej_im;
    out[xdi].re = x1_re - x2Ej_re;
    out[xdi].im = x2Ej_im - x1_im;
  }
  out[idx].re = in[idx].re;
  out[idx].im = -in[idx].im;
}

void rfft_shuffle_before_ifft_inplc(complex * restrict buf, float *coef_re, float *coef_im, int size)
{
  int idx, xdi;

  buf[0].im = buf[0].re; /* PI-freq ignored!!! */
  for(idx=1, xdi=size-1; idx<size/2; idx++, xdi--){
    float x1_re = buf[idx].re + buf[xdi].re;
    float x1_im = buf[idx].im - buf[xdi].im;
    float x2Ej_re = buf[idx].re - buf[xdi].re; /* real of x2[idx] * exp(-j*PI*i/size) */
    float x2Ej_im = buf[idx].im + buf[xdi].im; /* imaginary of x2[idx] * exp(-j*PI*i/size) */
    float x2_re = x2Ej_re * coef_re[idx] - x2Ej_im * coef_im[idx]; /* real of x2 */
    float x2_im = x2Ej_re * coef_im[idx] + x2Ej_im * coef_re[idx]; /* imaginary of x2 */
    buf[idx].re = x1_re - x2_im;
    buf[idx].im = x1_im + x2_re;
    buf[xdi].re = x1_re + x2_im;
    buf[xdi].im = x2_re - x1_im;
  }
  buf[idx].re *= 2;
  buf[idx].im *= -2;
}

void rfft_shuffle_before_ifft_outplc(complex * restrict in, complex * restrict out, float *coef_re, float *coef_im, int size)
{
  int idx, xdi;

  out[0].re = out[0].im = in[0].re; /* PI-freq ignored!!! */
  for(idx=1, xdi=size-1; idx<size/2; idx++, xdi--){
    float x1_re = in[idx].re + in[xdi].re;
    float x1_im = in[idx].im - in[xdi].im;
    float x2Ej_re = in[idx].re - in[xdi].re; /* real of x2[idx] * exp(-j*PI*i/size) */
    float x2Ej_im = in[idx].im + in[xdi].im; /* imaginary of x2[idx] * exp(-j*PI*i/size) */
    float x2_re = x2Ej_re * coef_re[idx] - x2Ej_im * coef_im[idx]; /* real of x2 */
    float x2_im = x2Ej_re * coef_im[idx] + x2Ej_im * coef_re[idx]; /* imaginary of x2 */
    out[idx].re = x1_re - x2_im;
    out[idx].im = x1_im + x2_re;
    out[xdi].re = x1_re + x2_im;
    out[xdi].im = x2_re - x1_im;
  }
  out[idx].re = 2 * in[idx].re;
  out[idx].im = -2 * in[idx].im;
}
