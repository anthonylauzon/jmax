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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

/* cubic interpolation macros by Norbert Schnell and Peter Hanappe */

#ifndef _FTS_IDEFIX_H_
#define _FTS_IDEFIX_H_

#include <math.h>

#define FTS_IDEFIX_INDEX_BITS 31
#define FTS_IDEFIX_INDEX_MAX 2147483647 /* index is signed */

#define FTS_IDEFIX_FRAC_BITS 32
#define FTS_IDEFIX_FRAC_MAX ((unsigned int)4294967295U)
#define FTS_IDEFIX_FRAC_RANGE ((double)4294967296.0L)

typedef struct _idefix_
{
  int index;
  unsigned int frac;
} fts_idefix_t;

#define fts_idefix_get_index(x) ((int)(x).index)
#define fts_idefix_get_frac(x) ((double)((x).frac) / FTS_IDEFIX_FRAC_RANGE)

#define fts_idefix_get_float(x) ((x).index + ((double)((x).frac) / FTS_IDEFIX_FRAC_RANGE))

#define fts_idefix_set_int(x, i) ((x)->index = (i), (x)->frac = 0)
#define fts_idefix_set_float(x, f) ((x)->index = floor(f), (x)->frac = ((double)(f) - (x)->index) * FTS_IDEFIX_FRAC_RANGE)

#define fts_idefix_set_zero(x) ((x)->index = 0, (x)->frac = 0)
#define fts_idefix_set_max(x) ((x)->index = FTS_IDEFIX_INDEX_MAX, (x)->frac = FTS_IDEFIX_FRAC_MAX)

#define fts_idefix_negate(x) ((x)->index = -(x)->index - ((x)->frac > 0), (x)->frac = (FTS_IDEFIX_FRAC_MAX - (x)->frac) + 1)

#define fts_idefix_incr(x, c) ((x)->frac += (c).frac, (x)->index += ((c).index + ((x)->frac < (c).frac)))

#define fts_idefix_add(x, a, b) ((x)->frac = (a).frac + (b).frac, (x)->index = (a).index + ((b).index + ((x)->frac < (a).frac)))
#define fts_idefix_sub(x, a, b) ((x)->index = (a).index - ((b).index + ((a).frac < (b).frac)), (x)->frac = (a).frac - (b).frac)

#define fts_idefix_lshift(x, c, i) ((x)->index = ((c).index << (i)) + ((c).frac >> (FTS_IDEFIX_FRAC_BITS - (i))), (x)->frac = (c).frac << (i))

#define fts_idefix_lt(x, c) (((x).index < (c).index) || (((x).index == (c).index) && ((x).frac < (c).frac)))
#define fts_idefix_gt(x, c) (((x).index > (c).index) || (((x).index == (c).index) && ((x).frac > (c).frac)))
#define fts_idefix_eq(x, c) (((x).index == (c).index) && ((x).frac == (c).frac))
#define fts_idefix_is_zero(x) (((x).index == 0) && ((x).frac == 0))

#endif
