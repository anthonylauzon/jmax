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

/* define/fill.h */

#include <fts/lang/veclib/include/unroll.h>
#include <fts/lang/veclib/include/unwrap.h>


/***************************************************
 *
 *    scalar vector conversion
 *
 *      SCALAR -> VECTOR
 *      no explicit use of temporary variables
 *
 */
 
/***************************************************
 *
 *    unrolling
 *
 */
 
#define NO_UNROLL_FILL(fun) fun(scl, out[i+0]);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_FILL(fun)\
  fun(scl, out[i+0]);\
  fun(scl, out[i+1]);\
  fun(scl, out[i+2]);\
  fun(scl, out[i+3]);

#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_FILL(fun)\
  fun(scl, out[i+0]);\
  fun(scl, out[i+1]);\
  fun(scl, out[i+2]);\
  fun(scl, out[i+3]);\
  fun(scl, out[i+4]);\
  fun(scl, out[i+5]);\
  fun(scl, out[i+6]);\
  fun(scl, out[i+7]);

#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_FILL(fun)\
  fun(scl, out[i+0]);\
  fun(scl, out[i+1]);\
  fun(scl, out[i+2]);\
  fun(scl, out[i+3]);\
  fun(scl, out[i+4]);\
  fun(scl, out[i+5]);\
  fun(scl, out[i+6]);\
  fun(scl, out[i+7]);\
  fun(scl, out[i+8]);\
  fun(scl, out[i+9]);\
  fun(scl, out[i+10]);\
  fun(scl, out[i+11]);\
  fun(scl, out[i+12]);\
  fun(scl, out[i+13]);\
  fun(scl, out[i+14]);\
  fun(scl, out[i+15]);
 
#else

#define UNROLL_FILL(fun) NO_UNROLL_FILL(fun)

#endif


/***************************************************
 *
 *    function bodies
 *
 */
 
#define FTS_VEC_FILL(t_scl, t_out, fun)\
 NO_UNROLL_LOOP(\
  NO_UNROLL_FILL(fun)\
 )

#define FTS_VECX_FILL(t_scl, t_out, fun)\
 UNROLL_LOOP(\
  UNROLL_FILL(fun)\
 )

#define FTL_UNWRAP_FILL(t_scl, t_out)\
 UNWRAP_SCL(0, t_scl, scl)\
 UNWRAP_VEC(1, t_out, out)\
 UNWRAP_SIZE(2)


/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_FILL(name, t_scl, t_out, fun)\
 void fts_vec_ ## name(t_scl scl, t_out *out, int size)\
 {FTS_VEC_FILL(t_scl, t_out, fun)}
  
#define DEFINE_FTS_VECX_FILL(name, t_scl, t_out, fun)\
 void fts_vecx_ ## name(t_scl scl, t_out *out, int size)\
 {FTS_VECX_FILL(t_scl, t_out, fun)}

#define DEFINE_FTL_VEC_FILL(name, t_scl, t_out, fun)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_FILL(t_scl, t_out)\
  FTS_VEC_FILL(t_scl, t_out, fun)\
 }

#define DEFINE_FTL_VECX_FILL(name, t_scl, t_out, fun)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_FILL(t_scl, t_out)\
  FTS_VECX_FILL(t_scl, t_out, fun)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_FILL(name, t_scl, t_out, fun)\
 extern void fts_vec_ ## name(t_scl scl, t_out *out, int size);
 
#define DECLARE_FTS_VECX_FILL(name, t_scl, t_out, fun)\
 extern void fts_vecx_ ## name(t_scl scl, t_out *out, int size);
 
#define DECLARE_FTL_VEC_FILL(name, t_scl, t_out, fun)\
 extern void ftl_vec_ ## name(FTL_ARG);

#define DECLARE_FTL_VECX_FILL(name, t_scl, t_out, fun)\
 extern void ftl_vecx_ ## name(FTL_ARG);

