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

#include "sys.h"
#include "lang.h"
#include "runtime.h"
#include <string.h>

/***************************************************************
 *
 *   soundfile formats
 *
 */

void
fts_soundfile_format_platform_init(void)
{
}

/***************************************************************
 *
 *   SGI soundfiles
 *     
 */

struct fts_soundfile
{
  char unused;
};

/*************************************************************
 *
 *  open/close for reading/writing single channel float buffers
 *
 */

fts_soundfile_t *fts_soundfile_open_read_float(fts_symbol_t file_name, fts_symbol_t format, double sr, int onset)
{
  char full_path[1024];
  const char *path = fts_symbol_name(file_name);

  /* find file in all possible locations */
  fts_file_get_read_path(path, full_path);

  if(format)
    {
      if(format == fts_s_float) /* read raw floats */
	{
	  /* *** set file format to raw floats and open soundfile file */
	}
      else /* float is only supported raw format for now */
	{
	  post("unsupported raw soundfile format: %s\n", fts_symbol_name(format));
	  return 0;
	}
    }
  else
    {
      /* *** open sound file reading the header */
    }
  
  /* return fts_soundfile "object" if successfull or 0 if not */
  if(0) /* *** open sucessfull */
    {  
      if(onset) /* skip offset */
	{
	  int n_skip;

	  /* skip */

	  if(n_skip < onset)
	    {
	      /* *** close file */
	      post("can not open soundfile %s with onset of %d samples\n", fts_symbol_name(file_name), onset);
	      return 0;
	    }
	}
      
      /* *** return fts_sounfile_t * */
      return 0;
    }
  else
    return 0;
}

fts_soundfile_t *fts_soundfile_open_write_float(fts_symbol_t file_name, fts_symbol_t format_name, double sr)
{
  char full_path[1024];
  const char *path = fts_symbol_name(file_name);
  int af_format;

  /* get full path of file location */
  fts_file_get_write_path(path, full_path);

  if(format_name)
    {
      /* *** set format */
    }
  else /* no format specified (use file extension or default format) */
    {
      char *extension = strrchr(fts_symbol_name(file_name), '.');
    }
  
  /* *** init common parameters (rate, channels, ...?) */
  /* *** open file */

  if(0)  /* *** open sucessfull */
    return 0;
  else
    {
      /* *** return fts_sounfile_t * */
      return 0;
    }
}

void fts_soundfile_close(fts_soundfile_t *soundfile)
{
  if(soundfile)
    {
      /* *** close file */
    }
}

/*****************************************************
 *
 *  read/write single channel float buffers
 *
 */

int 
fts_soundfile_read_float(fts_soundfile_t *soundfile, float *buffer, int size)
{
  if(soundfile)
    {
      int n_samples = 0;
      
      /* *** read buffer */
      
      if(n_samples > 0)
	return n_samples;
      else
	return 0;
    }
  else
    return 0;
}

int
fts_soundfile_write_float(fts_soundfile_t *soundfile, float *buffer, int size)
{
  if(soundfile)
    {
      int  n_samples = 0;

      /* *** write buffer */

      if(n_samples > 0)
	return n_samples;
      else
	return 0;
    }
  else
    return 0;
}

/* (fd) HACK !!!!!!!!!!!!!! */
int fts_soundfile_get_size(fts_soundfile_t *soundfile)
{
  return 1024;
}

double fts_soundfile_get_samplerate(fts_soundfile_t *soundfile)
{
  return 44.1;
}






















