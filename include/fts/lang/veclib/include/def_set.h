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

/* define/set.h */

#include <fts/lang/veclib/include/unroll.h>
#include <fts/lang/veclib/include/unwrap.h>



/***************************************************
 *
 *    no input, outputs vector
 *
 *      -> VECTOR
 *
 */
 
/***************************************************
 *
 *    unrolling
 *
 */

#define NO_UNROLL_SET(fun) fun(out[i+0]);

#if VECLIB_LOOP_UNROLL == 4

#define UNROLL_SET(fun)\
  fun(out[i+0]);\
  fun(out[i+1]);\
  fun(out[i+2]);\
  fun(out[i+3]);

#elif VECLIB_LOOP_UNROLL == 8

#define UNROLL_SET(fun)\
  fun(out[i+0]);\
  fun(out[i+1]);\
  fun(out[i+2]);\
  fun(out[i+3]);\
  fun(out[i+4]);\
  fun(out[i+5]);\
  fun(out[i+6]);\
  fun(out[i+7]);

#elif VECLIB_LOOP_UNROLL == 16

#define UNROLL_SET(fun)\
  fun(out[i+0]);\
  fun(out[i+1]);\
  fun(out[i+2]);\
  fun(out[i+3]);\
  fun(out[i+4]);\
  fun(out[i+5]);\
  fun(out[i+6]);\
  fun(out[i+7]);\
  fun(out[i+8]);\
  fun(out[i+9]);\
  fun(out[i+10]);\
  fun(out[i+11]);\
  fun(out[i+12]);\
  fun(out[i+13]);\
  fun(out[i+14]);\
  fun(out[i+15]);
 
#else

#define UNROLL_SET(fun) NO_UNROLL_SET(fun)

#endif


/***************************************************
 *
 *    function bodies
 *
 */
 
#define FTS_VEC_SET(t_out, fun)\
 NO_UNROLL_LOOP(\
  NO_UNROLL_SET(fun)\
 )

#define FTS_VECX_SET(t_out, fun)\
 UNROLL_LOOP(\
  UNROLL_SET(fun)\
 )

#define FTL_UNWRAP_SET(t_out)\
 UNWRAP_VEC(0, t_out, out)\
 UNWRAP_SIZE(1)


/***************************************************
 *
 *    function definition
 *
 */
 
#define DEFINE_FTS_VEC_SET(name, t_out, fun)\
 void fts_vec_ ## name(t_out *out, int size)\
 {FTS_VEC_SET(t_out, fun)}
  
#define DEFINE_FTS_VECX_SET(name, t_out, fun)\
 void fts_vecx_ ## name(t_out *out, int size)\
 {FTS_VECX_SET(t_out, fun)}

#define DEFINE_FTL_VEC_SET(name, t_out, fun)\
 void ftl_vec_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_SET(t_out)\
  FTS_VEC_SET(t_out, fun)\
 }

#define DEFINE_FTL_VECX_SET(name, t_out, fun)\
 void ftl_vecx_ ## name(FTL_ARG)\
 {\
  FTL_UNWRAP_SET(t_out)\
  FTS_VECX_SET(t_out, fun)\
 }


/***************************************************
 *
 *    function declaration
 *
 */
 
#define DECLARE_FTS_VEC_SET(name, t_out, fun)\
 FTS_API void fts_vec_ ## name(t_out *out, int size);
 
#define DECLARE_FTS_VECX_SET(name, t_out, fun)\
 FTS_API void fts_vecx_ ## name(t_out *out, int size);
 
#define DECLARE_FTL_VEC_SET(name, t_out, fun)\
 FTS_API void ftl_vec_ ## name(FTL_ARG);

#define DECLARE_FTL_VECX_SET(name, t_out, fun)\
 FTS_API void ftl_vecx_ ## name(FTL_ARG);
