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

#include <fts/fts.h>

extern void ucs_config(void);
extern void dsp_config(void);
extern void arch_config(void);

extern void const_config(void);
extern void argument_init(void);

extern void print_config(void);
extern void blip_config(void);

extern void bus_config(void);
extern void bus_tilda_config(void);

static void
system_module_init(void)
{
  ucs_config();
  dsp_config();
  arch_config();

  const_config();
  argument_init();

  print_config();
  blip_config();

  bus_config();
  bus_tilda_config();
}

fts_module_t system_module = {"system", "Basic System Objects", system_module_init, 0, 0};
