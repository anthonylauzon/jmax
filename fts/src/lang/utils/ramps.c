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
 *  ramps
 *
 */

void 
fts_ramp_init(fts_ramp_t *ramp, float value)
{
  fts_ramp_value_set(&ramp->value, value);
  ramp->n_steps = 0;
}

void 
fts_ramp_zero(fts_ramp_t *ramp)
{
  fts_ramp_value_zero(&ramp->value);
  ramp->n_steps = 0;
}

void 
fts_ramp_set_target(fts_ramp_t *ramp, float target, float time, float rate)
{
  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);

      if(n_steps < 1) n_steps = 1;
      ramp->n_steps = n_steps;

      fts_ramp_value_set_target(&ramp->value, target, n_steps);
    }
  else
    {
      ramp->n_steps = 0;
      fts_ramp_value_set(&ramp->value, target);
    }
}

void 
fts_ramp_set_interval(fts_ramp_t *ramp, float interval, float time, float rate)
{
  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);

      if(n_steps < 1) n_steps = 1;
      ramp->n_steps = n_steps;

      fts_ramp_value_set_interval(&ramp->value, interval, n_steps);
    }
  else
    {
      ramp->n_steps = 0;
      fts_ramp_value_add(&ramp->value, interval);
    }
}

void 
fts_ramp_set_slope(fts_ramp_t *ramp, float slope, float time, float rate)
{
  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);

      if(n_steps < 1) 
	n_steps = 1;
      ramp->n_steps = n_steps;

      if(slope < 0.0f)
	slope *= -1.0f;

      fts_ramp_value_set_interval(&ramp->value, time * slope, n_steps);
    }
  else
    {
      ramp->n_steps = 0;
      fts_ramp_value_freeze(&ramp->value);
    }
}

void 
fts_ramp_set_incr_clip(fts_ramp_t *ramp, double incr, float clip)
{
  int n_steps = (clip - fts_ramp_value_get(&ramp->value)) / incr;

  if(n_steps < 0)
    n_steps *= -1;
  
  fts_ramp_value_set_incr(&ramp->value, incr, n_steps);
  ramp->n_steps = n_steps;
}

void 
fts_ramp_jump(fts_ramp_t *ramp)
{
  fts_ramp_value_jump(&ramp->value);
  ramp->n_steps = 0;
}

void 
fts_ramp_freeze(fts_ramp_t *ramp)
{
  fts_ramp_value_freeze(&ramp->value);
  ramp->n_steps = 0;
}

void
fts_ramp_incr(fts_ramp_t *ramp)
{
  if(ramp->n_steps > 1)
    {
      fts_ramp_value_incr(&ramp->value);
      ramp->n_steps--;
    }
  else if(ramp->n_steps > 0)
    {
      fts_ramp_value_jump(&ramp->value);
      ramp->n_steps = 0;
    }
}

void
fts_ramp_vec_fill(fts_ramp_t * restrict ramp, float *out, long size)
{
  long i;
  
  if(ramp->n_steps <= 0)
    {
      float target = ramp->value.target;

      for(i=0; i<size; i++)
	out[i] = target;
    }
  else
    {
      float incr = ramp->value.incr / size;
      float base = ramp->value.current;

      for(i=0; i<size; i++)
	out[i] = base + i * incr;

      fts_ramp_value_incr(&ramp->value);
      ramp->n_steps--;
    }
}

void
fts_ramp_vec_mul(fts_ramp_t * restrict ramp, float *in, float *out, long size)
{
  long i;
  
  if(ramp->n_steps <= 0)
    {
      float target = ramp->value.target;

      for(i=0; i<size; i++)
	out[i] = in[i] * target;
    }

  else
    {
      float incr = ramp->value.incr / size;
      float base = ramp->value.current;

      for(i=0; i<size; i++)
	out[i] = in[i] * (base + i * incr);

      fts_ramp_value_incr(&ramp->value);
      ramp->n_steps--;
    }
}

void
fts_ramp_vec_mul_add(fts_ramp_t * restrict ramp, float *in, float *out, long size)
{
  long i;
  
  if(ramp->n_steps <= 0)
    {
      float target = ramp->value.target;

      for(i=0; i<size; i++)
	out[i] += in[i] * target;
    }

  else
    {
      float incr = ramp->value.incr / size;
      float base = ramp->value.current;

      for(i=0; i<size; i++)
	out[i] += in[i] * (base + i * incr);

      fts_ramp_value_incr(&ramp->value);
      ramp->n_steps--;
    }
}

/*****************************************
 *
 *  ramp arrays
 *
 */

void
fts_ramp_array_zero(fts_ramp_array_t *ramp_array)
{
  int i;

  for(i=0; i<ramp_array->size; i++)
    fts_ramp_value_zero(ramp_array->values + i);
  
  ramp_array->n_steps = 0;
}

