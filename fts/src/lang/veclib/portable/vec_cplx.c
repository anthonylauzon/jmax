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

DEFINE_FTS_VEC_CFUN(cabs, complex, complex, FUN_cabs)
DEFINE_FTS_VEC_CFUN(conj, complex, complex, FUN_conj)
DEFINE_FTS_VEC_CFUN(csqr, complex, complex, FUN_csqr)
DEFINE_FTS_VEC_CFUN(cpolar, complex, complex, FUN_cpolar)
DEFINE_FTS_VEC_CFUN(crect, complex, complex, FUN_crect)

DEFINE_FTS_VEC_CFUN(cabsf, complex, float, FUN_cabsf)
DEFINE_FTS_VEC_CFUN(csqrf, complex, float, FUN_csqrf)



DEFINE_FTL_VEC_CFUN(cabs, complex, complex, FUN_cabs)
DEFINE_FTL_VEC_CFUN(conj, complex, complex, FUN_conj)
DEFINE_FTL_VEC_CFUN(csqr, complex, complex, FUN_csqr)
DEFINE_FTL_VEC_CFUN(cpolar, complex, complex, FUN_cpolar)
DEFINE_FTL_VEC_CFUN(crect, complex, complex, FUN_crect)

DEFINE_FTL_VEC_CFUN(cabsf, complex, float, FUN_cabsf)
DEFINE_FTL_VEC_CFUN(csqrf, complex, float, FUN_csqrf)


