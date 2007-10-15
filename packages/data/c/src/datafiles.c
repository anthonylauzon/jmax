/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
 */

/** @file data.c data object doc
 */

#include <fts/fts.h>
#include <fts/packages/data/data.h>

#if HAVE_FLAC
#  include <FLAC/stream_decoder.h>
#endif




/******************************************************************************
 *
 *  fmat text file import/export
 *
 */
static fts_method_status_t
fmat_import_textfile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t file_name = fts_get_symbol(at);
    fts_atomfile_t *file = fts_atomfile_open_read(file_name);
    float *ptr = fmat_get_ptr(self);
    int m = 0;
    int n = 0;
    int i = 0;
    int j = 0;
    fts_atom_t a;
    char c;
    
    if(file != NULL)
    {
      while(fts_atomfile_read(file, &a, &c))
      {
        int alloc = self->alloc;
        m = i + 1;
        
        /* first row determines # of columns */
        if(i == 0)
          n = j + 1;
        
        /* grow matrix */
        while(m * n > alloc)
          alloc += 256;
        
        fmat_reshape(self, 1, alloc);
        ptr = fmat_get_ptr(self);
        
        if(j < n)
        {
          if(fts_is_number(&a))
            ptr[i * n + j] = (float)fts_get_number_float(&a);
          else
            ptr[i * n + j] = 0.0;
          
          j++;
          
          if(c == '\n'  ||  c == '\r')
          {
            for(; j<n; j++)
              ptr[i * n + j] = 0.0;
            
            /* reset to beginning of next row */
            i++;
            j = 0;
          }
        }
        else if (c == '\n'  ||  c == '\r')
        {
          /* reset to beginning of next row */
          i++;
          j = 0;
        }
      }
      
      /* maybe empty rest of last line */
      if(j > 0)
      {
        i++;
        j = 0;
      }
      
      fmat_reshape(self, m, n);
      
      fts_atomfile_close(file);
      
      if(m * n > 0)
      {
        fts_object_changed(o);
        fts_set_object(ret, o);
      }
      else
        fts_object_error(o, "import: couldn't read any text data from file \"%s\"", fts_symbol_name(file_name));
    }
    else
      fts_object_error(o, "import: cannot open text file \"%s\"", fts_symbol_name(file_name));
  }
  
  return fts_ok;
}


static fts_method_status_t
fmat_export_textfile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t file_name = fts_get_symbol(at);
    fts_atomfile_t *file;
    float *ptr = fmat_get_ptr(self);
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    int i, j;
    
    file = fts_atomfile_open_write(file_name);
    
    if(file != NULL)
    {
      /* write the content of the fmat */
      for(i=0; i<m; i++)
      {
        float *row = ptr + i * n;
        fts_atom_t a;
        
        for(j=0; j<n-1; j++)
        {
          fts_set_float(&a, row[j]);
          fts_atomfile_write(file, &a, ' ');
        }
        
        fts_set_float(&a, row[n - 1]);
        fts_atomfile_write(file, &a, '\n');
      }
      
      fts_atomfile_close(file);
      
      if(m * n > 0)
        fts_set_object(ret, o);
      else
        fts_object_error(o, "export: coudn't write any text data to file \"%s\"", fts_symbol_name(file_name));
    }
    else
      fts_object_error(o, "export: cannot open text file \"%s\"", fts_symbol_name(file_name));
  }
  
  return fts_ok;
}



/******************************************************************************
 *
 *  fmat audio file import/export
 *
 */

