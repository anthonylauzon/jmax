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

/*
 * Modified by mdc to work on Linux:
 *
 * 1- suppressed all the dm calls: they are not part of the audio library
 *    and they will never be; substituted with af equivalent calls.
 * 2- Include <audiofile.h>, and not <dmedia/audiofile.h>; they are the same,
 *    also if this is not very clear from the sgi doc.
 * 3- Renamed to af.c, and local functions renamed from sgiaf_ to fts_af_.
 * 
 * 4- Added in platform.h the #define HAVE_AF_VIRTUAL_PARAMETERS to platforms
 *    that support the setVirtual* library of functions in the AF (i.e. SGI :).
 *    Used also in other file, like sfdev.c; used here to choose.
 */


#include <audiofile.h>
#include <string.h>

#include "sys.h"
#include "lang.h"
#include "runtime/files/soundfiles.h"
#include "runtime/files/soundformats.h"

/***************************************************************
 *
 *   SGI soundfile formats
 *
 */

void
fts_soundfile_format_platform_init(void)
{
  afSetErrorHandler(NULL);	/* avoid stupid error printing in the af library */

  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_aiffc), AF_FILE_AIFFC); 
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_aiff), AF_FILE_AIFF);
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_next), AF_FILE_NEXTSND);
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_bicsf), AF_FILE_BICSF);

#ifdef AF_FILE_SOUNDESIGNER2
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_sdII), AF_FILE_SOUNDESIGNER2);
#endif

  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_float), AF_FILE_RAWDATA);

  fts_soundfile_format_set_default(fts_s_aiff);
}

/* converting the symbols for supported file formats into 
   the corresponding SGI audio file type integers 
   (returns AF_FILE_UNKNOWN for unsupported symbols) */

static int
fts_af_get_format(fts_symbol_t name)
{
  fts_atom_t *descr;

  descr = fts_soundfile_format_get_descriptor(name);
 
  if (descr)
    return fts_get_int(descr);
  else
    return AF_FILE_UNKNOWN;
}

/***************************************************************
 *
 *   SGI soundfiles
 *     
 */

struct fts_soundfile
{
  AFfilehandle af_handle;
};

/*****************************************************
 *
 *  local utilities
 *
 */

/* making a new FTS soundfile "object from an SGI AFhandle 
   and freeing it (after the file is closed) */

static fts_soundfile_t *
fts_af_soundfile_new(AFfilehandle af_handle)
{
  fts_soundfile_t *sf = fts_malloc(sizeof(fts_soundfile_t));
  sf->af_handle = af_handle;
  
  return sf;
}

static void
fts_af_soundfile_free(fts_soundfile_t *sf)
{
  fts_free(sf);
}


/* setting internal (virtual) format to float (-1.0 ... 1.0) 
   (and maybe a rate to convert to) */

#ifdef HAVE_AF_VIRTUAL_PARAMETERS 
static void
fts_af_internal_format_set_float(AFfilehandle af_handle, float sr)
{
  /* Set the internal (virtual) audio format parameters */

  afSetVirtualSampleFormat(af_handle, AF_DEFAULT_TRACK, AF_SAMPFMT_FLOAT, 32);
  afSetVirtualChannels(af_handle, AF_DEFAULT_TRACK, 1);
  
  if (sr > 0.0f)
    afSetVirtualRate(af_handle, AF_DEFAULT_TRACK,(double)sr);

  afSetVirtualPCMMapping(af_handle, AF_DEFAULT_TRACK, 1.0, 0.0, 0.0, 0.0);
}
#endif

/* setting file format to read or write raw floats like internal format
   (no rate set) */

static void
fts_af_file_format_set_float(AFfilesetup af_setup)
{
  afInitFileFormat(af_setup, AF_FILE_RAWDATA);

  /* create list of file format parameters */
  
  afInitSampleFormat(af_setup, AF_DEFAULT_TRACK, AF_SAMPFMT_FLOAT, 32);
  afInitChannels(af_setup, AF_DEFAULT_TRACK, 1);
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
  AFfilesetup af_setup = afNewFileSetup();
  AFfilehandle af_handle;

  /* find file in all possible locations */
  if (!fts_file_get_read_path(path, full_path))
     return 0;

  if (format)
    {
      if (format == fts_s_float) /* read raw floats */
	{
	  /* set file format to raw floats and open soundfile file */
	  fts_af_file_format_set_float(af_setup);
	  af_handle = afOpenFile(full_path, "r", af_setup); /* open file */
	}
      else /* float is only supported raw format for now */
	{
	  post("unsupported raw soundfile format: %s\n", fts_symbol_name(format));
	  return 0;
	}
    }
  else
    {
      /* open sound file reading the header */
      af_handle = afOpenFile(full_path, "r", NULL); /* open file */
    }
  
  afFreeFileSetup(af_setup);

  /* return fts_soundfile "object" if successfull or 0 if not */
  if (af_handle != AF_NULL_FILEHANDLE)
    {  
      if (onset)
	{
	  int n_skip = afSeekFrame(af_handle, AF_DEFAULT_TRACK, onset);
	  if (n_skip < onset)
	    {
	      afCloseFile(af_handle);
	      post("onset failed: %d samples\n", onset);
	      return 0;
	    }
	}
#ifdef HAVE_AF_VIRTUAL_PARAMETERS 
      fts_af_internal_format_set_float(af_handle, sr);
#endif
      return fts_af_soundfile_new(af_handle);
    }
  else
    return 0;
}

