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
#include <fts/fts.h>
#include "samplepool.h"

#define SAMPLE_WINDOW_BITS 12
#define SAMPLE_WINDOW_SIZE (1 << SAMPLE_WINDOW_BITS)

static float * restrict sample_window = 0;

static void
sample_move(sample_t **from, sample_t **to)
{
  sample_t *sample = *from;
  
  /* cut out sample at from pointer */
  *from = sample->next;
  
  /* insert sample at to pointer */
  sample->next = *to;
  *to = sample;
}

void 
sample_pool_set_size(sample_pool_t *pool, int size)
{
  int i;

  if(size > pool->size)
    {
      if(pool->mem)
	pool->mem = fts_realloc(pool->mem, size * sizeof(sample_t));
      else
	pool->mem = fts_malloc(size * sizeof(sample_t));

      /* add new samples to inactive list */
      for(i=pool->size; i<size; i++)
	{
	  sample_t *sample = pool->mem + i;
	  
	  sample->next = pool->inactive;
	  pool->inactive = sample;
	}

      pool->size = size;
    }
}

sample_pool_t *
sample_pool_new(int size)
{
  sample_pool_t *pool = fts_malloc(sizeof(sample_pool_t));
  int i;
  
  pool->mem = 0;
  pool->size = 0;
  pool->inactive = 0;
  pool->scheduled = 0;
  pool->active = 0;

  sample_pool_set_size(pool, size);

  pool->time = 0;

  return pool;
}

void 
sample_pool_delete(sample_pool_t *pool)
{
  fts_free(pool->mem);
  fts_free(pool);
}

void 
sample_pool_reset(sample_pool_t *pool)
{
  int i;

  pool->time = 0;

  /* add new samples to inactive list */
  for(i=0; i<pool->size; i++)
    {
      sample_t *sample = pool->mem + i;
      
      sample->next = pool->inactive;
      pool->inactive = sample;
    }
}

void 
sample_pool_run(sample_pool_t *pool, float *out, int n_tick)
{
  int time = pool->time;
  int next = time + n_tick;
  sample_t **sample_ptr;

  /* activate delayed samples */
  sample_ptr = &pool->scheduled;
  while(*sample_ptr && (*sample_ptr)->time <= next)
    {
      (*sample_ptr)->time -= time;
      sample_move(sample_ptr, &pool->active);
    }

  /* run active samples */
  sample_ptr = &pool->active;
  while(*sample_ptr && (*sample_ptr)->time <= next)
    {
      sample_t *sample = *sample_ptr;
      float *buf = fvec_get_ptr(sample->fvec);
      fts_idefix_t index = sample->index;
      int delay = sample->time;
      float * restrict output = out + delay;
      int cut = n_tick - delay;
      fts_intphase_t wind_index = sample->wind_index;
      fts_intphase_t wind_incr = sample->wind_incr;
      fts_idefix_t incr = sample->incr;
      float amp = sample->amp;
      int left = sample->left;
      int i;
      
      /* check for end of sample */
      if(left <= cut)
	{
	  /* end of sample */
	  cut = left;
	  
	  /* release fvec */
	  fts_object_release((fts_object_t *)sample->fvec);

	  /* desactivate sample */
	  sample_move(sample_ptr, &pool->inactive);
	}
      else
	{
	  sample_ptr = &sample->next;
	  sample->left = left - cut;
	}
      
      for(i=0; i<cut; i++)
	{
	  float f;
	  int wi = fts_intphase_get_index(wind_index, SAMPLE_WINDOW_BITS);
	  
	  fts_cubic_idefix_interpolate(buf, index, &f);

	  output[i] += f * sample_window[wi] * amp;

	  wind_index += wind_incr;
	  fts_idefix_incr(&index, incr);
	}
      
      sample->index = index;
      sample->wind_index = wind_index;	      
      sample->time = 0;
    }

  pool->time = next;
}

sample_t *
sample_schedule(sample_pool_t *pool, fvec_t *fvec, double delay, double onset, double length, double incr, float amp)
{
  sample_t * restrict sample = pool->inactive;

  if(sample)
    {
      int i_delay, i_length, i_duration;
      double f_delay;
      
      i_delay = (int)delay;
      f_delay = delay - i_delay;
      
      /* for fractional delay just start a little ealier in the source buffer */
      onset -= f_delay * incr;
      
      i_length = (int)length; /* number of samples read from file */
      i_duration = (int)(length / incr); /* number of samples output */
      
      /* sample */
      sample->length = i_length;
      sample->duration = i_duration;
      sample->amp = amp;
      sample->fvec = fvec;
      fts_object_refer((fts_object_t *)fvec);
      
      /* timing */
      sample->time = pool->time + i_delay;
      sample->left = i_duration;
      
      /* phase */
      fts_idefix_set_float(&sample->index, onset);
      fts_idefix_set_float(&sample->incr, incr);
      
      /* window */
      sample->wind_incr = FTS_INTPHASE_RANGE / i_duration;
      sample->wind_index = 0;
      
      /* activate sample */
      sample_move(&pool->inactive, &pool->scheduled);
    }
      
  return sample;
}

void
sample_pool_config(void)
{
  sample_window = fts_fftab_get_hanning(SAMPLE_WINDOW_SIZE);
}
