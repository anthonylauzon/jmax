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
 */

#include "numeric.h"

extern void abs_config(void);
extern void clip_config(void);
extern void fit_config(void);
extern void wrap_config(void);
extern void rewrap_config(void);

extern void exscale_config(void);
extern void logscale_config(void);

extern void range_config(void);

extern void for_config(void);
extern void count_config(void);
extern void ramp_config(void);

extern void random_config(void);
extern void drunk_config(void);


void
numeric_config(void)
{
  abs_config();
  clip_config();
  fit_config();
  wrap_config();
  rewrap_config();

  exscale_config();
  logscale_config();

  range_config();

  for_config();
  count_config();
  ramp_config();

  random_config();
  drunk_config();
}

