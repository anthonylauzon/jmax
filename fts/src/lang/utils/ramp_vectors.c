/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

/*****************************************
 *
 *  ramp vector elements
 *
 */

#define fts_float_vector_ramp_element_zero(ramp_vector, i) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->value[i] = (ramp_vector)->target[i] = (float)0.0 \
)

#define fts_float_vector_ramp_element_set(ramp_vector, i, x) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->value[i] = (ramp_vector)->target[i] = (x) \
)

#define fts_float_vector_ramp_element_set_hold(ramp_vector, i, x) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->target[i] = (x) \
)

#define fts_float_vector_ramp_element_get(ramp_vector, i) \
  ((ramp_vector)->value[i])

#define fts_float_vector_ramp_element_get_target(ramp_vector, i) \
  ((ramp_vector)->target[i])

#define fts_float_vector_ramp_element_add(ramp_vector, i, x) \
  ((ramp_vector)->value[i] = (ramp_vector)->target[i] += (x))

#define fts_float_vector_ramp_element_set_target(ramp_vector, i, x, n_steps) \
( \
  (ramp_vector)->incr[i] = ((x) - (ramp_vector)->value[i]) / (n_steps), \
  (ramp_vector)->target[i] = (x) \
)

#define fts_float_vector_ramp_element_set_interval(ramp_vector, i, x, n_steps) \
( \
  (ramp_vector)->incr[i] = (x) / (n_steps), \
  (ramp_vector)->target[i] = (ramp_vector)->value[i] + (x) \
)

#define fts_float_vector_ramp_element_set_incr(ramp_vector, i, x, n_steps) \
( \
  (ramp_vector)->incr[i] = (x), \
  (ramp_vector)->target[i] = (ramp_vector)->value[i] + n_steps * (x) \
)

#define fts_float_vector_ramp_element_jump(ramp_vector, i) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->value[i] = (ramp_vector)->target[i] \
)

#define fts_float_vector_ramp_element_freeze(ramp_vector, i) \
( \
  (ramp_vector)->incr[i] = (float)0.0, \
  (ramp_vector)->target[i] = (ramp_vector)->value[i] \
)

#define fts_float_vector_ramp_element_incr(ramp_vector, i) \
  ((ramp_vector)->value[i] += (ramp_vector)->incr[i])

/*****************************************
 *
 *  ramp vectors
 *
 */

void
fts_float_vector_ramp_zero(fts_float_vector_ramp_t *ramp_vector)
{
  int i;

  for(i=0; i<ramp_vector->size; i++)
    fts_float_vector_ramp_element_zero(ramp_vector, i);
  
  ramp_vector->n_steps = 0;
}

void
fts_float_vector_ramp_zero_tail(fts_float_vector_ramp_t *ramp_vector, int index)
{
  int i;

  for(i=index; i<ramp_vector->size; i++)
    fts_float_vector_ramp_element_zero(ramp_vector, i);
}

fts_float_vector_ramp_t *
fts_float_vector_ramp_new(int size)
{
  int i;
  fts_float_vector_ramp_t *ramp_vector;

  ramp_vector = (fts_float_vector_ramp_t *)fts_malloc(sizeof(fts_float_vector_ramp_t));

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

  fts_float_vector_ramp_zero(ramp_vector);

  return ramp_vector;
}

void
fts_float_vector_ramp_set_size(fts_float_vector_ramp_t *ramp_vector, int size)
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
      fts_float_vector_ramp_element_zero(ramp_vector, i);
}

void
fts_float_vector_ramp_delete(fts_float_vector_ramp_t *ramp_vector)
{
  int i;

  fts_free(ramp_vector->value);
  fts_free(ramp_vector->target);
  fts_free(ramp_vector->incr);
  fts_free(ramp_vector);
}

void 
fts_float_vector_ramp_set_targets(fts_float_vector_ramp_t *ramp_vector, float *targets, int size, float time, float rate)
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
	fts_float_vector_ramp_element_set_target(ramp_vector, i, targets[i], n_steps);
    }
  else
    {
      for(i=0; i<size; i++)
	fts_float_vector_ramp_element_set(ramp_vector, i, targets[i]);

      ramp_vector->n_steps = 0;
    }
}

void 
fts_float_vector_ramp_set_targets_hold_and_jump(fts_float_vector_ramp_t *ramp_vector, float *targets, int size, float time, float rate)
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
	      if(fts_float_vector_ramp_element_get_target(ramp_vector, i) == 0.0)
		/* transition time > 0, but last target was zero: jump to new target */
		fts_float_vector_ramp_element_set(ramp_vector, i, target);
	      else
		/* normal transition */
		fts_float_vector_ramp_element_set_target(ramp_vector, i, target, n_steps);
	    }
	  else
	    /* transition time > 0, but target is 0: hold current value (jumps to 0 when n_steps gets 0) */
	    fts_float_vector_ramp_element_set_hold(ramp_vector, i, 0.0);
	}
    }
  else
    {
      for(i=0; i<size; i++)
	fts_float_vector_ramp_element_set(ramp_vector, i, targets[i]);

      ramp_vector->n_steps = 0;
    }
}

void 
fts_float_vector_ramp_set_intervals(fts_float_vector_ramp_t *ramp_vector, float *intervals, int size, float time, float rate)
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
	fts_float_vector_ramp_element_set_interval(ramp_vector, i, intervals[i], n_steps);
    }
  else
    {
      for(i=0; i<size; i++)
	fts_float_vector_ramp_element_set(ramp_vector, i, fts_float_vector_ramp_element_get(ramp_vector, i) + intervals[i]);

      ramp_vector->n_steps = 0;
    }
}

void 
fts_float_vector_ramp_set_slopes(fts_float_vector_ramp_t *ramp_vector, float *slopes, int size, float time, float rate)
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
	fts_float_vector_ramp_element_set_interval(ramp_vector, i, time * slopes[i], n_steps);
    }
  else
    ramp_vector->n_steps = 0;
}

void 
fts_float_vector_ramp_jump(fts_float_vector_ramp_t *ramp_vector)
{
  int i;
  
  for(i=0; i<ramp_vector->size; i++)
    fts_float_vector_ramp_element_jump(ramp_vector, i);
  
  ramp_vector->n_steps = 0;
}

void 
fts_float_vector_ramp_freeze(fts_float_vector_ramp_t *ramp_vector)
{
  int i;
  
  for(i=0; i<ramp_vector->size; i++)
    fts_float_vector_ramp_element_freeze(ramp_vector, i);
  
  ramp_vector->n_steps = 0;
}

void
fts_float_vector_ramp_incr(fts_float_vector_ramp_t *ramp_vector)
{
  int i;

  if(ramp_vector->n_steps > 1)
    {
      for(i=0; i<ramp_vector->size; i++)
	fts_float_vector_ramp_element_incr(ramp_vector, i);

      ramp_vector->n_steps--;
    }
  else if(ramp_vector->n_steps > 0)
    {
      for(i=0; i<ramp_vector->size; i++)
	fts_float_vector_ramp_element_jump(ramp_vector, i);

      ramp_vector->n_steps = 0;
    }
}
