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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sys.h"
#include "lang.h"
#include "runtime/files/soundfiles.h"
#include "runtime/files/soundformats.h"
#include "runtime/STtools.h"

/***************************************************************
 *
 *   init soundfile formats (for writing)
 *
 */

void
fts_soundfile_format_platform_init(void)
{
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_aiffc), SF_AIFC); 
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_aiff), SF_AIFF);
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_wav), SF_WAV);
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_bicsf), SF_MACHINE);

  fts_soundfile_format_set_default(fts_s_aiff);
}

/* converting the symbols for supported file formats into 
   the corresponding SGI audio file type integers 
   (returns AF_FILE_UNKNOWN for unsupported symbols) */

static int
sttools_get_format(fts_symbol_t name)
{
  fts_atom_t *descr;

  descr = fts_soundfile_format_get_descriptor(name);
 
  if(descr)
    return fts_get_int(descr);
  else
    return SF_UNKNOWN;
}

/***************************************************************
 *
 *   SGI soundfiles
 *     
 */

struct fts_soundfile
{
  int fd;
  SFHEADER *header;
  fts_srconv_t *srconv;
};

/*****************************************************
 *
 *  local utilities
 *
 */

/* making a new FTS soundfile "object from an SGI AFhandle 
   and freeing it (after the file is closed) */

static fts_soundfile_t *
sttools_soundfile_new(int fd, SFHEADER *header)
{
  fts_soundfile_t *sf = fts_malloc(sizeof(fts_soundfile_t));

  sf->fd = fd;

  if(header)
    {
      sf->header = fts_malloc(sizeof(SFHEADER));
      *sf->header = *header;
    }
  else
    sf->header = 0;

  sf->srconv = 0;

  return sf;
}

static void
sttools_soundfile_delete(fts_soundfile_t *sf)
{
  if(sf->header)
    fts_free(sf->header);

  fts_free(sf);
}

static int
sttools_soundfile_set_srconv(fts_soundfile_t *sf, double ratio, int size)
{
  fts_srconv_t *srconv;

  if(sf->srconv)
    fts_srconv_delete(sf->srconv);

  srconv = fts_srconv_new(ratio, size, 1);
  if(srconv)
    {
      sf->srconv = srconv;
      return 1;
    }
  else
      return 0;
}

/*************************************************************
 *
 *  open/close for reading/writing single channel float buffers
 *
 */

fts_soundfile_t *
fts_soundfile_open_read_float(fts_symbol_t file_name, fts_symbol_t format, float sr, int onset)
{
  char full_path[1024];
  const char *path = fts_symbol_name(file_name);
  fts_soundfile_t *sf;

  /* find file in all possible locations */
  if(!fts_file_get_read_path(path, full_path))
    return 0;

  if(format)
    {
      /* open to read raw floats */
      if(format == fts_s_float)
	{
	  int byte_onset = onset * sizeof(float);
	  int did_onset = 0;
	  int fd;

	  /* open raw file */
	  fd = open(full_path, O_RDONLY);
	  if(fd < 0)
	    return 0;
	  
	  /* skip onset */
	  did_onset = lseek(fd, byte_onset, SEEK_SET);
	  if(did_onset != byte_onset)
	    {
	      close(fd);
	      post("onset failed: %d samples\n", onset);
	      return 0;
	    }
	  else
	    sf = sttools_soundfile_new(fd, 0);
	}
      else /* no other supported raw format than float for now */
	{
	  post("unsupported raw soundfile format: %s\n", fts_symbol_name(format));
	  return 0;
	}
    }
  else
    {
      /* open file, read header and return soundfile handle */

      SFHEADER header;
      int fd;
      float sr_file;

      /* open sound file reading the header */
      fd = STreadopensf(full_path, &header);
      sf = sttools_soundfile_new(fd, &header);

      sr_file = (float)sfsrate(&header);

      /* inplicit sampling rate conversion */
      if(sr > 0.0 && sr_file != sr)
	{
	 double ratio = sr / sr_file;
	 int read_buffer_size = SF_BUFSIZE / sfclass(&header);
	 
	 /* (try to) instantiate the sr conversion buffer onbject */
	 if(!sttools_soundfile_set_srconv(sf, ratio, read_buffer_size))
	   {
	     post("sample rate conversion ratio out of bounds: (%f/%f)\n", sr, sr_file);
	     sttools_soundfile_delete(sf);
	     STclose(fd, &header);
	     return 0;
	   }
	}
      
      /* skip onset */
      if(STseekToSample(fd, &header, onset) < 0)
	{
	  post("onset of failed: %d samples\n", onset);
	  STclose(fd, &header);
	  sttools_soundfile_delete(sf);
	  return 0;
	}
    }
  
  return sf;
}