static fts_method_status_t
fmat_import_audiofile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t file_name = fts_get_symbol(at);
    fts_audiofile_t *sf = fts_audiofile_open_read(file_name);
    
    if(sf != NULL)
    {
      int m = fts_audiofile_get_num_frames(sf);
      int n = fts_audiofile_get_num_channels(sf);
      int wanted  = 0;	/* number of sample frames to load */
      int offset  = 0;
      int channel = 0;
      double  sr  = 0;	/* resample if not zero */
      fmat_t *orig = NULL;
      float  *ptr;
      
      /* parse further import arguments <offset> <length> <channel> <sr>
        no arg or 0 or string mean: all/as is */
      switch (ac)
      { /* fallthrough! */
        default:
        case 5:
          if (fts_is_number(at + 4))
            sr = fts_get_number_float(at + 4);
          
        case 4: /* channel selection ignored so far */
          if (fts_is_number(at + 3))
            channel = fts_get_number_int(at + 3);
          
        case 3:
          if (fts_is_number(at + 2))
            wanted = fts_get_number_int(at + 2);
          
        case 2:
          if (fts_is_number(at + 1))
            offset = fts_get_number_int(at + 1);
          
        case 1:
          /* no additional arguments, filename already parsed, do nothing */
          break;
      }
      
      /* check args */
      if (sr <= 0  ||  sr == fts_audiofile_get_sample_rate(sf))
        sr = 0;
      
      if (offset > 0  &&  offset < m)
        m -= offset;
      
      if (wanted > 0  &&  wanted < m)
        m = wanted;
      
      if (sr == 0)
      {
        fmat_reshape(self, m, n);
        ptr = fmat_get_ptr(self);
      }
      else  /* temp buffer to be resampled */
      {
        orig = fmat_create(m + 2, n);
        ptr  = fmat_get_ptr(orig);
      }
      
      /* move to position and read samples */
      if (offset)
        fts_audiofile_seek(sf, offset);
      
      m = fts_audiofile_read_interleaved(sf, ptr, n, m);
      fmat_reshape(self, m, n);
      
      fts_audiofile_close(sf);      
      
      if (m > 0)
      {
        if (sr != 0)
        {
          fmat_resample(self, orig, sr / fts_audiofile_get_sample_rate(sf));
          fts_object_destroy((fts_object_t *) orig);
        }
        
        fts_object_changed(o);
        fts_set_object(ret, o);
      }
      else
        fts_object_error(o, "import: coudn't read any audio data from file \"%s\"", fts_symbol_name(file_name));
    }
    else
      fts_object_error(o, "import: cannot open audio file \"%s\"", fts_symbol_name(file_name));
  }

return fts_ok;
}

static fts_method_status_t
fmat_export_audiofile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t file_name = fts_get_symbol(at);
    float *ptr = fmat_get_ptr(self);
    int m = fmat_get_m(self);
    int n = fmat_get_n(self);
    fts_audiofile_t *sf = NULL;
    double sr = fts_audio_get_sr();
    fts_symbol_t sample_format = fts_s_int16;
    int size = 0;
    
    if(ac > 1 && fts_is_number(at + 1))
      sr = fts_get_number_float(at + 1);
    
    if(ac > 2 && fts_is_symbol(at + 2))
      sample_format = fts_get_symbol(at + 2);
    
    if(sr <= 1.0)
      sr = 44100.0;
    
    sf = fts_audiofile_open_write(file_name, n, (int)sr, s, sample_format);
    
    if(sf != NULL)
    {
      size = fts_audiofile_write_interleaved(sf, ptr, n, m);
      fts_audiofile_close(sf);
      
      if(size > 0)
        fts_set_object(ret, o);
      else
        fts_object_error(o, "export: coudn't write any audio data to file \"%s\"", fts_symbol_name(file_name));
    }
    else
      fts_object_error(o, "export: cannot open audio file \"%s\"", fts_symbol_name(file_name));
  }
  
  return fts_ok;
}





/******************************************************************************
 *
 *  fmat flac import/export
 *
 */

#if HAVE_FLAC

