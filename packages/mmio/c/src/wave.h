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
 */
#ifndef _WAVE_H
#define _WAVE_H

#include "mmio.h"
#include <windows.h>
#include <mmsystem.h>
#include "windowsx.h"

typedef struct _wave_t wave_t;

struct _wave_t {  
  HMMIO hmmio;
  WAVEFORMATEX format;
  MMCKINFO riff_chunk;
  MMCKINFO data_chunk;
  char* buffer;
  unsigned int buf_size;
};

void wave_delete(wave_t* wave);
int wave_read_8bits(wave_t* wave, float** buf, int nbuf, unsigned int buflen);
int wave_read_16bits(wave_t* wave, float** buf, int nbuf, unsigned int buflen);
int wave_write_8bits(wave_t* wave, float** buf, int nbuf, unsigned int buflen);
int wave_write_16bits(wave_t* wave, float** buf, int nbuf, unsigned int buflen);
int wave_realloc(wave_t* wave, unsigned int size);

int mmio_loader_open_read(fts_audiofile_t* aufile);
int mmio_loader_open_write(fts_audiofile_t* aufile);
int mmio_loader_read(fts_audiofile_t* aufile, float** buf, int nbuf, unsigned int buflen);
int mmio_loader_write(fts_audiofile_t* aufile, float** buf, int nbuf, unsigned int buflen);
int mmio_loader_close(fts_audiofile_t* aufile);
char* mmio_loader_error(fts_audiofile_t* aufile);


#endif /* _WAVE_H */
