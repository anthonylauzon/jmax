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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#ifndef _SIGNAL_DELAY_H_
#define _SIGNAL_DELAY_H_

typedef struct 
{
  fts_object_t o;
  float *samples; /* pointer to delay line */
  int phase; /* current ring buffer phase */
  int size; /* virtual delay line size in samples */
  int alloc; /* size of biggest allocation */
  double length; /* delay line size given in msec */
  fts_object_t *stage; /* DSP stage */
  double sr;
  int n_tick;
} delayline_t;

#define delayline_get_samples(d) ((d)->samples)
#define delayline_get_phase(d) ((d)->phase)
#define delayline_get_size(d) ((d)->size)
#define delayline_get_stage(d) ((d)->stage)
#define delayline_get_sr(d) ((d)->sr)
#define delayline_get_n_tick(d) ((d)->n_tick)

extern fts_metaclass_t *dline_metaclass;
extern fts_metaclass_t *drain_metaclass;

#endif




