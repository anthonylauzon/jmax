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

extern void itab_config(void);
extern void ftab_config(void);
extern void nsplit_config(void);
extern void wrap_config(void);
extern void rewrap_config(void);

static void
numeric_init(void)
{
  itab_config();
  ftab_config();
  nsplit_config();
  wrap_config();
  rewrap_config();
}

fts_module_t numeric_module = {"numeric", "numeric classes", numeric_init};

