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

typedef struct _binop
{
  fts_symbol_t name;
  operator_fun_t recycle[MAX_N_DATA_TYPES][MAX_N_DATA_TYPES];
  operator_fun_t inplace[MAX_N_DATA_TYPES][MAX_N_DATA_TYPES];
} binop_t;

extern void binop_declare_fun(fts_symbol_t binop, operator_fun_t fun, fts_symbol_t left, fts_symbol_t right, fts_symbol_t version);
extern binop_t *binop_get(fts_symbol_t name);

#define binop_call_fun_recycle(binop, ops) ((binop)->recycle[(ops)[0].type.id][(ops)[1].type.id])(ops)
#define binop_call_fun_inplace(binop, ops) ((binop)->inplace[(ops)[0].type.id][(ops)[1].type.id])(ops)

#endif
