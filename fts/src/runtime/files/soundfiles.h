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
 * fts_soundfile_open_to_read_float(fts_symbol_t file_name, fts_symbol_t format, float sr)
 *   file_name ... the file name
 *   format ... specifies the format of raw data files 
 *   sr ... specifies a sampling rate - values other than 0 may course a conversion
 *
 * fts_soundfile_open_to_write_float(fts_symbol_t file_name, fts_symbol_t format, float sr);
 *   file_name ... the file name
 *   format ... specifies the format wich in case of 0 defaults to platform dependend default format
 *   sr ... specifies a the sampling rate for the file header as is ignored for all raw data types
 */

extern fts_soundfile_t *fts_soundfile_open_read_float(fts_symbol_t file_name, fts_symbol_t format, float sr, int onset);
extern fts_soundfile_t *fts_soundfile_open_write_float(fts_symbol_t file_name, fts_symbol_t format, float sr);
extern void fts_soundfile_close(fts_soundfile_t *soundfile);

extern int fts_soundfile_read_float(fts_soundfile_t *soundfile, float *buffer, int size);
extern int fts_soundfile_write_float(fts_soundfile_t *soundfile, float *buffer, int size);

#endif
