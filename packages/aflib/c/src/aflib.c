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

/*
 * Implementation of audio file loader using Linux/SGI audiofile library
 *  - no virtual file format
 */

#include <fts/fts.h>

#include <stdarg.h>
#include <audiofile.h>

#define ERRBUF_SIZE 1024

static char aflib_loader_errbuf[ERRBUF_SIZE];

static void 
aflib_loader_error(fts_audiofile_t *audiofile, const char *format, ...)
{
  va_list args; 

  va_start( args, format); 
  vsnprintf( aflib_loader_errbuf, ERRBUF_SIZE, format, args); 
  va_end( args); 

  fts_audiofile_set_error(audiofile, aflib_loader_errbuf);

  fts_log( "[aflib] %s\n", aflib_loader_errbuf);
}

typedef struct
{
  AFfilehandle af_file;
  void *buffer;
  unsigned int buffer_length;
  unsigned int alloc_size;
} aflib_handle_t;

static aflib_handle_t *
aflib_handle_alloc(void)
{
  aflib_handle_t *handle = (aflib_handle_t *)fts_malloc(sizeof(aflib_handle_t));
  
  handle->af_file = NULL;
  handle->buffer = NULL;
  handle->buffer_length = 0;  
  handle->alloc_size = 0;  

  return handle;
}

static void
aflib_handle_free(aflib_handle_t *handle)
{
  if(handle->buffer)
    fts_free(handle->buffer);

  fts_free(handle);
}

static int 
aflib_loader_open_read(fts_audiofile_t* audiofile)
{
  const char *filename = fts_audiofile_get_filename(audiofile);
  aflib_handle_t *handle = aflib_handle_alloc();
  AFfilehandle af_file;
  int file_sample_format;
  int file_sample_width;

  if ((af_file = afOpenFile(filename, "r", NULL)) == AF_NULL_FILEHANDLE)
    {
      aflib_loader_error(audiofile, "Failed to open file %s", filename);
      return -1;
    }

  afGetSampleFormat( af_file, AF_DEFAULT_TRACK, &file_sample_format, &file_sample_width);

  if ((file_sample_format != AF_SAMPFMT_TWOSCOMP) || (file_sample_width != 16))
    {
      aflib_loader_error(audiofile, "Invalid file format (must be signed 16 bits)");
      afCloseFile(af_file);
      return -1;
    }

  handle->af_file = af_file;

  fts_audiofile_set_handle(audiofile, handle);
  fts_audiofile_set_num_channels(audiofile, afGetChannels( af_file, AF_DEFAULT_TRACK));
  fts_audiofile_set_sample_rate(audiofile, (int)afGetRate( af_file, AF_DEFAULT_TRACK));
  fts_audiofile_set_sample_format_by_name(audiofile, fts_s_int16);
  fts_audiofile_set_num_frames(audiofile, afGetFrameCount(af_file, AF_DEFAULT_TRACK));

  return 0;
}

static int 
aflib_loader_open_write(fts_audiofile_t* audiofile)
{
  const char *filename = fts_audiofile_get_filename(audiofile);
  aflib_handle_t *handle = aflib_handle_alloc();
  AFfilehandle af_file;
  AFfilesetup af_setup;

  
  af_setup = afNewFileSetup();
  
  afInitSampleFormat(af_setup, AF_DEFAULT_TRACK,
		     fts_audiofile_get_sample_format(audiofile),
		     fts_audiofile_get_bytes_per_sample(audiofile));
  afInitRate(af_setup, AF_DEFAULT_TRACK, (double)fts_audiofile_get_sample_rate(audiofile));
  afInitChannels(af_setup, AF_DEFAULT_TRACK, fts_audiofile_get_num_channels(audiofile));
  

  if ((af_file = afOpenFile(filename, "w", af_setup)) == AF_NULL_FILEHANDLE)
  {
      aflib_loader_error(audiofile, "Failed to open file %s", filename);
      return -1;
  }
  handle->af_file = af_file;
  fts_audiofile_set_handle(audiofile, handle);

  afFreeFileSetup(af_setup);
  return 0;
}


