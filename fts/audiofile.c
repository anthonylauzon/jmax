/*
 * jMax
 * Copyright (C) 2004 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 *
 *  FTS audiofile API (reading and writing of audio files)
 *
 */
#include <string.h>
#include <stdio.h>
#include <fts/fts.h>
#include <ftsconfig.h>
#ifdef WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

#include <sndfile.h>

#ifndef TRUE
#define TRUE 1
#endif

/* sample formats */
fts_symbol_t fts_s_int8;
fts_symbol_t fts_s_int16;
fts_symbol_t fts_s_int24;
fts_symbol_t fts_s_int32;
fts_symbol_t fts_s_float32;
fts_symbol_t fts_s_float64;

/* file formats */
fts_symbol_t fts_s_audio;
fts_symbol_t fts_s_aiff;
fts_symbol_t fts_s_aif;
fts_symbol_t fts_s_wav;
fts_symbol_t fts_s_snd;
fts_symbol_t fts_s_au;
fts_symbol_t fts_s_sf;
fts_symbol_t fts_s_raw;
fts_symbol_t fts_s_sf;

static void
audiofile_set_format(fts_audiofile_t *audiofile, fts_symbol_t format)
{
  audiofile->sfinfo.format &= (SF_FORMAT_SUBMASK | SF_FORMAT_ENDMASK);
  
  if(format == fts_s_raw)
    audiofile->sfinfo.format |= SF_FORMAT_RAW;
  else if(format == fts_s_aiff)
    audiofile->sfinfo.format |= SF_FORMAT_AIFF;
  else if(format == fts_s_aif)
    audiofile->sfinfo.format |= SF_FORMAT_AIFF;
  else if(format == fts_s_wav)
    audiofile->sfinfo.format |= SF_FORMAT_WAV;
  else if(format == fts_s_snd)
    audiofile->sfinfo.format |= SF_FORMAT_AU;
  else if(format == fts_s_sf)
    audiofile->sfinfo.format |= SF_FORMAT_IRCAM;
  else
    audiofile->sfinfo.format |= SF_FORMAT_AIFF;
}

static void
audiofile_set_sample_format(fts_audiofile_t *audiofile, fts_symbol_t name)
{
  audiofile->sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_ENDMASK);
  
  if(name == fts_s_int8)
    audiofile->sfinfo.format |= SF_FORMAT_PCM_S8;
  else if(name == fts_s_int16)
    audiofile->sfinfo.format |= SF_FORMAT_PCM_16;
  else if(name == fts_s_int24)
    audiofile->sfinfo.format |= SF_FORMAT_PCM_24;
  else if(name == fts_s_int32)
    audiofile->sfinfo.format |= SF_FORMAT_PCM_32;
  else if(name == fts_s_float32)
    audiofile->sfinfo.format |= SF_FORMAT_FLOAT;
  else if(name == fts_s_float64)
    audiofile->sfinfo.format |= SF_FORMAT_DOUBLE;
}

#define ERRBUF_SIZE 1024
static char audiofile_errbuf[ERRBUF_SIZE];

static void 
audiofile_error(fts_audiofile_t *audiofile, const char *format, ...)
{
  va_list args; 

  va_start( args, format); 
  vsnprintf( audiofile_errbuf, ERRBUF_SIZE, format, args); 
  va_end( args); 

  audiofile->error = audiofile_errbuf;
}

static fts_audiofile_t *
audiofile_new(fts_symbol_t filename, fts_symbol_t mode)
{
  fts_audiofile_t *audiofile = (fts_audiofile_t *)fts_malloc(sizeof(fts_audiofile_t));

  audiofile->filename = filename;
  audiofile->mode = mode;
  audiofile->file_format = audiofile_file_format_null;
  audiofile->sample_format = audiofile_sample_format_null;
  memset(&audiofile->sfinfo, 0, sizeof(audiofile->sfinfo));
  audiofile->sfhandle = NULL;
  audiofile->error = NULL;
  
  return audiofile;
}

static int
audiofile_set_info(fts_audiofile_t *audiofile, int channels, int sample_rate, fts_symbol_t format, fts_symbol_t sample_format)
{
  if(format == NULL || format == fts_s_audio || format == fts_s_default)
  {
    char *suffix = strrchr((char *)fts_symbol_name(audiofile->filename), '.');
    
    if(suffix != NULL)
      format = fts_new_symbol(suffix + 1);
    else
      format = fts_s_aiff;        
  }
  
  if(sample_format == NULL)
    sample_format = fts_s_int16;
  
  if(channels > 0)
    audiofile->sfinfo.channels = channels;
  
  if(sample_rate > 0)
    audiofile->sfinfo.samplerate = sample_rate;

  audiofile_set_format(audiofile, format);
  audiofile_set_sample_format(audiofile, sample_format);
  
  return (sf_format_check(&audiofile->sfinfo) == SF_TRUE);
}

fts_audiofile_t *
fts_audiofile_open_read(fts_symbol_t name)
{
  const char *filename = (const char *)fts_symbol_name(name);
  char str[1024];
  char *fullpath = fts_file_find(filename, str, 1023);
  
  if(fullpath != NULL)
  {
    fts_audiofile_t *audiofile = audiofile_new(name, fts_s_read);
    
    audiofile->sfhandle = sf_open(fullpath, SFM_READ, &audiofile->sfinfo);
    
    if(audiofile->sfhandle != NULL)
      return audiofile;
    
    fts_free(audiofile);
  }
  
  return NULL;
}