typedef struct _import_data
{
  /* from the file to import */
  int numframes;	
  int numchannels;
  double sr_orig;
  int bits_per_sample;
  float scale_factor;

  /* import arguments */
  int wanted;		/* number of sample frames to load */
  int offset;
  int channel;
  double  sr_wanted;	/* resample if not zero */

  /* importer data */
  fmat_t *fmat;
  fmat_t *orig;
  float  *ptr;
  int	  numread;	/* number of sample frames actually read */
} fmat_import_args_t;

static void fmat_import_init (fmat_import_args_t *args, fmat_t *fmat,
			      int ac, const fts_atom_t *at)
{
  bzero(args, sizeof(fmat_import_args_t));

  args->scale_factor = 1;
  args->fmat = fmat;

  /* parse further import arguments <offset> <length> <channel> <sr>
     no arg or 0 or string mean: all/as is */
  switch (ac)
  { /* FALLTHROUGH! */
    default:
    case 5:
      if (fts_is_number(at + 4))
        args->sr_wanted = fts_get_number_float(at + 4);
      
    case 4: /* channel selection ignored so far */
      if (fts_is_number(at + 3))
        args->channel = fts_get_number_int(at + 3);
      
    case 3:
      if (fts_is_number(at + 2))
        args->wanted = fts_get_number_int(at + 2);
      
    case 2:
      if (fts_is_number(at + 1))
        args->offset = fts_get_number_int(at + 1);
      
    case 1:
      /* no additional arguments, filename already parsed, do nothing */
    break;
  }
 }

static void fmat_import_begin (fmat_import_args_t *args, 
			       int m, int n, double sr, int bits)
{
  args->numframes   	= m;
  args->numchannels 	= n;
  args->sr_orig     	= sr;
  args->bits_per_sample = bits;
  args->scale_factor    = (double) 1. / (double) (1 << (bits - 1));

  /* check args */
  if (args->sr_wanted <= 0  ||  args->sr_wanted == args->sr_orig)
    args->sr_wanted = 0;
  
  if (args->offset > 0  &&  args->offset < args->numframes)
    args->numframes -= args->offset;
  
  if (args->wanted > 0  &&  args->wanted < args->numframes)
    args->numframes = args->wanted;
  else
    args->wanted = args->numframes;
  
  if (args->sr_wanted == 0)
  {
    fmat_reshape(args->fmat, args->numframes, args->numchannels);
    args->orig = args->fmat;
    args->ptr  = fmat_get_ptr(args->fmat);
  }
  else  /* temp buffer to be resampled */
  {
    args->orig = fmat_create(args->numframes + 2, args->numchannels);
    args->ptr  = fmat_get_ptr(args->orig);
  }    
}

static void fmat_import_finish (fmat_import_args_t *args)
{
  /* crop to actually read samples */
  fmat_reshape(args->orig, args->numread, args->numchannels);

  if (args->numread > 0  &&  args->sr_wanted != 0)
  {
    fmat_resample(args->fmat, args->orig, args->sr_wanted / args->sr_orig);
    fts_object_destroy((fts_object_t *) args->orig);
  }
}


/* called when the decoder has decoded a metadata block */
static void flacdec_metadata_callback (const FLAC__StreamDecoder  *flacdec, 
				       const FLAC__StreamMetadata *metadata, 
				       void *client_data)
{
  switch (metadata->type)
  {
    case FLAC__METADATA_TYPE_STREAMINFO:
    {
      fmat_import_args_t *args = (fmat_import_args_t *) client_data;
      int    n    = metadata->data.stream_info.channels;
      int    m    = metadata->data.stream_info.total_samples / n;
      double sr   = metadata->data.stream_info.sample_rate;
      int    bits = metadata->data.stream_info.bits_per_sample;

      fmat_import_begin(args, m, n, sr, bits);

      /* fts_post("flacdec_metadata_callback: type %d  bits %d factor %g\n", 
	 metadata->type, bits, (double) args->scale_factor); */

      /* seek to first wanted sample: at any point after the stream
	 decoder has been initialized, the client can call this
	 function to seek to an exact sample within the
	 stream. Subsequently, the first time the write callback is
	 called it will be passed a (possibly partial) block starting
	 at that sample. */
      FLAC__stream_decoder_seek_absolute(flacdec, args->offset * n);
    }
    break;

    default:
      /* not handled */
    break;
  }
}


