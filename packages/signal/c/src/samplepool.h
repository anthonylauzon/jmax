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
#ifndef _GPOOL_H_
#define _GPOOL_H_

#include <fts/fts.h>
#include <utils/c/include/utils.h>
#include <data/c/include/fvec.h>

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
