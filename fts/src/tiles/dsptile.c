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
/*
   Temporary definition of the dsp chain tile.

   Soon to be reorganized.

   Actually, just install the dsp_poll funciton; really, it should incorporate
   the installation of the good dsp chain, and it should incorporate
   the polled function (?? or should just go away in runtime ???).

*/

#include "sys.h"
#include "lang.h"
#include "runtime.h"

static void dsp_tile_init(void);

fts_module_t fts_dsptile_module = {"Dsp", "Dsp executor", dsp_tile_init, 0};

static void
dsp_tile_init(void)
{
  fts_sched_declare(dsp_chain_poll, require, fts_new_symbol("control"), "dsp_chain_poll");
}





