/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _GPOOL_H_
#define _GPOOL_H_

#include <fts/fts.h>
#include "fvec.h"

typedef struct _sample
{
  fvec_t *fvec; /* the samples */
  int duration; /* output duration */ 
  int length; /* source length */ 
  float amp; /* amplitude */

  enum {state_fadein, state_sustain, state_fadeout} state;
  int fadein; /* duration in samples */
  int sustain;
  int fadeout;

  fts_idefix_t index; /* fractional sample index */
  fts_idefix_t incr; /* fractional sample incr */

  fts_intphase_t wind_index; /* window phase */
  fts_intphase_t wind_incr; /* window phase increment */

  int time; /* scheduling time / delay */
  int left; /* number of samples left to output */

  struct _sample *next; /* for list of active and inactive samples */
} sample_t;

typedef struct _sample_pool
{
  sample_t *mem;
  int size;
  sample_t *inactive; /* list of inactive samples */
  sample_t *scheduled; /* list of scheduled samples */
  sample_t *active; /* list of active samples */
  int time;
} sample_pool_t;

extern sample_pool_t *sample_pool_new(int n);
extern void sample_pool_delete(sample_pool_t *pool);
extern void sample_pool_set_size(sample_pool_t *pool, int size);

extern void sample_pool_run(sample_pool_t *pool, float *out, int n_tick);
extern sample_t *sample_schedule(sample_pool_t *pool, fvec_t *fvec, double delay, double onset, double length, double incr, float amp);

#endif
