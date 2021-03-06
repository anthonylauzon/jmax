/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include "control.h"

extern void bus_config(void);

extern void sync_config(void);
extern void tup_config(void);

extern void fifo_config(void);
extern void stack_config(void);

extern void switch_config(void);
extern void oneshot_config(void);
extern void demux_config(void);
extern void select_config(void);
extern void change_config(void);

extern void metro_config(void);
extern void delay_config(void);
extern void retard_config(void);
extern void speedlim_config(void);
extern void timer_config(void);

void
control_config(void)
{
  bus_config();

  sync_config();
  tup_config();

  fifo_config(),
  stack_config(),

  switch_config();
  oneshot_config();
  demux_config();
  select_config();
  change_config();

  metro_config();
  delay_config();
  retard_config();
  speedlim_config();
  timer_config();
}
