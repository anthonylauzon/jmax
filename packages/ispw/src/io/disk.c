/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.2 $ IRCAM $Date: 1998/10/28 15:51:33 $
 *
 * writesf~, readsf~: disk recording and playback
 *
 * Now supported on the following platforms :
 *  - SGI version : uses AIFF files and AF 1.0 library, should move to the new one.
 *  - SOLARIS2 : uses SND files and libaudio library
 * 
 * 
 * vreadsf is not implemented yet
 *
 * TO DO : reimplement sound files access using devices
 *
 */

#include "fts.h"

#include <string.h>


static fts_symbol_t writesf_mono_function = 0;
static fts_symbol_t writesf_stereo_function = 0;

static fts_symbol_t readsf_mono_function = 0;
static fts_symbol_t readsf_stereo_function = 0;

/* utility function to find the real path */

static char buf[1024];

static const char *get_readsf_path(fts_symbol_t filename)
{
  fts_file_get_read_path(fts_symbol_name(filename), buf);
  
  return buf;
}

static const char *get_writesf_path(fts_symbol_t filename)
{
  fts_file_get_write_path(fts_symbol_name(filename), buf);

  return buf;
}
  


/* SGI and SOLARIS2 dependent implementation of writesf;
   should go in an other file ! */

#if defined(SGI)

#include <audiofile.h>
#include <audiofile.h>

#elif defined(SOLARIS2)

#include <fcntl.h>
#include <sys/audioio.h>
#include <multimedia/libaudio.h>
#include <multimedia/audio_device.h>
#include <multimedia/audio_encode.h>
#include "ST/sfheader.h"
#include "ST/sfiles.h"
#define SF_SND 124

#endif

#define SIGNAL_TO_SHORT_COEF ((float) 32768)
#define SHORT_TO_SIGNAL_COEF ((float) 32768)

typedef struct 
{
  fts_object_t _o;
  fts_symbol_t sndfile;
#ifdef SGI
  AFfilehandle fh;
#elif defined(SOLARIS2)
  int fh;
  SFHEADER sfhdr;
#endif
  int state;
  int nchans;
} writesf_t;


#if defined(SGI)
static int
writesf_file_open(writesf_t *this)
{
  AFfilesetup fs;    

  fs = AFnewfilesetup();
  AFinitfilefmt(fs, AF_FILE_AIFF); 
  AFinitsampfmt(fs, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16L);
  AFinitchannels(fs, AF_DEFAULT_TRACK, this->nchans);

  this->fh = AFopenfile(get_writesf_path(this->sndfile), "w", fs);
  AFfreefilesetup(fs);

  if (!this->fh)
    {
      post("writesf~: cannot open file '%s' for writing\n", 
	   fts_symbol_name(this->sndfile));
      return 0;
    }
  
  return 1;
}

static void
writesf_file_close(writesf_t *this)
{
  AFclosefile(this->fh);
  this->fh = 0;
}

#elif defined(SOLARIS2)

enum {
  _8BITS = 1,
  _16BITS,
  _24BITS,
  _32BITS,
  _SHORT,
  _FLOAT
};

static int
writesf_get_hint(const char *hint, int *psize, int *detail)
{
  if (!hint)
    {
      *psize = 0;
      *detail = 0;
      return 1;
    }

  if (!strcmp(hint, "8b"))
    {
      *psize = SF_8bit;
      *detail = _8BITS;
    }
  else if (!strcmp(hint, "16b"))
    {
      *psize = SF_16bit;
      *detail = _16BITS;
    }
  else if (!strcmp(hint, "24b"))
    {
      *psize = SF_24bit;
      *detail = _24BITS;
    }
  else if (!strcmp(hint, "32b"))
    {
      *psize = SF_32bit;
      *detail = _32BITS;
    }
  else if (!strcmp(hint, "short"))
    {
      *psize = SF_SHORT;
      *detail = _SHORT;
    }
  else if (!strcmp(hint, "float"))
    {
      *psize = SF_8bit;
      *detail = _FLOAT;
    }
  else
    {
      post("writesf~: invalid hint in open message '%s' ; "
	   "must be: 8b, 16b, 24b, 32b, short or float\n", hint);
      return 0;
    }

  return 1;
}

