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
/* unroll.h */

#ifndef _UNROLL_H_
#define _UNROLL_H_

/*
 *  These are the unrolling makros used in the definition files: "define/?*.h".
 *  For each unroll factor VECLIB_LOOP_UNROLL (NO, 4, 8, 16) are defined the following makros.
 *  The posfixes "_H" and "_Q" mean a real unrolling by just half or a quarter of the unroll factor.
 *  This is done in order to use not too much of registers if there are many operators (registers) involved in the 
 *  computation.
 *
 *  The loop with increment of the index i:
 *  
 *    UNROLL_LOOP(body) *  ... 
 *    UNROLL_LOOP_H(body)
 *    UNROLL_LOOP_Q(body)
 *
 *  Define local vaiables - index is append to the name given with "reg":
 *  
 *    UNROLL_REGS(t_reg, reg)
 *    UNROLL_REGS_H(t_reg, reg)
 *    UNROLL_REGS_Q(t_reg, reg)
 *
 *  Store element(s) of one real input vector to registers:
 *  
 *    UNROLL_IN(in)
 *    UNROLL_IN_H(in)
 *    UNROLL_IN_Q(in)
 *
 *  Store element(s) of one complex input vector to registers:
 *  
 *    UNROLL_CIN(in)
 *    UNROLL_CIN_H(in)
 *    UNROLL_CIN_Q(in)
 *
 *  Store element(s) of two real input vectors to registers:
 *  
 *    UNROLL_2IN(in0, in1)
 *    UNROLL_2IN_H(in0, in1)
 *    UNROLL_2IN_Q(in0, in1)
 *
 *  Store element(s) of two complex input vectors to registers:
 *  
 *    UNROLL_2CIN(in0, in1)
 *    UNROLL_2CIN_H(in0, in1)
 *    UNROLL_2CIN_Q(in0, in1)
 *
 *  Store element(s) of one real result vector to memory:
 *  
 *    UNROLL_OUT(out)
 *    UNROLL_OUT_H(out)
 *    UNROLL_OUT_Q(out)
 *
 *  Store element(s) of one complex result vector to memory:
 *  
 *    UNROLL_COUT(out)
 *    UNROLL_COUT_H(out)
 *    UNROLL_COUT_Q(out)
 *
 *  Store element(s) of two real result vectors to memory:
 *  
 *    UNROLL_2OUT(out0, out1)
 *    UNROLL_2OUT_H(out0, out1)
 *    UNROLL_2OUT_Q(out0, out1)
 *
 *  Store element(s) of two complex result vectors to memory:
 *  
 *    UNROLL_2COUT(out0, out1)
 *    UNROLL_2COUT_H(out0, out1)
 *    UNROLL_2COUT_Q(out0, out1)
 *
 */

/***********************************************
 *
 *    NO unroll
 *
 */

#define NO_UNROLL_LOOP(body) {int i; for(i=0; i<size; i++){body}}
#define NO_UNROLL_REGS(t_reg, reg) t_reg reg ## _0;
#define NO_UNROLL_IN(in) in ## _0 = in ## _0 = in[i];
#define NO_UNROLL_CIN(in) (in ## _0.re = in[i].re, in ## _0.im = in[i].im);
#define NO_UNROLL_2IN(in0, in1) (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);
#define NO_UNROLL_2CIN(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);
#define NO_UNROLL_OUT(out) out[i] = out ## _0;
#define NO_UNROLL_COUT(out) (out[i].re = out ## _0.re, out[i].im = out ## _0.im);
#define NO_UNROLL_2OUT(out0, out1) (out0[i] = out0 ## _0, out1[i] = out1 ## _0); 
#define NO_UNROLL_2COUT(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);

/***********************************************
 *
 *    unroll by 4
 *
 */
#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_LOOP(body) {int i; for(i=0; i<size; i+=4){body}}
#define UNROLL_LOOP_H(body) {int i; for(i=0; i<size; i+=2){body}}
#define UNROLL_LOOP_Q(body) {int i; for(i=0; i<size; i++){body}}

#define UNROLL_REGS(t_reg, reg) t_reg reg ## _0, reg ## _1, reg ## _2, reg ## _3;
#define UNROLL_REGS_H(t_reg, reg) t_reg reg ## _0, reg ## _1;
#define UNROLL_REGS_Q(t_reg, reg) t_reg reg ## _0;

#define UNROLL_IN(in)\
 in ## _0 = in[i];\
 in ## _1 = in[i+1];\
 in ## _2 = in[i+2];\
 in ## _3 = in[i+3];
#define UNROLL_IN_H(in)\
 in ## _0 = in[i];\
 in ## _1 = in[i+1];
#define UNROLL_IN_Q(in)\
 in ## _0 = in[i];

