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

#ifndef _WTO_H_
#define _WTO_H_

typedef struct _fts_wto
{
  float *table;
  long size;
  double index;
  double incr;
} fts_wto_t;

#define fts_wto_set_phase(wto, phase) \
  (wto->index = (double)wto->size * phase)

#define fts_wto_set_phase_incr(wto, phase_incr) \
  (wto->incr = (double)wto->size * phase_incr) /* phase_incr = freq / samplerate */

extern void fts_wto_run(fts_wto_t *wto, float *out, long n);
extern void fts_wto_run_freq(fts_wto_t *wto, float *freq, float *out, long n);
extern void fts_wavetab_osc_init(fts_wto_t *wto, float* table, int size);

#endif