static int 
aflib_loader_buffer_length(fts_audiofile_t* audiofile, unsigned int length)
{
  aflib_handle_t *handle = (aflib_handle_t *)fts_audiofile_get_handle(audiofile);
  int size;

  if(length <= 0)
    length = 1024; /* default buffer size */

  size = length * fts_audiofile_get_bytes_per_sample(audiofile) * fts_audiofile_get_num_channels(audiofile);

  if(size > handle->alloc_size)
    {
      handle->buffer = fts_realloc(handle->buffer, size);
      handle->alloc_size = size;
    }

  handle->buffer_length = length;

  return 0;
}

static int 
aflib_loader_read(fts_audiofile_t* audiofile, float **buf, int n_buf, unsigned int buflen)
{
  aflib_handle_t *handle = (aflib_handle_t *)fts_audiofile_get_handle(audiofile);
  int file_channels = fts_audiofile_get_num_channels(audiofile);
  short *buffer = (short *)handle->buffer;
  unsigned int buffer_length = handle->buffer_length;
  int n_total = 0;
  int n_read = 0;
  int ch, n_ch;

  if(n_buf > file_channels)
    n_ch = file_channels;
  else
    n_ch = n_buf;
  
  do
    {
      n_read = afReadFrames(handle->af_file, AF_DEFAULT_TRACK, buffer, buffer_length);
      
      for(ch=0; ch<n_ch; ch++)
	{
	  float *out = buf[ch] + n_total;
	  int j = ch;
	  int i;
	  
	  for(i=0; i<n_read; i++)
	    {
	      out[i] = (float)buffer[j] / 32767.0f;
	      j += file_channels;
	    }
	}
      
      n_total += n_read;
      
    } while(n_read > 0 && n_total < buflen);
  
  /* fill remaining buffers with zeroes */
  for(; ch<n_buf; ch++)
    {
      float *out = buf[ch];
      int i;
      
      for(i=0; i<buflen; i++)
	out[i] = 0.0;
    }
  
  return n_total;
}

static int 
aflib_loader_write(fts_audiofile_t* audiofile, float** buf, int n_buf, unsigned int buflen)
{
  aflib_handle_t *handle = (aflib_handle_t *)fts_audiofile_get_handle(audiofile);
  int file_channels = fts_audiofile_get_num_channels(audiofile);
  short *buffer = (short *)handle->buffer;
  unsigned int buffer_length = handle->buffer_length;
  int n_total = 0;
  int n_write = 0;
  int ch, n_ch;

  if(n_buf > file_channels)
      n_ch = file_channels;
  else
      n_ch = n_buf;
  
  do
  {
      for(ch=0; ch<n_ch; ch++)
      {
	  float *in = buf[ch] + n_total;
	  int j = ch;
	  int i;
	  
	  for(i=0; i < buffer_length; i++)
	  {
	      buffer[j] = (short)(in[i] * 32767.0f);
	      j += file_channels;
	  }
      }
      
      n_write = afWriteFrames(handle->af_file, AF_DEFAULT_TRACK, buffer, buffer_length);
      
      
      n_total += n_write;
      
    } while(n_write > 0 && n_total < buflen);
  
  
  return n_total;

}

static int 
aflib_loader_seek(fts_audiofile_t* audiofile, unsigned int offset)
{
  /* not yet implemented */
  return -1;
}

static int 
aflib_loader_close(fts_audiofile_t* audiofile)
{
  aflib_handle_t *handle = (aflib_handle_t *)fts_audiofile_get_handle( audiofile);

  if(handle)
    {
      if(handle->af_file)
	afCloseFile(handle->af_file);

      aflib_handle_free(handle);
    }

  return 0;
}

static fts_audiofile_loader_t 
aflib_loader = { 
  aflib_loader_open_write,
  aflib_loader_open_read,
  aflib_loader_buffer_length,
  aflib_loader_write,
  aflib_loader_read,
  aflib_loader_seek,
  aflib_loader_close,
};

void aflib_config(void)
{
  fts_audiofile_set_loader( "Linux/SGI Audiofile library", &aflib_loader);
}
