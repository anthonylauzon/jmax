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

/* vecbit.h */

#include "lang/veclib/include/vecdef.h"


/**********************************************************
 *
 *		logic inversion
 *
 */

DECLARE_VEC_FUN(FUN, (bitnot, int, int, FUN_bitnot))


/**********************************************************
 *
 *		vector/vector logic and shift
 *
 */

DECLARE_VEC_FUN(OP, (bitand, int, int, int, OP_bitand))
DECLARE_VEC_FUN(OP, (bitor, int, int, int, OP_bitor))
DECLARE_VEC_FUN(OP, (bitxor, int, int, int, OP_bitxor))

DECLARE_VEC_FUN(OP, (bitshl, int, int, int, OP_bitshl))
DECLARE_VEC_FUN(OP, (bitshr, int, int, int, OP_bitshr))


/**********************************************************
 *
 *		vector/scalar logic and shift
 *
 */

DECLARE_VEC_FUN(OPSCL, (scl_bitand, int, int, int, OP_bitand))
DECLARE_VEC_FUN(OPSCL, (scl_bitor, int, int, int, OP_bitor))
DECLARE_VEC_FUN(OPSCL, (scl_bitxor, int, int, int, OP_bitxor))

DECLARE_VEC_FUN(OPSCL, (scl_bitshl, int, int, int, OP_bitshl))
DECLARE_VEC_FUN(OPSCL, (scl_bitshr, int, int, int, OP_bitshr))