static int
writesf_get_format(SFHEADER *hdr, const char *file, const char *hint)
{
  const char *p = strrchr(file, '.');
  int size;
  int detail;

  if (!p)
    {
      post("no extension specified on file '%s'\n", file);
      return 0;
    }

  if (!writesf_get_hint(hint, &size, &detail))
    return 0;

  if (!strcmp(p, ".aiff") || !strcmp(p, ".aif"))
    {
      if (detail && detail != _8BITS && detail != _16BITS &&
	  detail != _24BITS && detail != _32BITS)
	{
	  post("writesf~: incompatible format directive for .aiff file '%s' (must be 8b, 16b, 24b or 32b)\n", hint);
	  return 0;
	}

      sfmachine(hdr) = SF_AIFF;
      sfclass(hdr) = (size ? size : SF_16bit);
    }
  else if (!strcmp(p, ".aifc"))
    {
      if (detail && detail != _8BITS && detail != _16BITS &&
	  detail != _24BITS && detail != _32BITS)
	{
	  post("writesf~: incompatible format directive for .aifc file '%s' (must be 8b, 16b, 24b or 32b)\n", hint);
	  return 0;
	}

      sfmachine(hdr) = SF_AIFC;
      sfclass(hdr) = (size ? size : SF_16bit);
    }
  else if (!strcmp(p, ".snd") || !strcmp(p, ".au"))
    {
      if (detail && detail != _16BITS)
	{
	  post("writesf~: incompatible format directive for .snd file '%s' (must be 16b)\n", hint);
	  return 0;
	}

      sfmachine(hdr) = SF_SND;
      sfclass(hdr) = (size ? size : SF_16bit);
    }
  else if (!strcmp(p, ".sf"))
    {
      if (detail && detail != _SHORT && detail != _FLOAT)
	{
	  post("writesf~: incompatible format directive for .sf file '%s' (must be short or float)\n", hint);
	  return 0;
	}

      sfmachine(hdr) = SF_MACHINE;
      sfclass(hdr) = (size ? size : SF_FLOAT);
    }
  else
    {
      post("writesf~: extension '%s' is not a valid extension for sound file ;"
	   "use one of those: .snd, .au, .aiff, .aifc or .sf\n", p);
      return 0;
    }

  return 1;
}

static int
writesf_file_open(writesf_t *this, fts_symbol_t shint)
{
  const char *file = get_writesf_path(this->sndfile);
  const char *hint = (shint ? fts_symbol_name(shint) : (const char *)0);

  /*  this->fh = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0666); */
  this->fh = open(file, O_RDWR | O_TRUNC | O_CREAT, 0666);

  if (this->fh < 0)
    {
      post("writesf~: cannot open file '%s' for writing\n", file);
      return 0;
    }

  if (!writesf_get_format(&this->sfhdr, file, hint))
    return 0;
    
  if (sfmachine(&this->sfhdr) == SF_SND)
    {
      Audio_hdr hdr;
      /* PCM 16 bits linear */
      hdr.sample_rate      = fts_param_get_float(fts_s_sampling_rate, 44100.);
      hdr.samples_per_unit = 1;
      hdr.bytes_per_unit   = 2; /* 16 bits */
      hdr.channels         = this->nchans;
      hdr.encoding         = AUDIO_ENCODING_LINEAR;
      hdr.endian           = AUDIO_ENDIAN_BIG; /* sparc is big endian */
      hdr.data_size        = AUDIO_UNKNOWN_SIZE;

      if (audio_write_filehdr(this->fh, &hdr, "", 1) != AUDIO_SUCCESS)
	{
	  post("writesf~: cannot write file header for file '%s'\n", file);
	  return 0;
	}
    }
  else
    {
      SFHEADER *hdr = &this->sfhdr;

      sfmagic1(hdr)  = SF_MAGIC1;
      sfmagic2(hdr)  = SF_MAGIC2;
      sfsrate(hdr)   = fts_param_get_float(fts_s_sampling_rate, 44100.);
      sfchans(hdr)   = this->nchans;

      if (STwheader(this->fh, hdr) < 0)
	{
	  post("writesf~: cannot write file header for file '%s'\n", file);
	  return 0;
	}
    }

  return 1;
}

static void
writesf_file_close(writesf_t *this)
{
  if (sfmachine(&this->sfhdr) == SF_SND)
    close(this->fh);
  else
    {
      if (STclose(this->fh, &this->sfhdr) < 0)
	post("ST_file_close error\n");
      close(this->fh);
    }
  this->fh = 0;
}
#endif

static void
writesf_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  int state = fts_get_number_arg(ac, at, 0, 0);

  /*
     Messy behaviour, for compatibilty with old Max 0.26:
     a 0 message !close! the file, so you have to go thru open/1
     to play it again; also, 1 is accepted only if the file was
     already open.

     */

  if (state)
    {
      if (this->fh)
	this->state = state;
    }
  else
    {
      if (this->fh)
	{
	  writesf_file_close(this);
	  this->state = 0;
	}
    }
}


