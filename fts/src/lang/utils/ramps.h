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
#ifndef _RAMPS_H_
#define _RAMPS_H_

/*****************************************
 *
 *  ramp values
 *
 */

typedef struct _fts_ramp_value
{
  double current;
  double target;
  double incr;
} fts_ramp_value_t;

#define fts_ramp_value_zero(ramp_value) \
( \
  (ramp_value)->current = (ramp_value)->target = 0.0, \
  (ramp_value)->incr = 0.0 \
)


#define fts_ramp_value_set(ramp_value, x) \
  ((ramp_value)->current = (ramp_value)->target = (x))

#define fts_ramp_value_get(ramp_value) \
  ((ramp_value)->current)


#define fts_ramp_value_add(ramp_value, x) \
  ((ramp_value)->current = (ramp_value)->target += (x))


#define fts_ramp_value_set_target(ramp_value, x, n_steps) \
( \
  (ramp_value)->target = (x), \
  (ramp_value)->incr = ((x) - (ramp_value)->current) / (n_steps) \
)

#define fts_ramp_value_set_interval(ramp_value, x, n_steps) \
( \
  (ramp_value)->target = (ramp_value)->current + (x), \
  (ramp_value)->incr = (x) / (n_steps) \
)

#define fts_ramp_value_set_incr(ramp_value, x, n_steps) \
( \
  (ramp_value)->incr = (x), \
  (ramp_value)->target = (ramp_value)->current + n_steps * (x) \
)


#define fts_ramp_value_jump(ramp_value) \
( \
  (ramp_value)->incr = 0.0, \
  (ramp_value)->current = (ramp_value)->target \
)

#define fts_ramp_value_freeze(ramp_value) \
( \
  (ramp_value)->incr = 0.0, \
  (ramp_value)->target = (ramp_value)->current \
)


#define fts_ramp_value_incr(ramp_value) \
  ((ramp_value)->current += (ramp_value)->incr)


/*****************************************
 *
 *  ramps
 *
 */

typedef struct _fts_ramp
{
  fts_ramp_value_t value;
  int n_steps;
} fts_ramp_t;

extern void fts_ramp_init(fts_ramp_t *ramp, float value);
extern void fts_ramp_zero(fts_ramp_t *ramp);

extern void fts_ramp_set_target(fts_ramp_t *ramp, float target, float time, float rate);
extern void fts_ramp_set_interval(fts_ramp_t *ramp, float interval, float time, float rate);
extern void fts_ramp_set_slope(fts_ramp_t *ramp, float slope, float time, float rate);
extern void fts_ramp_set_incr_clip(fts_ramp_t *ramp, double incr, float clip);

extern void fts_ramp_jump(fts_ramp_t *ramp);
extern void fts_ramp_freeze(fts_ramp_t *ramp);

extern void fts_ramp_incr(fts_ramp_t *ramp);

#define fts_ramp_end(ramp) ((ramp)->n_steps <= 0)

#define fts_ramp_get_value(ramp) ((ramp)->value.current)

void fts_ramp_vec_fill(fts_ramp_t * restrict ramp, float *out, long size);
void fts_ramp_vec_mul(fts_ramp_t * restrict ramp, float *in, float *out, long size);
void fts_ramp_vec_mul_add(fts_ramp_t * restrict ramp, float *in, float *out, long size);

/*****************************************
 *

 *  ramp arrays
 *
 */

typedef struct _fts_ramp_array
{
  fts_ramp_value_t *values;
  int n_steps;
  int size;
  int alloc;
} fts_ramp_array_t;


extern fts_ramp_array_t *fts_ramp_array_new(int size);
extern void fts_ramp_array_delete(fts_ramp_array_t *ramp_array);
extern void fts_ramp_array_zero(fts_ramp_array_t *ramp_array);

extern void fts_ramp_array_set_targets(fts_ramp_array_t *ramp_array, float *targets, int size, float time, float rate);
extern void fts_ramp_array_set_intervals(fts_ramp_array_t *ramp_array, float *intervals, int size, float time, float rate);
extern void fts_ramp_array_set_slopes(fts_ramp_array_t *ramp_array, float *slopes, int size, float time, float rate);

extern void fts_ramp_array_jump(fts_ramp_array_t *ramp_array);
extern void fts_ramp_array_freeze(fts_ramp_array_t *ramp_array);

extern void fts_ramp_array_incr(fts_ramp_array_t *ramp_array);

#define fts_ramp_array_get_size(ramp_array) ((ramp_array)->size)
extern void fts_ramp_array_set_size(fts_ramp_array_t *ramp_array, int size);

#define fts_ramp_array_get_value(ramp_array, i) ((ramp_array)->values[i].current)

#endif
