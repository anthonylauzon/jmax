/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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
extern fts_symbol_t fts_soundfile_format_get_default( void);

#endif
