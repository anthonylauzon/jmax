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
#ifndef _MONOPS_H_
#define _MONOPS_H_

#include "fts.h"
#include "types.h"
#include "operators.h"

extern fts_symbol_t monops_s_recycle;
extern fts_symbol_t monops_s_inplace;

typedef struct _monop
{
  fts_symbol_t name;
  operator_fun_t recycle[MAX_N_DATA_TYPES];
  operator_fun_t inplace[MAX_N_DATA_TYPES];
} monop_t;

extern void monop_declare_fun(fts_symbol_t monop, operator_fun_t fun, fts_symbol_t type, fts_symbol_t version);
extern monop_t *monop_get(fts_symbol_t name);

#define monop_call_fun_recycle(monop, ops) ((monop)->recycle[(ops)[0].type.id])(ops)
#define monop_call_fun_inplace(monop, ops) ((monop)->inplace[(ops)[0].type.id])(ops)

#endif