fts_audiofile_t *
fts_audiofile_open_read_format(fts_symbol_t name, int channels, int sample_rate, fts_symbol_t format, fts_symbol_t sample_format)
{
  const char *filename = (const char *)fts_symbol_name(name);
  char str[1024];
  char *fullpath = fts_file_find(filename, str, 1023);
  
  if(fullpath != NULL)
  {
    fts_audiofile_t *audiofile = audiofile_new(name, fts_s_read);
    
    if(audiofile_set_info(audiofile, channels, sample_rate, format, sample_format))
    {
      audiofile->sfhandle = sf_open(fullpath, SFM_READ, &audiofile->sfinfo);
      
      if(audiofile->sfhandle != NULL)
        return audiofile;
    }
    
    fts_free(audiofile);
  }
  
  return NULL;
}

fts_audiofile_t * 
fts_audiofile_open_write (fts_symbol_t name, int channels, int sample_rate,
			  fts_symbol_t format, fts_symbol_t sample_format)
{
  char *filename = (char *)fts_symbol_name(name);
  char str[1024];
  char *fullpath = fts_make_absolute_path(NULL, filename, str, 1023);
  
  if(fullpath != NULL)
  {
    fts_audiofile_t *audiofile = audiofile_new(name, fts_s_write);
    
    if(audiofile_set_info(audiofile, channels, sample_rate, format, sample_format))
    {
      audiofile->sfhandle = sf_open(fullpath, SFM_WRITE, &audiofile->sfinfo);

      if(audiofile->sfhandle != NULL)
        return audiofile;
    }

    fts_free(audiofile);
  }
  
  return NULL;
}

void 
fts_audiofile_close(fts_audiofile_t *audiofile)
{
  if(audiofile != NULL)
  {
    sf_close(audiofile->sfhandle);
    fts_free(audiofile);
  }
}

int
fts_audiofile_read(fts_audiofile_t *audiofile, float **buf, int n_buf, int size)
{
  int n_channels = audiofile->sfinfo.channels;
  float *buffer = (float *)alloca(sizeof(float) * size * n_channels);
  int n, i, j, k;
  
  n = sf_read_float(audiofile->sfhandle, buffer, size * n_channels);
  
  /* un-interleave samples */
  for(i=0, k=0; k<n; i++, k+=n_channels)
  {
    for(j=0; j<n_buf; j++)
      buf[j][i] = buffer[k + j];
  }
  
  return n / n_channels;
}

int 
fts_audiofile_read_interleaved(fts_audiofile_t *audiofile, float *buf, int n_channels, int size)
{
  if(n_channels == audiofile->sfinfo.channels)
    return sf_read_float(audiofile->sfhandle, buf, size * n_channels) / n_channels;
  else 
    return 0;
}

int 
fts_audiofile_write(fts_audiofile_t *audiofile, float **buf, int n_buf, int size)
{
  int n_channels = audiofile->sfinfo.channels;
  float *buffer = (float *)alloca(sizeof(float) * size * n_channels);
  int n, i, j, k;
  
  if(n_buf > n_channels)
    n_buf = n_channels;
  
  /* interleave samples */
  for(i=0, k=0; i<size; i++, k+=n_channels)
  {
    for(j=0; j<n_buf; j++)
      buffer[k + j] = buf[j][i];

    for(; j<n_channels; j++)
      buffer[k + j] = 0.0;
}
  
  n = sf_write_float(audiofile->sfhandle, buffer, size * n_channels);
  
  return n / n_channels;
}

int 
fts_audiofile_write_interleaved(fts_audiofile_t *audiofile, float *buf, int n_channels, int size)
{
  if(n_channels == audiofile->sfinfo.channels)
    return sf_write_float(audiofile->sfhandle, buf, size * n_channels) / n_channels;
  else 
    return 0;
}

int 
fts_audiofile_seek(fts_audiofile_t *audiofile, int offset)
{
  /* not yet implemented */
  return -1;
}


void
fts_audiofile_import_handler(fts_class_t *cl, fts_method_t meth)
{
  fts_class_import_handler(cl, fts_new_symbol("audio"), meth);
  fts_class_import_handler(cl, fts_s_aiff, meth);
  fts_class_import_handler(cl, fts_s_aif, meth);
  fts_class_import_handler(cl, fts_s_wav, meth);
  fts_class_import_handler(cl, fts_s_snd, meth);
  fts_class_import_handler(cl, fts_s_raw, meth);
  fts_class_import_handler(cl, fts_s_sf, meth);
}


void
fts_audiofile_export_handler(fts_class_t *cl, fts_method_t meth)
{
  fts_class_import_handler(cl, fts_new_symbol("audio"), meth);
  fts_class_export_handler(cl, fts_s_aiff, meth);
  fts_class_export_handler(cl, fts_s_aif, meth);
  fts_class_export_handler(cl, fts_s_wav, meth);
  fts_class_export_handler(cl, fts_s_snd, meth);
  fts_class_export_handler(cl, fts_s_raw, meth);
  fts_class_export_handler(cl, fts_s_sf, meth);
}


FTS_MODULE_INIT(audiofile)
{
  fts_s_audio = fts_new_symbol("audio");
  fts_s_aiff = fts_new_symbol("aiff");
  fts_s_aif = fts_new_symbol("aif");
  fts_s_wav = fts_new_symbol("wav");
  fts_s_snd = fts_new_symbol("snd");
  fts_s_raw = fts_new_symbol("raw");
  fts_s_sf = fts_new_symbol("sf");

  fts_s_int8 = fts_new_symbol("int8");
  fts_s_int16 = fts_new_symbol("int16");
  fts_s_int24 = fts_new_symbol("int24");
  fts_s_int32 = fts_new_symbol("int32");
  fts_s_float32 = fts_new_symbol("float32");
  fts_s_float64 = fts_new_symbol("float64");
}
