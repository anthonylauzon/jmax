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
/* kernels/misc.h */

#ifndef _KERNELSMISC_H_
#define _KERNELSMISC_H_

#include "lang/veclib/complex.h"


/*******************************************************************
 *
 *    miscellanous functions
 *
 *    in/decrement:
 *      FUN_inc ... increment
 *      FUN_dec ... decrement
 *
 *    min/maximum:
 *      FUN_min ... find minimum element
 *      FUN_max ... find maximum element
 *
 *    sum:
 *      FUN_sum ... sum vector elements
 *      FUN_csum ... sum vector elements
 *
 *    zero/fill:
 *      FUN_zero ... zero vector elements
 *      FUN_czero ... zero vector elements
 *
 *    clip:
 *      FUN_clip ... clip vector elements
 *
 */


/*******************************************************************
 *
 *    in/decrement
 *
 */
 
#define FUN_inc(in, out) ((out) = (in)++)
#define FUN_dec(in, out) ((out) = (in)--)


/*******************************************************************
 *
 *    min/maximum
 *
 */
 
#define FUN_min(in, scl) ((scl) = (((in) < (scl))? (in): (scl)))
#define FUN_max(in, scl) ((scl) = (((in) > (scl))? (in): (scl)))


/*******************************************************************
 *
 *    sum
 *
 */
 
#define FUN_sum(in, scl) ((scl) += (in))
#define FUN_csum(in, scl) ((scl).re += (in).re, (scl).im += (in).im)


/*******************************************************************
 *
 *    zero
 *
 */
 
#define FUN_zero(out) ((out) = 0)
#define FUN_czero(out) ((out).re = (out).im = 0)


/*******************************************************************
 *
 *    clip
 *
 */
 
#define FUN_clip(in, min, max, out) ((out) = ((in) < (min))? (min): (((in) > (max))? (max): (in)))


#endif /* _KERNELSMISC_H_ */
