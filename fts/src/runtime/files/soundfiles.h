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

#ifndef _SOUNDFILES_H_
#define _SOUNDFILES_H_

/* platform dependend soundfile implementation */
typedef struct fts_soundfile fts_soundfile_t;

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

extern fts_soundfile_t *fts_soundfile_open_read_float(fts_symbol_t file_name, fts_symbol_t format, double sr, int onset);
extern fts_soundfile_t *fts_soundfile_open_write_float(fts_symbol_t file_name, fts_symbol_t format, double sr);
extern void fts_soundfile_close(fts_soundfile_t *soundfile);

extern int fts_soundfile_read_float(fts_soundfile_t *soundfile, float *buffer, int size);
extern int fts_soundfile_write_float(fts_soundfile_t *soundfile, float *buffer, int size);

extern int fts_soundfile_get_size(fts_soundfile_t *soundfile);
extern double fts_soundfile_get_samplerate(fts_soundfile_t *soundfile);

#endif