fts_soundfile_t *
fts_soundfile_open_write_float(fts_symbol_t file_name, fts_symbol_t format_name, float sr)
{
  char full_path[1024];
  const char *path = fts_symbol_name(file_name);
  AFfilesetup af_setup = afNewFileSetup();
  AFfilehandle af_handle;
  int af_format;

  /* get full path of file location */
  fts_file_get_write_path(path, full_path);

  if (format_name)
    {
      af_format = fts_af_get_format(format_name);

      switch(af_format)
	{
	case AF_FILE_UNKNOWN:
	  {
	    post("unknown soundfile format: %s\n", fts_symbol_name(format_name));
	    return 0;
	  }
	case AF_FILE_RAWDATA:
	  {
	    if (format_name == fts_s_float) /* set format to raw floats */
	      {
		afInitFileFormat(af_setup, AF_FILE_RAWDATA);
		fts_af_file_format_set_float(af_setup);
		break;
	      }
	  }
	default:
	  {
	    afInitFileFormat(af_setup, af_format);
	  }
	}
    }
  else /* no format specified (use file extension or default format) */
    {
      char *extension = strrchr(fts_symbol_name(file_name), '.');
      
      if (extension)
	af_format = fts_af_get_format(fts_new_symbol(extension + 1));
      else
	af_format = AF_FILE_UNKNOWN;
      
      if (af_format == AF_FILE_UNKNOWN)
	afInitFileFormat(af_setup, AF_FILE_AIFF);
      else
	afInitFileFormat(af_setup, af_format);
    }
  
  afInitRate(af_setup, AF_DEFAULT_TRACK, sr);
  afInitChannels(af_setup, AF_DEFAULT_TRACK, 1);
    
  af_handle = afOpenFile(full_path, "w", af_setup);

  afFreeFileSetup(af_setup);
  
  if (af_handle == AF_NULL_FILEHANDLE)
    return 0;
  else
    {
#ifdef HAVE_AF_VIRTUAL_PARAMETERS 
      fts_af_internal_format_set_float(af_handle, sr);
#endif
      return fts_af_soundfile_new(af_handle);
    }
}

void fts_soundfile_close(fts_soundfile_t *soundfile)
{
  if (soundfile)
    {
      afCloseFile(soundfile->af_handle);
      fts_af_soundfile_free(soundfile);
    }
}

/*****************************************************
 *
 *  read/write single channel float buffers
 *
 */

#ifdef HAVE_AF_VIRTUAL_PARAMETERS 
int fts_soundfile_read_float(fts_soundfile_t *soundfile, float *buffer, int size)
{
  if (soundfile)
    {
      int n_samples;
      
      n_samples = afReadFrames(soundfile->af_handle, AF_DEFAULT_TRACK, (void *)buffer, size);
      
      if (n_samples > 0)
	return n_samples;
      else
	return 0;
    }
  else
    return 0;
}

int fts_soundfile_write_float(fts_soundfile_t *soundfile, float *buffer, int size)
{
  if (soundfile)
    {
      int  n_samples;

      n_samples = afWriteFrames(soundfile->af_handle, AF_DEFAULT_TRACK, (void *)buffer, size);

      if (n_samples > 0)
	return n_samples;
      else
	return 0;
    }
  else
    return 0;
}
#else

/* Version without the virtual format; currently, work only 
   for ...
*/

#define READ_BLOCK_SIZE 1024

int fts_soundfile_read_float(fts_soundfile_t *soundfile, float *buffer, int size)
{
  if (soundfile)
    {
      int read;
      int sampfmt;
      int sampwidth;

      afGetSampleFormat(soundfile->af_handle, AF_DEFAULT_TRACK, &sampfmt, &sampwidth);

      if ((sampfmt == AF_SAMPFMT_TWOSCOMP) && (sampwidth == 16))
	{
	  int read_size;
	  short buf[READ_BLOCK_SIZE];

	  read = 0;

	  while (size > 0)
	    {
	      int i, n;

	      read_size = size > READ_BLOCK_SIZE ? READ_BLOCK_SIZE : size;

	      n = afReadFrames(soundfile->af_handle, AF_DEFAULT_TRACK, (void *)buf, read_size);

	      /* Exit in errors */

	      if (n <= 0)
		return read;
		
	      /* Convert to float */

	      for (i = 0; i < n; i++)
		buffer[read++] =  ((float) buf[i] / 32768.0f);


	      /* Exit in eofs */

	      if (n < read_size)
		return read;

	      /* Update the counter */

	      size -= n;
	    }

	  return read;
	}
      else
	{
	  /* Add other formats here */

	  return 0;
	}
    }
  else
    return 0;
}

#define WRITE_BLOCK_SIZE 1024

int fts_soundfile_write_float(fts_soundfile_t *soundfile, float *buffer, int size)
{
  if (soundfile)
    {
      int wrote;
      int sampfmt;
      int sampwidth;

      afGetSampleFormat(soundfile->af_handle, AF_DEFAULT_TRACK, &sampfmt, &sampwidth);

      if ((sampfmt == AF_SAMPFMT_TWOSCOMP) && (sampwidth == 16))
	{
	  short buf[WRITE_BLOCK_SIZE];

	  wrote = 0;

	  while (size > 0)
	    {
	      int i, n;
	      int frames;

	      frames = (size > WRITE_BLOCK_SIZE ? WRITE_BLOCK_SIZE : size);

	      /* Convert to short  */

	      for (i = 0; i < frames; i++)
		buf[wrote++] =  (short) (buffer[i] / 32768.0f);

	      n = afWriteFrames(soundfile->af_handle, AF_DEFAULT_TRACK, (void *)buf, frames);

	      if (n <= 0)
		return wrote;

	      /* Update the counter */

	      size -= n;
	    }

	  return wrote;
	}
      else
	{
	  /* Add other formats here */

	  return 0;
	}
    }
  else
    return 0;
}
#endif
