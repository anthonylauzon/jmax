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

DEFINE_FTS_VECX_FUN(exp, float, float, FUN_exp)
DEFINE_FTS_VECX_FUN(log, float, float, FUN_log)
DEFINE_FTS_VECX_FUN(log10, float, float, FUN_log10)

DEFINE_FTS_VECX_OPSCL(expb, float, float, float, FUN_expb)
DEFINE_FTS_VECX_OPSCL(logb, float, float, float, FUN_logb)



DEFINE_FTL_VECX_FUN(exp, float, float, FUN_exp)
DEFINE_FTL_VECX_FUN(log, float, float, FUN_log)
DEFINE_FTL_VECX_FUN(log10, float, float, FUN_log10)

DEFINE_FTL_VECX_OPSCL(expb, float, float, float, FUN_expb)
DEFINE_FTL_VECX_OPSCL(logb, float, float, float, FUN_logb)

