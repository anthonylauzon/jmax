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


/* ftlfun.c */

#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"
#include "lang/dsp.h"
#include "lang/veclib/ftlfun.h"

/* just vec so far */
/* just unrolled so far */
/* just float so far */
#define DSP_DECLARE_ALL_TYPE_OP(fun_name, sym_name) \
{\
  fts_symbol_t the_symbol;\
  char the_string[25];\
  sprintf(the_string, "vec_f%s", sym_name);\
  the_symbol = fts_new_symbol_copy(the_string);\
  ftl_sym.fun_name.f.vec.vec = the_symbol;\
  dsp_declare_function(the_symbol, ftl_vecx_f ## fun_name);\
  sprintf(the_string, "vec_scl_f%s", sym_name);\
  the_symbol = fts_new_symbol_copy(the_string);\
  ftl_sym.fun_name.f.vec.scl = the_symbol;\
  dsp_declare_function(the_symbol, ftl_vecx_scl_f ## fun_name);\
}

/* just vec so far */
/* just unrolled so far */
/* just float so far */
#define DSP_DECLARE_ALL_TYPE_FUN(fun_name, sym_name) \
{\
  fts_symbol_t the_symbol;\
  char the_string[25];\
  sprintf(the_string, "vec_f%s", sym_name);\
  the_symbol = fts_new_symbol_copy(the_string);\
  ftl_sym.fun_name.f.vec = the_symbol;\
  dsp_declare_function(the_symbol, ftl_vecx_f ## fun_name);\
}

/* anyway just for vectors */
/* just unrolled so far */
/* just float so far */
#define DSP_DECLARE_ALL_TYPE_VEC_FUN(fun_name, sym_name) \
{\
  fts_symbol_t the_symbol;\
  char the_string[25];\
  sprintf(the_string, "vec_f%s", sym_name);\
  the_symbol = fts_new_symbol_copy(the_string);\
  ftl_sym.fun_name.f = the_symbol;\
  dsp_declare_function(the_symbol, ftl_vecx_f ## fun_name);\
}

/* just vec so far */
/* just unrolled so far */
/* also just float, but these do not have sure the "f" in the name */
#define DSP_DECLARE_ONE_TYPE_FUN(fun_name, sym_name) \
{\
  fts_symbol_t the_symbol;\
  char the_string[25];\
  sprintf(the_string, "vec_%s", sym_name);\
  the_symbol = fts_new_symbol_copy(the_string);\
  ftl_sym.fun_name.vec = the_symbol;\
  dsp_declare_function(the_symbol, ftl_vecx_ ## fun_name);\
}

ftl_function_symbols_t ftl_sym;

void ftl_functions_init(void)
{  
  DSP_DECLARE_ALL_TYPE_OP(add, "add")
  DSP_DECLARE_ALL_TYPE_OP(sub, "sub")
  DSP_DECLARE_ALL_TYPE_OP(mul, "mul")
  DSP_DECLARE_ALL_TYPE_OP(div, "div")
  DSP_DECLARE_ALL_TYPE_OP(bus, "bus")
  DSP_DECLARE_ALL_TYPE_OP(vid, "vid")

  DSP_DECLARE_ALL_TYPE_FUN(clip, "clip")
  DSP_DECLARE_ALL_TYPE_FUN(abs, "abs")
  
  DSP_DECLARE_ALL_TYPE_VEC_FUN(fill, "fill")
  DSP_DECLARE_ALL_TYPE_VEC_FUN(zero, "zero")
  DSP_DECLARE_ALL_TYPE_VEC_FUN(cpy, "cpy")

  DSP_DECLARE_ONE_TYPE_FUN(sin, "sin")
  DSP_DECLARE_ONE_TYPE_FUN(cos, "cos")
  DSP_DECLARE_ONE_TYPE_FUN(tan, "tan")
  DSP_DECLARE_ONE_TYPE_FUN(asin, "asin")
  DSP_DECLARE_ONE_TYPE_FUN(acos, "acos")
  DSP_DECLARE_ONE_TYPE_FUN(atan, "atan")
  DSP_DECLARE_ONE_TYPE_FUN(sinh, "sinh")
  DSP_DECLARE_ONE_TYPE_FUN(cosh, "cosh")
  DSP_DECLARE_ONE_TYPE_FUN(tanh, "tanh")
  
  DSP_DECLARE_ONE_TYPE_FUN(exp, "exp")
  DSP_DECLARE_ONE_TYPE_FUN(log, "log")
  DSP_DECLARE_ONE_TYPE_FUN(log10, "log10")
  DSP_DECLARE_ONE_TYPE_FUN(expb, "expb")
  DSP_DECLARE_ONE_TYPE_FUN(logb, "logb")
  
  DSP_DECLARE_ONE_TYPE_FUN(sqrt, "sqrt")
  DSP_DECLARE_ONE_TYPE_FUN(rsqr, "rsqr")
  
  DSP_DECLARE_ALL_TYPE_FUN(abs, "abs")

#ifndef LINUXPC  
  DSP_DECLARE_ONE_TYPE_FUN(cabsf, "cabsf");
  DSP_DECLARE_ONE_TYPE_FUN(csqrf, "csqrf");
#endif

  DSP_DECLARE_ONE_TYPE_FUN(ffillre, "ffillre");
  DSP_DECLARE_ONE_TYPE_FUN(ffillim, "ffillim");
  DSP_DECLARE_ONE_TYPE_FUN(re, "re");
  DSP_DECLARE_ONE_TYPE_FUN(im, "im");
  DSP_DECLARE_ONE_TYPE_FUN(fcpyre, "fcpyre");
  DSP_DECLARE_ONE_TYPE_FUN(fcpyim, "fcpyim");
  DSP_DECLARE_ONE_TYPE_FUN(csplit, "csplit");
  DSP_DECLARE_ONE_TYPE_FUN(cmerge, "cmerge");
}






