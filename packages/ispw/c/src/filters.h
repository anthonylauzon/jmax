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
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#ifndef _FILTERS_H
#define _FILTERS_H

#include "ispw.h"

#if defined(USE_FP_ONSET)
#define FILTERS_FP_ONSET(c) (1e-37 + (c))
#else
#define FILTERS_FP_ONSET(c) (c)
#endif

typedef struct
{
  float a0;
  float a1;
  float a2;
  float b1;
  float b2;
} biquad_coefs_t;

typedef struct
{
  float ynm1; /* y(n-1) */
  float ynm2; /* y(n-2) */
  float xnm1; /* X(n-1) */
  float xnm2; /* x(n-2) */
} biquad_state_t;

ISPW_API void compute_biquad(float *x, float *y, biquad_state_t *state, biquad_coefs_t *coefs, int n);

#endif
