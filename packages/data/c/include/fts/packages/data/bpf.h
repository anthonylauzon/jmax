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
 */

#ifndef _DATA_BPF_H_
#define _DATA_BPF_H_

#include <fts/packages/data/data.h>

/*************************************************
 *
 *  single break point
 *
 */
typedef struct _bp_
{
  double time; /* absolute break point time */
  double value; /* break point value */
  double slope; /* slope to next value */
} bp_t;

/*************************************************
 *
 *  break point function (array of break points)
 *
 */
typedef struct _bpf_
{
  data_object_t o;
  bp_t *points; /* break points */
  int alloc;
  int size;
  int index; /* index cache for get_interpolated method */
  int opened; /* non zero if editor open */
} bpf_t;

DATA_API fts_symbol_t bpf_symbol;
DATA_API fts_class_t *bpf_type;

#define bpf_get_size(b) ((b)->size)

#define bpf_get_time(b, i) ((b)->points[i].time)
#define bpf_get_value(b, i) ((b)->points[i].value)
#define bpf_get_slope(b, i) ((b)->points[i].slope)

#define bpf_get_duration(b) ((b)->size > 0? (b)->points[(b)->size - 1].time: 0.0)
#define bpf_get_target(b) ((b)->size > 0? (b)->points[(b)->size - 1].value: 0.0)

DATA_API void bpf_clear(bpf_t *bpf);
DATA_API void bpf_copy(bpf_t *bpf, bpf_t *copy);
DATA_API void bpf_append_point(bpf_t *bpf, double time, double value);
DATA_API void bpf_set_point(bpf_t *bpf, int index, double time, double value);
DATA_API void bpf_insert_point(bpf_t *bpf, double time, double value);
DATA_API void bpf_remove_points(bpf_t *bpf, int index, int n);
DATA_API void bpf_simplify(bpf_t *bpf, double time, double value);

#endif
