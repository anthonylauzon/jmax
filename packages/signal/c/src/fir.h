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
 */

#include <fts/fts.h>


/****************************
 *  
 *  FTL nonrecursive filter functions
 *
 *    ftl_fir_1 ... 1st order
 *      float *in
 *      float *out
 *      iir_1_state_t *state
 *      iir_1_coefs_t *coefs
 *      long n_tick
 *
 *    ftl_fir_2 ... 2nd order
 *      float *in
 *      float *out
 *      iir_2_state_t *state
 *      iir_2coefs_t *coefs
 *      long n_tick
 *
 *    ftl_fir_n ... nth order
 *      float *in
 *      float *out
 *      iir_n_state_t *state
 *      iir_n_coefs_t *coefs
 *      long n_order
 *      long n_tick
 *
 *
 */

extern void ftl_fir_1(fts_word_t *argv);
extern void ftl_fir_2(fts_word_t *argv);
extern void ftl_fir_n(fts_word_t *argv);

