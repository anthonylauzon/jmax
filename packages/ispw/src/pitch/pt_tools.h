/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