static FLAC__StreamDecoderWriteStatus 
flacdec_write_callback (const FLAC__StreamDecoder *flacdec, 
		 	const FLAC__Frame         *frame, 
			const FLAC__int32         *const buffer[], 
			void			  *client_data)
{
  fmat_import_args_t *args = (fmat_import_args_t *) client_data;
  float *ptr = args->ptr;
  float  factor = args->scale_factor;
  int m = frame->header.blocksize;
  int n = args->numchannels;
  int c, i, j;

  /* fts_post("flacdec_write_callback: samples %d channels %d\n", m, n); */

  if (args->numread + m > args->wanted)
    m = args->wanted - args->numread;

  /* copy split int channels from flac to interleaved fmat float samples */
  for (c = 0; c < n; c++)
  {
    const FLAC__int32 *const buf = buffer[c];
    
    for (i = 0, j = c; i < m; i++, j += n)
    {
      ptr[j] = factor * buf[i];
    }
  }

  args->ptr     += m * n;
  args->numread += m;

  return args->numread < args->wanted 
    ? FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE
    : FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}

static void flacdec_error_callback (const FLAC__StreamDecoder *flacdec, 
				    FLAC__StreamDecoderErrorStatus status, 
				    void *client_data)
{
  fmat_import_args_t *args = (fmat_import_args_t *) client_data;

  fts_post("flac decoding error number %d after %d samples read\n", 
	   status, args->numread);
}


static fts_method_status_t
fmat_import_flac (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fmat_t *self = (fmat_t *) o;
  
  if (ac > 0  &&  fts_is_symbol(at))
  {
    fmat_import_args_t args;
    fts_symbol_t filesymb = fts_get_symbol(at);
    const char  *filename = (const char *) fts_symbol_name(filesymb);
    char	 str[1024];
    char	*fullpath = fts_file_find(filename, str, 1023);

    if (fullpath != NULL)
    { /* create a new instance */
      FLAC__StreamDecoder *flacdec = FLAC__stream_decoder_new();

      if (flacdec)
      { /* No need to override the decoder options with
	   FLAC__stream_decoder_set_*() since by default, 
	   only the STREAMINFO block is returned via the metadata callback. */

	fmat_import_init(&args, self, ac, at);

	/* decoding directly from a file */
	if (FLAC__stream_decoder_init_file(flacdec, fullpath,
					   flacdec_write_callback,
					   flacdec_metadata_callback,
					   flacdec_error_callback,
					   &args) 
	    == FLAC__STREAM_DECODER_INIT_STATUS_OK)
	{ /* process the stream from the current location until the
	     read callback returns
	     FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM or
	     FLAC__STREAM_DECODER_READ_STATUS_ABORT. The client will
	     get one metadata, write, or error callback per metadata
	     block, audio frame, or sync error, respectively. */
	  FLAC__stream_decoder_process_until_end_of_stream(flacdec);

	  FLAC__stream_decoder_finish(flacdec);
	}
	FLAC__stream_decoder_delete(flacdec);

	fmat_import_finish(&args);

	if (args.numread > 0)
	{
	  fts_object_changed(o);
	  fts_set_object(ret, o);
	}
	else
	  fts_object_error(o, "import: coudn't read any audio data from file \"%s\" (at \"%s\")", filename, fullpath);
      }
      else
	fts_object_error(o, "import: cannot open audio file \"%s\" (at \"%s\")", filename, fullpath);
    }
  }

  return fts_ok;
}

#endif	/* HAVE_FLAC */


/******************************************************************************
 *
 *  ivec text file import/export
 *
 */
