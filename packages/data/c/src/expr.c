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

static int
data_expr_new(int ac, const fts_atom_t *at, fts_atom_t *result)
{
  fts_symbol_t class_name = fts_get_symbol(at + 1);
  fts_data_t *data = data_create(class_name, ac - 2, at + 2);

  fts_data_set_const(data);

  if(data)
    {
      fts_set_data(result, data);
      return FTS_EXPRESSION_OK;
    }
  else
    return FTS_EXPRESSION_SYNTAX_ERROR;
}

void
data_expr_init(void)
{
  fts_expression_declare_fun(fts_new_symbol("new"), data_expr_new);
}
