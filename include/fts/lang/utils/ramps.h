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

#define fts_ramp_value_zero(v) \
( \
  (v)->incr = (double)0.0, \
  (v)->current = (v)->target = (double)0.0 \
)

#define fts_ramp_value_set(v, x) \
( \
  (v)->incr = (double)0.0, \
  (v)->current = (v)->target = (x) \
)

#define fts_ramp_value_set_hold(v, x) \
( \
  (v)->incr = (double)0.0, \
  (v)->target = (x) \
)

#define fts_ramp_value_get(v) \
  ((v)->current)

#define fts_ramp_value_get_target(v) \
  ((v)->target)

#define fts_ramp_value_add(v, x) \
  ((v)->current = (v)->target += (x))

#define fts_ramp_value_set_target(v, x, n_steps) \
( \
  (v)->incr = ((x) - (v)->current) / (n_steps), \
  (v)->target = (x) \
)

#define fts_ramp_value_set_interval(v, x, n_steps) \
( \
  (v)->incr = (x) / (n_steps), \
  (v)->target = (v)->current + (x) \
)

#define fts_ramp_value_set_incr(v, x, n_steps) \
( \
  (v)->incr = (x), \
  (v)->target = (v)->current + n_steps * (x) \
)

#define fts_ramp_value_jump(v) \
( \
  (v)->incr = (double)0.0, \
  (v)->current = (v)->target \
)

#define fts_ramp_value_freeze(v) \
( \
  (v)->incr = (double)0.0, \
  (v)->target = (v)->current \
)

#define fts_ramp_value_incr(v) \
  ((v)->current += (v)->incr)

#define fts_ramp_value_incr_by(v, n) \
  ((v)->current += ((v)->incr * (n)))


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

#define fts_ramp_running(r) ((r)->n_steps > 0)

#define fts_ramp_get_value(r) ((r)->value.current)
#define fts_ramp_get_incr(r) ((r)->value.incr)
#define fts_ramp_get_steps(r) ((r)->n_steps)

/*extern void fts_ramp_jump(fts_ramp_t *ramp);*/
extern void fts_ramp_freeze(fts_ramp_t *ramp);
extern void fts_ramp_incr(fts_ramp_t *ramp);
extern void fts_ramp_incr_by(fts_ramp_t *ramp, int n);

/* vector functions set with control rate (sr / n) */
extern void fts_ramp_vec_fill(fts_ramp_t * restrict ramp, float *out, int size);
extern void fts_ramp_vec_mul(fts_ramp_t * restrict ramp, float *in, float *out, int size);
extern void fts_ramp_vec_mul_add(fts_ramp_t * restrict ramp, float *in, float *out, int size);

/* macro implementations */
#define fts_ramp_jump(r) (fts_ramp_value_jump(&(r)->value), (r)->n_steps = 0)

#define fts_ramp_freeze(r) (fts_ramp_value_freeze(&(r)->value), (r)->n_steps = 0)

#define fts_ramp_incr(r) \
  if((r)->n_steps > 1) \
    { \
      fts_ramp_value_incr(&(r)->value); \
      (r)->n_steps--; \
    } \
  else if((r)->n_steps > 0) \
    { \
      fts_ramp_value_jump(&(r)->value); \
      (r)->n_steps = 0; \
    } \

#define fts_ramp_incr_by(r, n) \
  if((r)->n_steps > n) \
    { \
      fts_ramp_value_incr_by(&(r)->value, (n)); \
      (r)->n_steps -= n; \
    } \
  else \
    { \
      fts_ramp_value_jump(&(r)->value); \
      (r)->n_steps = 0; \
    } \

#endif
