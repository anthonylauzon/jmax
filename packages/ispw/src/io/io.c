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

extern void dac_config(void);
extern void adc_config(void);

#ifdef HAS_DTD
extern void disk_config(void);
extern void newdisk_config(void);
#endif

static void
io_module_init(void)
{
  dac_config();
  adc_config();

#ifdef HAS_DTD
  disk_config();
  newdisk_config();
#endif
}

fts_module_t io_module = {"io", "ISPW signal i/o classes", io_module_init};