static void
writesf_open(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 0, 0);
  int ret;

  if (this->state)
    {
      post("writesf~: open: already active\n");
      return;
    }

  if (this->fh)
    writesf_file_close(this);

  if (s)
    this->sndfile = s;

#ifdef SGI
  if (writesf_file_open(this))
    this->state = 1;
#elif defined(SOLARIS2)
  if (writesf_file_open(this, fts_get_symbol_arg(ac, at, 1, 0)))
    this->state = 1;
#endif
}


static void
writesf_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this  = (writesf_t *)o;

  if (this->state)
    post("writesf~: active\n");
  else if (this->fh)
    post("writesf~: opened\n");
  else
    post("writesf~: inactive\n");
}


#if defined(SGI)

static void
ftl_mono_writesf(fts_word_t *argv)
{
  writesf_t *this = (writesf_t *)fts_word_get_ptr(argv+2);
  static int ntimes;

  if (this->state)
    {
      float *chan1 = (float *)fts_word_get_ptr(argv);
      int size = fts_word_get_long(argv+1), i;
      short samps[512];

      for (i = 0; i < size; i += 8)
	{
	  short f0, f1, f2, f3, f4, f5, f6, f7;
	  
	  f0 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 0]);
	  f1 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 1]);
	  f2 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 2]);
	  f3 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 3]);
	  f4 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 4]);
	  f5 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 5]);
	  f6 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 6]);
	  f7 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 7]);
	    
	  samps[i + 0] = f0;
	  samps[i + 1] = f1;
	  samps[i + 2] = f2;
	  samps[i + 3] = f3;
	  samps[i + 4] = f4;
	  samps[i + 5] = f5;
	  samps[i + 6] = f6;
	  samps[i + 7] = f7;
	}

      AFwriteframes(this->fh, AF_DEFAULT_TRACK, samps, size);
    }
}


#elif defined(SOLARIS2)

static void
ftl_mono_writesf(fts_word_t *argv)
{
  writesf_t *this = (writesf_t *)fts_word_get_ptr(argv+2);
  static int ntimes;

  if (this->state)
    {
      float *chan1 = (float *)fts_word_get_ptr(argv);
      int size = fts_word_get_long(argv+1), i;

      if (sfmachine(&this->sfhdr) == SF_SND)
	{
	  short samps[512];
	  for (i = 0; i < size; i += 8)
	    {
	      short f0, f1, f2, f3, f4, f5, f6, f7;
	      
	      f0 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 0]);
	      f1 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 1]);
	      f2 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 2]);
	      f3 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 3]);
	      f4 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 4]);
	      f5 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 5]);
	      f6 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 6]);
	      f7 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 7]);
	    
	      samps[i + 0] = f0;
	      samps[i + 1] = f1;
	      samps[i + 2] = f2;
	      samps[i + 3] = f3;
	      samps[i + 4] = f4;
	      samps[i + 5] = f5;
	      samps[i + 6] = f6;
	      samps[i + 7] = f7;
	    }
	  write(this->fh, samps, size * sizeof(short));
	}
      else
	{
	  float samps[512];
	  memcpy(samps, chan1, size * sizeof(float));
	  STconvertFromFloatBuffer(&this->sfhdr, samps, size);
	  STwrite(this->fh, (char *)samps, size * sfclass(&this->sfhdr));
	}
    }
}

#endif

#if defined(SGI)

static void
ftl_stereo_writesf(fts_word_t *argv)
{
  writesf_t *this = (writesf_t *)fts_word_get_ptr(argv+3);
  short samps[512];
  static int ntimes;

  if (this->fh && this->state)
    {
      float *chan1 = (float *)fts_word_get_ptr(argv);
      float *chan2 = (float *)fts_word_get_ptr(argv+1);
      int size = fts_word_get_long(argv+2), i, i2;

      for (i = 0, i2 = 0; i < size; i += 4, i2 += 8)
	{
	  short f10, f11, f12, f13, f20, f21, f22, f23;
	    
	  f10 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 0]);
	  f20 = (short)(SIGNAL_TO_SHORT_COEF * chan2[i + 0]);

	  f11 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 1]);
	  f21 = (short)(SIGNAL_TO_SHORT_COEF * chan2[i + 1]);

	  f12 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 2]);
	  f22 = (short)(SIGNAL_TO_SHORT_COEF * chan2[i + 2]);
	    
	  f13 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 3]);
	  f23 = (short)(SIGNAL_TO_SHORT_COEF * chan2[i + 3]);
	    
	  samps[i2 + 0] = f10;
	  samps[i2 + 1] = f20;

	  samps[i2 + 2] = f11;
	  samps[i2 + 3] = f21;
	  
	  samps[i2 + 4] = f12;
	  samps[i2 + 5] = f22;
	  
	  samps[i2 + 6] = f13;
	  samps[i2 + 7] = f23;
	}

      AFwriteframes(this->fh, AF_DEFAULT_TRACK, samps, size);
    }
}

