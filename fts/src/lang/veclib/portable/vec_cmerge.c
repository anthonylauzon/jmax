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

/***********************************************
 *
 *    plain loop version
 *
 */
 
#define FTS_VEC_CMERGE(in0, in1, out) \
{\
  int i;\
  for(i=0; i<size; i++){\
    (out[i+0].re = in0[i+0], out[i+0].im = in1[i+0]);\
  }\
}


/***************************************************
 *
 *    function definition
 *
 */
 
void fts_vec_cmerge(float *in0, float *in1, complex *out, int size)
{
  FTS_VEC_CMERGE(in0, in1, out)
}
  
void ftl_vec_cmerge(FTL_ARG)
{
  UNWRAP_VEC(0, float, in0)
  UNWRAP_VEC(1, float, in1)
  UNWRAP_VEC(2, complex, out)
  UNWRAP_SIZE(3)
  FTS_VEC_CMERGE(in0, in1, out)
}

