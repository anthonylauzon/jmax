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

#ifndef _WAVETAB_H_
#define _WAVETAB_H_

#include <fts/fts.h>

#define WAVE_TAB_BITS 9
#define WAVE_TAB_SIZE (1 << WAVE_TAB_BITS)

extern fts_symbol_t sym_nowrap;

typedef struct
{
  float value;
  float slope;
} wavetab_samp_t;

typedef struct {
  wavetab_samp_t *table;
  fts_symbol_t sym;
  int refcnt;
  int nowrap;
} wavetab_t;

extern wavetab_samp_t *cos_table;

extern void wavetable_init(void);

extern void wavetable_load(wavetab_t *wavetab);
extern wavetab_t *wavetable_new(fts_symbol_t name, fts_symbol_t wrap_mode);
extern void wavetable_delete(wavetab_t *wavetab);
int wavetable_make_cos(void);

#endif
