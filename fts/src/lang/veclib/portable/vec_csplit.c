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

/***************************************************
 *
 *    function body
 *
 */

#define FTS_VEC_CSPLIT(in, out0, out1) \
{\
  int i;\
  for(i=0; i<size; i++){\
    out0[i] = in[i].re;\
    out1[i] = in[i].im;\
  }\
}

/***************************************************
 *
 *    function definition
 *
 */
 
void fts_vec_csplit(complex *in, float *out0, float *out1, int size)
{
  FTS_VEC_CSPLIT(in, out0, out1)
}
  
void ftl_vec_csplit(FTL_ARG)
{
  UNWRAP_VEC(0, complex, in)
  UNWRAP_VEC(1, float, out0)
  UNWRAP_VEC(2, float, out1)
  UNWRAP_SIZE(3)
  FTS_VEC_CSPLIT(in, out0, out1)
}
