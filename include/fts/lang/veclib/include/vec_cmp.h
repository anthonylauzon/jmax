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

/* veccomp.h */

#include <fts/lang/veclib/include/vecdef.h>


/**********************************************************
 *
 *		vector/vector comparisons
 *
 */

/* veccomp_float.c */
DECLARE_VEC_FUN(OP, (fcmp_eq, float, float, int, COMP_eq))
DECLARE_VEC_FUN(OP, (fcmp_neq, float, float, int, COMP_neq))
DECLARE_VEC_FUN(OP, (fcmp_leq, float, float, int, COMP_leq))
DECLARE_VEC_FUN(OP, (fcmp_geq, float, float, int, COMP_geq))
DECLARE_VEC_FUN(OP, (fcmp_le, float, float, int, COMP_le))
DECLARE_VEC_FUN(OP, (fcmp_gr, float, float, int, COMP_gr))
 
/* veccomp_int.c */
DECLARE_VEC_FUN(OP, (icmp_eq, int, int, int, COMP_eq))
DECLARE_VEC_FUN(OP, (icmp_neq, int, int, int, COMP_neq))
DECLARE_VEC_FUN(OP, (icmp_leq, int, int, int, COMP_leq))
DECLARE_VEC_FUN(OP, (icmp_geq, int, int, int, COMP_geq))
DECLARE_VEC_FUN(OP, (icmp_le, int, int, int, COMP_le))
DECLARE_VEC_FUN(OP, (icmp_gr, int, int, int, COMP_gr))


/**********************************************************
 *
 *		vector/scalar comparisons
 *
 */

/* veccomp_float.c */
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_eq, float, float, int, COMP_eq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_neq, float, float, int, COMP_neq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_leq, float, float, int, COMP_leq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_geq, float, float, int, COMP_geq))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_le, float, float, int, COMP_le))
DECLARE_VEC_FUN(OPSCL, (scl_fcmp_gr, float, float, int, COMP_gr))
 
/* veccomp_int.c */
DECLARE_VEC_FUN(OPSCL, (scl_icmp_eq, int, int, int, COMP_eq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_neq, int, int, int, COMP_neq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_leq, int, int, int, COMP_leq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_geq, int, int, int, COMP_geq))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_le, int, int, int, COMP_le))
DECLARE_VEC_FUN(OPSCL, (scl_icmp_gr, int, int, int, COMP_gr))

