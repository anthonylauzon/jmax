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

#include "sys.h"
#include "lang/veclib/include/vecdef.h"

DEFINE_FTS_VEC_OPSCL(scl_cadd, complex, complex, complex, OP_cadd)
DEFINE_FTS_VEC_OPSCL(scl_csub, complex, complex, complex, OP_csub)
DEFINE_FTS_VEC_OPSCL(scl_cmul, complex, complex, complex, OP_cmul)
DEFINE_FTS_VEC_OPSCL(scl_cdiv, complex, complex, complex, OP_cdiv)
DEFINE_FTS_VEC_OPSCL(scl_cbus, complex, complex, complex, OP_cbus)
DEFINE_FTS_VEC_OPSCL(scl_cvid, complex, complex, complex, OP_cvid)


DEFINE_FTL_VEC_OPSCL(scl_cadd, complex, complex, complex, OP_cadd)
DEFINE_FTL_VEC_OPSCL(scl_csub, complex, complex, complex, OP_csub)
DEFINE_FTL_VEC_OPSCL(scl_cmul, complex, complex, complex, OP_cmul)
DEFINE_FTL_VEC_OPSCL(scl_cdiv, complex, complex, complex, OP_cdiv)
DEFINE_FTL_VEC_OPSCL(scl_cbus, complex, complex, complex, OP_cbus)
DEFINE_FTL_VEC_OPSCL(scl_cvid, complex, complex, complex, OP_cvid)
