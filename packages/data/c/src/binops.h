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
#ifndef _BINOPS_H_
#define _BINOPS_H_

#include "fts.h"
#include "types.h"
#include "operators.h"
#include "recycle.h"

extern fts_symbol_t binops_s_recycle;
extern fts_symbol_t binops_s_inplace;

extern void binop_declare_fun(fts_symbol_t binop_name, operator_fun_t fun, 
			      fts_symbol_t left_type, fts_symbol_t right_type, fts_symbol_t version);
extern operator_matrix_t binop_get_matrix(fts_symbol_t binop_name, fts_symbol_t version);

#endif
