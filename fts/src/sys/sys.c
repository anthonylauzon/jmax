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
/*
  Sys level initialization 
  */

#include "sys.h"

extern void fts_platform_init(void);

void
fts_sys_init(void)
{
  fts_platform_init();

  mem_init();
}