fts_ramp_array_t *
fts_ramp_array_new(int size)
{
  int i;
  fts_ramp_array_t *ramp_array;

  ramp_array = (fts_ramp_array_t *)fts_malloc(sizeof(fts_ramp_array_t));

  if(size > 0)
    {
      ramp_array->values = (fts_ramp_value_t *)fts_malloc(sizeof(fts_ramp_value_t) * size);
      ramp_array->size = size;
      ramp_array->alloc = size;
    }
  else
    {
      ramp_array->values = 0;
      ramp_array->size = 0;
      ramp_array->alloc = 0;
    }

  fts_ramp_array_zero(ramp_array);

  return ramp_array;
}

void
fts_ramp_array_set_size(fts_ramp_array_t *ramp_array, int size)
{
  if(size > ramp_array->alloc)
    {
      if(ramp_array->alloc)
	ramp_array->values = (fts_ramp_value_t *)fts_realloc((void *)ramp_array->values, sizeof(fts_ramp_value_t) * size);
      else
	ramp_array->values = (fts_ramp_value_t *)fts_malloc(sizeof(fts_ramp_value_t) * size);

      ramp_array->alloc = size;
    }

    ramp_array->size = size;
}

void
fts_ramp_array_delete(fts_ramp_array_t *ramp_array)
{
  int i;

  fts_free(ramp_array->values);
  fts_free(ramp_array);
}

void 
fts_ramp_array_set_targets(fts_ramp_array_t *ramp_array, float *targets, int size, float time, float rate)
{
  int inter_size;
  int i = 0;

  if(ramp_array->size >= size)
    inter_size = size;
  else
    inter_size = ramp_array->size;
  
  fts_ramp_array_set_size(ramp_array, size);

  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);
      
      if(n_steps < 1) n_steps = 1;
      ramp_array->n_steps = n_steps;
      
      /* set interpolation steps */
      for(; i<inter_size; i++)
	fts_ramp_value_set_target(&ramp_array->values[i], targets[i], n_steps);
    }
  else
    ramp_array->n_steps = 0;
  
  for(; i<size; i++)
    fts_ramp_value_set(&ramp_array->values[i], targets[i]);
}

void 
fts_ramp_array_set_intervals(fts_ramp_array_t *ramp_array, float *intervals, int size, float time, float rate)
{
  int inter_size;
  int i = 0;

  if(ramp_array->size >= size)
    inter_size = size;
  else
    inter_size = ramp_array->size;
  
  fts_ramp_array_set_size(ramp_array, size);

  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);
      
      if(n_steps < 1) n_steps = 1;
      ramp_array->n_steps = n_steps;
      
      /* set interpolation steps */
      for(; i<inter_size; i++)
	fts_ramp_value_set_interval(&ramp_array->values[i], intervals[i], n_steps);
    }
  else
    ramp_array->n_steps = 0;
  
  for(; i<size; i++)
    fts_ramp_value_add(&ramp_array->values[i], intervals[i]);
}

void 
fts_ramp_array_set_slopes(fts_ramp_array_t *ramp_array, float *slopes, int size, float time, float rate)
{
  int inter_size;
  int i = 0;

  if(ramp_array->size >= size)
    inter_size = size;
  else
    inter_size = ramp_array->size;
  
  fts_ramp_array_set_size(ramp_array, size);

  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);
      
      if(n_steps < 1) n_steps = 1;
      ramp_array->n_steps = n_steps;
      
      /* set interpolation steps */
      for(; i<inter_size; i++)
	fts_ramp_value_set_interval(&ramp_array->values[i], time * slopes[i], n_steps);
    }
  else
    ramp_array->n_steps = 0;
  
  for(; i<size; i++)
    fts_ramp_value_freeze(&ramp_array->values[i]);
}

void 
fts_ramp_array_jump(fts_ramp_array_t *ramp_array)
{
  int i;
  
  for(i=0; i<ramp_array->size; i++)
    fts_ramp_value_jump(ramp_array->values + i);
  
  ramp_array->n_steps = 0;
}

void 
fts_ramp_array_freeze(fts_ramp_array_t *ramp_array)
{
  int i;
  
  for(i=0; i<ramp_array->size; i++)
    fts_ramp_value_freeze(ramp_array->values + i);
  
  ramp_array->n_steps = 0;
}

void
fts_ramp_array_incr(fts_ramp_array_t *ramp_array)
{
  int i;

  if(ramp_array->n_steps > 1)
    {
      for(i=0; i<ramp_array->size; i++)
	fts_ramp_value_incr(ramp_array->values + i);

      ramp_array->n_steps--;
    }
  else if(ramp_array->n_steps > 0)
    {
      for(i=0; i<ramp_array->size; i++)
	fts_ramp_value_jump(ramp_array->values + i);

      ramp_array->n_steps = 0;
    }
}
