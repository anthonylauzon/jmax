/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/* vecdef.h */

#ifndef _VECDEF_H_
#define _VECDEF_H_

#include <math.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/veclib/complex.h"

/***************************************************
 *
 *    function kernels
 *       op: two operant functions
 *       fun: vector -> vector transform functions
 *       misc: miscellanous functions
 */

#include "lang/veclib/include/kern_op.h"
#include "lang/veclib/include/kern_fun.h"
#include "lang/veclib/include/kern_misc.h"

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

#include "lang/veclib/include/def_op.h"
#include "lang/veclib/include/def_cop.h"
#include "lang/veclib/include/def_dot.h"
#include "lang/veclib/include/def_opscl.h"
#include "lang/veclib/include/def_twoscl.h"
#include "lang/veclib/include/def_fun.h"
#include "lang/veclib/include/def_cfun.h"
#include "lang/veclib/include/def_vecscl.h"
#include "lang/veclib/include/def_fill.h"
#include "lang/veclib/include/def_set.h"

#endif /* _VECDEF_H_ */




