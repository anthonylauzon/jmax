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

#include <string.h>
#include <fts/fts.h>

/* sample formats */
fts_symbol_t fts_s_int8;
fts_symbol_t fts_s_int16;
fts_symbol_t fts_s_int24;
fts_symbol_t fts_s_int32;
fts_symbol_t fts_s_uint8;
fts_symbol_t fts_s_uint16;
fts_symbol_t fts_s_uint24;
fts_symbol_t fts_s_uint32;
fts_symbol_t fts_s_float32;
fts_symbol_t fts_s_float64;

/* file formats */
fts_symbol_t fts_s_aiff;
fts_symbol_t fts_s_wav;
fts_symbol_t fts_s_snd;
fts_symbol_t fts_s_raw;

fts_audiofile_loader_t* fts_audiofile_loader = NULL;

int 
fts_audiofile_set_loader(char* name, fts_audiofile_loader_t* loader)
{
  fts_audiofile_loader = loader;
  fts_log("[audiofile] Setting audio file loader to %s\n", name);
  return 0;
}

void
fts_audiofile_set_file_format_by_suffix(fts_audiofile_t* aufile, fts_symbol_t suffix)
{
  if(suffix == fts_s_aiff)
    aufile->file_format = audiofile_aiff;
  else if(suffix == fts_s_wav)
    aufile->file_format = audiofile_wave;
  else if(suffix == fts_s_snd)
    aufile->file_format = audiofile_snd;
  else
    aufile->file_format = audiofile_file_format_null;
}

void
fts_audiofile_set_sample_format_by_name(fts_audiofile_t* aufile, fts_symbol_t name)
{
  if(name == fts_s_int8)
    {
      aufile->sample_format = audiofile_int8;
      aufile->bytes_per_sample = 1;
    }
  else if(name == fts_s_int16)
    {
      aufile->sample_format = audiofile_int16;
      aufile->bytes_per_sample = 2;
    }
  else if(name == fts_s_int24)
    {
      aufile->sample_format = audiofile_int24;
      aufile->bytes_per_sample = 3;
    }
  else if(name == fts_s_int32)
    {
      aufile->sample_format = audiofile_int32;
      aufile->bytes_per_sample = 4;
    }
  else if(name == fts_s_uint8)
    {
      aufile->sample_format = audiofile_uint8;
      aufile->bytes_per_sample = 1;
    }
  else if(name == fts_s_uint16)
    {
      aufile->sample_format = audiofile_uint16;
      aufile->bytes_per_sample = 2;
    }
  else if(name == fts_s_uint24)
    {
      aufile->sample_format = audiofile_uint24;
      aufile->bytes_per_sample = 3;
    }
  else if(name == fts_s_uint32)
    {
      aufile->sample_format = audiofile_uint32;
      aufile->bytes_per_sample = 4;
    }
  else if(name == fts_s_float32)
    {
      aufile->sample_format = audiofile_float32;
      aufile->bytes_per_sample = 4;
    }
  else if(name == fts_s_float64)
    {
      aufile->sample_format = audiofile_float64;
      aufile->bytes_per_sample = 8;
    }
  else
    {
      aufile->sample_format = audiofile_sample_format_null;
      aufile->bytes_per_sample = 0;
    }
}

static fts_audiofile_t *
audiofile_new(fts_symbol_t filename, fts_symbol_t mode)
{
  fts_audiofile_t *aufile = (fts_audiofile_t *)fts_malloc(sizeof(fts_audiofile_t));

  aufile->filename = filename;
  aufile->mode = mode;

  aufile->channels = 0;
  aufile->sample_rate = 0;
  aufile->sample_format = audiofile_sample_format_null;
  aufile->bytes_per_sample = 0;
  aufile->file_format = audiofile_file_format_null;
  aufile->frames = 0;
  
  aufile->handle = NULL;
  aufile->error = NULL;

  return aufile;
}

fts_audiofile_t *
fts_audiofile_open_read(fts_symbol_t filename)
{
  if (fts_audiofile_loader != NULL) 
    {
      fts_audiofile_t *aufile = audiofile_new(filename, fts_s_read);
      
      /* open file */
      fts_audiofile_loader->open_read(aufile);

      /* allocate buffer with default length */
      fts_audiofile_loader->buffer_length(aufile, 0);

      return aufile;
    }
  else
    {
      fts_log("[audiofile] trying to open audiofile without loader set");
      return NULL;
    }
}

fts_audiofile_t * 
fts_audiofile_open_write(fts_symbol_t filename, int channels, int sample_rate, fts_symbol_t sample_format)
{
  if (fts_audiofile_loader != NULL) 
    {
      fts_audiofile_t *aufile = audiofile_new(filename, fts_s_write);
      char *suffix = strrchr(filename, '.');

      aufile->channels = channels;
      aufile->sample_rate = sample_rate;
      
      if(suffix != NULL)
	fts_audiofile_set_file_format_by_suffix(aufile, fts_new_symbol_copy(suffix + 1));
      else
	fts_audiofile_set_file_format_by_suffix(aufile, 0);

      /* open file */
      fts_audiofile_loader->open_write(aufile);
      
      /* allocate buffer with default length */
      fts_audiofile_loader->buffer_length(aufile, 0);

      return aufile;
    }
  else
    {
      fts_log("[audiofile] trying to open audiofile without loader set");
      return NULL;
    }
}

void 
fts_audiofile_close(fts_audiofile_t* aufile)
{
  if(fts_audiofile_loader != NULL)
    {
      if(aufile)
	{
	  fts_audiofile_loader->close(aufile);
	  fts_free(aufile);
	}
    }
}

void 
fts_kernel_audiofile_init(void)
{
  fts_s_int8 = fts_new_symbol("int8");
  fts_s_int16 = fts_new_symbol("int16");
  fts_s_int24 = fts_new_symbol("int24");
  fts_s_int32 = fts_new_symbol("int32");
  fts_s_uint8 = fts_new_symbol("uint8");
  fts_s_uint16 = fts_new_symbol("uint16");
  fts_s_uint24 = fts_new_symbol("uint24");
  fts_s_uint32 = fts_new_symbol("uint32");
  fts_s_float32 = fts_new_symbol("float32");
  fts_s_float64 = fts_new_symbol("float64");

  fts_s_aiff = fts_new_symbol("aiff");
  fts_s_wav = fts_new_symbol("wav");
  fts_s_snd = fts_new_symbol("snd");
  fts_s_raw = fts_new_symbol("raw");
}