#elif defined(SOLARIS2)

static void
ftl_stereo_writesf(fts_word_t *argv)
{
  writesf_t *this = (writesf_t *)fts_word_get_ptr(argv+3);
  static int ntimes;

  if (this->fh && this->state)
    {
      float *chan1 = (float *)fts_word_get_ptr(argv);
      float *chan2 = (float *)fts_word_get_ptr(argv+1);
      int size = fts_word_get_long(argv+2), i, i2;

      if (sfmachine(&this->sfhdr) == SF_SND)
	{
	  short samps[512];
	  for (i = 0, i2 = 0; i < size; i += 4, i2 += 8)
	    {
	      short f10, f11, f12, f13, f20, f21, f22, f23;
	      
	      f10 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 0]);
	      f20 = (short)(SIGNAL_TO_SHORT_COEF * chan2[i + 0]);
	      
	      f11 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 1]);
	      f21 = (short)(SIGNAL_TO_SHORT_COEF * chan2[i + 1]);
	      
	      f12 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 2]);
	      f22 = (short)(SIGNAL_TO_SHORT_COEF * chan2[i + 2]);
	      
	      f13 = (short)(SIGNAL_TO_SHORT_COEF * chan1[i + 3]);
	      f23 = (short)(SIGNAL_TO_SHORT_COEF * chan2[i + 3]);
	    
	      samps[i2 + 0] = f10;
	      samps[i2 + 1] = f20;
	      
	      samps[i2 + 2] = f11;
	      samps[i2 + 3] = f21;
	      
	      samps[i2 + 4] = f12;
	      samps[i2 + 5] = f22;
	      
	      samps[i2 + 6] = f13;
	      samps[i2 + 7] = f23;
	    }
	  write(this->fh, samps, size * 2 * sizeof(short));
	}
      else
	{
	  float samps[512];
	  for (i = 0, i2 = 0; i < size; i += 4, i2 += 8)
	    {
	      samps[i2 + 0] = chan1[i + 0];
	      samps[i2 + 1] = chan2[i + 0];
	      
	      samps[i2 + 2] = chan1[i + 1];
	      samps[i2 + 3] = chan2[i + 1];
	      
	      samps[i2 + 4] = chan1[i + 2];
	      samps[i2 + 5] = chan2[i + 2];
	      
	      samps[i2 + 6] = chan1[i + 3];
	      samps[i2 + 7] = chan2[i + 3];
	    }

	  STconvertFromFloatBuffer(&this->sfhdr, samps, 2 * size);
	  STwrite(this->fh, (char *)samps, 2 * size * sfclass(&this->sfhdr));
	}
    }
}

#endif

static void
writesf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[4];

  if (this->nchans == 1)
    {
      /* generation for mono file */

      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
      fts_set_long  (argv + 1, fts_dsp_get_input_size(dsp, 0));
      fts_set_ptr   (argv + 2, o);
      dsp_add_funcall(writesf_mono_function, 3, argv);
    }
  else if (this->nchans == 2)
    {
      /* generation for stereofile */

      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
      fts_set_long  (argv + 2, fts_dsp_get_input_size(dsp, 0));
      fts_set_ptr   (argv + 3, o);

      dsp_add_funcall(writesf_stereo_function, 4, argv);
    }
}

static void
writesf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  long int nchans = fts_get_long_arg(ac, at, 1, 0); /* ignore other args */

  if (nchans < 1)
    nchans = 1;
  else if (nchans > 2)
    nchans = 2;

  this->nchans = nchans;
  this->fh = 0;
  this->state = 0;

  dsp_list_insert(o);
}

static void
writesf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  if (this->fh)
    writesf_file_close(this);

  dsp_list_remove(o);
}


/* SGI dependent implementation of readsf */
 

typedef struct
{
  fts_object_t _o;
  fts_symbol_t sndfile;
#ifdef SGI
  AFfilehandle fh;
#elif defined(SOLARIS2)
  int fh;
  SFHEADER sfhdr;
#endif
  int state;
  int nchans;
} readsf_t;


#if defined(SGI)
static int
readsf_file_open(readsf_t *this)
{
  this->fh = AFopenfile(get_readsf_path(this->sndfile), "r", 0);

  return (this->fh != 0);
}

static void
readsf_file_close(readsf_t *this)
{
  AFclosefile(this->fh);
  this->fh = 0;
}

