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

#ifndef _RAMP_VECTORS_H_
#define _RAMP_VECTORS_H_

/*****************************************
 *
 *  ramp vectors
 *
 */

typedef struct _fts_framps
{
  float *value;
  float *target;
  float *incr;
  int n_steps;
  int size;
  int alloc;
} fts_framps_t;

extern fts_framps_t *fts_framps_new(int size);
extern void fts_framps_delete(fts_framps_t *ramp_vector);
extern void fts_framps_zero(fts_framps_t *ramp_vector);
extern void fts_framps_zero_tail(fts_framps_t *ramp_vector, int index);

extern void fts_framps_set_targets(fts_framps_t *ramp_vector, float *targets, int size, float time, float rate);
extern void fts_framps_set_targets_hold_and_jump(fts_framps_t *ramp_vector, float *targets, int size, float time, float rate);
extern void fts_framps_set_intervals(fts_framps_t *ramp_vector, float *intervals, int size, float time, float rate);
extern void fts_framps_set_slopes(fts_framps_t *ramp_vector, float *slopes, int size, float time, float rate);

extern void fts_framps_jump(fts_framps_t *ramp_vector);
extern void fts_framps_freeze(fts_framps_t *ramp_vector);

extern void fts_framps_incr(fts_framps_t *ramp_vector);

#define fts_framps_get_size(ramp_vector) ((ramp_vector)->size)
extern void fts_framps_set_size(fts_framps_t *ramp_vector, int size);

#define fts_framps_get(ramp_vector) ((ramp_vector)->value)

#endif
