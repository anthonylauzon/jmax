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
#ifndef _PT_OBJ_H_
#define _PT_OBJ_H_

#include "fts.h"

/* filter bands */
#define pt_common_CHANNELS_PER_OCTAVE 24
#define pt_common_N_AMP_COEFFS 8
#define PB_0 0
#define PB_1 24
#define PB_2 38
#define PB_3 48
#define PB_4 56
#define PB_5 62
#define PB_6 67
#define PB_7 72
#define pt_common_EXTRA_CHANNELS PB_7

#define pt_common_MAX_n_points 1024
#define pt_common_MAX_ld_n_points 10
#define pt_common_MAX_n_channels (pt_common_CHANNELS_PER_OCTAVE * (pt_common_MAX_ld_n_points - 2))

#define pt_common_new_

typedef struct{
  float *main;
  float *end;
  float *fill;
  complex *for_fft;
} pt_common_buf_t;

typedef struct{
  float freq_ref;
  int pitch_rough;
  long pitch_low;
  long pitch_high;
  float pitch_stick;
  float pitch_stretch;
  float power_off;
  float power_on;
  float quality_on;
  float quality_off;
  float coeffs[pt_common_N_AMP_COEFFS];
  long debounce_time;
} pt_common_ctl_t;

typedef struct{
  float pitch_base;
  float freq_base;
  int index_low;
  int index_high;
  int last_index;
} pt_common_stat_t;

typedef struct{
  fts_object_t header;
  int n_points;
  int n_overlap;
  int n_channels;
  float srate;
  pt_common_buf_t buf;
  pt_common_ctl_t ctl;
  pt_common_stat_t stat;
} pt_common_obj_t;

#endif /* _PT_OBJ_H_ */

