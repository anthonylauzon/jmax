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

extern void pitch_config(void);
extern void pt_config(void);

static void
pitch_module_init(void)
{
  pitch_config();
  pt_config();
}

fts_module_t pitch_module = {"pitch", "ISPW pitch analysis classes", pitch_module_init};

