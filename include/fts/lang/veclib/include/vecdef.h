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

/* vecdef.h */

#ifndef _VECDEF_H_
#define _VECDEF_H_

#include <math.h>
#include <fts/sys.h>
#include <fts/lang/mess.h>
#include <fts/lang/veclib/complex.h>

/***************************************************
 *
 *    function kernels
 *       op: two operant functions
 *       fun: vector -> vector transform functions
 *       misc: miscellanous functions
 */

#include <fts/lang/veclib/include/kern_op.h>
#include <fts/lang/veclib/include/kern_fun.h>
#include <fts/lang/veclib/include/kern_misc.h>

/***************************************************
 *
 *    function definitions
 *  
 *      DEFINE_FTS_VEC_ ## class ## (definition arguments)
 *      DEFINE_FTS_VECX_ ## class ## (definition arguments)
 *      DEFINE_FTL_VEC_ ## class ## (definition arguments)
 *      DEFINE_FTL_VECX_ ## class ## (definition arguments)
 *
 *    function declarations
 *
 *      DECLARE_FTS_VEC_ ## class ## (definition arguments)
 *      DECLARE_FTS_VECX_ ## class ## (definition arguments)
 *      DECLARE_FTL_VEC_ ## class ## (definition arguments)
 *      DECLARE_FTL_VECX_ ## class ## (definition arguments)
 *
 *    resulting functions
 *
 *      void fts_vec_ ## name ## (function arguments)
 *      void fts_vecx_ ## name ## (function arguments)
 *      void ftl_vec_ ## name ## (fts_word_t *argv)
 *      void ftl_vecx_ ## name ## (fts_word_t *argv)
 *
 *
 ***************************************************
 *
 *    function marcos
 *
 *      FTS_VEC_ ## class ## (definition arguments)
 *      FTS_VECX_ ## class ## (definition arguments)
 *      FTL_VEC_ ## class ## (definition arguments)
 *      FTL_VECX_ ## class ## (definition arguments)
 *
 *
 ***************************************************
 *
 *    classes and arguments
 *
 *      OP: (name, t_in0, t_in1, t_out, op) ... vector vector -> vector
 *      COP: (name, t_in0, t_in1, t_out, op) ... complex: vector vector -> vector
 *      DOT: (name, t_in0, t_in1, t_ptr, op, init) ... vector vector -> scalar
 *      OPSCL: (name, t_in, t_scl, t_out, op) ... vector scalar -> vector
 *      TWOSCL: (name, t_in, t_scl0, t_scl1, t_out, op) ... vector scalar scalar -> vector
 *
 *      FUN: (name, t_in, t_out, fun) ... vector -> vector
 *      CFUN: (name, t_in, t_out, fun) ... complex: vector -> vector
 *      VECSCL: (name, t_in, t_ptr, op, init) ... vector -> scalar
 *      FILL: (name, t_scl, t_out, fun) ... scalar -> vector
 *      SET: (name, t_out, fun) ... -> vector
 *
 *        t_in, t_in0, t_in1 ... type of vector argument (float, int, complex, ... - no "*" for pointer!!)
 *        t_out, t_out0, t_out1 ... type of vector result (float, int, complex, ... - no "*" for pointer!!)
 *        t_scl, t_scl0, t_scl1 ... type of scalar argument
 *        t_ptr ... type of scalar result
 *        fun, op ... defined functions and operations (see all.h)
 *        init ... initialization value for scalar
 *          (set "scl" for no initialization or "*ptr" for init by result scalar fun arg)
 *
 */

#include <fts/lang/veclib/include/def_op.h>
#include <fts/lang/veclib/include/def_cop.h>
#include <fts/lang/veclib/include/def_dot.h>
#include <fts/lang/veclib/include/def_opscl.h>
#include <fts/lang/veclib/include/def_twoscl.h>
#include <fts/lang/veclib/include/def_fun.h>
#include <fts/lang/veclib/include/def_cfun.h>
#include <fts/lang/veclib/include/def_vecscl.h>
#include <fts/lang/veclib/include/def_fill.h>
#include <fts/lang/veclib/include/def_set.h>

#endif /* _VECDEF_H_ */



