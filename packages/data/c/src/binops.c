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
#include "fts.h"
#include "data.h"
#include "binops.h"
#include "operators.h"

static fts_hash_table_t binop_table;

static op_t *
binop_operand_type_error(op_t *ops)
{
  const char *left_type_str = fts_symbol_name(op_get_type_name(ops[0]));
  const char *right_type_str = fts_symbol_name(op_get_type_name(ops[1]));

  post("binop: wrong operands (%s and %s)\n", left_type_str, right_type_str);

  return 0;
}

static binop_t *
binop_new(fts_symbol_t name)
{
  fts_atom_t atom;
  binop_t *binop = (binop_t *)fts_malloc(sizeof(binop_t));
  int i, j;

  fts_set_ptr(&atom, binop);
  fts_hash_table_insert(&binop_table, name, &atom);
  
  binop->name = name;

  for(i=0; i<MAX_N_DATA_TYPES; i++)
    for(j=0; j<MAX_N_DATA_TYPES; j++)
      {
	binop->recycle[i][j] = binop_operand_type_error;
	binop->inplace[i][j] = binop_operand_type_error;
      }

  return binop;
}

void
binop_declare_fun(fts_symbol_t name, operator_fun_t fun, fts_symbol_t left_type, fts_symbol_t right_type, fts_symbol_t version)
{
  fts_atom_t atom;
  operator_matrix_t funs;
  int left_id, right_id;
  binop_t *binop;

  if(fts_hash_table_lookup(&binop_table, name, &atom))
    binop = fts_get_ptr(&atom);      
  else
    binop = binop_new(name);

  if(version == op_s_recycle)
    funs = binop->recycle;
  else if(version == op_s_inplace)
    funs = binop->inplace;	
  else
    return;

  left_id = data_type_get_id_by_name(left_type);
  right_id = data_type_get_id_by_name(right_type);

  funs[left_id][right_id] = fun;
}

binop_t *
binop_get(fts_symbol_t name)
{
  fts_atom_t atom;

  if(fts_hash_table_lookup(&binop_table, name, &atom))
    return fts_get_ptr(&atom);
  else
    return 0;
}

void
binops_init(void)
{
  fts_hash_table_init(&binop_table);
}



