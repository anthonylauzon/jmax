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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