fts_soundfile_t *
fts_soundfile_open_write_float(fts_symbol_t file_name, fts_symbol_t format_name, float sr)
{
  char full_path[1024];
  const char *path = fts_symbol_name(file_name);
  int sf_format;

  /* get full path of file location */
  fts_file_get_write_path(path, full_path);

  if(format_name)
    {
      /* open to write raw floats */
      if(format_name == fts_s_float)
	{
	  int fd;
	  
	  fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	  if(fd < 0)
	    return 0;
	  else
	    return sttools_soundfile_new(fd, 0);
	}
      else
	{
	  /* set specified file format or exit when unknown */
	  sf_format = sttools_get_format(format_name);	  

	  if(sf_format == SF_UNKNOWN)
	    {
	      post("unknown soundfile format: %s\n", fts_symbol_name(format_name));
	      return 0;
	    }
	}
    }
  else
    {
      /* no format specified (use file extension or default format) */

      char *extension = strrchr(fts_symbol_name(file_name), '.');
      
      if(extension)
	sf_format = sttools_get_format(fts_new_symbol(extension + 1));
      else
	sf_format = SF_AIFF; /* set default format */
    }

  /* open file for give format, write header and return sounfile handle */
  {
    SFHEADER header;
    int fd;    
    
    /* set header */
    sfmagic1(&header) = SF_MAGIC1;
    sfmagic2(&header) = SF_MAGIC2;
    sfmachine(&header) = sf_format;
    sfsrate(&header) = sr;
    sfchans(&header) = 1;
    sfclass(&header) = SF_16bit;	  
    
    fd = STwriteopensf(full_path, &header);
    
    if(fd < 0)
      return 0;
    else
      return sttools_soundfile_new(fd, &header);
  }
}

void fts_soundfile_close(fts_soundfile_t *sf)
{
  if(sf)
    {
      if(sf->header)
	STclose(sf->fd, sf->header);
      else
	close(sf->fd);

      sttools_soundfile_delete(sf);
    }
}

/*****************************************************
 *
 *  read/write single channel float buffers
 *
 */

int 
fts_soundfile_read_float(fts_soundfile_t *sf, float *buffer, int size)
{
  if(sf)
    {
      if(sf->header)
	{
	  float read_buffer[SF_BUFSIZE];
	  int read_buffer_size = SF_BUFSIZE / sfclass(sf->header);
	  fts_srconv_t *srconv = sf->srconv;
	  int n_stored = 0;
	  
	  while(n_stored < size)
	    {
	      int n_read = size - n_stored; /* samples left to read */
	      int n_got;
	      
	      if(n_read > read_buffer_size)
		n_read =  read_buffer_size;
	      
	      n_got = STreadSampBuffer(sf->fd, sf->header, (void *)read_buffer, n_read);
	      
	      if(n_got <= 0)
		break;
	      
	      STconvertToFloatBuffer(sf->header, (void *)read_buffer, n_got);
	      
	      if(srconv)
		{
		  /* convert sample rate from read_buffer to buffer */
		  n_stored += fts_srconv(srconv, read_buffer, buffer + n_stored, n_got, size - n_stored, 1);
		}
	      else
		{
		  int i;

		  /* just copy from read_buffer to buffer */
		  for(i=0; i<n_got; i++) 
		    (buffer + n_stored)[i] = read_buffer[i];

		  n_stored += n_got;
		}
	    }
	  
	  return n_stored;
	}
      else
	return (read(sf->fd, buffer, size * sizeof(float)) / sizeof(float));	  
    }
  else
    return 0;
}

int
fts_soundfile_write_float(fts_soundfile_t *sf, float *buffer, int size)
{
  if(sf)
    {
      if(sf->header)
	{
	  float write_buffer[SF_BUFSIZE];
	  int write_buffer_size = SF_BUFSIZE / sfclass(sf->header);
	  int n_wrote = 0;
	  int i;
	  
	  while(n_wrote < size)
	    {
	      int n_write = size - n_wrote; /* samples left to write */
	      int n_wrote_now;
	      
	      if(n_write > write_buffer_size)
		n_write = write_buffer_size;
	      
	      /* well, stupid, but... */
	      for(i=0; i<n_write; i++)
		write_buffer[i] = (buffer + n_wrote)[i];
	      
	      STconvertFromFloatBuffer(sf->header, (void *)write_buffer, n_write);
	      
	      n_wrote_now = STwriteSampBuffer(sf->fd, sf->header, (void *)write_buffer, n_write);
	      
	      if(n_wrote_now <= 0)
		break;
	      
	      n_wrote += n_wrote_now;
	    }

	  return n_wrote;
	}
      else
	return (write(sf->fd, buffer, size * sizeof(float)) / sizeof(float));	  
    }
  else
    return 0;
}
