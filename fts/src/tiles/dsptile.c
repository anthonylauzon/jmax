/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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





