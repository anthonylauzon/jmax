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


#include "utils.h"
#include "rfft_shuffle.h"
#include "cfft.h"

/**************************************************************************************
 *
 *    stucture and root pointer for fft lookups
 */
 
typedef struct{
  float *cos;
  float *sin;
  int *bitrev;
} fft_lookup_t;
 
static fft_lookup_t the_fft_lookups[FTS_FFT_MAX_SIZE_LOG2] =
{
  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
};

/* ultility */
static int
ilog2(int size)
{
  int log_size, i;

  for(log_size=-1, i=size; i; i>>=1, log_size++)
    ;

  return log_size;
}

/**************************************************************************************
 *
 *    fts_is_fft_size()
 */
 
int fts_is_fft_size(int n)
{
  if(n < FTS_FFT_MIN_SIZE || n > FTS_FFT_MAX_SIZE) return(0);

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
 
int 
fts_fft_declaresize(int size)
{
  int log_size = ilog2(size);

  /* power of 2? */
  if(1 << log_size != size)
    return 0;
  
  if(!the_fft_lookups[log_size].cos)
    {
      the_fft_lookups[log_size].cos = fts_fftab_get_cosine(size);
      the_fft_lookups[log_size].sin = fts_fftab_get_sine(size);
      the_fft_lookups[log_size].bitrev = cfft_make_bitreversed_table(size);
    }

  return 1;
}

/**************************************************************************************\
 *
 *    the complex fft and ifft
 */

void
fts_cfft_inplc(complex *buf, int size)
{
  fft_lookup_t *lookups = &(the_fft_lookups[ilog2(size)]);

  cfft_bitreversal_inplc(buf, lookups->bitrev, size);
  cfft_inplc(buf, lookups->cos, lookups->sin, size);
}

void
fts_cfft(complex *in, complex *out, int size)
{
  fft_lookup_t *lookups = &(the_fft_lookups[ilog2(size)]);

  if(in == out)
    cfft_bitreversal_inplc(out, lookups->bitrev, size);
  else
    cfft_bitreversal_outplc(in, out, lookups->bitrev, size);
  cfft_inplc(out, lookups->cos, lookups->sin, size);
}

void
fts_cifft_inplc(complex *buf, int size)
{
  fft_lookup_t *lookups = &(the_fft_lookups[ilog2(size)]);

  cfft_bitreversal_inplc(buf, lookups->bitrev, size);
  cifft_inplc(buf, lookups->cos, lookups->sin, size);
}

void
fts_cifft(complex *in, complex *out, int size)
{
  fft_lookup_t *lookups = &(the_fft_lookups[ilog2(size)]);

  if(in == out)
    cfft_bitreversal_inplc(out, lookups->bitrev, size);
  else
    cfft_bitreversal_outplc(in, out, lookups->bitrev, size);
  cifft_inplc(out, lookups->cos, lookups->sin, size);
}

/**************************************************************************************\
 *
 *    the "real" fft
 */

void
fts_rfft_inplc(float *buf, int size)
{
  fft_lookup_t *lookups = &(the_fft_lookups[ilog2(size)]);
  int complex_size = size >> 1;

  cfft_bitreversal_over_inplc((complex *)buf, lookups->bitrev, 2, complex_size);
  cfft_inplc_over_coef((complex *)buf, lookups->cos, lookups->sin, 2, complex_size);
  rfft_shuffle_after_fft_inplc((complex *)buf, lookups->cos, lookups->sin, complex_size);
}

void
fts_rfft(float *in, complex *out, int size)
{
  fft_lookup_t *lookups = &(the_fft_lookups[ilog2(size)]);
  int complex_size = size >> 1;
    
  if(in == (float *)out)
    cfft_bitreversal_over_inplc(out, lookups->bitrev, 2, complex_size);
  else
    cfft_bitreversal_over_outplc((complex *)in, out, lookups->bitrev, 2, complex_size);
  cfft_inplc_over_coef(out, lookups->cos, lookups->sin, 2, complex_size);
  rfft_shuffle_after_fft_inplc(out, lookups->cos, lookups->sin, complex_size);
}

void
fts_rifft_inplc(float *buf, int size)
{
  fft_lookup_t *lookups = &(the_fft_lookups[ilog2(size)]);
  int complex_size = size >> 1;
    
  rfft_shuffle_before_ifft_inplc((complex *)buf, lookups->cos, lookups->sin, complex_size);
  cfft_bitreversal_over_inplc((complex *)buf, lookups->bitrev, 2, complex_size);
  cifft_inplc_over_coef((complex *)buf, lookups->cos, lookups->sin, 2, complex_size);
}

void
fts_rifft(complex *in, float *out, int size)
{
  fft_lookup_t *lookups = &(the_fft_lookups[ilog2(size)]);
  int complex_size = size >> 1;
    
  if(in == (complex *)out)
    rfft_shuffle_before_ifft_inplc((complex *)out, lookups->cos, lookups->sin, complex_size);
  else
    rfft_shuffle_before_ifft_outplc(in, (complex *)out, lookups->cos, lookups->sin, complex_size);
  cfft_bitreversal_over_inplc((complex *)out, lookups->bitrev, 2, complex_size);
  cifft_inplc_over_coef((complex *)out, lookups->cos, lookups->sin, 2, complex_size);
}
