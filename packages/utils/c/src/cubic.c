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
 */

/* coefficient atable for cubic interpolation */

#include "utils.h"

fts_cubic_coefs_t fts_cubic_table[FTS_CUBIC_TABLE_SIZE];

void
fts_cubic_init(void)
{
  int i;
  float f;
  fts_cubic_coefs_t *p;

  p = fts_cubic_table;

  for( i = 0; i < FTS_CUBIC_TABLE_SIZE; i++)
    {
      f = i * (1.0 / FTS_CUBIC_TABLE_SIZE);
      p->pm1 = -0.1666667 * f * (1 - f) * (2 - f);
      p->p0 = 0.5 * (1 + f) * (1 - f) * (2 - f);
      p->p1 = 0.5 * (1 + f) * f * (2 - f);
      p->p2 = -0.1666667 * (1 + f) * f * (1 - f);
      p++;
    }
}
