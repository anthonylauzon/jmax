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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include <math.h>

#include <fts/fts.h>
#include <ftsprivate/srconv.h>

#define FILTER_SIZE 24
#define FILTER_OVER_BITS 11
#define FILTER_OVER (1 << FILTER_OVER_BITS) /* 2048 */

static double * restrict fts_srconv_filter = 0;

/********************************************************
 *
 *  conversion filter
 *
 */

static double *
fts_srconv_make_filter(int size, int over)
{
  double *filter;
  int i;
 
  filter = (double *)fts_malloc(over * size * sizeof(double));

  filter[0] = (double)1.0;

  for(i=1; i<size*over; i++) 
    {
      double x = (double)i / (double)over;
      double sinc = ((x == (double)0.0)? (double)1.0: sin(x * M_PI) / (x * M_PI));
      double window = 0.54 + 0.46 * cos(M_PI * (double)i / (double)(size * over)); /* hamming */
      
      filter[i] = sinc * window;
    }

  return filter;
}

/********************************************************
 *
 *  conversion handle
 *
 */

fts_srconv_t *
fts_srconv_new(double ratio, int in_size, int channels)
{
  fts_srconv_t *srconv;
  double inv_ratio = (double)1.0 / ratio;
  int c;
 
  if(ratio > FTS_SRCONV_MAX_RATIO || inv_ratio > FTS_SRCONV_MAX_RATIO)
    {
      fprintf(stderr, "srconv: can't change rate by more than a factor of %f\n", FTS_SRCONV_MAX_RATIO);
      return 0;
    }
 
  if(channels > FTS_SRCONV_MAX_CHANNELS) 
    {
      fprintf(stderr, "srconv: can't change rate on more than %d channels\n", FTS_SRCONV_MAX_CHANNELS);
      return 0;
    }
 
  srconv = (fts_srconv_t *)fts_malloc(sizeof(fts_srconv_t));

  if(!fts_srconv_filter)
    fts_srconv_filter = fts_srconv_make_filter(FILTER_SIZE, FILTER_OVER);

  for(c=0; c<channels; c++)
    {
      int i;
      
      srconv->buffer[c] = (double *)fts_malloc((in_size + 2 * FILTER_SIZE) * sizeof(double));
      if(!srconv->buffer[c]) 
	{
	  fprintf(stderr, "srconv: allocation error!\n");
	  return 0;
	}
      
      for(i=0; i<2*FILTER_SIZE; i++)
	srconv->buffer[c][i] = 0.0;
    }
  
  srconv->in_size = in_size;
  srconv->in_index = FILTER_SIZE;
  srconv->channels = channels;
  srconv->ratio = ratio;
  srconv->filter_index = 0;

  if(ratio >= (double)1.0)
    {
      /* up sampling */
      srconv->incr = (int)((double)FILTER_OVER * inv_ratio + (double)0.5);
      srconv->step = FILTER_OVER;
      srconv->norm = 1.0;
    }
  else
    {
      /* down sampling */
      srconv->incr = FILTER_OVER;
      srconv->step = (int)((double)FILTER_OVER * ratio + (double)0.5);
      srconv->norm = (double)srconv->step / (double)FILTER_OVER;
    }
    
  return srconv;
}

void
fts_srconv_delete(fts_srconv_t *srconv)
{
  int c;
 
  for(c=0; c<srconv->channels; c++)
    fts_free(srconv->buffer[c]);
}

int 
fts_srconv(fts_srconv_t * restrict srconv, float *in, float *out, int in_size, int out_size, int channels)
{
  int incr = srconv->incr;
  int step = srconv->step;
  double norm = srconv->norm;
  int in_index;
  int filter_index;
  int out_index;
  int c;
 
  if(out_size <= 0)
    return 0;

  for(c=0; c<channels; c++)
    {
      double * restrict buffer = srconv->buffer[c];
      int i;

      in_index = srconv->in_index;
      filter_index = srconv->filter_index;

      /* copy input to convolution buffer (leaving the first 2*FILTER_SIZE points untouched) */
      for (i=0; i<in_size; i++) 
	buffer[2 * FILTER_SIZE + i] = in[c + i * channels];

      /* convolution */
      for(out_index=0; in_index<in_size+FILTER_SIZE && out_index<out_size; out_index++)
	{
	  double sum = buffer[in_index] * fts_srconv_filter[filter_index];

	  for(i=1; i<FILTER_SIZE; i++) 
	    {
	      sum += 
		buffer[in_index - i] * fts_srconv_filter[i * step + filter_index] +
		buffer[in_index + i] * fts_srconv_filter[i * step - filter_index];
	    }
	  
	  out[out_index * channels + c] = (float)(sum * norm);

	  /* increment indices */ 
	  filter_index += incr;
	  in_index += filter_index / step;
	  filter_index = filter_index % step; /* wrap filter index into first step */
	}

      /* copy convolution tail at beginning of buffer */
      bcopy(buffer + in_size, buffer, 2 * FILTER_SIZE * sizeof(double));  
    }
  
  srconv->in_index = in_index - in_size;
  srconv->filter_index = filter_index;
  
  return out_index;
}

