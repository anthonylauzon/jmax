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

extern void integer_config(void);
extern void bangbang_config(void);
extern void sma_config(void);

static void sources_init(void)
{
  integer_config();
  bangbang_config();
  sma_config();
}

fts_module_t sources_module = { "sources", "objects examples with sources", sources_init, 0, 0, 0};