#define UNROLL_CIN(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);\
 (in ## _1.re = in[i+1].re, in ## _1.im = in[i+1].im);\
 (in ## _2.re = in[i+2].re, in ## _2.im = in[i+2].im);\
 (in ## _3.re = in[i+3].re, in ## _3.im = in[i+3].im);
#define UNROLL_CIN_H(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);\
 (in ## _1.re = in[i+1].re, in ## _1.im = in[i+1].im);
#define UNROLL_CIN_Q(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);
 
#define UNROLL_2IN(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);\
 (in0 ## _1 = in0[i+1], in1 ## _1 = in1[i+1]);\
 (in0 ## _2 = in0[i+2], in1 ## _2 = in1[i+2]);\
 (in0 ## _3 = in0[i+3], in1 ## _3 = in1[i+3]);
#define UNROLL_2IN_H(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);\
 (in0 ## _1 = in0[i+1], in1 ## _1 = in1[i+1]);
#define UNROLL_2IN_Q(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);

#define UNROLL_2CIN(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);\
 (in0 ## _1.re = in0[i+1].re, in1 ## _1.re = in1[i+1].re, in0 ## _1.im = in0[i+1].im, in1 ## _1.im = in1[i+1].im);\
 (in0 ## _2.re = in0[i+2].re, in1 ## _2.re = in1[i+2].re, in0 ## _2.im = in0[i+2].im, in1 ## _2.im = in1[i+2].im);\
 (in0 ## _3.re = in0[i+3].re, in1 ## _3.re = in1[i+3].re, in0 ## _3.im = in0[i+3].im, in1 ## _3.im = in1[i+3].im);
#define UNROLL_2CIN_H(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);\
 (in0 ## _1.re = in0[i+1].re, in1 ## _1.re = in1[i+1].re, in0 ## _1.im = in0[i+1].im, in1 ## _1.im = in1[i+1].im);
#define UNROLL_2CIN_Q(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);

#define UNROLL_OUT(out)\
 out[i] = out ## _0;\
 out[i+1] = out ## _1;\
 out[i+2] = out ## _2;\
 out[i+3] = out ## _3;
#define UNROLL_OUT_H(out)\
 out[i] = out ## _0;\
 out[i+1] = out ## _1;
#define UNROLL_OUT_Q(out)\
 out[i] = out ## _0;

#define UNROLL_COUT(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);\
 (out[i+1].re = out ## _1.re, out[i+1].im = out ## _1.im);\
 (out[i+2].re = out ## _2.re, out[i+2].im = out ## _2.im);\
 (out[i+3].re = out ## _3.re, out[i+3].im = out ## _3.im);
#define UNROLL_COUT_H(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);\
 (out[i+1].re = out ## _1.re, out[i+1].im = out ## _1.im);
#define UNROLL_COUT_Q(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);

#define UNROLL_2OUT(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);\
 (out0[i+1] = out0 ## _1, out1[i+1] = out1 ## _1);\
 (out0[i+2] = out0 ## _2, out1[i+2] = out1 ## _2);\
 (out0[i+3] = out0 ## _3, out1[i+3] = out1 ## _3);
#define UNROLL_2OUT_H(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);\
 (out0[i+1] = out0 ## _1, out1[i+1] = out1 ## _1);
#define UNROLL_2OUT_Q(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);
 
#define UNROLL_2COUT(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);\
 (out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im, out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im);\
 (out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im, out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im);\
 (out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im, out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im);
#define UNROLL_2COUT_H(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);\
 (out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im, out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im);
#define UNROLL_2COUT_Q(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);

/***********************************************
 *
 *    unroll by 8
 *
 */
#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_LOOP(body) {int i; for(i=0; i<size; i+=8){body}}
#define UNROLL_LOOP_H(body) {int i; for(i=0; i<size; i+=4){body}}
#define UNROLL_LOOP_Q(body) {int i; for(i=0; i<size; i+=2){body}}

#define UNROLL_REGS(t_reg, reg)\
 t_reg reg ## _0, reg ## _1, reg ## _2, reg ## _3, reg ## _4, reg ## _5, reg ## _6, reg ## _7;
#define UNROLL_REGS_H(t_reg, reg) t_reg reg ## _0, reg ## _1, reg ## _2, reg ## _3;
#define UNROLL_REGS_Q(t_reg, reg) t_reg reg ## _0, reg ## _1;

#define UNROLL_IN(in)\
 in ## _0 = in[i];\
 in ## _1 = in[i+1];\
 in ## _2 = in[i+2];\
 in ## _3 = in[i+3];\
 in ## _4 = in[i+4];\
 in ## _5 = in[i+5];\
 in ## _6 = in[i+6];\
 in ## _7 = in[i+7];
#define UNROLL_IN_H(in)\
 in ## _0 = in[i];\
 in ## _1 = in[i+1];\
 in ## _2 = in[i+2];\
 in ## _3 = in[i+3];
#define UNROLL_IN_Q(in)\
 in ## _0 = in[i];\
 in ## _1 = in[i+1];

#define UNROLL_CIN(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);\
 (in ## _1.re = in[i+1].re, in ## _1.im = in[i+1].im);\
 (in ## _2.re = in[i+2].re, in ## _2.im = in[i+2].im);\
 (in ## _3.re = in[i+3].re, in ## _3.im = in[i+3].im);\
 (in ## _4.re = in[i+4].re, in ## _4.im = in[i+4].im);\
 (in ## _5.re = in[i+5].re, in ## _5.im = in[i+5].im);\
 (in ## _6.re = in[i+6].re, in ## _6.im = in[i+6].im);\
 (in ## _7.re = in[i+7].re, in ## _7.im = in[i+7].im);
#define UNROLL_CIN_H(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);\
 (in ## _1.re = in[i+1].re, in ## _1.im = in[i+1].im);\
 (in ## _2.re = in[i+2].re, in ## _2.im = in[i+2].im);\
 (in ## _3.re = in[i+3].re, in ## _3.im = in[i+3].im);
#define UNROLL_CIN_Q(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);\
 (in ## _1.re = in[i+1].re, in ## _1.im = in[i+1].im);

#define UNROLL_2IN(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);\
 (in0 ## _1 = in0[i+1], in1 ## _1 = in1[i+1]);\
 (in0 ## _2 = in0[i+2], in1 ## _2 = in1[i+2]);\
 (in0 ## _3 = in0[i+3], in1 ## _3 = in1[i+3]);\
 (in0 ## _4 = in0[i+4], in1 ## _4 = in1[i+4]);\
 (in0 ## _5 = in0[i+5], in1 ## _5 = in1[i+5]);\
 (in0 ## _6 = in0[i+6], in1 ## _6 = in1[i+6]);\
 (in0 ## _7 = in0[i+7], in1 ## _7 = in1[i+7]);
#define UNROLL_2IN_H(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);\
 (in0 ## _1 = in0[i+1], in1 ## _1 = in1[i+1]);\
 (in0 ## _2 = in0[i+2], in1 ## _2 = in1[i+2]);\
 (in0 ## _3 = in0[i+3], in1 ## _3 = in1[i+3]);
#define UNROLL_2IN_Q(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);\
 (in0 ## _1 = in0[i+1], in1 ## _1 = in1[i+1]);

#define UNROLL_2CIN(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);\
 (in0 ## _1.re = in0[i+1].re, in1 ## _1.re = in1[i+1].re, in0 ## _1.im = in0[i+1].im, in1 ## _1.im = in1[i+1].im);\
 (in0 ## _2.re = in0[i+2].re, in1 ## _2.re = in1[i+2].re, in0 ## _2.im = in0[i+2].im, in1 ## _2.im = in1[i+2].im);\
 (in0 ## _3.re = in0[i+3].re, in1 ## _3.re = in1[i+3].re, in0 ## _3.im = in0[i+3].im, in1 ## _3.im = in1[i+3].im);\
 (in0 ## _4.re = in0[i+4].re, in1 ## _4.re = in1[i+4].re, in0 ## _4.im = in0[i+4].im, in1 ## _4.im = in1[i+4].im);\
 (in0 ## _5.re = in0[i+5].re, in1 ## _5.re = in1[i+5].re, in0 ## _5.im = in0[i+5].im, in1 ## _5.im = in1[i+5].im);\
 (in0 ## _6.re = in0[i+6].re, in1 ## _6.re = in1[i+6].re, in0 ## _6.im = in0[i+6].im, in1 ## _6.im = in1[i+6].im);\
 (in0 ## _7.re = in0[i+7].re, in1 ## _7.re = in1[i+7].re, in0 ## _7.im = in0[i+7].im, in1 ## _7.im = in1[i+7].im);
#define UNROLL_2CIN_H(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);\
 (in0 ## _1.re = in0[i+1].re, in1 ## _1.re = in1[i+1].re, in0 ## _1.im = in0[i+1].im, in1 ## _1.im = in1[i+1].im);\
 (in0 ## _2.re = in0[i+2].re, in1 ## _2.re = in1[i+2].re, in0 ## _2.im = in0[i+2].im, in1 ## _2.im = in1[i+2].im);\
 (in0 ## _3.re = in0[i+3].re, in1 ## _3.re = in1[i+3].re, in0 ## _3.im = in0[i+3].im, in1 ## _3.im = in1[i+3].im);
#define UNROLL_2CIN_Q(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);\
 (in0 ## _1.re = in0[i+1].re, in1 ## _1.re = in1[i+1].re, in0 ## _1.im = in0[i+1].im, in1 ## _1.im = in1[i+1].im);

#define UNROLL_OUT(out)\
 out[i] = out ## _0;\
 out[i+1] = out ## _1;\
 out[i+2] = out ## _2;\
 out[i+3] = out ## _3;\
 out[i+4] = out ## _4;\
 out[i+5] = out ## _5;\
 out[i+6] = out ## _6;\
 out[i+7] = out ## _7;
#define UNROLL_OUT_H(out)\
 out[i] = out ## _0;\
 out[i+1] = out ## _1;\
 out[i+2] = out ## _2;\
 out[i+3] = out ## _3;
#define UNROLL_OUT_Q(out)\
 out[i] = out ## _0;\
 out[i+1] = out ## _1;

#define UNROLL_COUT(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);\
 (out[i+1].re = out ## _1.re, out[i+1].im = out ## _1.im);\
 (out[i+2].re = out ## _2.re, out[i+2].im = out ## _2.im);\
 (out[i+3].re = out ## _3.re, out[i+3].im = out ## _3.im);\
 (out[i+4].re = out ## _4.re, out[i+4].im = out ## _4.im);\
 (out[i+5].re = out ## _5.re, out[i+5].im = out ## _5.im);\
 (out[i+6].re = out ## _6.re, out[i+6].im = out ## _6.im);\
 (out[i+7].re = out ## _7.re, out[i+7].im = out ## _7.im);
#define UNROLL_COUT_H(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);\
 (out[i+1].re = out ## _1.re, out[i+1].im = out ## _1.im);\
 (out[i+2].re = out ## _2.re, out[i+2].im = out ## _2.im);\
 (out[i+3].re = out ## _3.re, out[i+3].im = out ## _3.im);
#define UNROLL_COUT_Q(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);\
 (out[i+1].re = out ## _1.re, out[i+1].im = out ## _1.im);

#define UNROLL_2OUT(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);\
 (out0[i+1] = out0 ## _1, out1[i+1] = out1 ## _1);\
 (out0[i+2] = out0 ## _2, out1[i+2] = out1 ## _2);\
 (out0[i+3] = out0 ## _3, out1[i+3] = out1 ## _3);\
 (out0[i+4] = out0 ## _4, out1[i+4] = out1 ## _4);\
 (out0[i+5] = out0 ## _5, out1[i+5] = out1 ## _5);\
 (out0[i+6] = out0 ## _6, out1[i+6] = out1 ## _6);\
 (out0[i+7] = out0 ## _7, out1[i+7] = out1 ## _7);
#define UNROLL_2OUT_H(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);\
 (out0[i+1] = out0 ## _1, out1[i+1] = out1 ## _1);\
 (out0[i+2] = out0 ## _2, out1[i+2] = out1 ## _2);\
 (out0[i+3] = out0 ## _3, out1[i+3] = out1 ## _3);
#define UNROLL_2OUT_Q(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);\
 (out0[i+1] = out0 ## _1, out1[i+1] = out1 ## _1);

#define UNROLL_2COUT(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);\
 (out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im, out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im);\
 (out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im, out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im);\
 (out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im, out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im);\
 (out0[i+4].re = out0 ## _4.re, out1[i+4].im = out1 ## _4.im, out0[i+4].re = out0 ## _4.re, out1[i+4].im = out1 ## _4.im);\
 (out0[i+5].re = out0 ## _5.re, out1[i+5].im = out1 ## _5.im, out0[i+5].re = out0 ## _5.re, out1[i+5].im = out1 ## _5.im);\
 (out0[i+6].re = out0 ## _6.re, out1[i+6].im = out1 ## _6.im, out0[i+6].re = out0 ## _6.re, out1[i+6].im = out1 ## _6.im);\
 (out0[i+7].re = out0 ## _7.re, out1[i+7].im = out1 ## _7.im, out0[i+7].re = out0 ## _7.re, out1[i+7].im = out1 ## _7.im);
#define UNROLL_2COUT_H(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);\
 (out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im, out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im);\
 (out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im, out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im);\
 (out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im, out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im);
#define UNROLL_2COUT_Q(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);\
 (out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im, out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im);

/***********************************************
 *
 *    unroll by 16
 *
 */
#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_LOOP(body) {int i; for(i=0; i<size; i+=16){body}}
#define UNROLL_LOOP_H(body) {int i; for(i=0; i<size; i+=8){body}}
#define UNROLL_LOOP_Q(body) {int i; for(i=0; i<size; i+=4){body}}

#define UNROLL_REGS(t_reg, reg)\
 t_reg reg ## _0, reg ## _1, reg ## _2, reg ## _3, reg ## _4, reg ## _5, reg ## _6, reg ## _7,\
 reg ## _8, reg ## _9, reg ## _10, reg ## _11, reg ## _12, reg ## _13, reg ## _14, reg ## _15;
#define UNROLL_REGS_H(t_reg, reg)\
 t_reg reg ## _0, reg ## _1, reg ## _2, reg ## _3, reg ## _4, reg ## _5, reg ## _6, reg ## _7;
#define UNROLL_REGS_Q(t_reg, reg)\
 t_reg reg ## _0, reg ## _1, reg ## _2, reg ## _3;

#define UNROLL_IN(in)\
 in ## _0 = in[i];\
 in ## _1 = in[i+1];\
 in ## _2 = in[i+2];\
 in ## _3 = in[i+3];\
 in ## _4 = in[i+4];\
 in ## _5 = in[i+5];\
 in ## _6 = in[i+6];\
 in ## _7 = in[i+7];\
 in ## _8 = in[i+8];\
 in ## _9 = in[i+9];\
 in ## _10 = in[i+10];\
 in ## _11 = in[i+11];\
 in ## _12 = in[i+12];\
 in ## _13 = in[i+13];\
 in ## _14 = in[i+14];\
 in ## _15 = in[i+15];
#define UNROLL_IN_H(in)\
 in ## _0 = in[i];\
 in ## _1 = in[i+1];\
 in ## _2 = in[i+2];\
 in ## _3 = in[i+3];\
 in ## _4 = in[i+4];\
 in ## _5 = in[i+5];\
 in ## _6 = in[i+6];\
 in ## _7 = in[i+7];
#define UNROLL_IN_Q(in)\
 in ## _0 = in[i];\
 in ## _1 = in[i+1];\
 in ## _2 = in[i+2];\
 in ## _3 = in[i+3];

#define UNROLL_CIN(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);\
 (in ## _1.re = in[i+1].re, in ## _1.im = in[i+1].im);\
 (in ## _2.re = in[i+2].re, in ## _2.im = in[i+2].im);\
 (in ## _3.re = in[i+3].re, in ## _3.im = in[i+3].im);\
 (in ## _4.re = in[i+4].re, in ## _4.im = in[i+4].im);\
 (in ## _5.re = in[i+5].re, in ## _5.im = in[i+5].im);\
 (in ## _6.re = in[i+6].re, in ## _6.im = in[i+6].im);\
 (in ## _7.re = in[i+7].re, in ## _7.im = in[i+7].im);\
 (in ## _8.re = in[i+8].re, in ## _8.im = in[i+8].im);\
 (in ## _9.re = in[i+9].re, in ## _9.im = in[i+9].im);\
 (in ## _10.re = in[i+10].re, in ## _10.im = in[i+10].im);\
 (in ## _11.re = in[i+11].re, in ## _11.im = in[i+11].im);
 (in ## _12.re = in[i+12].re, in ## _12.im = in[i+12].im);\
 (in ## _13.re = in[i+13].re, in ## _13.im = in[i+13].im);\
 (in ## _14.re = in[i+14].re, in ## _14.im = in[i+14].im);\
 (in ## _15.re = in[i+15].re, in ## _15.im = in[i+15].im);
#define UNROLL_CIN_H(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);\
 (in ## _1.re = in[i+1].re, in ## _1.im = in[i+1].im);\
 (in ## _2.re = in[i+2].re, in ## _2.im = in[i+2].im);\
 (in ## _3.re = in[i+3].re, in ## _3.im = in[i+3].im);\
 (in ## _4.re = in[i+4].re, in ## _4.im = in[i+4].im);\
 (in ## _5.re = in[i+5].re, in ## _5.im = in[i+5].im);\
 (in ## _6.re = in[i+6].re, in ## _6.im = in[i+6].im);\
 (in ## _7.re = in[i+7].re, in ## _7.im = in[i+7].im);
#define UNROLL_CIN_Q(in)\
 (in ## _0.re = in[i].re, in ## _0.im = in[i].im);\
 (in ## _1.re = in[i+1].re, in ## _1.im = in[i+1].im);\
 (in ## _2.re = in[i+2].re, in ## _2.im = in[i+2].im);\
 (in ## _3.re = in[i+3].re, in ## _3.im = in[i+3].im);

#define UNROLL_2IN(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);\
 (in0 ## _1 = in0[i+1], in1 ## _1 = in1[i+1]);\
 (in0 ## _2 = in0[i+2], in1 ## _2 = in1[i+2]);\
 (in0 ## _3 = in0[i+3], in1 ## _3 = in1[i+3]);\
 (in0 ## _4 = in0[i+4], in1 ## _4 = in1[i+4]);\
 (in0 ## _5 = in0[i+5], in1 ## _5 = in1[i+5]);\
 (in0 ## _6 = in0[i+6], in1 ## _6 = in1[i+6]);\
 (in0 ## _7 = in0[i+7], in1 ## _7 = in1[i+7]);\
 (in0 ## _8 = in0[i+8], in1 ## _8 = in1[i+8]);\
 (in0 ## _9 = in0[i+9], in1 ## _9 = in1[i+9]);\
 (in0 ## _10 = in0[i+10], in1 ## _10 = in1[i+10]);\
 (in0 ## _11 = in0[i+11], in1 ## _11 = in1[i+11]);\
 (in0 ## _12 = in0[i+12], in1 ## _12 = in1[i+12]);\
 (in0 ## _13 = in0[i+13], in1 ## _13 = in1[i+13]);\
 (in0 ## _14 = in0[i+14], in1 ## _14 = in1[i+14]);\
 (in0 ## _15 = in0[i+15], in1 ## _15 = in1[i+15]);
#define UNROLL_2IN_H(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);\
 (in0 ## _1 = in0[i+1], in1 ## _1 = in1[i+1]);\
 (in0 ## _2 = in0[i+2], in1 ## _2 = in1[i+2]);\
 (in0 ## _3 = in0[i+3], in1 ## _3 = in1[i+3]);\
 (in0 ## _4 = in0[i+4], in1 ## _4 = in1[i+4]);\
 (in0 ## _5 = in0[i+5], in1 ## _5 = in1[i+5]);\
 (in0 ## _6 = in0[i+6], in1 ## _6 = in1[i+6]);\
 (in0 ## _7 = in0[i+7], in1 ## _7 = in1[i+7]);
#define UNROLL_2IN_Q(in0, in1)\
 (in0 ## _0 = in0[i], in1 ## _0 = in1[i]);\
 (in0 ## _1 = in0[i+1], in1 ## _1 = in1[i+1]);\
 (in0 ## _2 = in0[i+2], in1 ## _2 = in1[i+2]);\
 (in0 ## _3 = in0[i+3], in1 ## _3 = in1[i+3]);

#define UNROLL_2CIN(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);\
 (in0 ## _1.re = in0[i+1].re, in1 ## _1.re = in1[i+1].re, in0 ## _1.im = in0[i+1].im, in1 ## _1.im = in1[i+1].im);\
 (in0 ## _2.re = in0[i+2].re, in1 ## _2.re = in1[i+2].re, in0 ## _2.im = in0[i+2].im, in1 ## _2.im = in1[i+2].im);\
 (in0 ## _3.re = in0[i+3].re, in1 ## _3.re = in1[i+3].re, in0 ## _3.im = in0[i+3].im, in1 ## _3.im = in1[i+3].im);\
 (in0 ## _4.re = in0[i+4].re, in1 ## _4.re = in1[i+4].re, in0 ## _4.im = in0[i+4].im, in1 ## _4.im = in1[i+4].im);\
 (in0 ## _5.re = in0[i+5].re, in1 ## _5.re = in1[i+5].re, in0 ## _5.im = in0[i+5].im, in1 ## _5.im = in1[i+5].im);\
 (in0 ## _6.re = in0[i+6].re, in1 ## _6.re = in1[i+6].re, in0 ## _6.im = in0[i+6].im, in1 ## _6.im = in1[i+6].im);\
 (in0 ## _7.re = in0[i+7].re, in1 ## _7.re = in1[i+7].re, in0 ## _7.im = in0[i+7].im, in1 ## _7.im = in1[i+7].im);\
 (in0 ## _8.re = in0[i+8].re, in1 ## _8.re = in1[i+8].re, in0 ## _8.im = in0[i+8].im, in1 ## _8.im = in1[i+8].im);\
 (in0 ## _9.re = in0[i+9].re, in1 ## _9.re = in1[i+9].re, in0 ## _9.im = in0[i+9].im, in1 ## _9.im = in1[i+9].im);\
 (in0 ## _10.re = in0[i+10].re, in1 ## _10.re = in1[i+10].re, in0 ## _10.im = in0[i+10].im, in1 ## _10.im = in1[i+10].im);\
 (in0 ## _11.re = in0[i+11].re, in1 ## _11.re = in1[i+11].re, in0 ## _11.im = in0[i+11].im, in1 ## _11.im = in1[i+11].im);\
 (in0 ## _12.re = in0[i+12].re, in1 ## _12.re = in1[i+12].re, in0 ## _12.im = in0[i+12].im, in1 ## _12.im = in1[i+12].im);\
 (in0 ## _13.re = in0[i+13].re, in1 ## _13.re = in1[i+13].re, in0 ## _13.im = in0[i+13].im, in1 ## _13.im = in1[i+13].im);\
 (in0 ## _14.re = in0[i+14].re, in1 ## _14.re = in1[i+14].re, in0 ## _14.im = in0[i+14].im, in1 ## _14.im = in1[i+14].im);\
 (in0 ## _15.re = in0[i+15].re, in1 ## _15.re = in1[i+15].re, in0 ## _15.im = in0[i+15].im, in1 ## _15.im = in1[i+15].im);
#define UNROLL_2CIN_H(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);\
 (in0 ## _1.re = in0[i+1].re, in1 ## _1.re = in1[i+1].re, in0 ## _1.im = in0[i+1].im, in1 ## _1.im = in1[i+1].im);\
 (in0 ## _2.re = in0[i+2].re, in1 ## _2.re = in1[i+2].re, in0 ## _2.im = in0[i+2].im, in1 ## _2.im = in1[i+2].im);\
 (in0 ## _3.re = in0[i+3].re, in1 ## _3.re = in1[i+3].re, in0 ## _3.im = in0[i+3].im, in1 ## _3.im = in1[i+3].im);\
 (in0 ## _4.re = in0[i+4].re, in1 ## _4.re = in1[i+4].re, in0 ## _4.im = in0[i+4].im, in1 ## _4.im = in1[i+4].im);\
 (in0 ## _5.re = in0[i+5].re, in1 ## _5.re = in1[i+5].re, in0 ## _5.im = in0[i+5].im, in1 ## _5.im = in1[i+5].im);\
 (in0 ## _6.re = in0[i+6].re, in1 ## _6.re = in1[i+6].re, in0 ## _6.im = in0[i+6].im, in1 ## _6.im = in1[i+6].im);\
 (in0 ## _7.re = in0[i+7].re, in1 ## _7.re = in1[i+7].re, in0 ## _7.im = in0[i+7].im, in1 ## _7.im = in1[i+7].im);
#define UNROLL_2CIN_Q(in0, in1)\
 (in0 ## _0.re = in0[i].re, in1 ## _0.re = in1[i].re, in0 ## _0.im = in0[i].im, in1 ## _0.im = in1[i].im);\
 (in0 ## _1.re = in0[i+1].re, in1 ## _1.re = in1[i+1].re, in0 ## _1.im = in0[i+1].im, in1 ## _1.im = in1[i+1].im);\
 (in0 ## _2.re = in0[i+2].re, in1 ## _2.re = in1[i+2].re, in0 ## _2.im = in0[i+2].im, in1 ## _2.im = in1[i+2].im);\
 (in0 ## _3.re = in0[i+3].re, in1 ## _3.re = in1[i+3].re, in0 ## _3.im = in0[i+3].im, in1 ## _3.im = in1[i+3].im);

#define UNROLL_OUT(out)\
 out[i] = out ## _0;\
 out[i+1] = out ## _1;\
 out[i+2] = out ## _2;\
 out[i+3] = out ## _3;\
 out[i+4] = out ## _4;\
 out[i+5] = out ## _5;\
 out[i+6] = out ## _6;\
 out[i+7] = out ## _7;\
 out[i+8] = out ## _8;\
 out[i+9] = out ## _9;\
 out[i+10] = out ## _10;\
 out[i+11] = out ## _11;\
 out[i+12] = out ## _12;\
 out[i+13] = out ## _13;\
 out[i+14] = out ## _14;\
 out[i+15] = out ## _15;
#define UNROLL_OUT_H(out)\
 out[i] = out ## _0;\
 out[i+1] = out ## _1;\
 out[i+2] = out ## _2;\
 out[i+3] = out ## _3;\
 out[i+4] = out ## _4;\
 out[i+5] = out ## _5;\
 out[i+6] = out ## _6;\
 out[i+7] = out ## _7;
#define UNROLL_OUT_Q(out)\
 out[i] = out ## _0;\
 out[i+1] = out ## _1;\
 out[i+2] = out ## _2;\
 out[i+3] = out ## _3;

#define UNROLL_COUT(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);\
 (out[i+1].re = out ## _1.re, out[i+1].im = out ## _1.im);\
 (out[i+2].re = out ## _2.re, out[i+2].im = out ## _2.im);\
 (out[i+3].re = out ## _3.re, out[i+3].im = out ## _3.im);\
 (out[i+4].re = out ## _4.re, out[i+4].im = out ## _4.im);\
 (out[i+5].re = out ## _5.re, out[i+5].im = out ## _5.im);\
 (out[i+6].re = out ## _6.re, out[i+6].im = out ## _6.im);\
 (out[i+7].re = out ## _7.re, out[i+7].im = out ## _7.im);\
 (out[i+8].re = out ## _8.re, out[i+8].im = out ## _8.im);\
 (out[i+9].re = out ## _9.re, out[i+9].im = out ## _9.im);\
 (out[i+10].re = out ## _10.re, out[i+10].im = out ## _10.im);\
 (out[i+11].re = out ## _11.re, out[i+11].im = out ## _11.im);\
 (out[i+12].re = out ## _12.re, out[i+12].im = out ## _12.im);\
 (out[i+13].re = out ## _13.re, out[i+13].im = out ## _13.im);\
 (out[i+14].re = out ## _14.re, out[i+14].im = out ## _14.im);\
 (out[i+15].re = out ## _15.re, out[i+15].im = out ## _15.im);
#define UNROLL_COUT_H(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);\
 (out[i+1].re = out ## _1.re, out[i+1].im = out ## _1.im);\
 (out[i+2].re = out ## _2.re, out[i+2].im = out ## _2.im);\
 (out[i+3].re = out ## _3.re, out[i+3].im = out ## _3.im);\
 (out[i+4].re = out ## _4.re, out[i+4].im = out ## _4.im);\
 (out[i+5].re = out ## _5.re, out[i+5].im = out ## _5.im);\
 (out[i+6].re = out ## _6.re, out[i+6].im = out ## _6.im);\
 (out[i+7].re = out ## _7.re, out[i+7].im = out ## _7.im);
#define UNROLL_COUT_Q(out)\
 (out[i].re = out ## _0.re, out[i].im = out ## _0.im);\
 (out[i+1].re = out ## _1.re, out[i+1].im = out ## _1.im);\
 (out[i+2].re = out ## _2.re, out[i+2].im = out ## _2.im);\
 (out[i+3].re = out ## _3.re, out[i+3].im = out ## _3.im);

#define UNROLL_2OUT(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);\
 (out0[i+1] = out0 ## _1, out1[i+1] = out1 ## _1);\
 (out0[i+2] = out0 ## _2, out1[i+2] = out1 ## _2);\
 (out0[i+3] = out0 ## _3, out1[i+3] = out1 ## _3);\
 (out0[i+4] = out0 ## _4, out1[i+4] = out1 ## _4);\
 (out0[i+5] = out0 ## _5, out1[i+5] = out1 ## _5);\
 (out0[i+6] = out0 ## _6, out1[i+6] = out1 ## _6);\
 (out0[i+7] = out0 ## _7, out1[i+7] = out1 ## _7);\
 (out0[i+8] = out0 ## _8, out1[i+8] = out1 ## _8);\
 (out0[i+9] = out0 ## _9, out1[i+9] = out1 ## _9);\
 (out0[i+10] = out0 ## _10, out1[i+10] = out1 ## _10);\
 (out0[i+11] = out0 ## _11, out1[i+11] = out1 ## _11);\
 (out0[i+12] = out0 ## _12, out1[i+12] = out1 ## _12);\
 (out0[i+13] = out0 ## _13, out1[i+13] = out1 ## _13);\
 (out0[i+14] = out0 ## _14, out1[i+14] = out1 ## _14);\
 (out0[i+15] = out0 ## _15, out1[i+15] = out1 ## _15);
#define UNROLL_2OUT_H(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);\
 (out0[i+1] = out0 ## _1, out1[i+1] = out1 ## _1);\
 (out0[i+2] = out0 ## _2, out1[i+2] = out1 ## _2);\
 (out0[i+3] = out0 ## _3, out1[i+3] = out1 ## _3);\
 (out0[i+4] = out0 ## _4, out1[i+4] = out1 ## _4);\
 (out0[i+5] = out0 ## _5, out1[i+5] = out1 ## _5);\
 (out0[i+6] = out0 ## _6, out1[i+6] = out1 ## _6);\
 (out0[i+7] = out0 ## _7, out1[i+7] = out1 ## _7);
#define UNROLL_2OUT_Q(out0, out1)\
 (out0[i] = out0 ## _0, out1[i] = out1 ## _0);\
 (out0[i+1] = out0 ## _1, out1[i+1] = out1 ## _1);\
 (out0[i+2] = out0 ## _2, out1[i+2] = out1 ## _2);\
 (out0[i+3] = out0 ## _3, out1[i+3] = out1 ## _3);

#define UNROLL_2COUT(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);\
 (out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im, out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im);\
 (out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im, out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im);\
 (out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im, out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im);\
 (out0[i+4].re = out0 ## _4.re, out1[i+4].im = out1 ## _4.im, out0[i+4].re = out0 ## _4.re, out1[i+4].im = out1 ## _4.im);\
 (out0[i+5].re = out0 ## _5.re, out1[i+5].im = out1 ## _5.im, out0[i+5].re = out0 ## _5.re, out1[i+5].im = out1 ## _5.im);\
 (out0[i+6].re = out0 ## _6.re, out1[i+6].im = out1 ## _6.im, out0[i+6].re = out0 ## _6.re, out1[i+6].im = out1 ## _6.im);\
 (out0[i+7].re = out0 ## _7.re, out1[i+7].im = out1 ## _7.im, out0[i+7].re = out0 ## _7.re, out1[i+7].im = out1 ## _7.im);\
 (out0[i+8].re = out0 ## _8.re, out1[i+8].im = out1 ## _8.im, out0[i+8].re = out0 ## _8.re, out1[i+8].im = out1 ## _8.im);\
 (out0[i+9].re = out0 ## _9.re, out1[i+9].im = out1 ## _9.im, out0[i+9].re = out0 ## _9.re, out1[i+9].im = out1 ## _9.im);\
 (out0[i+10].re = out0 ## _10.re, out1[i+10].im = out1 ## _10.im, out0[i+10].re = out0 ## _10.re, out1[i+10].im = out1 ## _10.im);\
 (out0[i+11].re = out0 ## _11.re, out1[i+11].im = out1 ## _11.im, out0[i+11].re = out0 ## _11.re, out1[i+11].im = out1 ## _11.im);\
 (out0[i+12].re = out0 ## _12.re, out1[i+12].im = out1 ## _12.im, out0[i+12].re = out0 ## _12.re, out1[i+12].im = out1 ## _12.im);\
 (out0[i+13].re = out0 ## _13.re, out1[i+13].im = out1 ## _13.im, out0[i+13].re = out0 ## _13.re, out1[i+13].im = out1 ## _13.im);\
 (out0[i+14].re = out0 ## _14.re, out1[i+14].im = out1 ## _14.im, out0[i+14].re = out0 ## _14.re, out1[i+14].im = out1 ## _14.im);\
 (out0[i+15].re = out0 ## _15.re, out1[i+15].im = out1 ## _15.im, out0[i+15].re = out0 ## _15.re, out1[i+15].im = out1 ## _15.im);
#define UNROLL_2COUT_H(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);\
 (out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im, out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im);\
 (out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im, out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im);\
 (out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im, out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im);\
 (out0[i+4].re = out0 ## _4.re, out1[i+4].im = out1 ## _4.im, out0[i+4].re = out0 ## _4.re, out1[i+4].im = out1 ## _4.im);\
 (out0[i+5].re = out0 ## _5.re, out1[i+5].im = out1 ## _5.im, out0[i+5].re = out0 ## _5.re, out1[i+5].im = out1 ## _5.im);\
 (out0[i+6].re = out0 ## _6.re, out1[i+6].im = out1 ## _6.im, out0[i+6].re = out0 ## _6.re, out1[i+6].im = out1 ## _6.im);\
 (out0[i+7].re = out0 ## _7.re, out1[i+7].im = out1 ## _7.im, out0[i+7].re = out0 ## _7.re, out1[i+7].im = out1 ## _7.im);
#define UNROLL_2COUT_Q(out0, out1)\
 (out0[i].re = out0 ## _0.re, out1[i].re = out1 ## _0.re, out0[i].im = out0 ## _0.im, out1[i].im = out1 ## _0.im);\
 (out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im, out0[i+1].re = out0 ## _1.re, out1[i+1].im = out1 ## _1.im);\
 (out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im, out0[i+2].re = out0 ## _2.re, out1[i+2].im = out1 ## _2.im);\
 (out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im, out0[i+3].re = out0 ## _3.re, out1[i+3].im = out1 ## _3.im);

/***********************************************
 *
 *    unroll not specified --> don't
 *
 */
#else

#define UNROLL_LOOP(body) NO_UNROLL_LOOP(body)
#define UNROLL_LOOP_H(body) NO_UNROLL_LOOP(body)
#define UNROLL_LOOP_Q(body) NO_UNROLL_LOOP(body)

#define UNROLL_REGS(t_reg, reg) NO_UNROLL_REGS(t_reg, reg)
#define UNROLL_REGS_H(t_reg, reg) NO_UNROLL_REGS(t_reg, reg)
#define UNROLL_REGS_Q(t_reg, reg) NO_UNROLL_REGS(t_reg, reg)

#define UNROLL_IN(in) NO_UNROLL_IN(in)
#define UNROLL_IN_H(in) NO_UNROLL_IN(in)
#define UNROLL_IN_Q(in) NO_UNROLL_IN(in)

#define UNROLL_CIN(in) NO_UNROLL_CIN(in)
#define UNROLL_CIN_H(in) NO_UNROLL_CIN(in)
#define UNROLL_CIN_Q(in) NO_UNROLL_CIN(in)

#define UNROLL_2IN(in0, in1) NO_UNROLL_2IN(in0, in1)
#define UNROLL_2IN_H(in0, in1) NO_UNROLL_2IN(in0, in1)
#define UNROLL_2IN_Q(in0, in1) NO_UNROLL_2IN(in0, in1)

#define UNROLL_2CIN(in0, in1) NO_UNROLL_2CIN(in0, in1)
#define UNROLL_2CIN_H(in0, in1) NO_UNROLL_2CIN(in0, in1)
#define UNROLL_2CIN_Q(in0, in1) NO_UNROLL_2CIN(in0, in1)

#define UNROLL_OUT(out) NO_UNROLL_OUT(out)
#define UNROLL_OUT_H(out) NO_UNROLL_OUT(out)
#define UNROLL_OUT_Q(out) NO_UNROLL_OUT(out)

#define UNROLL_COUT(out) NO_UNROLL_COUT(out)
#define UNROLL_COUT_H(out) NO_UNROLL_COUT(out)
#define UNROLL_COUT_Q(out) NO_UNROLL_COUT(out)

#define UNROLL_2OUT(out0, out1) NO_UNROLL_2OUT(out0, out1)
#define UNROLL_2OUT_H(out0, out1) NO_UNROLL_2OUT(out0, out1)
#define UNROLL_2OUT_Q(out0, out1) NO_UNROLL_2OUT(out0, out1)

#define UNROLL_2COUT(out0, out1) NO_UNROLL_2COUT(out0, out1)
#define UNROLL_2COUT_H(out0, out1) NO_UNROLL_2COUT(out0, out1)
#define UNROLL_2COUT_Q(out0, out1) NO_UNROLL_2COUT(out0, out1)

#endif

#endif /* _UNROLL_H_ */