#define IVEC_BLOCK_SIZE 256

static void
ivec_grow(ivec_t *vec, int size)
{
  int alloc = vec->alloc;
  
  while(!alloc || size > alloc)
    alloc += IVEC_BLOCK_SIZE;
  
  ivec_set_size(vec, alloc);
}

static int 
ivec_read_atomfile(ivec_t *vec, fts_symbol_t file_name)
{
  fts_atomfile_t *file = fts_atomfile_open_read(file_name);
  int n = 0;
  fts_atom_t a;
  char c;
  
  if(!file)
    return -1;
  
  while(fts_atomfile_read(file, &a, &c))
  {
    if(n >= vec->alloc)
      ivec_grow(vec, n);
    
    if(fts_is_number(&a))
      ivec_set_element(vec, n, fts_get_number_int(&a));
    else
      ivec_set_element(vec, n, 0);
    
    n++;
  }
  
  ivec_set_size(vec, n);
  
  fts_atomfile_close(file);
  
  return (n);
}

static int
ivec_write_atomfile(ivec_t *vec, fts_symbol_t file_name)
{
  fts_atomfile_t *file;
  int size = ivec_get_size(vec);
  int i;
  
  file = fts_atomfile_open_write(file_name);
  
  if(!file)
    return -1;
  
  /* write the content of the vec */
  for(i=0; i<size; i++)     
  {
    fts_atom_t a;
    
    fts_set_int(&a, ivec_get_element(vec, i));
    fts_atomfile_write(file, &a, '\n');
  }
  
  fts_atomfile_close(file);
  
  return (i);
}

static fts_method_status_t
ivec_import_textfile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  
  if(file_name != NULL)
  {
    int size = ivec_read_atomfile(this, file_name);
    
    if(size <= 0)
      fts_post("ivec: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
    else
      fts_object_set_state_dirty( o);
  }
  
  return fts_ok;
}

static fts_method_status_t
ivec_export_textfile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *this = (ivec_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  
  if(file_name != NULL)
  {
    int size = ivec_write_atomfile(this, file_name);
    
    if(size < 0)
      fts_post("ivec: can't export to text file \"%s\"\n", fts_symbol_name(file_name));
  }
  
  return fts_ok;
}

/******************************************************************************
 *
 *  mat text file import/export
 *
 */
#define MAT_BLOCK_SIZE 256

static void
mat_grow(mat_t *mat, int size)
{
  int alloc = mat->alloc;
  
  while(size > alloc)
    alloc += MAT_BLOCK_SIZE;
  
  mat_set_size(mat, alloc, 1);  /* initialises added atoms */
}


static int
mat_read_atomfile_newline(mat_t *mat, fts_symbol_t file_name)
{
  fts_atomfile_t *file = fts_atomfile_open_read(file_name);
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;
  
  if(!file)
    return -1;
  
  mat_set_size(mat, 0, 0);
  
  while(fts_atomfile_read(file, &a, &c))
  {
    m = i + 1;
    
    /* first row determines # of columns */    
    if(i == 0)
      n = j + 1;
    
    if(m * n > mat->alloc)
      mat_grow(mat, m * n);
    
    if(j < n)
    {
      mat->data[i * n + j] = a;
      j++;
      
      if(c == '\n' || c == '\r')
      {
        /* reset to beginning of next row */
        i++;
        j = 0;
      }
    }
    else if(c == '\n' || c == '\r')
    {
      /* reset to beginning of next row */
      i++;
      j = 0;
    }
  }
  
  /* maybe empty rest of last line */
  if(j > 0)
  {
    i++;
    j = 0;
  }
  
  mat->m = m;
  mat->n = n;
  
  fts_atomfile_close(file);
  
  return(m * n);
}