#elif defined(SOLARIS2)

static int
readsf_file_open(readsf_t *this)
{
  const char *file = get_readsf_path(this->sndfile);
  Audio_hdr hdr;
  char buf[80];

  this->fh = open(file, O_RDONLY);

  if (this->fh < 0)
    {
      post("readsf~: cannot open file '%s' for reading\n", file);
      return 0;
    }

  if (audio_read_filehdr(this->fh, &hdr, buf, sizeof(buf)-1) == AUDIO_SUCCESS)
    {
      if (hdr.sample_rate != fts_param_get_float(fts_s_sampling_rate, 44100.))
	{
	  post("readsf~: incompatible sampling rate for~ file '%s' :"
	       "sampling rate file %d, fts sampling rate %d)\n",
	       file, hdr.sample_rate, fts_param_get_float(fts_s_sampling_rate, 44100.));
	  return 0;
	}

      if (hdr.bytes_per_unit != 2 || hdr.encoding != AUDIO_ENCODING_LINEAR)
	{
	  post("readsf~: file '%s' is not PCM 16 bits linear\n", file);
	  return 0;
	}
      
      if (hdr.channels != this->nchans)
	{
	  if (hdr.channels == 1)
	    post("readsf~: file '%s' is mono, expected stereo\n", file);
	  else
	    post("readsf~; file '%s' is stereo, expected mono\n", file);
	  
	  return 0;
	}

      sfmachine(&this->sfhdr) = SF_SND;
      sfclass(&this->sfhdr) = SF_16bit;
    }
  else if (!STrheader(this->fh, &this->sfhdr))
    {
      if (sfchans(&this->sfhdr) != this->nchans)
	{
	  if (sfchans(&this->sfhdr) == 1)
	    post("readsf~: file '%s' is mono, expected stereo\n", file);
	  else
	    post("readsf~; file '%s' is stereo, expected mono\n", file);
	  
	  return 0;
	}
    }
  else
    post("readsf~: invalid header in file '%s'\n", file);

  return 1;
}

static void
readsf_file_close(readsf_t *this)
{
  if (sfmachine(&this->sfhdr) == SF_SND)
    close(this->fh);
  else
    {
      if (STclose(this->fh, &this->sfhdr) < 0)
	post("ST_file_close error\n");
      close(this->fh);
    }
  this->fh = 0;
}
#endif

static void
readsf_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int state = fts_get_number_arg(ac, at, 0, 0);

  /*
     Messy behaviour, for compatibilty with old Max 0.26:
     a 0 message !close! the file, so you have to go thru open/1
     to play it again; also, 1 is accepted only if the file was
     already open.

     */

  if (state)
    {
      if (this->fh)
	this->state = state;
    }
  else
    {
      if (this->fh)
	{
	  readsf_file_close(this);
	  this->state = 0;
	}
    }
}


static void
readsf_open(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 0, 0);
  int ret;

  this->state = 0;		/* assure that playback do not start at open time */

  if (this->fh)
    readsf_file_close(this);

  if (s)
    this->sndfile = s;

  ret = readsf_file_open(this);

  if (! ret)
    post("readsf~: cannot open file `%s' for reading\n", fts_symbol_name(this->sndfile));
}



static void
readsf_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this  = (readsf_t *)o;

  if (this->state)
    post("readsf~: active\n");
  else if (this->fh)
    post("readsf~: opened\n");
  else
    post("readsf~: inactive\n");
}

/*
   DSP  this implementation support only mono and stero files.
   
*/

#if defined(SGI)
static void
readsf_mono_wrapper(fts_word_t *argv)
{
  readsf_t *this = (readsf_t *)fts_word_get_ptr(argv+2);
  float *chan1 = (float *)fts_word_get_ptr(argv);
  int size = fts_word_get_long(argv+1);
  short samps[512];
  int i;

  if (this->state && AFreadframes(this->fh, AF_DEFAULT_TRACK, samps, size))
    {
      for (i = 0; i < size; i += 8)
	{
	  float f0, f1, f2, f3, f4, f5, f6, f7;

	  f0 = ((float)samps[i + 0])/SHORT_TO_SIGNAL_COEF;
	  f1 = ((float)samps[i + 1])/SHORT_TO_SIGNAL_COEF;
	  f2 = ((float)samps[i + 2])/SHORT_TO_SIGNAL_COEF;
	  f3 = ((float)samps[i + 3])/SHORT_TO_SIGNAL_COEF;
	  f4 = ((float)samps[i + 4])/SHORT_TO_SIGNAL_COEF;
	  f5 = ((float)samps[i + 5])/SHORT_TO_SIGNAL_COEF;
	  f6 = ((float)samps[i + 6])/SHORT_TO_SIGNAL_COEF;
	  f7 = ((float)samps[i + 7])/SHORT_TO_SIGNAL_COEF;

	  chan1[i + 0] = f0;
	  chan1[i + 1] = f1;
	  chan1[i + 2] = f2;
	  chan1[i + 3] = f3;
	  chan1[i + 4] = f4;
	  chan1[i + 5] = f5;
	  chan1[i + 6] = f6;
	  chan1[i + 7] = f7;
	}
    }
  else
    {
      for (i = 0; i < size; i += 8)
	{
	  chan1[i + 0] = 0.0f;
	  chan1[i + 1] = 0.0f;
	  chan1[i + 2] = 0.0f;
	  chan1[i + 3] = 0.0f;
	  chan1[i + 4] = 0.0f;
	  chan1[i + 5] = 0.0f;
	  chan1[i + 6] = 0.0f;
	  chan1[i + 7] = 0.0f;
	}
    }
}

