/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

fts_symbol_t fts_s_S;
fts_symbol_t fts_s_KS;
fts_symbol_t fts_s_msec;
fts_symbol_t fts_s_sec;

/*********************************************************
 *
 *    the units hash table
 *
 */
 
static fts_hash_table_t the_units_hashtable;



/*********************************************************
 *
 *    units
 *
 */

/* the hashtable entry */

typedef struct
{
  fts_symbol_t base;		/* symbol of base unit  */
  fts_unit_conv_t conv;		/* conversion factor or function */
  int depends;			/* flag: non zero, if value depends on a global state
				   (sampling rate, tick size, ...) */
} fts_unit_t;

/* ACHTUNG: base unit should not have a dependency! */


/*********************************************************
 *
 *    SPI functions
 *
 */
 
/* predefined dummy for base units */

static float  fts_unit_conv_identity(float value, void *nix)
{
  return value;
}


int fts_unit_add(fts_symbol_t unit_name, fts_symbol_t base, fts_unit_conv_t conv, int depends)
{
  fts_atom_t a;
  fts_unit_t *unit;

  unit = (fts_unit_t *)fts_malloc(sizeof(fts_unit_t));

  fts_set_ptr(&a, unit);
  if (! fts_hash_table_insert(&the_units_hashtable, unit_name, &a))
    {
      fts_free((void *)unit);
      return(0);
    }

  unit->base = base;

  if(unit_name == base)
    unit->conv = fts_unit_conv_identity;
  else
    unit->conv = conv;

  unit->depends = depends;

  return 1;
}


/*********************************************************
 *
 *    API user functions
 *
 */

int
fts_is_unit(fts_symbol_t unit_name, fts_symbol_t base_name)
{
  fts_atom_t unit;

  return fts_hash_table_lookup(&the_units_hashtable, unit_name, &unit);
}


int
fts_unit_is_of_base(fts_symbol_t unit_name, fts_symbol_t base_name)
{
  fts_atom_t data;
  fts_unit_t *unit;

  if (! fts_hash_table_lookup(&the_units_hashtable, unit_name, &data))
    return 0;
  else
    {
      unit = (fts_unit_t *)fts_get_ptr(&data);
      return (unit->base == base_name);
    }
}

float
fts_unit_convert_to_base(fts_symbol_t unit_name, float value, void *dependency)
{
  fts_atom_t data;
  fts_unit_t *unit;

  if (! fts_hash_table_lookup(&the_units_hashtable, unit_name, &data))
    return 0.0f; /* check it in advance! */

  unit = (fts_unit_t *)fts_get_ptr(&data);

  if (unit->base == unit_name)
    return value;

  return unit->conv(value, dependency);
}


int
fts_unit_depends(fts_symbol_t unit_name)
{
  fts_atom_t data;
  fts_unit_t *unit;

  if (! fts_hash_table_lookup(&the_units_hashtable, unit_name, &data))
    return 0;

  unit = (fts_unit_t *)fts_get_ptr(&data);
  return unit->depends;
}


/*********************************************************
 *
 *    sample units (duration)
 *
 */

/* conversion functions */
/*
static float fts_unit_conv_KS_to_S(float KS, void *nix){return(KS * 1024.0f);}
static float fts_unit_conv_sec_to_S(float sec, void *sr){return((long)(*((float *)sr) * sec + 0.5f));}
static float fts_unit_conv_msec_to_S(float msec, void *sr){return((long)(*((float *)sr) * 0.001 * msec + 0.5f));}
*/

/*
   MDC: trying with *NO* rounding: we return a float, the real converted value;
   any conversion can be done later
*/

static float
fts_unit_conv_KS_to_S(float KS, void *nix)
{
  return KS * 1024.0f;
}

static float
fts_unit_conv_sec_to_S(float sec, void *sr)
{
  return (*((float *)sr) * sec);
}

static float
fts_unit_conv_msec_to_S(float msec, void *sr)
{
  return (*((float *)sr) * 0.001f * msec);
}

fts_symbol_t 
fts_unit_get_samples_arg(int ac, const fts_atom_t *at, int n, fts_symbol_t def)
{
  if (n < ac && fts_is_symbol(&at[n]))
    {
      fts_symbol_t unit_name = fts_get_symbol(&at[n]);
  
      if (fts_unit_is_of_base(unit_name, fts_s_S))
	return unit_name;
      else
	return def;
    }
  else
    return def;
}


void fts_units_init(void)
{
  fts_hash_table_init(&the_units_hashtable);

  fts_s_S = fts_new_symbol("S");
  fts_s_KS = fts_new_symbol("KS");
  fts_s_msec = fts_new_symbol("msec");
  fts_s_sec = fts_new_symbol("sec");
  fts_unit_add(fts_s_S, fts_s_S, 0, 0);
  fts_unit_add(fts_s_KS, fts_s_S, fts_unit_conv_KS_to_S, 0);
  fts_unit_add(fts_s_msec, fts_s_S, fts_unit_conv_msec_to_S, 1);
  fts_unit_add(fts_s_sec, fts_s_S, fts_unit_conv_sec_to_S, 1);
}
