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

#include "sys.h"
#include "lang.h"
#include "runtime/files/soundformats.h"

/* generally explicitly supported file format symbols (see down for explain) */
fts_symbol_t fts_s_aiffc = 0;
fts_symbol_t fts_s_aiff = 0;
fts_symbol_t fts_s_next = 0;
fts_symbol_t fts_s_snd = 0;
fts_symbol_t fts_s_au = 0;
fts_symbol_t fts_s_bicsf = 0;
fts_symbol_t fts_s_sf = 0;
fts_symbol_t fts_s_sdII = 0;
fts_symbol_t fts_s_wav = 0;
fts_symbol_t fts_s_signed8 = 0;
fts_symbol_t fts_s_signed16 = 0;
fts_symbol_t fts_s_signed24 = 0;
fts_symbol_t fts_s_signed32 = 0;

static fts_soundfile_format_t format_aiffc = {"extended Audio Interchange File Format (AIFFC)", FTS_NULL};
static fts_soundfile_format_t format_aiff = {"Audio Interchange File Format (AIFF)", FTS_NULL};
static fts_soundfile_format_t format_next = {"NeXT .snd and Sun .au", FTS_NULL};
static fts_soundfile_format_t format_bicsf = {"Berkeley/IRCAM/CARL Sound File format", FTS_NULL};
static fts_soundfile_format_t format_sdII = {"Sound Designer File Format II", FTS_NULL};
static fts_soundfile_format_t format_wav = {"Microsoft RIFF .wav format", FTS_NULL};
static fts_soundfile_format_t format_float = {"raw 32 bit floats", FTS_NULL};
static fts_soundfile_format_t format_signed8 = {"raw 8 bit twos complement integers", FTS_NULL};
static fts_soundfile_format_t format_signed16 = {"raw 16 bit twos complement integers", FTS_NULL};
static fts_soundfile_format_t format_signed24 = {"raw 24 bit twos complement integers", FTS_NULL};
static fts_soundfile_format_t format_signed32 = {"raw 32 bit twos complement integers", FTS_NULL};

/************************************************************************
 *
 *  standard formats hash table
 *
 */


static fts_hash_table_t the_fts_soundfile_standard_formats_hashtable;

static fts_soundfile_format_t *
fts_soundfile_format_get_by_name(fts_symbol_t name)
{
  fts_atom_t data;
  
  if (! name)
    return(0);
  
  if(fts_hash_table_get(&the_fts_soundfile_standard_formats_hashtable, name, &data))
    return (fts_soundfile_format_t *) fts_get_ptr(&data);
  else
    return 0;
}

int
fts_soundfile_format_exists(fts_symbol_t name)
{
  fts_atom_t data;
  return(fts_hash_table_get(&the_fts_soundfile_standard_formats_hashtable, name, &data));
}

void
fts_soundfile_format_add(fts_symbol_t name, fts_soundfile_format_t *format)
{
  fts_atom_t data;

  fts_set_ptr(&data, format);
  fts_hash_table_put(&the_fts_soundfile_standard_formats_hashtable, name, &data);
}

void
fts_soundfile_format_remove(fts_symbol_t name)
{
  fts_hash_table_remove(&the_fts_soundfile_standard_formats_hashtable, name);
}

/************************************************************************
 *
 *  get standard format parameters from hash table
 *
 */

char *
fts_soundfile_format_get_explain(fts_symbol_t name)
{
  static char unknown_soundfile_format_string[] = "unknown soundfile format";
  fts_soundfile_format_t *format;

  format = fts_soundfile_format_get_by_name(name);

  if(format)
    return format->explain;
  else
    return unknown_soundfile_format_string;
}

fts_atom_t *
fts_soundfile_format_get_descriptor(fts_symbol_t name)
{
  fts_soundfile_format_t *format;

  format = fts_soundfile_format_get_by_name(name);

  if(format)
    return &format->descr;
  else
    return 0;
}

/************************************************************************
 *
 *  default format name
 *
 */

static fts_symbol_t default_format_name;

void
fts_soundfile_format_set_default(fts_symbol_t name)
{
  default_format_name = name;
}


fts_symbol_t 
fts_soundfile_format_get_default()
{
  return default_format_name;
}


int
fts_soundfile_format_is_raw(fts_symbol_t name)
{
  return (name == fts_s_float || name == fts_s_signed8 || name == fts_s_signed16 ||name == fts_s_signed24 || name == fts_s_signed32);
}

/************************************************************************
 *
 *  init standard formats hash table and symbols
 *
 */

void
fts_soundfile_format_init(void)
{
  fts_soundfile_format_t format;
  fts_hash_table_init(&the_fts_soundfile_standard_formats_hashtable);
  
  fts_s_aiffc = fts_new_symbol("aiffc");
  fts_s_aiff = fts_new_symbol("aiff");
  fts_s_next = fts_new_symbol("next");
  fts_s_snd = fts_new_symbol("snd");
  fts_s_au = fts_new_symbol("au");
  fts_s_sf = fts_new_symbol("sf");
  fts_s_bicsf = fts_new_symbol("bicsf");
  fts_s_sdII = fts_new_symbol("sdII");
  fts_s_wav = fts_new_symbol("wav");
  fts_s_signed8 = fts_new_symbol("signed8");
  fts_s_signed16 = fts_new_symbol("signed16");
  fts_s_signed24 = fts_new_symbol("signed24");
  fts_s_signed32 = fts_new_symbol("signed32");
  
  fts_soundfile_format_add(fts_s_aiffc, &format_aiffc);
  fts_soundfile_format_add(fts_s_aiff, &format_aiff);
  fts_soundfile_format_add(fts_s_next, &format_next);
  fts_soundfile_format_add(fts_s_snd, &format_next);
  fts_soundfile_format_add(fts_s_au, &format_next);
  fts_soundfile_format_add(fts_s_bicsf, &format_bicsf);
  fts_soundfile_format_add(fts_s_sf, &format_bicsf);
  fts_soundfile_format_add(fts_s_sdII, &format_sdII);
  fts_soundfile_format_add(fts_s_wav, &format_wav);
  fts_soundfile_format_add(fts_s_float, &format_float);
  fts_soundfile_format_add(fts_s_signed8, &format_signed8);
  fts_soundfile_format_add(fts_s_signed16, &format_signed16);
  fts_soundfile_format_add(fts_s_signed24, &format_signed24);
  fts_soundfile_format_add(fts_s_signed32, &format_signed32);

  fts_soundfile_format_platform_init();
}