#elif defined(SOLARIS2)

static void
readsf_mono_wrapper(fts_word_t *argv)
{
  readsf_t *this = (readsf_t *)fts_word_get_ptr(argv+2);
  float *chan1 = (float *)fts_word_get_ptr(argv);
  int size = fts_word_get_long(argv+1);
  short samps[512];
  int i;

  if (this->state && read(this->fh, samps, size * sfclass(&this->sfhdr)) > 0)
    {
      if (sfmachine(&this->sfhdr) == SF_SND)
	for (i = 0; i < size; i += 8)
	  {
	    float f0, f1, f2, f3, f4, f5, f6, f7;

	    f0 = ((float)samps[i + 0])/SHORT_TO_SIGNAL_COEF;
	    f1 = ((float)samps[i + 1])/SHORT_TO_SIGNAL_COEF;
	    f2 = ((float)samps[i + 2])/SHORT_TO_SIGNAL_COEF;
	    f3 = ((float)samps[i + 3])/SHORT_TO_SIGNAL_COEF;
	    f4 = ((float)samps[i + 4])/SHORT_TO_SIGNAL_COEF;
	    f5 = ((float)samps[i + 5])/SHORT_TO_SIGNAL_COEF;
	    f6 = ((float)samps[i + 6])/SHORT_TO_SIGNAL_COEF;
	    f7 = ((float)samps[i + 7])/SHORT_TO_SIGNAL_COEF;

	    chan1[i + 0] = f0;
	    chan1[i + 1] = f1;
	    chan1[i + 2] = f2;
	    chan1[i + 3] = f3;
	    chan1[i + 4] = f4;
	    chan1[i + 5] = f5;
	    chan1[i + 6] = f6;
	    chan1[i + 7] = f7;
	  }
      else
	{
	  STconvertToFloatBuffer(&this->sfhdr, samps, size);
	  memcpy(chan1, samps, size * sizeof(float));
	}

    }
  else
    {
      for (i = 0; i < size; i += 8)
	{
	  chan1[i + 0] = 0.0f;
	  chan1[i + 1] = 0.0f;
	  chan1[i + 2] = 0.0f;
	  chan1[i + 3] = 0.0f;
	  chan1[i + 4] = 0.0f;
	  chan1[i + 5] = 0.0f;
	  chan1[i + 6] = 0.0f;
	  chan1[i + 7] = 0.0f;
	}
    }
}
#endif

#if defined(SGI)

