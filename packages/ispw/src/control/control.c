/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"

extern void at_config(void);
extern void bangbang_config(void);
extern void change_config(void);
extern void clock_config(void);
extern void delay_config(void);
extern void drunk_config(void);
extern void gate_config(void);
extern void line_config(void);
extern void loadbang_config(void);
extern void metro_config(void);
extern void nbangs_config(void);
extern void pack_config(void);
extern void pipe_config(void);
extern void poly_config(void);
extern void prepend_config(void);
extern void print_config(void);
extern void random_config(void);
extern void route_config(void);
extern void select_config(void);
extern void speedlim_config(void);
extern void split_config(void);
extern void time_config(void);
extern void timer_config(void);
extern void trigger_config(void);
extern void unpack_config(void);
extern void voxalloc_config(void);

extern void abs_config(void);
extern void clip_config(void);
extern void exscale_config(void);
extern void binop_config(void);
extern void ftom_config(void);
extern void logscale_config(void);
extern void mtof_config(void);
extern void scale_config(void);
extern void blip_config(void);


static void
control_module_init(void)
{
  at_config();
  bangbang_config();
  change_config();
  clock_config();
  delay_config();
  drunk_config();
  gate_config();
  line_config();
  loadbang_config();
  metro_config();
  nbangs_config();
  pack_config();
  pipe_config();
  poly_config();
  prepend_config();
  print_config();
  random_config();
  route_config();
  select_config();
  speedlim_config();
  split_config();
  time_config();
  timer_config();
  trigger_config();
  unpack_config();
  voxalloc_config();

  abs_config();
  clip_config();
  exscale_config();
  binop_config();
  ftom_config();
  logscale_config();
  mtof_config();
  scale_config();

  blip_config();
}

fts_module_t control_module = {"control", "ISPW miscellaneous control classes", control_module_init};

