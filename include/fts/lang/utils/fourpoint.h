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

#ifndef _FTS_FOURPOINT_H_
#define _FTS_FOURPOINT_H_

#define FTS_FOURPOINT_TABLE_BITS 8
#define FTS_FOURPOINT_TABLE_SIZE (1 << FTS_FOURPOINT_TABLE_BITS)

#define FTS_FOURPOINT_LOST_BITS 8

#define FTS_FOURPOINT_FRAC_BITS (FTS_FOURPOINT_TABLE_BITS + FTS_FOURPOINT_LOST_BITS)
#define FTS_FOURPOINT_FRAC_SIZE (1 << FTS_FOURPOINT_FRAC_BITS)

typedef int fts_fourpoint_index_t;

typedef struct
{
  float pm1;
  float p0;
  float p1;
  float p2;
} fts_fourpoint_t;

extern fts_fourpoint_t fts_fourpoint_table[];

extern int fts_fourpoint_table_make(void);

#define fts_fourpoint_index_scale(f) ((f) * FTS_FOURPOINT_FRAC_SIZE)
#define fts_fourpoint_index_get_int(i) ((i) >> FTS_FOURPOINT_FRAC_BITS)
#define fts_fourpoint_index_get_frac(i) ((i) & (FTS_FOURPOINT_FRAC_SIZE - 1))


#define fts_fourpoint_calc(x, p) \
  ((x)[-1] * (p)->pm1 + (x)[0] * (p)->p0 + (x)[1] * (p)->p1 + (x)[2] * (p)->p2)
#define fts_fourpoint_calc_first(x, p) \
  ((x)[0] * ((p)->pm1 + (p)->p0) + (x)[1] * (p)->p1 + (x)[2] * (p)->p2)

#define fts_fourpoint_interpolate(p, i, y) \
  do { \
    float* q = (p) + ((i) >> FTS_FOURPOINT_FRAC_BITS); \
    fts_fourpoint_t *ft = fts_fourpoint_table + (((i) >> FTS_FOURPOINT_LOST_BITS) & (FTS_FOURPOINT_TABLE_SIZE - 1)); \
    *(y) = fts_fourpoint_calc(q, ft); \
  } while(0)

#define fts_fourpoint_interpolate_range(p, i, y, a, n) \
  do { \
    float* q = (p) + ((i) >> FTS_FOURPOINT_FRAC_BITS); \
    fts_fourpoint_t *ft = fts_fourpoint_table + (((i) >> FTS_FOURPOINT_LOST_BITS) & (FTS_FOURPOINT_TABLE_SIZE - 1)); \
    if(q > (a) + 1 && q < (a) + (n) - 2) *(y) = fts_fourpoint_calc(q, ft); \
    else *(y) = 0.0; \
  } while(0)

#define fts_fourpoint_interpolate_frac(p, f, y) \
  do { \
    fts_fourpoint_index_t i = ((f) * (float)FTS_FOURPOINT_TABLE_SIZE); \
    fts_fourpoint_t *ft = fts_fourpoint_table + (i & (FTS_FOURPOINT_TABLE_SIZE - 1)); \
    *(y) = fts_fourpoint_calc((p), ft); \
  } while(0)

#define fts_fourpoint_interpolate_first_frac(p, f, y) \
  do { \
    fts_fourpoint_index_t i = ((f) * (float)FTS_FOURPOINT_TABLE_SIZE); \
    fts_fourpoint_t *ft = fts_fourpoint_table + (i & (FTS_FOURPOINT_TABLE_SIZE - 1)); \
    *(y) = fts_fourpoint_calc_first((p), ft); \
  } while(0)

#endif
