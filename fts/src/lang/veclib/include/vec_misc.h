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

#include "lang/veclib/include/vecdef.h"

/* min, max element of vector */

DECLARE_VEC_FUN(VECSCL, (fmin, float, float, FUN_min, *ptr))
DECLARE_VEC_FUN(VECSCL, (imin, int, int, FUN_min, *ptr))

DECLARE_VEC_FUN(VECSCL, (fmax, float, float, FUN_max, *ptr))
DECLARE_VEC_FUN(VECSCL, (imax, int, int, FUN_max, *ptr))


/* sum vector elements */

DECLARE_VEC_FUN(VECSCL, (fsum, float, float, FUN_sum, 0.))
DECLARE_VEC_FUN(VECSCL, (isum, int, int, FUN_sum, 0))
DECLARE_VEC_FUN(VECSCL, (csum, complex, complex, FUN_csum, 0))


/* clip vector */

DECLARE_VEC_FUN(TWOSCL, (fclip, float, float, float, float, FUN_clip))
DECLARE_VEC_FUN(TWOSCL, (iclip, int, int, int, int, FUN_clip))