static void
readsf_stereo_wrapper(fts_word_t *argv)
{
  readsf_t *this = (readsf_t *)fts_word_get_ptr(argv+3);
  float *chan1 = (float *)fts_word_get_ptr(argv);
  float *chan2 = (float *)fts_word_get_ptr(argv+1);
  int size = fts_word_get_long(argv+2);
  short samps[512];
  int i2, i;

  if (this->state && AFreadframes(this->fh, AF_DEFAULT_TRACK, samps, size))
    {
      for (i = 0, i2 = 0; i < size; i += 4, i2 += 8)
	{
	  float f10, f11, f12, f13, f20, f21, f22, f23;

	  f10 = ((float)samps[i2 +  0])/SHORT_TO_SIGNAL_COEF;
	  f20 = ((float)samps[i2 +  1])/SHORT_TO_SIGNAL_COEF;
	      
	  f11 = ((float)samps[i2 +  2])/SHORT_TO_SIGNAL_COEF;
	  f21 = ((float)samps[i2 +  3])/SHORT_TO_SIGNAL_COEF;

	  f12 = ((float)samps[i2 +  4])/SHORT_TO_SIGNAL_COEF;
	  f22 = ((float)samps[i2 +  5])/SHORT_TO_SIGNAL_COEF;

	  f13 = ((float)samps[i2 +  6])/SHORT_TO_SIGNAL_COEF;
	  f23 = ((float)samps[i2 +  7])/SHORT_TO_SIGNAL_COEF;

	  chan1[i + 0] = f10;
	  chan2[i + 0] = f20;

	  chan1[i + 1] = f11;
	  chan2[i + 1] = f21;

	  chan1[i + 2] = f12;
	  chan2[i + 2] = f22;

	  chan1[i + 3] = f13;
	  chan2[i + 3] = f23;
	}
    }
  else
    {
      for (i = 0; i < size; i += 8)
	{
	  chan1[i + 0] = 0.0f;
	  chan1[i + 1] = 0.0f;
	  chan1[i + 2] = 0.0f;
	  chan1[i + 3] = 0.0f;
	  chan1[i + 4] = 0.0f;
	  chan1[i + 5] = 0.0f;
	  chan1[i + 6] = 0.0f;
	  chan1[i + 7] = 0.0f;

	  chan2[i + 0] = 0.0f;
	  chan2[i + 1] = 0.0f;
	  chan2[i + 2] = 0.0f;
	  chan2[i + 3] = 0.0f;
	  chan2[i + 4] = 0.0f;
	  chan2[i + 5] = 0.0f;
	  chan2[i + 6] = 0.0f;
	  chan2[i + 7] = 0.0f;
	}
    }

}

#elif defined(SOLARIS2)

static void
readsf_stereo_wrapper(fts_word_t *argv)
{
  readsf_t *this = (readsf_t *)fts_word_get_ptr(argv+3);
  float *chan1 = (float *)fts_word_get_ptr(argv);
  float *chan2 = (float *)fts_word_get_ptr(argv+1);
  int size = fts_word_get_long(argv+2);
  int i2, i;

  if (this->state)
    {
      if (sfmachine(&this->sfhdr) == SF_SND)
	{
	  short samps[512];
	  if (read(this->fh, samps, size * 2 * sfclass(&this->sfhdr)) > 0)
	    for (i = 0, i2 = 0; i < size; i += 4, i2 += 8)
	      {
		float f10, f11, f12, f13, f20, f21, f22, f23;
		
		f10 = ((float)samps[i2 +  0])/SHORT_TO_SIGNAL_COEF;
		f20 = ((float)samps[i2 +  1])/SHORT_TO_SIGNAL_COEF;
	      
		f11 = ((float)samps[i2 +  2])/SHORT_TO_SIGNAL_COEF;
		f21 = ((float)samps[i2 +  3])/SHORT_TO_SIGNAL_COEF;
		
		f12 = ((float)samps[i2 +  4])/SHORT_TO_SIGNAL_COEF;
		f22 = ((float)samps[i2 +  5])/SHORT_TO_SIGNAL_COEF;
	    
		f13 = ((float)samps[i2 +  6])/SHORT_TO_SIGNAL_COEF;
		f23 = ((float)samps[i2 +  7])/SHORT_TO_SIGNAL_COEF;
		
		chan1[i + 0] = f10;
		chan2[i + 0] = f20;
		
		chan1[i + 1] = f11;
		chan2[i + 1] = f21;
		
		chan1[i + 2] = f12;
		chan2[i + 2] = f22;
		
		chan1[i + 3] = f13;
		chan2[i + 3] = f23;
	      }
	}
      else
	{
	  float fsamps[512];
	  if (read(this->fh, fsamps, size * 2 * sfclass(&this->sfhdr)) > 0)
	    {
	      STconvertToFloatBuffer(&this->sfhdr, fsamps, 2 * size);
	      for (i = 0, i2 = 0; i < size; i += 4, i2 += 8)
		{
		  chan1[i + 0] = fsamps[i2 + 0];
		  chan2[i + 0] = fsamps[i2 + 1];
		  
		  chan1[i + 1] = fsamps[i2 + 2];
		  chan2[i + 1] = fsamps[i2 + 3];
	    
		  chan1[i + 2] = fsamps[i2 + 4];
		  chan2[i + 2] = fsamps[i2 + 5];
	    
		  chan1[i + 3] = fsamps[i2 + 6];
		  chan2[i + 3] = fsamps[i2 + 7];
		}
	    }
	}
    }
  else
    {
      for (i = 0; i < size; i += 8)
	{
	  chan1[i + 0] = 0.0f;
	  chan1[i + 1] = 0.0f;
	  chan1[i + 2] = 0.0f;
	  chan1[i + 3] = 0.0f;
	  chan1[i + 4] = 0.0f;
	  chan1[i + 5] = 0.0f;
	  chan1[i + 6] = 0.0f;
	  chan1[i + 7] = 0.0f;

	  chan2[i + 0] = 0.0f;
	  chan2[i + 1] = 0.0f;
	  chan2[i + 2] = 0.0f;
	  chan2[i + 3] = 0.0f;
	  chan2[i + 4] = 0.0f;
	  chan2[i + 5] = 0.0f;
	  chan2[i + 6] = 0.0f;
	  chan2[i + 7] = 0.0f;
	}
    }

}
#endif

