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

/* extern void profiler_config(void); */
extern void hfpe_config(void);

static void tools_init(void)
{
  /* profiler_config(); */
  hfpe_config();
}

fts_module_t tools_module = { "tools", "tools module", tools_init};
