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

#include <Carbon/Carbon.h>
#include <fts/fts.h>

#define ERRBUF_SIZE 1024

static char qtfile_loader_errbuf[ERRBUF_SIZE];

static void 
qtfile_loader_error(fts_audiofile_t *audiofile, const char *format, ...)
{
  va_list args; 

  va_start( args, format); 
  vsnprintf( qtfile_loader_errbuf, ERRBUF_SIZE, format, args); 
  va_end( args); 

  fts_audiofile_set_error(audiofile, qtfile_loader_errbuf);

  fts_log( "[qtfile] %s\n", qtfile_loader_errbuf);
}

typedef struct
{
  short file_ref;
  void *buffer;
  unsigned int buffer_length;
  unsigned int alloc_size;
} qtfile_handle_t;

static qtfile_handle_t *
qtfile_handle_alloc(void)
{
  qtfile_handle_t *handle = (qtfile_handle_t *)fts_malloc(sizeof(qtfile_handle_t));
  
  handle->file_ref = 0;
  handle->buffer = NULL;
  handle->buffer_length = 0;  
  handle->alloc_size = 0;  

  return handle;
}

static void
qtfile_handle_free(qtfile_handle_t *handle)
{
  if(handle->buffer)
    fts_free(handle->buffer);

  fts_free(handle);
}

static int 
qtfile_loader_open_read(fts_audiofile_t* audiofile)
{
  const char *filename = fts_audiofile_get_filename(audiofile);
  qtfile_handle_t *handle = qtfile_handle_alloc();
  OSStatus err = noErr;
  FSRef fs_ref;
  FSSpec fs_spec;
  short file_ref;
  SoundComponentData descr;
  unsigned long n_frames;
  unsigned long offset;
  
  if(FSPathMakeRef(filename, &fs_ref, NULL) != noErr ||
     FSGetCatalogInfo(&fs_ref, kFSCatInfoNone, NULL, NULL, &fs_spec, NULL) != noErr ||
     FSpOpenDF(&fs_spec, fsRdPerm, &file_ref) != noErr)
    {
      qtfile_loader_error(audiofile, "Failed to open file %s", filename);
      return -1;
    }
  
  err = ParseAIFFHeader(file_ref, &descr, &n_frames, &offset);
  if(err != noErr)
    {
      qtfile_loader_error(audiofile, "Failed to open file %s", filename);
      return -1;
    }
  
  if(descr.format != k16BitBigEndianFormat)
    {
      qtfile_loader_error(audiofile, "Invalid file format (must be signed 16 bits)");
      return -1;
    }
  
  /* seek file to beginning of sample data */
  err = SetFPos(file_ref, fsFromStart, offset);
  if(err != noErr)
    {
      qtfile_loader_error(audiofile, "Failed to seek file %s to beginning of sample data", filename);
      return -1;
    }

  handle->file_ref = file_ref;

  fts_audiofile_set_handle(audiofile, handle);
  fts_audiofile_set_num_channels(audiofile, descr.numChannels);
  fts_audiofile_set_sample_rate(audiofile, descr.sampleRate);
  fts_audiofile_set_bytes_per_sample(audiofile, descr.sampleSize / 8);
  fts_audiofile_set_num_frames(audiofile, n_frames);

  return 0;
}

static int 
qtfile_loader_open_write(fts_audiofile_t* audiofile)
{
  /* not yet implemented */
  return -1;
}

static int 
qtfile_loader_buffer_length(fts_audiofile_t* audiofile, unsigned int length)
{
  qtfile_handle_t *handle = (qtfile_handle_t *)fts_audiofile_get_handle(audiofile);
  int size;

  if(length <= 0)
    length = 1024; /* default buffer size */

  size = length * fts_audiofile_get_bytes_per_sample(audiofile) * fts_audiofile_get_num_channels(audiofile);

  if(size && size > handle->alloc_size)
    {
      handle->buffer = fts_realloc(handle->buffer, size);
      handle->alloc_size = size;
    }

  handle->buffer_length = length;

  return 0;
}

static int 
qtfile_loader_read(fts_audiofile_t* audiofile, float **buf, int n_buf, unsigned int buflen)
{
  qtfile_handle_t *handle = (qtfile_handle_t *)fts_audiofile_get_handle(audiofile);
  int file_channels = fts_audiofile_get_num_channels(audiofile);
  int bytes_per_sample = fts_audiofile_get_bytes_per_sample(audiofile);
  int bytes_per_frame = bytes_per_sample * file_channels;
  short file_ref = handle->file_ref;
  short *buffer = (short *)handle->buffer;
  unsigned int buffer_length = handle->buffer_length;
  int n_total = 0;
  SInt32 n_read = 0;
  int ch, n_ch;
  
  if(n_buf > file_channels)
    n_ch = file_channels;
  else
    n_ch = n_buf;
  
  do
    {
      if(n_total + buffer_length > buflen)
	buffer_length = buflen - n_total;
      
      /* bytes to read */
      n_read = buffer_length * bytes_per_frame;

      /* read bytes */
      FSRead(file_ref, &n_read, buffer);

      /* frames read*/
      n_read /= bytes_per_frame;

      /* convert */
      for(ch=0; ch<n_ch; ch++)
	{
	  float *out = buf[ch] + n_total;
	  int j = ch;
	  int i;
	  
	  for(i=0; i<n_read; i++)
	    {
	      out[i] = (float)buffer[j] / 32767.0f;
	      j += file_channels;;
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
qtfile_loader_write(fts_audiofile_t* audiofile, float** buf, int n_buf, unsigned int buflen)
{
  /* not yet implemented */
  return -1;
}

static int 
qtfile_loader_seek(fts_audiofile_t* audiofile, unsigned int offset)
{
  /* not yet implemented */
  return -1;
}

static int 
qtfile_loader_close(fts_audiofile_t* audiofile)
{
  qtfile_handle_t *handle = (qtfile_handle_t *)fts_audiofile_get_handle(audiofile);

  if(handle)
    {
      if(handle->file_ref)
	FSClose(handle->file_ref);

      qtfile_handle_free(handle);
    }

  return 0;
}

static fts_audiofile_loader_t 
qtfile_loader = 
{ 
  qtfile_loader_open_write,
  qtfile_loader_open_read,
  qtfile_loader_buffer_length,
  qtfile_loader_write,
  qtfile_loader_read,
  qtfile_loader_seek,
  qtfile_loader_close,
};

void qtfile_config(void)
{
  fts_audiofile_set_loader( "QuickTime Audiofiles", &qtfile_loader);
}
