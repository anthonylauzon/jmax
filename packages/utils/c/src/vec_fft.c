/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/packages/utils/utils.h>
#include "rfft_shuffle.h"
#include "cfft.h"

/**************************************************************************************
 *
 *    stucture and root pointer for fft lookups
 */
 
typedef struct
{
  unsigned int size;
  float *cos;
  float *sin;
  unsigned int *bitrev;
} fft_lookup_t;
 
static fft_lookup_t the_fft_lookups[32] =
{
  {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL},
  {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL},
  {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, 
  {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL},
  {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, 
  {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL},
  {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, 
  {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL}, {0, NULL, NULL, NULL},
};

static int 
log_dualis(unsigned int n)
{
  unsigned int log2 = 0;
  int i;
  
  for(i=n>>1; i; i>>=1)
    log2++;
  
  return log2;
}

static fft_lookup_t *
fft_lookups_get_or_make(int log2_size)
{
  fft_lookup_t *lookups = the_fft_lookups + log2_size;
  int fft_size = 1 << log2_size;
  
  if(lookups->size == 0 && fft_size >= FTS_FFT_MIN_SIZE)
  {
    lookups->size = fft_size;
    lookups->cos = fts_fftab_get_cosine(fft_size);
    lookups->sin = fts_fftab_get_sine(fft_size);
    lookups->bitrev = cfft_make_bitreversed_table(fft_size);
  }
  
  return lookups;
}

static fft_lookup_t *
fft_lookups_get(unsigned int size)
{
  unsigned int log2_size = log_dualis(size);
  
  return fft_lookups_get_or_make(log2_size);
}

int 
fts_is_fft_size(unsigned int n)
{
  if(n < FTS_FFT_MIN_SIZE)
    return 0;

  /* power of 2? */
  while((n >>= 1) && !(n & 1))
    ;

  return(n == 1);
}

int 
fts_fft_declaresize(unsigned int size)
{
  if(fts_is_fft_size(size))
    {
      fft_lookups_get(size);
      return 1;
    }
  else
    return 0;
}

unsigned int 
fts_get_fft_size(unsigned int size)
{
  unsigned int fft_size = FTS_FFT_MIN_SIZE;
  int i;

  for(i=((size-1)>>FTS_FFT_MIN_LOG2); i>0; i>>=1)
    fft_size <<= 1;

  return fft_size;;
}

/**************************************************************************************\
 *
 *    the complex fft and ifft
 */

void
fts_cfft_inplc(complex *buf, unsigned int size)
{
  fft_lookup_t *lookups = fft_lookups_get(size);

  cfft_bitreversal_inplc(buf, lookups->bitrev, lookups->size);
  cfft_inplc(buf, lookups->cos, lookups->sin, lookups->size);
}

void
fts_cfft(complex *in, complex *out, unsigned int size)
{
  fft_lookup_t *lookups = fft_lookups_get(size);

  if(in == out)
    cfft_bitreversal_inplc(out, lookups->bitrev, lookups->size);
  else
    cfft_bitreversal_outplc(in, out, lookups->bitrev, lookups->size);

  cfft_inplc(out, lookups->cos, lookups->sin, lookups->size);
}

void
fts_cifft_inplc(complex *buf, unsigned int size)
{
  fft_lookup_t *lookups = fft_lookups_get(size);

  cfft_bitreversal_inplc(buf, lookups->bitrev, lookups->size);
  cifft_inplc(buf, lookups->cos, lookups->sin, lookups->size);
}

void
fts_cifft(complex *in, complex *out, unsigned int size)
{
  fft_lookup_t *lookups = fft_lookups_get(size);

  if(in == out)
    cfft_bitreversal_inplc(out, lookups->bitrev, lookups->size);
  else
    cfft_bitreversal_outplc(in, out, lookups->bitrev, lookups->size);

  cifft_inplc(out, lookups->cos, lookups->sin, lookups->size);
}

/**************************************************************************************\
 *
 *    the "real" fft
 *
 */

void
fts_rfft_inplc(float *buf, unsigned int size)
{
  fft_lookup_t *lookups = fft_lookups_get(size);
  unsigned int complex_size = lookups->size >> 1;

  cfft_bitreversal_over_inplc((complex *)buf, lookups->bitrev, complex_size);
  cfft_inplc_over_coef((complex *)buf, lookups->cos, lookups->sin, complex_size);
  rfft_shuffle_after_fft_inplc((complex *)buf, lookups->cos, lookups->sin, complex_size);
}

void
fts_rfft(float *in, complex *out, unsigned int size)
{
  fft_lookup_t *lookups = fft_lookups_get(size);
  unsigned int complex_size = lookups->size >> 1;
    
  if(in == (float *)out)
    cfft_bitreversal_over_inplc(out, lookups->bitrev, complex_size);
  else
    cfft_bitreversal_over_outplc((complex *)in, out, lookups->bitrev, complex_size);

  cfft_inplc_over_coef(out, lookups->cos, lookups->sin, complex_size);
  rfft_shuffle_after_fft_inplc(out, lookups->cos, lookups->sin, complex_size);
}

void
fts_rifft_inplc(float *buf, unsigned int size)
{
  fft_lookup_t *lookups = fft_lookups_get(size);
  unsigned int complex_size = lookups->size >> 1;
    
  rfft_shuffle_before_ifft_inplc((complex *)buf, lookups->cos, lookups->sin, complex_size);
  cfft_bitreversal_over_inplc((complex *)buf, lookups->bitrev, complex_size);
  cifft_inplc_over_coef((complex *)buf, lookups->cos, lookups->sin, complex_size);
}

void
fts_rifft(complex *in, float *out, unsigned int size)
{
  fft_lookup_t *lookups = fft_lookups_get(size);
  unsigned int complex_size = lookups->size >> 1;
    
  if(in == (complex *)out)
    rfft_shuffle_before_ifft_inplc((complex *)out, lookups->cos, lookups->sin, complex_size);
  else
    rfft_shuffle_before_ifft_outplc(in, (complex *)out, lookups->cos, lookups->sin, complex_size);

  cfft_bitreversal_over_inplc((complex *)out, lookups->bitrev, complex_size);
  cifft_inplc_over_coef((complex *)out, lookups->cos, lookups->sin, complex_size);
}

/**************************************************************************************\
 *
 *  optimized static FFT routines (allowing for temporary buffers)
 *
 */
#ifndef WIN32
#define HAS_MACOSX_VECLIB
#endif

#if defined HAS_MACOSX_VECLIB

#include <veclib/vDSP.h>

struct fts_fft
{
  unsigned int size;
  int log2_size;
  int real;
  FFTSetup setup;
  float *buffer;
  DSPSplitComplex split;
};

static FFTSetup fft_setups[32] =
{
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

void
fts_fft_set_size(fts_fft_t *fft, unsigned int size)
{
  int log2_size = log_dualis(size);
  FFTSetup setup = fft_setups[log2_size];
  
  if(setup == NULL)
    setup = fft_setups[log2_size] = create_fftsetup(log2_size, 0);
  
  fft->size = 1 << log2_size;
  fft->log2_size = log2_size;
  fft->setup = setup;
  
  if(fft->buffer != NULL)
    fts_free(fft->buffer);
  
  fft->buffer = fts_malloc(2 * size * sizeof(float));
  fft->split.realp = fft->buffer;
  fft->split.imagp = fft->buffer + size;
}

unsigned int 
fts_fft_get_size(fts_fft_t *fft)
{
  return fft->size;
}

fts_fft_t *
fts_fft_new(unsigned int size)
{
  fts_fft_t *fft = fts_malloc(sizeof(fts_fft_t));

  fft->buffer = NULL;
  fft->split.realp = NULL;
  fft->split.imagp = NULL;
  
  fts_fft_set_size(fft, size);
  
  return fft;
}

void
fts_fft_delete(fts_fft_t *fft)
{
  fts_free(fft->buffer);
  fts_free(fft);
}

static void
fill_complex(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, unsigned int spec_size, float scale)
{
  unsigned int i, j;
  
  if(size > spec_size)
    size = spec_size;
  
  switch(stride)
  {
    case 0:
      /* complex to split */
      ctoz((DSPComplex *)in, 2, &fft->split, 1, size);
      
      /* scale */
      if(scale != 1.0)
      {
        vsmul(fft->split.realp, 1, &scale, fft->split.realp, 1, size);
        vsmul(fft->split.imagp, 1, &scale, fft->split.imagp, 1, size);
      }
        
      /* zero padding */
      bzero(fft->split.realp + size, (spec_size - size) * sizeof(float));
      bzero(fft->split.imagp + size, (spec_size - size) * sizeof(float));      
      break;
      
    case 1:
      /* copy real input without stride */
      memcpy(fft->buffer, in, size * sizeof(float));
      
      /* scale */
      if(scale != 1.0)
        vsmul(fft->buffer, 1, &scale, fft->buffer, 1, size);
        
        /* zero padding and initializing imaginary part to zero */
        bzero(fft->buffer + size, (2 * spec_size - size) * sizeof(float));      
      break;
      
    default:
      /* copy real input with stride and scale */
      for(i=0, j=0; i<size; i++, j+=stride)
        fft->buffer[i] = in[j] * scale;
      
      /* zero padding and initializing imaginary part to zero */
      bzero(fft->buffer + size, (2 * spec_size - size) * sizeof(float));  
      break;
  }
}

static void
fill_real(fts_fft_t *fft, float *in, unsigned int size, int stride, float scale)
{
  unsigned int spec_size = fft->size >> 1;  
  unsigned int i, j;
  
  size >>= 1;
  scale *= 0.5;
  
  if(size > spec_size)
    size = spec_size;
  
  if(stride < 2)
  {
    ctoz((DSPComplex *)in, 2, &fft->split, 1, size);
    
    /* scale */
    if(scale != 1.0)
    {
      vsmul(fft->split.realp, 1, &scale, fft->split.realp, 1, size);
      vsmul(fft->split.imagp, 1, &scale, fft->split.imagp, 1, size);
    }
  }
  else
  {
    /* copy real input with stride and scale */
    for(i=0, j=0; i<spec_size; i++, j+=2*stride)
    {
      fft->split.realp[i] = in[j] * scale;
      fft->split.imagp[i] = in[j + stride] * scale;
    }
  }
  
  /* zero padding */
  bzero(fft->split.realp + size, (spec_size - size) * sizeof(float));
  bzero(fft->split.imagp + size, (spec_size - size) * sizeof(float));      
}

void
fts_fft_complex(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, float scale)
{
  fill_complex(fft, in, size, stride, fft->size, scale);
  fft_zip(fft->setup, &fft->split, 1, fft->log2_size, FFT_FORWARD);
  ztoc(&fft->split, 1, (DSPComplex *)out, 2, fft->size);
}

void
fts_fft_inverse_complex(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, float scale)
{
  fill_complex(fft, in, size, stride, fft->size, scale);
  fft_zip(fft->setup, &fft->split, 1, fft->log2_size, FFT_INVERSE);
  ztoc(&fft->split, 1, (DSPComplex *)out, 2, fft->size);
}

void
fts_fft_real(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, float scale, float *ny)
{
  float f;
  
  fill_real(fft, in, size, stride, scale);
  fft_zrip(fft->setup, &fft->split, 1, fft->log2_size, FFT_FORWARD);
  
  f = fft->split.imagp[0];
  fft->split.imagp[0] = 0.0;
  
  ztoc(&fft->split, 1, (DSPComplex *)out, 2, fft->size >> 1);
  
  *ny = f;
}

void
fts_fft_inverse_real(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, float scale, float *ny)
{
  fill_complex(fft, in, size, stride, fft->size >> 1, scale);  
  
  fft->split.imagp[0] = *ny;
  
  fft_zrip(fft->setup, &fft->split, 1, fft->log2_size, FFT_INVERSE);
  ztoc(&fft->split, 1, (DSPComplex *)out, 2, fft->size >> 1);
}

#else

struct fts_fft
{
  unsigned int size;
  unsigned int spec_size;
  int log2_size;
  fft_lookup_t *lookups;
};

unsigned int 
fts_fft_get_size(fts_fft_t *fft)
{
  return fft->size;
}

void
fts_fft_set_size(fts_fft_t *fft, unsigned int size)
{
  int log2_size = log_dualis(size);
  
  fft->size = 1 << log2_size;
  fft->log2_size = log2_size;
  fft->lookups = fft_lookups_get_or_make(log2_size);
}

fts_fft_t *
fts_fft_new(unsigned int size)
{
  fts_fft_t *fft = fts_malloc(sizeof(fts_fft_t));
  
  fts_fft_set_size(fft, size);
    
  return fft;
}

void
fts_fft_delete(fts_fft_t *fft)
{
  fts_free(fft);
}

static void
fill_complex(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, unsigned int spec_size, float scale)
{
  complex *fft_ptr = (complex *)out;
  unsigned int i, j;
  
  if(size > spec_size)
    size = spec_size;
  
  switch(stride)
  {
    case 0:
      /* fill from complex */
      for(i=0; i<2*spec_size; i++)
        out[i] = in[i] * scale;
      break;
      
    case 1:
      /* fill from real */
      for(i=0; i<spec_size; i++)
      {
        fft_ptr[i].re = in[i] * scale;
        fft_ptr[i].im = 0.0;
      }      
      break;
      
    default:
      /* fill complex from real with stride > 1  */
      for(i=0, j=0; i<spec_size; i++, j+=stride)
      {
        fft_ptr[i].re = in[j] * scale;
        fft_ptr[i].im = in[j + 1] * scale;
      }
      break;
  }
}

static void
fill_real(fts_fft_t *fft, float *in, unsigned int size, int stride, float *out, float scale)
{
  unsigned int out_size = fft->size;
  unsigned int i, j;
  
  if(size > out_size)
    size = out_size;
  
  if(stride < 2)
  {
    for(i=0; i<size; i++)
      out[i] = in[i] * scale;
  }
  else
  {
    for(i=0, j=0; i<size; i++, j+=stride)
      out[i] = in[j] * scale;
  }
  
  /* zero padding */
  for(; i<out_size; i++)
    out[i] = 0.0;    
}

void
fts_fft_complex(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, float scale)
{
  fill_complex(fft, in, size, stride, out, fft->size, scale);
  
  cfft_bitreversal_inplc((complex *)out, fft->lookups->bitrev, fft->size);
  cfft_inplc((complex *)out, fft->lookups->cos, fft->lookups->sin, fft->size);
}

void
fts_fft_inverse_complex(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, float scale)
{
  fill_complex(fft, in, size, stride, out, fft->size, scale);
  
  cfft_bitreversal_inplc((complex *)out, fft->lookups->bitrev, fft->size);
  cifft_inplc((complex *)out, fft->lookups->cos, fft->lookups->sin, fft->size);
}

void
fts_fft_real(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, float scale, float *ny)
{
  unsigned int spec_size = fft->size >> 1;  
  float f;
  
  fill_real(fft, in, size, stride, out, scale);
    
  cfft_bitreversal_over_inplc((complex *)out, fft->lookups->bitrev, spec_size);
  cfft_inplc_over_coef((complex *)out, fft->lookups->cos, fft->lookups->sin, spec_size);
  rfft_shuffle_after_fft_inplc((complex *)out, fft->lookups->cos, fft->lookups->sin, spec_size);
  
  f = out[1];
  out[1] = 0.0;
  *ny = f;
}

void
fts_fft_inverse_real(fts_fft_t *fft, float *in, unsigned int size, unsigned int stride, float *out, float scale, float *ny)
{
  unsigned int spec_size = fft->size >> 1;  

  fill_complex(fft, in, size, stride, out, spec_size, scale);
  out[1] = *ny;
  
  rfft_shuffle_before_ifft_inplc((complex *)out, fft->lookups->cos, fft->lookups->sin, spec_size);
  cfft_bitreversal_over_inplc((complex *)out, fft->lookups->bitrev, spec_size);
  cifft_inplc_over_coef((complex *)out, fft->lookups->cos, fft->lookups->sin, spec_size);
}

#endif

