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
#include "fts.h"


/****************************
 *  
 *  FTL recursive filter functions
 *
 *    ftl_iir_1 ... 1 pole
 *    ftl_iir_2 ... 2 pole
 *    ftl_iir_3 ... 3 pole
 *    ftl_iir_4 ... 4 pole
 *      float *in
 *      float *out
 *      float *state
 *      float *coefs
 *      long n_tick
 *
 */

extern void ftl_iir_1(fts_word_t *argv);
extern void ftl_iir_2(fts_word_t *argv);
extern void ftl_iir_3(fts_word_t *argv);
extern void ftl_iir_4(fts_word_t *argv);