static void
readsf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[4];

  if (this->nchans == 1)
    {
      /* generation for mono file */

      fts_set_symbol(argv + 0, fts_dsp_get_output_name(dsp, 0));
      fts_set_long  (argv + 1, fts_dsp_get_output_size(dsp, 0));
      fts_set_ptr   (argv + 2, o);
      dsp_add_funcall(readsf_mono_function, 3, argv);
    }
  else if (this->nchans == 2)
    {
      /* generation for stereofile */

      fts_set_symbol(argv + 0, fts_dsp_get_output_name(dsp, 0));
      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 1));
      fts_set_long  (argv + 2, fts_dsp_get_output_size(dsp, 0));
      fts_set_ptr   (argv + 3, o);
      dsp_add_funcall(readsf_stereo_function, 4, argv);
    }
}

static void
readsf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  long int nchans = fts_get_long_arg(ac, at, 1, 0); /* ignore other args */

  if (nchans < 1)
    nchans = 1;
  else if (nchans > 2)
    nchans = 2;

  this->nchans = nchans;
  this->fh = 0;
  this->state = 0;

  dsp_list_insert(o); /* just put object in list */
}


static void
readsf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  if (this->fh)
    readsf_file_close(this);

  dsp_list_remove(o);
}



/*
 * Architecture independant part
 */

static fts_status_t
writesf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int nchans, i;
  fts_symbol_t a[4];

  nchans = fts_get_long_arg(ac, at, 1, 1);

  if (nchans < 1)
    nchans = 1;
  else if (nchans > 2)
    nchans = 2;

  fts_class_init(cl, sizeof(writesf_t), nchans, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, writesf_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, writesf_delete, 0, 0);
  
  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, writesf_put, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, writesf_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, writesf_number, 1, a);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("open"),  writesf_open, 2, a, 0);

  fts_method_define(cl, 0, fts_new_symbol("print"), writesf_print, 0, 0);

  for (i = 0; i < nchans; i++)
    dsp_sig_inlet(cl, i);

  writesf_mono_function = fts_new_symbol("writesf_mono");
  writesf_stereo_function = fts_new_symbol("writesf_stereo");
  dsp_declare_function(writesf_mono_function, ftl_mono_writesf);
  dsp_declare_function(writesf_stereo_function, ftl_stereo_writesf);

  /* DSP properties  */

  /*   fts_class_put_prop(cl, fts_s_dsp_is_sink, fts_true); */
  
  return fts_Success;
}


static fts_status_t
readsf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int nchans, i;
  fts_symbol_t a[4];

  nchans = fts_get_long_arg(ac, at, 1, 1);

  /*
     Note that the SGI version for the moment support only
     stereo and mono files.
     */

  if (nchans < 1)
    nchans = 1;
  else if (nchans > 2)
    nchans = 2;

  fts_class_init(cl, sizeof(readsf_t), 1, nchans, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;;
  a[2] = fts_s_int;;
  a[3] = fts_s_int;;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, readsf_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, readsf_delete, 0, 0);


  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, readsf_put, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, readsf_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, readsf_number, 1, a);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("open"),  readsf_open, 3, a, 0);

  fts_method_define(cl, 0, fts_new_symbol("print"), readsf_print, 0, 0);

  dsp_sig_inlet(cl, 0);	/* only for order forcing (usefull ?) */

  for (i = 0; i < nchans; i++)
    dsp_sig_outlet(cl, i);

  readsf_mono_function = fts_new_symbol("readsf_mono");
  readsf_stereo_function = fts_new_symbol("readsf_stereo");
  dsp_declare_function(readsf_mono_function, readsf_mono_wrapper);
  dsp_declare_function(readsf_stereo_function, readsf_stereo_wrapper);

  return fts_Success;
}

void
disk_config(void)
{
  fts_metaclass_create(fts_new_symbol("writesf~"),writesf_instantiate, fts_first_arg_equiv);
  fts_metaclass_create(fts_new_symbol("readsf~"),readsf_instantiate, fts_first_arg_equiv);
}
