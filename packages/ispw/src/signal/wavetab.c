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
#include <math.h>
#include <string.h>
#include "fts.h"
#include "wavetab.h"

fts_symbol_t sym_nowrap = 0;
wavetab_samp_t *cos_table = 0;

void
wavetable_load(wavetab_t *wavetab)
{
  const char *file_name = fts_symbol_name(wavetab->sym);
  float buf[WAVE_TAB_SIZE];

    if(file_name)
    {
      fts_soundfile_t *sf = fts_soundfile_open_read_float(wavetab->sym, 0, 0.0f, 0);
      int n_samples;
      int i;

      if(!sf)
	{
	  post("tab1~: %s: can not open wave table file\n", file_name);
	  return;
	}
      
      n_samples = fts_soundfile_read_float(sf, buf, WAVE_TAB_SIZE);
      fts_soundfile_close(sf);

      if(n_samples < WAVE_TAB_SIZE)
	{
	  post("tab1~: %s: can not read wave table\n", file_name);
	  return;
	}
      
      /* get value of first wavetable point */
      wavetab->table[0].value = buf[0];

      /* next values and slopes */
      for(i=1; i<WAVE_TAB_SIZE; i++)
	{
	  wavetab->table[i].value = buf[i];
	  wavetab->table[i-1].slope = (wavetab->table[i].value - wavetab->table[i-1].value);
	}

      /* get slope of last wavetable point in dependency of wrapping mode */
      if(strstr(file_name, "nowrap") || wavetab->nowrap)
	wavetab->table[WAVE_TAB_SIZE-1].slope = wavetab->table[WAVE_TAB_SIZE-2].slope;
      else
	wavetab->table[WAVE_TAB_SIZE-1].slope = (wavetab->table[0].value - wavetab->table[WAVE_TAB_SIZE-1].value);
    }    
}

wavetab_t *
wavetable_new(fts_symbol_t name, fts_symbol_t wrap_mode)
{
  wavetab_samp_t *table = (wavetab_samp_t *) fts_malloc(WAVE_TAB_SIZE * sizeof(wavetab_samp_t));
  wavetab_t *wavetab;

  if(!table)
    return 0;
  
  wavetab = (wavetab_t *)fts_malloc(sizeof(wavetab_t));
  wavetab->sym = name;
  wavetab->refcnt = 1;
  wavetab->table = table;

  if(wrap_mode == sym_nowrap)
    wavetab->nowrap = 1;
  else
    wavetab->nowrap = 0;

  wavetable_load(wavetab);
  return wavetab;
}

void
wavetable_delete(wavetab_t *wavetab)
{
  fts_free((char *)wavetab->table);
  fts_free(wavetab);
}

void
wavetable_init(void)
{
  sym_nowrap = fts_new_symbol("nowrap");
}

int
wavetable_make_cos(void)
{
  int i;

  cos_table = (wavetab_samp_t *)fts_malloc(WAVE_TAB_SIZE * sizeof(wavetab_samp_t));

  if (! cos_table)
    return (0);

  for (i=0; i<WAVE_TAB_SIZE; i++)
    cos_table[i].value = cos(i * (2 * 3.141593f / WAVE_TAB_SIZE));

  for (i=0; i<WAVE_TAB_SIZE; i++)
    cos_table[i].slope = (cos_table[(i + 1) & (WAVE_TAB_SIZE - 1)].value - cos_table[i].value);

  return (1);
}
