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
 */


#include <utils/c/include/utils.h>
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

static fft_lookup_t *
get_lookups(unsigned int size)
{
  fft_lookup_t *lookups = 0;
  unsigned int log2_size = 0;
  int i;

  for(i=size>>1; i; i>>=1)
    log2_size++;

  lookups = the_fft_lookups + log2_size;

  if(lookups->size == 0 && size >= FTS_FFT_MIN_SIZE)
    {
      int fft_size = 1 << log2_size;

      lookups->size = fft_size;
      lookups->cos = fts_fftab_get_cosine(fft_size);
      lookups->sin = fts_fftab_get_sine(fft_size);
      lookups->bitrev = cfft_make_bitreversed_table(fft_size);
    }

  return lookups;
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
      get_lookups(size);
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
  fft_lookup_t *lookups = get_lookups(size);

  cfft_bitreversal_inplc(buf, lookups->bitrev, lookups->size);
  cfft_inplc(buf, lookups->cos, lookups->sin, lookups->size);
}

void
fts_cfft(complex *in, complex *out, unsigned int size)
{
  fft_lookup_t *lookups = get_lookups(size);

  if(in == out)
    cfft_bitreversal_inplc(out, lookups->bitrev, lookups->size);
  else
    cfft_bitreversal_outplc(in, out, lookups->bitrev, lookups->size);

  cfft_inplc(out, lookups->cos, lookups->sin, lookups->size);
}

void
fts_cifft_inplc(complex *buf, unsigned int size)
{
  fft_lookup_t *lookups = get_lookups(size);

  cfft_bitreversal_inplc(buf, lookups->bitrev, lookups->size);
  cifft_inplc(buf, lookups->cos, lookups->sin, lookups->size);
}

void
fts_cifft(complex *in, complex *out, unsigned int size)
{
  fft_lookup_t *lookups = get_lookups(size);

  if(in == out)
    cfft_bitreversal_inplc(out, lookups->bitrev, lookups->size);
  else
    cfft_bitreversal_outplc(in, out, lookups->bitrev, lookups->size);

  cifft_inplc(out, lookups->cos, lookups->sin, lookups->size);
}

/**************************************************************************************\
 *
 *    the "real" fft
 */

void
fts_rfft_inplc(float *buf, unsigned int size)
{
  fft_lookup_t *lookups = get_lookups(size);
  unsigned int complex_size = lookups->size >> 1;

  cfft_bitreversal_over_inplc((complex *)buf, lookups->bitrev, complex_size);
  cfft_inplc_over_coef((complex *)buf, lookups->cos, lookups->sin, complex_size);
  rfft_shuffle_after_fft_inplc((complex *)buf, lookups->cos, lookups->sin, complex_size);
}

void
fts_rfft(float *in, complex *out, unsigned int size)
{
  fft_lookup_t *lookups = get_lookups(size);
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
  fft_lookup_t *lookups = get_lookups(size);
  unsigned int complex_size = lookups->size >> 1;
    
  rfft_shuffle_before_ifft_inplc((complex *)buf, lookups->cos, lookups->sin, complex_size);
  cfft_bitreversal_over_inplc((complex *)buf, lookups->bitrev, complex_size);
  cifft_inplc_over_coef((complex *)buf, lookups->cos, lookups->sin, complex_size);
}

void
fts_rifft(complex *in, float *out, unsigned int size)
{
  fft_lookup_t *lookups = get_lookups(size);
  unsigned int complex_size = lookups->size >> 1;
    
  if(in == (complex *)out)
    rfft_shuffle_before_ifft_inplc((complex *)out, lookups->cos, lookups->sin, complex_size);
  else
    rfft_shuffle_before_ifft_outplc(in, (complex *)out, lookups->cos, lookups->sin, complex_size);

  cfft_bitreversal_over_inplc((complex *)out, lookups->bitrev, complex_size);
  cifft_inplc_over_coef((complex *)out, lookups->cos, lookups->sin, complex_size);
}
