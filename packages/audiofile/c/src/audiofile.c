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

static char audiofile_loader_errbuf[ERRBUF_SIZE];

static char *audiofile_loader_error( fts_audiofile_t *audiofile)
{
  return audiofile_loader_errbuf;
}

static void audiofile_loader_log( const char *format, ...)
{
  va_list args; 

  va_start( args, format); 
  vsnprintf( audiofile_loader_errbuf, ERRBUF_SIZE, format, args); 
  va_end( args); 

  fts_log( "[audiofile] %s\n", audiofile_loader_errbuf);
}

/*----------------------------------------------------------------------
 *
 * handle stored in fts_audiofile_t structure
 *
 */

typedef struct afdata {
  AFfilehandle af_handle;
  void *buffer;
  unsigned int buffer_size;
} afdata_t;

static afdata_t *afdata_new( AFfilehandle af_handle)
{
  afdata_t *afdata;

  afdata = (afdata_t *)fts_malloc( sizeof( afdata_t));  
  afdata->af_handle = af_handle;
  afdata->buffer_size = 32768;
  /* FIXME: use the sample format to determine buffer size */
  afdata->buffer = fts_malloc( afdata->buffer_size);

  return afdata;
}

/*----------------------------------------------------------------------
 *
 * file open read
 *
 */
int audiofile_loader_open_read(fts_audiofile_t* audiofile)
{
  const char *filename;
  AFfilehandle af_handle;
  int file_sample_format, file_sample_width;
  afdata_t *afdata;

  filename = fts_symbol_name( fts_audiofile_get_filename(audiofile) );

  if ( (af_handle = afOpenFile( filename, "r", NULL)) == AF_NULL_FILEHANDLE)
    {
      audiofile_loader_log( "Failed to open file %s", filename);
      return -1;
    }

  afGetSampleFormat( af_handle, AF_DEFAULT_TRACK, &file_sample_format, &file_sample_width);

  if ((file_sample_format != AF_SAMPFMT_TWOSCOMP) || (file_sample_width != 16))
    {
      audiofile_loader_log( "Invalid file format (must be signed 16 bits)");
      return -1;
    }

  afdata = afdata_new( af_handle);
  fts_audiofile_set_handle( audiofile, afdata);
  fts_audiofile_set_sample_rate( audiofile, (int)afGetRate( afdata->af_handle, AF_DEFAULT_TRACK));
  fts_audiofile_set_num_channels( audiofile, afGetChannels( afdata->af_handle, AF_DEFAULT_TRACK));
  fts_audiofile_set_format( audiofile, fts_s_16bits);

  return 0;
}

/*----------------------------------------------------------------------
 *
 * file open write
 *
 */
int audiofile_loader_open_write(fts_audiofile_t* aufile)
{
  /* not yet implemented */
  return -1;
}

/*----------------------------------------------------------------------
 *
 * file reading
 *
 */
int audiofile_loader_read( fts_audiofile_t* audiofile, float **buf, int nbuf, unsigned int buflen)
{
  afdata_t *afdata = (afdata_t *)fts_audiofile_get_handle( audiofile);
  int n_read, channels, file_channels, samples, ch, i, j;
  short *buffer;

  buffer = (short *)afdata->buffer;
  /* FIXME: must reallocate buffer if buflen >= afdata->buffer_size */
  n_read = afReadFrames( afdata->af_handle, AF_DEFAULT_TRACK, afdata->buffer, buflen);

  file_channels = fts_audiofile_get_num_channels( audiofile);
  if (nbuf > file_channels)
    channels = file_channels;
  else
    channels = nbuf;

  if (buflen >= n_read)
    samples = n_read;
  else
    samples = buflen;

  for ( ch = 0; ch < channels; ch++)
    {
      float *out = buf[ch];

      j = ch;
      for ( i = 0; i < samples; i++)
	{
	  short s0 = buffer[j];

	  out[i] = (float)s0 / 32767.0f;
	  j += file_channels;
	}

      /* fill the rest of the buffer with zeroes */
      for ( ; i < buflen; i++)
	out[i] = 0.0;
    }

  /* fill remaining buffers with zeroes */
  for ( ; ch < nbuf; ch++)
    {
      float *out = buf[ch];

      for ( i = 0; i < buflen; i++)
	out[i] = 0.0;
    }

  return n_read;
}

/*----------------------------------------------------------------------
 *
 * file writing
 *
 */
int audiofile_loader_write(fts_audiofile_t* aufile, float** buf, int nbuf, unsigned int buflen)
{
  /* not yet implemented */
  return -1;
}

/*----------------------------------------------------------------------
 *
 * file seek
 *
 */
int audiofile_loader_seek(fts_audiofile_t* aufile, unsigned int offset)
{
  /* not yet implemented */
  return -1;
}

/*----------------------------------------------------------------------
 *
 * file close
 *
 */
int audiofile_loader_close(fts_audiofile_t* aufile)
{
  afdata_t *afdata = (afdata_t *)fts_audiofile_get_handle( audiofile);

  return afCloseFile( afdata->af_handle);
}

/*----------------------------------------------------------------------
 *
 * audiofile loader installation
 *
 */

static fts_audiofile_loader_t audiofile_loader = { 
  audiofile_loader_open_write,
  audiofile_loader_open_read,
  audiofile_loader_write,
  audiofile_loader_read,
  audiofile_loader_seek,
  audiofile_loader_close,
  audiofile_loader_error
};

void audiofile_config(void)
{
  fts_audiofile_set_loader( "Linux/SGI Audiofile library", &audiofile_loader);
}
