/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _WAVETAB_H_
#define _WAVETAB_H_

#include "fts.h"

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
extern int make_cos_table(void);

#endif
