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

#include <fts/fts.h>

static fts_symbol_t sym_S = 0;
static fts_symbol_t sym_KS = 0;
static fts_symbol_t sym_msec = 0;
static fts_symbol_t sym_sec = 0;

fts_symbol_t 
samples_unit_get_arg(int ac, const fts_atom_t *at, int n)
{
  if (n < ac && fts_is_symbol(at + n))
    {
      fts_symbol_t unit = fts_get_symbol(&at[n]);
  
      if(unit == sym_KS || unit == sym_S || unit == sym_sec || unit == sym_msec)
	return unit;
    }

  return 0;
}

float
samples_unit_convert(fts_symbol_t unit, float value, float sr)
{
  if(unit == sym_KS)
    return 1024.0f * value;
  else if(unit == sym_S)
    return value;
  else if(unit == sym_sec)
    return sr * value;
  else if(unit == sym_msec)
    return 0.001f * sr * value;
  else
    return 0;
}

fts_symbol_t 
samples_unit_get_default(void)
{
  return sym_msec;
}

void samples_unit_init(void)
{
  sym_S = fts_new_symbol("S");
  sym_KS = fts_new_symbol("KS");
  sym_msec = fts_new_symbol("msec");
  sym_sec = fts_new_symbol("sec");
}
