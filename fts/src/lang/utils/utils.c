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
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

extern void fts_units_init(void);
extern void fts_ffuns_init(void);

static void
fts_utils_init(void)
{
  fts_units_init();  
  fts_ffuns_init();
}

fts_module_t fts_utils_module = {"Utils", "Message system utilities", fts_utils_init, 0};
