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

DEFINE_FTS_VEC_OPSCL(scl_fcmp_eq, float, float, int, COMP_eq)
DEFINE_FTS_VEC_OPSCL(scl_fcmp_neq, float, float, int, COMP_neq)
DEFINE_FTS_VEC_OPSCL(scl_fcmp_leq, float, float, int, COMP_leq)
DEFINE_FTS_VEC_OPSCL(scl_fcmp_geq, float, float, int, COMP_geq)
DEFINE_FTS_VEC_OPSCL(scl_fcmp_lt, float, float, int, COMP_lt)
DEFINE_FTS_VEC_OPSCL(scl_fcmp_gt, float, float, int, COMP_gt)


DEFINE_FTL_VEC_OPSCL(scl_fcmp_eq, float, float, int, COMP_eq)
DEFINE_FTL_VEC_OPSCL(scl_fcmp_neq, float, float, int, COMP_neq)
DEFINE_FTL_VEC_OPSCL(scl_fcmp_leq, float, float, int, COMP_leq)
DEFINE_FTL_VEC_OPSCL(scl_fcmp_geq, float, float, int, COMP_geq)
DEFINE_FTL_VEC_OPSCL(scl_fcmp_lt, float, float, int, COMP_lt)
DEFINE_FTL_VEC_OPSCL(scl_fcmp_gt, float, float, int, COMP_gt)
