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

#ifndef _FTS_SOUNDFILE_H_
#define _FTS_SOUNDFILE_H_


typedef struct fts_soundfile_format
{
  char *explain; /* explanation string */
  fts_atom_t descr; /* platform dependent description */
} fts_soundfile_format_t;

/* platform dependend init routine */
FTS_API void fts_soundfile_format_platform_init(void);


/**********************************************************************************
 *
 *  explicitly supported standard soundfile formats (for writing and raw reading)
 *
 */

/* standard soundfile format symbols */
FTS_API fts_symbol_t fts_s_aiffc; /* extended Audio Interchange File Format (AIFF-C) */
FTS_API fts_symbol_t fts_s_aiff; /* Audio Interchange File Format (AIFF) */
FTS_API fts_symbol_t fts_s_next; /* NeXT .snd and Sun .au */
FTS_API fts_symbol_t fts_s_snd;
FTS_API fts_symbol_t fts_s_au;
FTS_API fts_symbol_t fts_s_bicsf; /* Berkeley/IRCAM/CARL Sound File format */
FTS_API fts_symbol_t fts_s_sf;
FTS_API fts_symbol_t fts_s_sdII; /* Sound Designer File Format II */
FTS_API fts_symbol_t fts_s_wav; /* Microsoft .wav */
FTS_API fts_symbol_t fts_s_unknown; /* unknown file format */

/* raw format symbols */
/* fts_s_float ... raw 32 bit floats */
FTS_API fts_symbol_t fts_s_signed8; /* raw 8 bit twos complement integers */
FTS_API fts_symbol_t fts_s_signed16; /* raw 16 bit twos complement integers */
FTS_API fts_symbol_t fts_s_signed24; /* raw 24 bit twos complement integers */
FTS_API fts_symbol_t fts_s_signed32; /* raw 32 bit twos complement integers */

FTS_API void fts_soundfile_format_add(fts_symbol_t name, fts_soundfile_format_t *format);
FTS_API void fts_soundfile_format_remove(fts_symbol_t name);
FTS_API int fts_soundfile_format_exists(fts_symbol_t name);

/* get explain and platform dependend descriptor for a soundfile format by name */
FTS_API char *fts_soundfile_format_get_explain(fts_symbol_t name);
FTS_API fts_atom_t *fts_soundfile_format_get_descriptor(fts_symbol_t name);

FTS_API void fts_soundfile_format_set_default(fts_symbol_t name);
FTS_API fts_symbol_t fts_soundfile_format_get_default(void);

FTS_API int fts_soundfile_format_is_raw(fts_symbol_t name);


/*********************************************************************************
 *
 *  platform dependend implemented routines:
 *
 *  simple routines to read/write float numbers (-1.0 ... 1.0) from/to a soundfile
 *  (the format is specified when opening the file for read or write)
 *
 * fts_soundfile_open_to_read_float(fts_symbol_t file_name, fts_symbol_t format, double sr)
 *   file_name ... the file name
 *   format ... specifies the format of raw data files 
 *   sr ... specifies a sampling rate - values other than 0 may course a conversion
 *
 * fts_soundfile_open_to_write_float(fts_symbol_t file_name, fts_symbol_t format, double sr);
 *   file_name ... the file name
 *   format ... specifies the format wich in case of 0 defaults to platform dependend default format
 *   sr ... specifies a the sampling rate for the file header as is ignored for all raw data types
 */

/* platform dependend soundfile implementation */
typedef struct fts_soundfile fts_soundfile_t;

FTS_API fts_soundfile_t *fts_soundfile_open_read_float(fts_symbol_t file_name, fts_symbol_t format, double sr, int onset);
FTS_API fts_soundfile_t *fts_soundfile_open_write_float(fts_symbol_t file_name, fts_symbol_t format, double sr);
FTS_API void fts_soundfile_close(fts_soundfile_t *soundfile);

FTS_API int fts_soundfile_read_float(fts_soundfile_t *soundfile, float *buffer, int size);
FTS_API int fts_soundfile_write_float(fts_soundfile_t *soundfile, float *buffer, int size);

FTS_API int fts_soundfile_get_size(fts_soundfile_t *soundfile);
FTS_API double fts_soundfile_get_samplerate(fts_soundfile_t *soundfile);
FTS_API int fts_soundfile_get_n_channels(fts_soundfile_t *soundfile);


#endif
