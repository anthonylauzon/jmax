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

#ifndef _SOUNDFORMATSS_H_
#define _SOUNDFORMATSS_H_

typedef struct fts_soundfile_format
{
  char *explain; /* explanation string */
  fts_atom_t descr; /* platform dependent description */
} fts_soundfile_format_t;

/* platform dependend init routine */
extern void fts_soundfile_format_platform_init(void);

/**********************************************************************************
 *
 *  explicitly supported standard soundfile formats (for writing and raw reading)
 *
 */

/* standard soundfile format symbols */
extern fts_symbol_t fts_s_aiffc; /* extended Audio Interchange File Format (AIFF-C) */
extern fts_symbol_t fts_s_aiff; /* Audio Interchange File Format (AIFF) */
extern fts_symbol_t fts_s_next; /* NeXT .snd and Sun .au */
extern fts_symbol_t fts_s_snd;
extern fts_symbol_t fts_s_au;
extern fts_symbol_t fts_s_bicsf; /* Berkeley/IRCAM/CARL Sound File format */
extern fts_symbol_t fts_s_sf;
extern fts_symbol_t fts_s_sdII; /* Sound Designer File Format II */
extern fts_symbol_t fts_s_wav; /* Microsoft .wav */
extern fts_symbol_t fts_s_unknown; /* unknown file format */

/* raw format symbols */
/* fts_s_float ... raw 32 bit floats */
extern fts_symbol_t fts_s_signed8; /* raw 8 bit twos complement integers */
extern fts_symbol_t fts_s_signed16; /* raw 16 bit twos complement integers */
extern fts_symbol_t fts_s_signed24; /* raw 24 bit twos complement integers */
extern fts_symbol_t fts_s_signed32; /* raw 32 bit twos complement integers */

extern void fts_soundfile_format_add(fts_symbol_t name, fts_soundfile_format_t *format);
extern void fts_soundfile_format_remove(fts_symbol_t name);
extern int fts_soundfile_format_exists(fts_symbol_t name);

/* get explain and platform dependend descriptor for a soundfile format by name */
extern char *fts_soundfile_format_get_explain(fts_symbol_t name);
extern fts_atom_t *fts_soundfile_format_get_descriptor(fts_symbol_t name);

extern void fts_soundfile_format_set_default(fts_symbol_t name);
extern fts_symbol_t fts_soundfile_format_get_default(void);

extern int fts_soundfile_format_is_raw(fts_symbol_t name);

#endif
