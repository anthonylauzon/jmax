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

#include <utils/c/include/utils.h>

/*****************************************
 *
 *  ramp vector elements
 *
 */

#define fts_framps_element_zero(ramp_vector, i) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->value[i] = (ramp_vector)->target[i] = (float)0.0 \
)

#define fts_framps_element_set(ramp_vector, i, x) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->value[i] = (ramp_vector)->target[i] = (x) \
)

#define fts_framps_element_set_hold(ramp_vector, i, x) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->target[i] = (x) \
)

#define fts_framps_element_get(ramp_vector, i) \
  ((ramp_vector)->value[i])

#define fts_framps_element_get_target(ramp_vector, i) \
  ((ramp_vector)->target[i])

#define fts_framps_element_add(ramp_vector, i, x) \
  ((ramp_vector)->value[i] = (ramp_vector)->target[i] += (x))

#define fts_framps_element_set_target(ramp_vector, i, x, n_steps) \
( \
  (ramp_vector)->incr[i] = ((x) - (ramp_vector)->value[i]) / (n_steps), \
  (ramp_vector)->target[i] = (x) \
)

#define fts_framps_element_set_interval(ramp_vector, i, x, n_steps) \
( \
  (ramp_vector)->incr[i] = (x) / (n_steps), \
  (ramp_vector)->target[i] = (ramp_vector)->value[i] + (x) \
)

#define fts_framps_element_set_incr(ramp_vector, i, x, n_steps) \
( \
  (ramp_vector)->incr[i] = (x), \
  (ramp_vector)->target[i] = (ramp_vector)->value[i] + n_steps * (x) \
)

#define fts_framps_element_jump(ramp_vector, i) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->value[i] = (ramp_vector)->target[i] \
)

#define fts_framps_element_freeze(ramp_vector, i) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->target[i] = (ramp_vector)->value[i] \
)

#define fts_framps_element_incr(ramp_vector, i) \
  ((ramp_vector)->value[i] += (ramp_vector)->incr[i])

/*****************************************
 *
 *  ramp vectors
 *
 */

void
fts_framps_zero(fts_framps_t *ramp_vector)
{
  int i;

  for(i=0; i<ramp_vector->size; i++)
    fts_framps_element_zero(ramp_vector, i);
  
  ramp_vector->n_steps = 0;
}

void
fts_framps_zero_tail(fts_framps_t *ramp_vector, int index)
{
  int i;

  for(i=index; i<ramp_vector->size; i++)
    fts_framps_element_zero(ramp_vector, i);
}

fts_framps_t *
fts_framps_new(int size)
{
  fts_framps_t *ramp_vector;

  ramp_vector = (fts_framps_t *)fts_malloc(sizeof(fts_framps_t));

  if(size > 0)
    {
      ramp_vector->value = (float *)fts_malloc(sizeof(float) * size);
      ramp_vector->target = (float *)fts_malloc(sizeof(float) * size);
      ramp_vector->incr = (float *)fts_malloc(sizeof(float) * size);
      ramp_vector->size = size;
      ramp_vector->alloc = size;
    }
  else
    {
      ramp_vector->value = 0;
      ramp_vector->target = 0;
      ramp_vector->incr = 0;
      ramp_vector->size = 0;
      ramp_vector->alloc = 0;
    }

  fts_framps_zero(ramp_vector);

  return ramp_vector;
}

void
fts_framps_set_size(fts_framps_t *ramp_vector, int size)
{
  int i = ramp_vector->size;

  if(size > ramp_vector->alloc)
    {
      if(ramp_vector->alloc)
	{
	  ramp_vector->value = (float *)fts_realloc((void *)ramp_vector->value, sizeof(float) * size);
	  ramp_vector->target = (float *)fts_realloc((void *)ramp_vector->target, sizeof(float) * size);
	  ramp_vector->incr = (float *)fts_realloc((void *)ramp_vector->incr, sizeof(float) * size);
	}
      else
	{
	  ramp_vector->value = (float *)fts_malloc(sizeof(float) * size);
	  ramp_vector->target = (float *)fts_malloc(sizeof(float) * size);
	  ramp_vector->incr = (float *)fts_malloc(sizeof(float) * size);
	}

      ramp_vector->alloc = size;
    }

    ramp_vector->size = size;
    
    /* zero (if) new tail */
    for(; i<size; i++)
      fts_framps_element_zero(ramp_vector, i);
}

