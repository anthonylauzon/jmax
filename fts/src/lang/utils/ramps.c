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
fts_ramp_set_target_hold_and_jump(fts_ramp_t *ramp, float target, float time, float rate)
{
  if (time > 0.0)
    {
      int n_steps = (int)(0.001f * time * rate + 0.5f);
      
      if(n_steps < 1) n_steps = 1;
      ramp->n_steps = n_steps;
      
      if(target != 0.0)
	{
	  if(fts_ramp_value_get_target(&ramp->value) == 0.0)
	    /* transition time > 0, but last target was zero: jump to new target */
	    fts_ramp_value_set(&ramp->value, target);
	  else
	    /* normal transition */
	    fts_ramp_value_set_target(&ramp->value, target, n_steps);
	}
      else
	{
	  /* transition time > 0, but target is 0: hold current value (jumps to 0 when n_steps gets 0) */
	  fts_ramp_value_set_hold(&ramp->value, 0.0);
	}
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


