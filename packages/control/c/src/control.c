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

extern void bus_config(void);
extern void throw_config(void);
extern void catch_config(void);
extern void switch_config(void);
extern void demux_config(void);

static void
control_init(void)
{
  bus_config();
  throw_config();
  catch_config();
  switch_config();
  demux_config();
}

fts_module_t control_module = {"control", "control classes", control_init};
