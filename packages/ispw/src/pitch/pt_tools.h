/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/* pt_tools.h */

#ifndef _PT_TOOLS_H_
#define _PT_TOOLS_H_


#include "fts.h"



typedef struct{
  float    *filter_coeffs; /* pointer to coeficients */
  int     n_filter_coeffs; /* # of coefficients */
  int     onset; /* onset for fft result in # of floats (not complex!) */
  float     pow_corr; /* power correction factor for low frequencies */
} kernel_t;

/* creating and/or returning the filter bank kernels */
extern void pt_common_init_millers_kernels(void);
extern void pt_common_print_millers_kernels(int channel);

/* Miller's "bounded Q" */

extern void pt_common_millers_bounded_q(complex *fft_spectrum, kernel_t *filter, float *q_pow_spec, int n_channels);


/* Miller's dum sqrt (ask him how it works) */

#define pt_common_init_millers_dumsqrt()
#define pt_common_millers_dumsqrt(x) sqrt(x)


/* Miller's mean power */
extern float pt_common_millers_mean_power(float *sig, float *temp, float *wind, int n_points);

/* Miller's fundamental pitch algorithm */
extern int pt_common_find_pitch_candidate(pt_common_obj_t *x, float *cand, float *pit_pow, float *tot_pow, int print_out);
extern float pt_common_candidate_frequency(pt_common_obj_t *x, float candidate);
extern float pt_common_candidate_midi_pitch(pt_common_obj_t *x, float candidate);

#endif /* _PT_TOOLS_H_ */
