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

#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VEC_OPSCL(scl_fadd, float, float, float, OP_add)
DEFINE_FTS_VEC_OPSCL(scl_fsub, float, float, float, OP_sub)
DEFINE_FTS_VEC_OPSCL(scl_fmul, float, float, float, OP_mul)
DEFINE_FTS_VEC_OPSCL(scl_fdiv, float, float, float, OP_div)
DEFINE_FTS_VEC_OPSCL(scl_fbus, float, float, float, OP_bus)
DEFINE_FTS_VEC_OPSCL(scl_fvid, float, float, float, OP_vid)


DEFINE_FTL_VEC_OPSCL(scl_fadd, float, float, float, OP_add)
DEFINE_FTL_VEC_OPSCL(scl_fsub, float, float, float, OP_sub)
DEFINE_FTL_VEC_OPSCL(scl_fmul, float, float, float, OP_mul)
DEFINE_FTL_VEC_OPSCL(scl_fdiv, float, float, float, OP_div)
DEFINE_FTL_VEC_OPSCL(scl_fbus, float, float, float, OP_bus)
DEFINE_FTL_VEC_OPSCL(scl_fvid, float, float, float, OP_vid)
