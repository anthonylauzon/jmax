/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

extern void explay_config(void);
extern void explode_config(void);

static void
fts_explode_init(void)
{
  explay_config();
  explode_config();
}

fts_module_t explode_module = {"explode", "explode and explay scorfollowing/sequencing classes", fts_explode_init};
