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

#ifndef _SRCONV_H_
#define _SRCONV_H_

#define FTS_SRCONV_MAX_CHANNELS 16

typedef struct _fts_srconv
{
  double *buffer[FTS_SRCONV_MAX_CHANNELS];
  int in_size;
  int channels;
  double ratio;
  int in_index;
  int filter_index;
  int incr; /* increment for output sample rate (counted in filter resolution) */
  int step; /* step of low-pass/interpolation filter coefficients (counted in filter resolution) */
  double norm; /* normalization factor (for down sampling) */
} fts_srconv_t;

/* new/delete conversion buffer object */
extern fts_srconv_t *fts_srconv_new(double ratio, int size, int n_channels);
extern void fts_srconv_delete(fts_srconv_t *srconv);

/* do conversion and return the number of output samples */
extern int fts_srconv(fts_srconv_t *srconv, float *in, float *out, int size, int out_size, int n_channels);

#endif
