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

#include <fts/sys.h>
#include <fts/lang/veclib/include/vecdef.h>

DEFINE_FTS_VEC_FUN(fcpy, float, float, FUN_cpy)
DEFINE_FTS_VEC_FUN(icpy, int, int, FUN_cpy)
DEFINE_FTS_VEC_CFUN(ccpy, complex, complex, FUN_ccpy) /* _CFUN: prefetches re and im! */

DEFINE_FTS_VEC_FILL(ffill, float, float, FUN_cpy)
DEFINE_FTS_VEC_FILL(ifill, int, int, FUN_cpy)
DEFINE_FTS_VEC_FILL(cfill, complex, complex, FUN_ccpy)

DEFINE_FTS_VEC_FUN(re, complex, float, FUN_re)
DEFINE_FTS_VEC_FUN(im, complex, float, FUN_im)

DEFINE_FTS_VEC_SET(fzero, float, FUN_zero)
DEFINE_FTS_VEC_SET(izero, int, FUN_zero)
DEFINE_FTS_VEC_SET(czero, complex, FUN_czero)

DEFINE_FTS_VEC_FUN(fcasti, float, int, FUN_cpy)
DEFINE_FTS_VEC_FUN(icastf, int, float, FUN_cpy)



DEFINE_FTL_VEC_FUN(fcpy, float, float, FUN_cpy)
DEFINE_FTL_VEC_FUN(icpy, int, int, FUN_cpy)
DEFINE_FTL_VEC_CFUN(ccpy, complex, complex, FUN_ccpy) /* _CFUN: prefetches re and im! */

DEFINE_FTL_VEC_FILL(ffill, float, float, FUN_cpy)
DEFINE_FTL_VEC_FILL(ifill, int, int, FUN_cpy)
DEFINE_FTL_VEC_FILL(cfill, complex, complex, FUN_ccpy)

DEFINE_FTL_VEC_FUN(re, complex, float, FUN_re)
DEFINE_FTL_VEC_FUN(im, complex, float, FUN_im)

DEFINE_FTL_VEC_SET(fzero, float, FUN_zero)
DEFINE_FTL_VEC_SET(izero, int, FUN_zero)
DEFINE_FTL_VEC_SET(czero, complex, FUN_czero)

DEFINE_FTL_VEC_FUN(fcasti, float, int, FUN_cpy)
DEFINE_FTL_VEC_FUN(icastf, int, float, FUN_cpy)
