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

#include <math.h>
#include <string.h>
#include <fts/fts.h>
#include "wavetab.h"

fts_symbol_t sym_nowrap = 0;
wavetab_samp_t *cos_table = 0;

void
wavetable_load(wavetab_t *wavetab)
{
  const char *file_name = wavetab->sym;
  float buf[WAVE_TAB_SIZE];

    if(file_name)
    {
      fts_audiofile_t *sf = fts_audiofile_open_read(wavetab->sym);
      int n_samples;
      int i;
      float* tmp = &buf[0];

      if(!fts_audiofile_is_valid(sf))
	{
	  post("tab1~: %s: can't open wave table file\n", file_name);
	  fts_audiofile_close(sf);
	  return;
	}
      
      n_samples = fts_audiofile_read(sf, &tmp, 1, WAVE_TAB_SIZE);
      fts_audiofile_close(sf);

      if(n_samples < WAVE_TAB_SIZE)
	{
	  post("tab1~: %s: can't read wave table\n", file_name);
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
