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
#include "binops.h"
#include "data.h"

fts_symbol_t binops_s_recycle = 0;
fts_symbol_t binops_s_inplace = 0;

typedef struct _binop
{
  operator_fun_t recycle[MAX_N_DATA_TYPES][MAX_N_DATA_TYPES];
  operator_fun_t inplace[MAX_N_DATA_TYPES][MAX_N_DATA_TYPES];
} binop_t;

static fts_hash_table_t binop_table;

static binop_t *
binop_new(fts_symbol_t binop_name)
{
  fts_atom_t atom;
  binop_t *binop = (binop_t *)fts_malloc(sizeof(binop_t));
  int i, j;

  fts_set_ptr(&atom, binop);
  fts_hash_table_insert(&binop_table, binop_name, &atom);
  
  for(i=0; i<MAX_N_DATA_TYPES; i++)
    for(j=0; j<MAX_N_DATA_TYPES; j++)
      {
	binop->recycle[i][j] = 0;
	binop->inplace[i][j] = 0;
      }

  return binop;
}

void
binop_declare_fun(fts_symbol_t binop_name, operator_fun_t fun, fts_symbol_t left_type, fts_symbol_t right_type, fts_symbol_t version)
{
  fts_atom_t atom;
  operator_matrix_t funs;
  int left_id, right_id;
  binop_t *binop;

  if(fts_hash_table_lookup(&binop_table, binop_name, &atom))
    binop = fts_get_ptr(&atom);      
  else
    binop = binop_new(binop_name);

  if(version == binops_s_recycle)
    funs = binop->recycle;
  else if(version == binops_s_inplace)
    funs = binop->inplace;	
  else
    return;

  left_id = data_type_get_id(left_type);
  right_id = data_type_get_id(right_type);

  funs[left_id][right_id] = fun;
}

operator_matrix_t 
binop_get_matrix(fts_symbol_t binop_name, fts_symbol_t version)
{
  fts_atom_t atom;

  if(fts_hash_table_lookup(&binop_table, binop_name, &atom))
    {
      binop_t *binop = fts_get_ptr(&atom);
      
      if(version == binops_s_recycle)
	return binop->recycle;
      else if(version == binops_s_inplace)
	return binop->inplace;	
      else
	return 0;
    }
  else
    return 0;
}

void
binops_init(void)
{
  fts_hash_table_init(&binop_table);
  
  binops_s_recycle = fts_new_symbol("recycle");
  binops_s_inplace = fts_new_symbol("inplace");
}