void
fts_framps_delete(fts_framps_t *ramp_vector)
{
  fts_free(ramp_vector->value);
  fts_free(ramp_vector->target);
  fts_free(ramp_vector->incr);
  fts_free(ramp_vector);
}

void 
fts_framps_set_targets(fts_framps_t *ramp_vector, float *targets, int size, float time, float rate)
{
  int i;

  if(size > ramp_vector->size)
    size = ramp_vector->size;

  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);
      
      if(n_steps < 1) n_steps = 1;
      ramp_vector->n_steps = n_steps;
      
      /* set interpolation steps */
      for(i=0; i<size; i++)
	fts_framps_element_set_target(ramp_vector, i, targets[i], n_steps);
    }
  else
    {
      for(i=0; i<size; i++)
	fts_framps_element_set(ramp_vector, i, targets[i]);

      ramp_vector->n_steps = 0;
    }
}

void 
fts_framps_set_targets_hold_and_jump(fts_framps_t *ramp_vector, float *targets, int size, float time, float rate)
{
  int i;

  if(size > ramp_vector->size)
    size = ramp_vector->size;

  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);
      
      if(n_steps < 1) n_steps = 1;
      ramp_vector->n_steps = n_steps;
      
      /* set interpolation steps */
      for(i=0; i<size; i++)
	{
	  float target = targets[i];

	  if(target != 0.0)
	    {
	      if(fts_framps_element_get_target(ramp_vector, i) == 0.0)
		/* transition time > 0, but last target was zero: jump to new target */
		fts_framps_element_set(ramp_vector, i, target);
	      else
		/* normal transition */
		fts_framps_element_set_target(ramp_vector, i, target, n_steps);
	    }
	  else
	    /* transition time > 0, but target is 0: hold current value (jumps to 0 when n_steps gets 0) */
	    fts_framps_element_set_hold(ramp_vector, i, 0.0);
	}
    }
  else
    {
      for(i=0; i<size; i++)
	fts_framps_element_set(ramp_vector, i, targets[i]);

      ramp_vector->n_steps = 0;
    }
}

void 
fts_framps_set_intervals(fts_framps_t *ramp_vector, float *intervals, int size, float time, float rate)
{
  int i;

  if(size > ramp_vector->size)
    size = ramp_vector->size;
  
  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);
      
      if(n_steps < 1) n_steps = 1;
      ramp_vector->n_steps = n_steps;
      
      /* set interpolation steps */
      for(i=0; i<size; i++)
	fts_framps_element_set_interval(ramp_vector, i, intervals[i], n_steps);
    }
  else
    {
      for(i=0; i<size; i++)
	fts_framps_element_set(ramp_vector, i, fts_framps_element_get(ramp_vector, i) + intervals[i]);

      ramp_vector->n_steps = 0;
    }
}

void 
fts_framps_set_slopes(fts_framps_t *ramp_vector, float *slopes, int size, float time, float rate)
{
  int i;

  if(size > ramp_vector->size)
    size = ramp_vector->size;
  
  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);
      
      if(n_steps < 1) n_steps = 1;
      ramp_vector->n_steps = n_steps;
      
      /* set interpolation steps */
      for(i=0; i<size; i++)
	fts_framps_element_set_interval(ramp_vector, i, time * slopes[i], n_steps);
    }
  else
    ramp_vector->n_steps = 0;
}

void 
fts_framps_jump(fts_framps_t *ramp_vector)
{
  int i;
  
  for(i=0; i<ramp_vector->size; i++)
    fts_framps_element_jump(ramp_vector, i);
  
  ramp_vector->n_steps = 0;
}

void 
fts_framps_freeze(fts_framps_t *ramp_vector)
{
  int i;
  
  for(i=0; i<ramp_vector->size; i++)
    fts_framps_element_freeze(ramp_vector, i);
  
  ramp_vector->n_steps = 0;
}

void
fts_framps_incr(fts_framps_t *ramp_vector)
{
  int i;

  if(ramp_vector->n_steps > 1)
    {
      for(i=0; i<ramp_vector->size; i++)
	fts_framps_element_incr(ramp_vector, i);

      ramp_vector->n_steps--;
    }
  else if(ramp_vector->n_steps > 0)
    {
      for(i=0; i<ramp_vector->size; i++)
	fts_framps_element_jump(ramp_vector, i);

      ramp_vector->n_steps = 0;
    }
}
