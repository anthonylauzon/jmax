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
#include "monops.h"
#include "operators.h"

fts_symbol_t monops_s_recycle = 0;
fts_symbol_t monops_s_inplace = 0;

static fts_hash_table_t monop_table;

static op_t *
monop_operand_type_error(op_t *ops)
{
  const char *type_str = fts_symbol_name(op_get_type_name(ops[0]));

  post("monop: %s: wrong operands (%s and %s)\n", type_str);

  return 0;
}

static monop_t *
monop_new(fts_symbol_t name)
{
  fts_atom_t atom;
  monop_t *monop = (monop_t *)fts_malloc(sizeof(monop_t));
  int i;

  fts_set_ptr(&atom, monop);
  fts_hash_table_insert(&monop_table, name, &atom);
  
  monop->name = name;

  for(i=0; i<MAX_N_DATA_TYPES; i++)
    {
      monop->recycle[i] = monop_operand_type_error;
      monop->inplace[i] = monop_operand_type_error;
    }

  return monop;
}

void
monop_declare_fun(fts_symbol_t name, operator_fun_t fun, fts_symbol_t type, fts_symbol_t version)
{
  fts_atom_t atom;
  operator_vector_t funs;
  int id;
  monop_t *monop;

  if(fts_hash_table_lookup(&monop_table, name, &atom))
    monop = fts_get_ptr(&atom);      
  else
    monop = monop_new(name);

  if(version == monops_s_recycle)
    funs = monop->recycle;
  else if(version == monops_s_inplace)
    funs = monop->inplace;	
  else
    return;

  id = data_type_get_id(type);

  funs[id] = fun;
}

monop_t *
monop_get(fts_symbol_t name)
{
  fts_atom_t atom;

  if(fts_hash_table_lookup(&monop_table, name, &atom))
    return fts_get_ptr(&atom);
  else
    return 0;
}

void
monops_init(void)
{
  fts_hash_table_init(&monop_table);
  
  monops_s_recycle = fts_new_symbol("recycle");
  monops_s_inplace = fts_new_symbol("inplace");
}
