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
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#ifndef _DATA_BPF_H_
#define _DATA_BPF_H_

#include <fts/fts.h>

typedef struct _bp_
{
  double time; /* absolute break point time */
  double value; /* break point value */
} bp_t;

typedef struct _bpf_
{
  fts_object_t o;
  bp_t *points; /* break points */
  int alloc;
  int size;
  int opened; /* non zero if editor open */
} bpf_t;

extern fts_data_class_t *bpf_data_class;
extern fts_symbol_t bpf_symbol;
extern fts_type_t bpf_type;

extern void bpf_set_size(bpf_t *bpf, int size);
#define bpf_get_size(b) ((b)->size)

#define bpf_clear(b) (bpf_set_size(b, 0))

#define bpf_get_time(b, i) ((b)->points[i].time)
#define bpf_get_value(b, i) ((b)->points[i].value)
#define bpf_set_point(b, i, t, v) ((b)->points[i].time = (t), (b)->points[i].value = (v))

/* bpf atoms */
#define bpf_atom_set(ap, x) fts_set_object_with_type((ap), (x), bpf_type)
#define bpf_atom_get(ap) ((bpf_t *)fts_get_object(ap))
#define bpf_atom_is(ap) (fts_is_a((ap), bpf_type))

#endif
