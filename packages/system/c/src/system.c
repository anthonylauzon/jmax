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

extern void ucs_config(void);
extern void dsp_config(void);
extern void arch_config(void);

extern var_config(void);
extern argument_init(void);

static void
system_module_init(void)
{
  ucs_config();
  dsp_config();
  arch_config();

  var_config();
  argument_init();
}

fts_module_t system_module = {"system", "Basic System Objects", system_module_init};
