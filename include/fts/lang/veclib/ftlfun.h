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


/* ftlfun.h */

#ifndef _FTS_FTLFUN_H_
#define _FTS_FTLFUN_H_

#include <fts/sys.h>
#include <fts/lang/mess.h>
#include <fts/lang/veclib/include/vecftl.h>

/*********************************************************************************
 *
 *  FTL vector function classes: (<loop/unrolled> not yet implemented - just like lots of other things)
 *
 *    ftl_all_type_fun_t
 *      <var>.<fun name>.<f/i/c>.<vec>(.<loop/unrolled>)
 *      <var>.<fun name>.<f/i/c>.<scl>
 *
 *    ftl_all_type_op_t
 *      <var>.<fun name>.<f/i/c>.<vec>.<vec/scl>(.<loop/unrolled>)
 *      <var>.<fun name>.<f/i/c>.<scl>
 *
 *    ftl_all_type_vec_fun_t
 *      <var>.<fun name>.<f/i/c>(.<loop/unrolled>)
 *
 *    ftl_one_type_fun_t
 *      <var>.<fun name>.<vec>(.<loop/unrolled>)
 *      <var>.<fun name>.<scl>
 *
 *    ftl_one_type_op_t
 *      <var>.<fun name>.<vec>.<vec/scl>(.<loop/unrolled>)
 *      <var>.<fun name>.<scl>
 *
 *    ftl_one_type_vec_fun_t
 *      <var>.<fun name>(.<loop/unrolled>)
 *
 */

/***********************************************************************
 *
 *    basic function types
 *
 */

typedef fts_symbol_t ftl_vec_fun_t;

typedef struct{
  ftl_vec_fun_t vec; /* vector/vector conjunction */
  ftl_vec_fun_t scl; /* vector/scalar conjunction */
} ftl_vec_op_t;

typedef fts_symbol_t ftl_scl_fun_t;
typedef fts_symbol_t ftl_scl_op_t;

typedef struct{
  ftl_vec_op_t vec;
} ftl_vec_and_scl_op_t;

typedef struct{
  ftl_vec_fun_t vec;
} ftl_vec_and_scl_fun_t;


/***********************************************************************
 *
 *    function classes
 *
 */
 
/* multi type function classes */

typedef struct{
  ftl_vec_and_scl_fun_t f;
} ftl_all_type_fun_t;

typedef struct{
  ftl_vec_and_scl_op_t f;
} ftl_all_type_op_t;

typedef struct{
  ftl_vec_fun_t f;
} ftl_all_type_vec_fun_t;

/* single type function classes */

typedef ftl_vec_and_scl_fun_t ftl_one_type_fun_t;
typedef ftl_vec_and_scl_op_t ftl_one_type_op_t;
typedef ftl_vec_fun_t ftl_one_type_vec_fun_t;


/***********************************************************************
 *
 *    the big mother of all functions
 *
 */
 
typedef struct{
  ftl_all_type_op_t add;
  ftl_all_type_op_t sub;
  ftl_all_type_op_t mul;
  ftl_all_type_op_t div;
  ftl_all_type_op_t bus;
  ftl_all_type_op_t vid;

  ftl_all_type_op_t eq; /* never complex */
  ftl_all_type_op_t neq; /* never complex */
  ftl_all_type_op_t leq; /* never complex */
  ftl_all_type_op_t geq; /* never complex */
  ftl_all_type_op_t lt; /* never complex */
  ftl_all_type_op_t gt; /* never complex */

  ftl_one_type_fun_t sin;
  ftl_one_type_fun_t cos;
  ftl_one_type_fun_t tan;
  ftl_one_type_fun_t asin;
  ftl_one_type_fun_t acos;
  ftl_one_type_fun_t atan;
  ftl_one_type_fun_t sinh;
  ftl_one_type_fun_t cosh;
  ftl_one_type_fun_t tanh;
  
  ftl_one_type_fun_t exp;
  ftl_one_type_fun_t log;
  ftl_one_type_fun_t log10;
  ftl_one_type_fun_t expb;
  ftl_one_type_fun_t logb;
  
  ftl_one_type_fun_t sqrt;
  ftl_one_type_fun_t rsqr;
} ftl_function_symbols_t;

FTS_API ftl_function_symbols_t ftl_sym;

#endif /* _FTLFUN_H_ */