static int
mat_write_atomfile_newline(mat_t *mat, fts_symbol_t file_name)
{
  fts_atomfile_t *file;
  int m = mat->m;
  int n = mat->n;
  int i, j;
  
  file = fts_atomfile_open_write(file_name);
  
  if(!file)
    return -1;
  
  /* write the content of the mat */
  for(i=0; i<m; i++)     
  {
    fts_atom_t *row = mat->data + i * n;
    
    for(j=0; j<n-1; j++)        
      fts_atomfile_write(file, row + j, ' ');
    
    fts_atomfile_write(file, row + n - 1, '\n');
  }
  
  fts_atomfile_close(file);
  return(m * n);
}

static int 
mat_read_atomfile_separator(mat_t *mat, fts_symbol_t file_name, fts_symbol_t separator, int ac, const fts_atom_t *at)
{
  fts_atomfile_t *file = fts_atomfile_open_read(file_name);
  int m = 0;
  int n = 0;
  int i = 0;
  int j = 0;
  fts_atom_t a;
  char c;
  
  if(!file)
    return -1;
  
  if(!separator)
    separator = fts_s_comma;
  
  mat_set_size(mat, 0, 0);
  
  while(fts_atomfile_read(file, &a, &c))
  {
    int skip = 0;
    int k;
    
    /* filter atoms */
    for(k=0; k<ac; k++)
    {
      /* ooops! */
      if(fts_atom_same_type(&a, &at[k]) && fts_get_int(&a) == fts_get_int(&at[k]))
      {
        skip = 1;
        break;
      }
    }
    
    if(!skip)
    {
      m = i + 1;
      
      if(fts_get_symbol(&a) == separator)
      {
        /* reset to beginning of next row */
        i++;
        j = 0;
      }
      else if(i == 0)
      {
        /* first row determines # of columns */    
        n = j + 1;
        
        if(n > mat->alloc)
          mat_grow(mat, n);
        
        mat->data[i * n + j] = a;
        j++;
      }
      else if(j < n)
      {
        if(m * n > mat->alloc)
          mat_grow(mat, m * n);
        
        mat->data[i * n + j] = a;
        j++;
      }
    }
  }
  
  /* maybe empty rest of last line */
  if(j > 0)
  {
    i++;
    j = 0;
  }
  
  if(n > 0)
  {
    mat->m = m;
    mat->n = n;
  }
  else
  {
    mat->m = 0;
    mat->n = 0;      
  }
  
  fts_atomfile_close(file);
  
  return(m * n);
}

static int
mat_write_atomfile_separator(mat_t *mat, fts_symbol_t file_name, fts_symbol_t separator)
{
  fts_atomfile_t *file;
  int m = mat->m;
  int n = mat->n;
  fts_atom_t sep;
  int i, j;
  
  file = fts_atomfile_open_write(file_name);
  
  if(!file)
    return -1;
  
  fts_set_symbol(&sep, separator);
  
  /* write the content of the mat */
  for(i=0; i<m; i++)     
  {
    fts_atom_t *row = mat->data + i * n;
    
    for(j=0; j<n; j++)  
      fts_atomfile_write(file, row + j, ' ');
    
    fts_atomfile_write(file, &sep, '\n');
  }
  
  fts_atomfile_close(file);
  
  return (m * n);
}

static fts_method_status_t
mat_import_textfile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
  int size = 0;
  
  if(!file_name)
    return fts_ok;
  if (separator)
    size = mat_read_atomfile_separator(self, file_name, separator, ac - 3, at + 3);
  else
    size = mat_read_atomfile_newline(self, file_name);
  
  if(size <= 0)
    fts_post("mat: can't import from text file \"%s\"\n", fts_symbol_name(file_name));
  else
  {
    if(mat_editor_is_open(self))
      mat_upload(self);
    
    fts_object_set_state_dirty(o);
  }
  
  return fts_ok;
}

