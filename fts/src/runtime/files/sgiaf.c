#include "sys.h"
#include "lang.h"
#include "runtime/files/soundfiles.h"
#include "runtime/files/soundformats.h"
#include <dmedia/audiofile.h>

/***************************************************************
 *
 *   SGI soundfile formats
 *
 */

void
fts_soundfile_format_platform_init(void)
{
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_aiffc), AF_FILE_AIFFC); 
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_aiff), AF_FILE_AIFF);
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_next), AF_FILE_NEXTSND);
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_bicsf), AF_FILE_BICSF);
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_sdII), AF_FILE_SOUNDESIGNER2);
  fts_set_int(fts_soundfile_format_get_descriptor(fts_s_float), AF_FILE_RAWDATA);
}

/* converting the symbols for supported file formats into 
   the corresponding SGI audio file type integers 
   (returns AF_FILE_UNKNOWN for unsupported symbols) */

static int
sgiaf_get_format(fts_symbol_t name)
{
  fts_atom_t *descr;

  descr = fts_soundfile_format_get_descriptor(name);
 
  if(descr)
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
sgiaf_soundfile_new(AFfilehandle af_handle)
{
  fts_soundfile_t *sf = fts_malloc(sizeof(fts_soundfile_t));
  sf->af_handle = af_handle;
  
  return sf;
}

static void
sgiaf_soundfile_free(fts_soundfile_t *sf)
{
  fts_free(sf);
}


/* setting internal (virtual) format to float (-1.0 ... 1.0) 
   (and maybe a rate to convert to) */

static void
sgiaf_internal_format_set_float(AFfilehandle af_handle, float sr)
{
  DMparams* af_params;
  
  /* create list of internal (virtual) audio format parameters */
  dmParamsCreate(&af_params);
  
  dmParamsSetInt(af_params, "DM_AUDIO_FORMAT", DM_AUDIO_FLOAT);
  dmParamsSetInt(af_params, "DM_AUDIO_CHANNELS", 1);

  if(sr > 0.0f)
    dmParamsSetFloat(af_params, "DM_AUDIO_RATE", (double)sr);

  dmParamsSetFloat(af_params, "DM_AUDIO_PCM_MAP_SLOPE", 1.0);
  dmParamsSetFloat(af_params, "DM_AUDIO_PCM_MAP_INTERCEPT", 0.0);
  dmParamsSetFloat(af_params, "DM_AUDIO_PCM_MAP_MAXCLIP", 0.0);
  dmParamsSetFloat(af_params, "DM_AUDIO_PCM_MAP_MINCLIP", 0.0);

  afSetVirtualFormatParams(af_handle, AF_DEFAULT_TRACK, af_params);
  
  /* destroy list */
  dmParamsDestroy(af_params);
}


/* setting file format to read or write raw floats like internal format
   (no rate set) */

static void
sgiaf_file_format_set_float(AFfilesetup af_setup)
{
  DMparams* af_params;
  
  afInitFileFormat(af_setup, AF_FILE_RAWDATA);

  /* create list of file format parameters */
  dmParamsCreate(&af_params);
  
  dmParamsSetInt(af_params, "DM_AUDIO_FORMAT", DM_AUDIO_FLOAT);
  dmParamsSetInt(af_params, "DM_AUDIO_CHANNELS", 1);
  dmParamsSetFloat(af_params, "DM_AUDIO_PCM_MAP_SLOPE", 1.0);
  dmParamsSetFloat(af_params, "DM_AUDIO_PCM_MAP_INTERCEPT", 0.0);
  dmParamsSetFloat(af_params, "DM_AUDIO_PCM_MAP_MAXCLIP", 0.0);
  dmParamsSetFloat(af_params, "DM_AUDIO_PCM_MAP_MINCLIP", 0.0);

  afInitFormatParams(af_setup, AF_DEFAULT_TRACK, af_params);

  /* destroy list */
  dmParamsDestroy(af_params);
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
  AFfilehandle af_handle;

  /* find file in all possible locations */
  fts_file_find(path, full_path);

  if(format)
    {
      if(format == fts_s_float) /* read raw floats */
	{
	  AFfilesetup af_setup = afNewFileSetup();

	  /* set file format to raw floats and open soundfile file */
	  sgiaf_file_format_set_float(af_setup);
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
  
  /* return fts_soundfile "object" if successfull or 0 if not */
  if(af_handle != AF_NULL_FILEHANDLE)
    {  
      if(onset)
	{
	  int n_skip = afSeekFrame(af_handle, AF_DEFAULT_TRACK, onset);
	  if(n_skip < onset)
	    {
	      afCloseFile(af_handle);
	      post("can not open soundfile %s with onset of %d samples\n", fts_symbol_name(file_name), onset);
	      return 0;
	    }
	}
      
      sgiaf_internal_format_set_float(af_handle, sr);
      return sgiaf_soundfile_new(af_handle);
    }
  else
    return 0;
}

fts_soundfile_t *
fts_soundfile_open_write_float(fts_symbol_t file_name, fts_symbol_t format_name, float sr)
{
  char full_path[1024];
  const char *path = fts_symbol_name(file_name);
  AFfilehandle af_handle;
  AFfilesetup af_setup = afNewFileSetup();
  int af_format;

  /* get full path of file location */
  fts_file_get_write_path(path, full_path);

  if(format_name)
    {
      af_format = sgiaf_get_format(format_name);

      switch(af_format)
	{
	case AF_FILE_UNKNOWN:
	  {
	    post("unknown format (%s) for soundfile %s \n", fts_symbol_name(format_name), fts_symbol_name(file_name));
	    return 0;
	  }
	case AF_FILE_RAWDATA:
	  {
	    if(format_name == fts_s_float) /* set format to raw floats */
	      {
		afInitFileFormat(af_setup, AF_FILE_RAWDATA);
		sgiaf_file_format_set_float(af_setup);
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
      
      if(extension)
	af_format = sgiaf_get_format(fts_new_symbol(extension + 1));
      else
	af_format = AF_FILE_UNKNOWN;
      
      if(af_format == AF_FILE_UNKNOWN)
	afInitFileFormat(af_setup, AF_FILE_AIFF);
      else
	afInitFileFormat(af_setup, af_format);
    }
  
  afInitRate(af_setup, AF_DEFAULT_TRACK, sr);
  afInitChannels(af_setup, AF_DEFAULT_TRACK, 1);
    
  af_handle = afOpenFile(full_path, "w", af_setup);

  if(af_handle == AF_NULL_FILEHANDLE)
    return 0;
  else
    {
      sgiaf_internal_format_set_float(af_handle, sr);
      return sgiaf_soundfile_new(af_handle);
    }
}

void fts_soundfile_close(fts_soundfile_t *soundfile)
{
  if(soundfile)
    {
      afCloseFile(soundfile->af_handle);
      sgiaf_soundfile_free(soundfile);
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
      int n_samples;
      
      n_samples = afReadFrames(soundfile->af_handle, AF_DEFAULT_TRACK, (void *)buffer, size);
      
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
      int  n_samples;

      n_samples = afWriteFrames(soundfile->af_handle, AF_DEFAULT_TRACK, (void *)buffer, size);

      if(n_samples > 0)
	return n_samples;
      else
	return 0;
    }
  else
    return 0;
}
