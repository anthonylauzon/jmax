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

/****************************
 *  
 *  biquad filter coefficients and states
 *
 */

typedef struct
{
  float a0;
  float a1;
  float a2;
  float b1;
  float b2;
} biquad_coefs_t;

/****************************
 *  
 *  filter state for direct form I:
 *
 *     y(n) = a0 * x(n) + a1 * x(n-1) + a2 * x(n-2) - b1 * y(n-1) - b2 * y(n-2) 
 *  
 */

typedef struct
{
  float ynm1; /* y(n-1) */
  float ynm2; /* y(n-2) */
  float xnm1; /* X(n-1) */
  float xnm2; /* x(n-2) */
} biquad_df1_state_t;



/****************************
 *  
 *  filter state for direct form II:
 *
 *     w(n) = x(n) - b1 * w(n-1) - b2 * w(n-2)
 *     y(n) = a0 * w(n) + a1 * w(n-1) + a2 * w(n-2)
 *
 */

typedef struct
{
  float wnm1; /* w(n-1) */
  float wnm2; /* w(n-2) */
} biquad_df2_state_t;


/****************************
 *  
 *  FTL biquad filter functions
 *
 *    ftl_biquad_df1
 *      float *in
 *      float *out
 *      biquad_state_df1_t *state
 *      biquad_coefs_t *coefs
 *      long n_tick
 *
 *    ftl_biquad_df2
 *      float *in
 *      float *out
 *      biquad_state_df2_t *state
 *      biquad_coefs_t *coefs
 *      long n_tick
 *
 *
 */

extern void ftl_biquad_df1(fts_word_t *argv); /* direct form I */
extern void ftl_biquad_df2(fts_word_t *argv); /* direct form II */

/* In place versions, only one argument as in/out */

extern void ftl_biquad_df1_inplace(fts_word_t *argv); /* direct form I */
extern void ftl_biquad_df2_inplace(fts_word_t *argv); /* direct form II */

