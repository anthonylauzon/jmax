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

extern void accum_config(void);
extern void bag_config(void);
extern void float_config(void);
extern void symbol_obj_config(void);
extern void funbuff_config(void);
extern void integer_config(void);
extern void pbank_config(void);
extern void table_config(void);
extern void value_config(void);
extern void expr_config(void);
extern void expr_doctor_init(void);

static void
data_module_init(void)
{
  accum_config();
  bag_config();
  float_config();
  integer_config();
  symbol_obj_config();
  funbuff_config();
  pbank_config();
  table_config();
  value_config();

  /* Note: expr is here because it needs to be in the same
     .so library than table

     The expr doctor evaluating new expressions.
     */

  expr_config();
  expr_doctor_init();
}

fts_module_t data_module = {"data", "ISPW data classes", data_module_init};
