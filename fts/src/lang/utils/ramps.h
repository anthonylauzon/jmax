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

#ifndef _FTS_RAMPS_H_
#define _FTS_RAMPS_H_

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
  (ramp_value)->incr = (double)0.0, \
  (ramp_value)->current = (ramp_value)->target = (double)0.0 \
)

#define fts_ramp_value_set(ramp_value, x) \
( \
  (ramp_value)->incr = (double)0.0, \
  (ramp_value)->current = (ramp_value)->target = (x) \
)

#define fts_ramp_value_set_hold(ramp_value, x) \
( \
  (ramp_value)->incr = (double)0.0, \
  (ramp_value)->target = (x) \
)

#define fts_ramp_value_get(ramp_value) \
  ((ramp_value)->current)

#define fts_ramp_value_get_target(ramp_value) \
  ((ramp_value)->target)

#define fts_ramp_value_add(ramp_value, x) \
  ((ramp_value)->current = (ramp_value)->target += (x))

#define fts_ramp_value_set_target(ramp_value, x, n_steps) \
( \
  (ramp_value)->incr = ((x) - (ramp_value)->current) / (n_steps), \
  (ramp_value)->target = (x) \
)

#define fts_ramp_value_set_interval(ramp_value, x, n_steps) \
( \
  (ramp_value)->incr = (x) / (n_steps), \
  (ramp_value)->target = (ramp_value)->current + (x) \
)

#define fts_ramp_value_set_incr(ramp_value, x, n_steps) \
( \
  (ramp_value)->incr = (x), \
  (ramp_value)->target = (ramp_value)->current + n_steps * (x) \
)

#define fts_ramp_value_jump(ramp_value) \
( \
  (ramp_value)->incr = (double)0.0, \
  (ramp_value)->current = (ramp_value)->target \
)

#define fts_ramp_value_freeze(ramp_value) \
( \
  (ramp_value)->incr = (double)0.0, \
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
extern void fts_ramp_set_target_hold_and_jump(fts_ramp_t *ramp, float target, float time, float rate);
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
/* void fts_ramp_vec_mul_add(fts_ramp_t * restrict ramp, float *in, float *out, long size); */

#define fts_ramp_vec_mul_add(r, x, y, n) \
  if((r)->n_steps <= 0) \
    { \
      float target = (r)->value.target; \
      int i; \
 \
      for(i=0; i<(n); i++) \
	(y)[i] += (x)[i] * target; \
    } \
  else \
    { \
      float incr = (r)->value.incr / (n); \
      float base = (r)->value.current; \
      int i; \
 \
      for(i=0; i<(n); i++) \
	(y)[i] += (x)[i] * (base + i * incr); \
 \
      fts_ramp_value_incr(&(r)->value); \
      (r)->n_steps--; \
    }

#endif



