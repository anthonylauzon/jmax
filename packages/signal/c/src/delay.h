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

#define DELAYLINE_ALLOC_TAIL 2 /* tail for cubic interpolation */
#define DELAYLINE_ALLOC_HEAD 1 /* head for cubic interpolation */

typedef struct 
{
  fts_dsp_object_t o;
  fts_dsp_edge_t *edge; /* DSP edge */
  float *buffer; /* pointer to delay line */
  int delay_size; /* size of delay buffer (max delay in samples) */
  int drain_size; /* size of drain buffer (max drain in samples) */
  int zero_onset; /* onset for buffer zeroing */
  int ring_size; /* size of ring buffer = size + 2 * n_tick */
  int alloc; /* size of biggest allocation */
  double delay_length; /* delay size given in msec */
  double drain_length; /* drain size given in msec */
  int phase; /* current ring buffer phase */
  double sr; /* cash sample rate */
  int n_tick; /* cash tick size */
} delayline_t;

#define delayline_get_edge(d) ((d)->edge)
#define delayline_get_buffer(d) ((d)->buffer)
#define delayline_get_ring_size(d) ((d)->ring_size)
#define delayline_get_delay_size(d) ((d)->delay_size)
#define delayline_get_drain_size(d) ((d)->drain_size)
#define delayline_get_phase(d) ((d)->phase)
#define delayline_get_sr(d) ((d)->sr)
#define delayline_get_n_tick(d) ((d)->n_tick)

extern fts_metaclass_t *delayline_metaclass;

#endif




