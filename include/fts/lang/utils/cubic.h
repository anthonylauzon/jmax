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

/* cubic interpolation by 8 bit coefficient table lookup based on fixed-point fractinal index (idefix.h) */

#ifndef _FTS_CUBIC_H_
#define _FTS_CUBIC_H_

#include "idefix.h"

#define FTS_CUBIC_TABLE_BITS 8
#define FTS_CUBIC_TABLE_SHIFT_BITS 24
#define FTS_CUBIC_TABLE_SIZE 256
#define FTS_CUBIC_TABLE_BIT_MASK 0xff000000

#define fts_cubic_get_table_index(i) \
   ((int)(((i).frac & FTS_CUBIC_TABLE_BIT_MASK) >> FTS_CUBIC_TABLE_SHIFT_BITS))

typedef struct
{
  float pm1;
  float p0;
  float p1;
  float p2;
} fts_cubic_coefs_t;

FTS_API fts_cubic_coefs_t fts_cubic_table[];

#define fts_cubic_calc(x, p) \
  ((x)[-1] * (p)->pm1 + (x)[0] * (p)->p0 + (x)[1] * (p)->p1 + (x)[2] * (p)->p2)

#define fts_cubic_interpolate(p, i, y) \
  do { \
    fts_cubic_coefs_t *ft = fts_cubic_table + fts_cubic_get_table_index(i); \
    *(y) = fts_cubic_calc(p + (i).index, ft); \
  } while(0)

#endif
