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
 */

#include <math.h>
#include "cfft.h"

/***************************************************************************
 *
 *    fft computation on bit reversed shuffled data
 *      for fft: coef = exp(j*2*PI*n/N), n = 0..N-1 
 *      for ifft: coef = exp(-j*2*PI*n/N), n = 0..N-1
 *         see routine: generate_fft_coefficients()
 *
 */

void 
cfft_inplc(complex * restrict buf, float * restrict coef_re, float * restrict coef_im, unsigned int size)
{
  unsigned int m, n;
  unsigned int j, k, up, down;
  
  for(up=1, down=size>>1; up<size; up<<=1, down>>=1)
    {
      for(j=0, k=0; j<up; j++, k+=down)
	{
	  float Wre = coef_re[k];
	  float Wim = coef_im[k];
	  unsigned int incr = 2 * up;
	  
	  for(m=j, n=j+up; m<size; m+=incr, n+=incr)
	    {
	      float Are = buf[m].re;
	      float Aim = buf[m].im;
	      float Bre = buf[n].re;
	      float Bim = buf[n].im;
	      float Cre = Bim * Wim + Bre * Wre;
	      float Cim = Bim * Wre - Bre * Wim;
	      buf[m].re = Are + Cre;
	      buf[m].im = Aim + Cim;
	      buf[n].re = Are - Cre;
	      buf[n].im = Aim - Cim;
	    }
	}
    }  
}

void 
cifft_inplc(complex * restrict buf, float * restrict coef_re, float * restrict coef_im, unsigned int size)
{
  unsigned int m, n;
  unsigned int j, k, up, down;
  
  for(up=1, down=size>>1; up<size; up<<=1, down>>=1)
    {
      for(j=0, k=0; j<up; j++, k+=down)
	{
	  float Wre = coef_re[k];
	  float Wim = coef_im[k];
	  unsigned int incr = 2 * up;
	  
	  for(m=j, n=j+up; m<size; m+=incr, n+=incr)
	    {
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

/* (I)FFT with over-sampled coefficient tables */

void 
cfft_inplc_over_coef(complex * restrict buf, float *coef_re, float *coef_im, unsigned int size)
{
  unsigned int m, n;
  unsigned int j, k, up, down;

  for(up=1, down=size>>1; up<size; up<<=1, down>>=1)
    {
      for(j=0, k=0; j<up; j++, k+=2*down)
	{
	  float Wre = coef_re[k];
	  float Wim = coef_im[k];
	  unsigned int incr = 2 * up;
	  
	  for(m=j, n=j+up; m<size; m+=incr, n+=incr)
	    {
	      float Are = buf[m].re;
	      float Aim = buf[m].im;
	      float Bre = buf[n].re;
	      float Bim = buf[n].im;
	      float Cre = Bim * Wim + Bre * Wre;
	      float Cim = Bim * Wre - Bre * Wim;
	      buf[m].re = Are + Cre;
	      buf[m].im = Aim + Cim;
	      buf[n].re = Are - Cre;
	      buf[n].im = Aim - Cim;
	    }
	}
    }  
}

void 
cifft_inplc_over_coef(complex * restrict buf, float *coef_re, float *coef_im, unsigned int size)
{
  unsigned int m, n;
  unsigned int j, k, up, down;

  for(up=1, down=size>>1; up<size; up<<=1, down>>=1)
    {
      for(j=0, k=0; j<up; j++, k+=2*down)
	{
	  float Wre = coef_re[k];
	  float Wim = coef_im[k];
	  unsigned int incr = 2 * up;
	  
	  for(m=j, n=j+up; m<size; m+=incr, n+=incr)
	    {
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

void 
cfft_bitreversal_inplc(complex * restrict buf, unsigned int * restrict bitrev, unsigned int size)
{
  unsigned int idx;
  complex z;

  for(idx=0; idx<size; idx++)
    {
      unsigned int xdi = bitrev[idx];
      if(xdi > idx)
	{
	  z = buf[idx];    
	  buf[idx] = buf[xdi];
	  buf[xdi] = z;    
	}
    }
}

void 
cfft_bitreversal_outplc(complex * restrict in, complex * restrict out, unsigned int * restrict bitrev, unsigned int size)
{
  unsigned int idx;

  for(idx=0; idx<size; idx++)
    {
      unsigned int xdi = bitrev[idx];

      out[xdi] = in[idx];
    }
}

/* bitreversal with oversampled index table */

void 
cfft_bitreversal_over_inplc(complex * restrict buf, unsigned int * restrict bitrev, unsigned int size)
{
  unsigned int idx;
  complex z;

  for(idx=0; idx<size; idx++)
    {
      unsigned int xdi = bitrev[2 * idx];

      if(xdi > idx)
	{
	  z = buf[idx];    
	  buf[idx] = buf[xdi];
	  buf[xdi] = z;    
	}
    }
}

void 
cfft_bitreversal_over_outplc(complex * restrict in, complex * restrict out, unsigned int * restrict bitrev, unsigned int size)
{
  unsigned int idx;

  for(idx=0; idx<size; idx++)
    {
      unsigned int xdi = bitrev[2 * idx];

      out[xdi] = in[idx];
    }
}

/***************************************************************************
 *
 *    compute table of bitreversed indices
 *
 */

unsigned int *
cfft_make_bitreversed_table(unsigned int size)
{
  unsigned int log_size;
  unsigned int *bitrev;
  unsigned int idx, xdi;
  unsigned int i, j;

  bitrev = (unsigned int *)fts_malloc(size * sizeof(unsigned int));

  for(log_size=-1, i=size; i; i>>=1, log_size++)
    ;

  for(i=0; i<size; i++)
    {
      idx = i;
      xdi = 0;

      for(j=1; j<log_size; j++)
	{
	  xdi += (idx & 1);
	  xdi <<= 1;
	  idx >>= 1;
	}
      
      bitrev[i] = xdi + (idx & 1);
    }
  
  return bitrev;
}