static fts_method_status_t
mat_export_textfile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  mat_t *self = (mat_t *) o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);
  int size = 0;  
  
  if(!file_name)
    return fts_ok;
  
  if (separator)
    size = mat_write_atomfile_separator(self, file_name, separator);
  else
    size = mat_write_atomfile_newline(self, file_name);
  
  if(size < 0)
    fts_post("mat: can't export to text file \"%s\"\n", fts_symbol_name(file_name));

  return fts_ok;
}

/******************************************************************************
 *
 *  dict text (Max coll) file import/export
 *
 */
#define DICT_ATOM_BUF_BLOCK_SIZE 64

static fts_atom_t *
dict_atom_buf_realloc(fts_atom_t *buf, int size)
{
  fts_atom_t *new_buf = (fts_atom_t *)fts_realloc(buf, sizeof(fts_atom_t) * size); /* double size */
  
  return new_buf;
}

static void
dict_atom_buf_free(fts_atom_t *buf, int size)
{
  if(buf)
    fts_free(buf);
}

static int 
dict_import_from_coll(dict_t *self, fts_symbol_t file_name)
{
  fts_atomfile_t *file = fts_atomfile_open_read(file_name);
  int atoms_alloc = DICT_ATOM_BUF_BLOCK_SIZE;
  fts_atom_t *atoms = 0;
  enum {read_key, read_comma, read_argument} state = read_key;
  char *error = 0;
  int i = 0;
  int n = 0;
  fts_atom_t key;
  fts_atom_t a;
  char c;
  
  if(!file)
    return 0;
  
  atoms = dict_atom_buf_realloc(atoms, atoms_alloc);
  
  dict_remove_all(self);
  fts_set_void(&key);
  
  while(error == 0 && fts_atomfile_read(file, &a, &c))
  {
    switch(state)
    {
      case read_key:
      {
        key = a;
        state = read_comma;
      }
        
        break;
        
      case read_comma:
      {
        if(fts_is_symbol(&a) && (fts_get_symbol(&a) == fts_new_symbol(",")))
          state = read_argument;
        else
          error = "comma expected";
      }
        
        break;
        
      case read_argument:
      {
        if(fts_is_symbol(&a) && (fts_get_symbol(&a) == fts_new_symbol(";")))
	      {
          if(n > 0)
          {
            if(fts_is_symbol(atoms + 0))
            {
              fts_symbol_t selector = fts_get_symbol(atoms + 0);
              
              if(selector == fts_s_int || selector == fts_s_float || selector == fts_s_symbol || fts_s_list)
                dict_store_atoms(self, &key, n - 1, atoms + 1);
            }
            else
              dict_store_atoms(self, &key, n, atoms);
            
            i++;
            n = 0;
            
            state = read_key;
          }
          else
            fts_post("dict: empty message found in coll file %s (ignored)\n", fts_symbol_name(file_name));
	      }
        else
	      {
          /* read argument */
          if(n >= atoms_alloc)
          {
            atoms_alloc += DICT_ATOM_BUF_BLOCK_SIZE;
            atoms = dict_atom_buf_realloc(atoms, atoms_alloc);
          }
          
          atoms[n] = a;
          n++;
	      }
      }
        
        break;
    }
  }
  
  if(error != 0)
    fts_post("dict: error reading coll file %s (%s)\n", fts_symbol_name(file_name), error);
  else if(state != read_key)
  {
    if(n > 0)
    {
      dict_store_atoms(self, &key, n, atoms);
      i++;
    }
    
    fts_post("dict: found unexpected ending in coll file %s\n", fts_symbol_name(file_name));
  }
  
  dict_atom_buf_free(atoms, atoms_alloc);
  fts_atomfile_close(file);
  
  return i;
}

