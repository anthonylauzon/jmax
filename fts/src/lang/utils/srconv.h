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