static int 
dict_export_to_coll(dict_t *self, fts_symbol_t file_name)
{
  fts_atomfile_t *file = fts_atomfile_open_write(file_name);
  fts_iterator_t key_iterator;
  fts_iterator_t value_iterator;
  int size = 0;
  int i;
  
  if(!file)
    return 0;
  
  fts_hashtable_get_keys(&self->hash, &key_iterator);
  fts_hashtable_get_values(&self->hash, &value_iterator);
  
  while(fts_iterator_has_more(&key_iterator))
  {
    fts_atom_t key, value;
    fts_symbol_t s = NULL;
    int ac = 0;
    const fts_atom_t *at = NULL;
    fts_atom_t a;
    
    fts_iterator_next(&key_iterator, &key);
    fts_iterator_next(&value_iterator, &value);
    
    if(fts_is_tuple(&value))
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(&value);
      
      s = fts_s_list;
      ac = fts_tuple_get_size(tuple);
      at = fts_tuple_get_atoms(tuple);
    }
    else if(fts_is_symbol(&value))
    {
      s = fts_s_symbol;
      ac = 1;
      at = &value;
    }
    
    else if(!fts_is_object(&value))
    {
      ac = 1;
      at = &value;
    }
    
    /* write key */
    fts_atomfile_write(file, &key, ' ');
    
    /* write comma */
    fts_set_symbol(&a, fts_s_comma);
    fts_atomfile_write(file, &a, ' ');
    
    /* write selector (if any) */
    if(s)
    {
      fts_set_symbol(&a, s);
      fts_atomfile_write(file, &a, ' ');
    }
    
    /* write arguments */
    for(i=0; i<ac; i++)
      fts_atomfile_write(file, at + i, ' ');
    
    /* write semicolon and new line */
    fts_set_symbol(&a, fts_s_semi);
    fts_atomfile_write(file, &a, '\n');
    
    size++;
  }
  
  fts_atomfile_close(file);
  
  return size;
}

static fts_method_status_t
dict_import_textfile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dict_t *self = (dict_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t file_name = fts_get_symbol(at);
    int size = dict_import_from_coll(self, file_name);    
    
    if(size > 0)
    {
      fts_object_set_state_dirty(o);	/* if obj persistent patch becomes dirty */
      
      if(dict_editor_is_open(self))
        dict_upload(self);      
    }
    else
      fts_post("dict: can't import from file \"%s\"\n", fts_symbol_name(file_name));
  }
  
  return fts_ok;
}

static fts_method_status_t
dict_export_textfile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  dict_t *self = (dict_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t file_name = fts_get_symbol(at);
    int size = dict_export_to_coll(self, file_name);    
    
    if(size <= 0)
      fts_post("dict: can't export to file \"%s\"\n", fts_symbol_name(file_name));  
  }
  
  return fts_ok;
}

FTS_MODULE_INIT(datafiles)
{
  /* fmat audio file import/export */
  fts_class_import_handler(fmat_class, fts_new_symbol("flac"), fmat_import_flac);
  fts_audiofile_import_handler(fmat_class, fmat_import_audiofile);
  fts_audiofile_export_handler(fmat_class, fmat_export_audiofile);
  fts_class_import_handler_default(fmat_class, fmat_import_audiofile);
  fts_class_export_handler_default(fmat_class, fmat_export_audiofile);
  
  /* fmat text file import/export */
  fts_atomfile_import_handler(fmat_class, fmat_import_textfile);
  fts_atomfile_export_handler(fmat_class, fmat_export_textfile);
  
  /* ivec text file import/export */
  fts_atomfile_import_handler(ivec_class, ivec_import_textfile);
  fts_atomfile_export_handler(ivec_class, ivec_export_textfile);
  
  /* ivec text file import/export */
  fts_atomfile_import_handler(mat_class, mat_import_textfile);
  fts_atomfile_export_handler(mat_class, mat_export_textfile);
  
  /* dict text file import/export */
  fts_atomfile_import_handler(dict_class, dict_import_textfile);
  fts_atomfile_export_handler(dict_class, dict_export_textfile);
  fts_class_import_handler(dict_class, fts_new_symbol("coll"), dict_import_textfile);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
