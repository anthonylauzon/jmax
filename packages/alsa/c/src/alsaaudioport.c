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
 */

/* 
 * This file include the jMax ALSA audio port.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <alsa/asoundlib.h>

#include <fts/fts.h>

#include "alsaaudio.h" 


#define GUESS_CHANNELS -1

/* Defaults:
   If these defaults are not available for current card, 
   we check which parameters are supported by audio card.
*/
#define DEFAULT_FORMAT s_s32_le
#define DEFAULT_ACCESS s_mmap_noninterleaved
#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 2048
#define DEFAULT_INPUT_CHANNELS 26
#define DEFAULT_OUTPUT_CHANNELS 26

/* ---------------------------------------------------------------------- */
/* Structure used for both capture and playback                           */
/* ---------------------------------------------------------------------- */
typedef struct _alsastream_t {
  snd_pcm_t* handle;
  int period_size;
  size_t bytes_per_sample, bytes_per_frame;
  int channels;
  int fd;

/* storing of current configuration */
  snd_pcm_stream_t stream;
  int mode;
  snd_pcm_access_t access;
  int format;
  int fifo_size;

} alsastream_t;

/* ---------------------------------------------------------------------- */
/* The fts_audioport_t derived class                                      */
/* ---------------------------------------------------------------------- */
typedef struct {
  fts_audioport_t head;
  alsastream_t capture;
  alsastream_t playback;

  void *input_buffer;
  void *output_buffer;
  int xrun;
  fts_symbol_t device_name;
  
  size_t to_output;
} alsaaudioport_t;

/* ********************************************************************** */
/* debug                                                                  */
/* ********************************************************************** */

static snd_output_t *log_for_post = 0;
static int alsastream_open( alsastream_t *stream, 
			    const char *pcm_name, 
			    int which_stream, 
			    int* format, 
			    int channels, 
			    int sampling_rate, 
			    int fifo_size, 
			    snd_pcm_access_t* access);

static void
alsaaudioport_update_audioport_input_functions(alsaaudioport_t* self, alsastream_t* stream);

static void
alsaaudioport_update_audioport_output_functions(alsaaudioport_t* self, alsastream_t* stream);

static void post_log( void)
{
  char *p, *q;

  if ( !log_for_post)
    return;

  snd_output_buffer_string( log_for_post, &p);

  do
  {
    q = index( p, '\n');
    if (q)
    {
      *q = '\0';
      post( "%s\n", p);
      p = q+1;
    }
  }
  while ( *p);

  snd_output_close( log_for_post);

  log_for_post = 0;
}

static snd_output_t *get_post_log( void)
{
  if (log_for_post)
    return log_for_post;
  
  snd_output_buffer_open( &log_for_post);

  return log_for_post;
}


static int post_snd_output_buffer(snd_output_t* out)
{
  size_t buffer_size;
  char* output_content;
    
  buffer_size = snd_output_buffer_string(out, &output_content);
  if (buffer_size > 0)
  {
    output_content[buffer_size - 1] = '\0';
  }
  post("%s\n", output_content);
  return buffer_size;
}

/* ********************************************************************** */
/* alsastream_t functions                                                 */
/* ********************************************************************** */

/** 
 * Convert ALSA access type to current use access type 
 * 
 * @param access input ALSA access type
 * 
 * @return access_index for function array
 * @return -1 if access type is unknown ....
 */
static int 
convert_alsa_access_to_access_index(snd_pcm_access_t access)
{
  int access_index;

  /* convert access to access_index */
  switch(access)
  {
  case SND_PCM_ACCESS_MMAP_NONINTERLEAVED:
    access_index = 0;
    break;
  case SND_PCM_ACCESS_MMAP_INTERLEAVED:
    access_index = 1;
    break;
  case SND_PCM_ACCESS_RW_NONINTERLEAVED:
    access_index = 2;
    break;
  case SND_PCM_ACCESS_RW_INTERLEAVED:
    access_index = 3;
    break;
  default:
    access_index = -1;
  }
  return access_index;
}

/** 
 * Get name of ALSA access
 * 
 * @param access ALSA access
 * 
 * @return name of ALSA access
 */
static const char* get_alsa_access_type_name(snd_pcm_access_t access)
{
  const char* access_type_name;

  switch(access)
  {
  case SND_PCM_ACCESS_MMAP_INTERLEAVED:
    access_type_name =  "mmap access with simple interleaved channels ";
    break;
  case SND_PCM_ACCESS_MMAP_NONINTERLEAVED:
    access_type_name =  "mmap access with simple non interleaved channels ";
    break;
  case SND_PCM_ACCESS_MMAP_COMPLEX:
    access_type_name =  "mmap access with complex placement ";
    break;
  case SND_PCM_ACCESS_RW_INTERLEAVED:
    access_type_name =  "snd_pcm_readi/snd_pcm_writei access ";
    break;

  case SND_PCM_ACCESS_RW_NONINTERLEAVED:
    access_type_name =  "snd_pcm_readn/snd_pcm_writen access ";
    break;

  default:
    access_type_name =  "Unknown access type ";
    break;       
  }

  return access_type_name;
}



/** 
 * Check if access is available for given handle.
 * If not try to find a better choice 
 * 
 * @param handle handle of PCM device
 * @param params hw_params of PCM device
 * @param access wanted access type
 * 
 * @return new access type
 */
static snd_pcm_access_t check_better_access_type(snd_pcm_t* handle, 
						 snd_pcm_hw_params_t* params, 
						 snd_pcm_access_t access)
{
  int test_result = -1;

  /* check if wanted access is available */
  if (snd_pcm_hw_params_test_access(handle, params, access) >= 0)
  {
    return access;
  }

  fts_log("[alsaaudioport] access type (%s) not available, try to find a better choice \n",
	  get_alsa_access_type_name(access));

  /* loop initialization */
  access = SND_PCM_ACCESS_MMAP_INTERLEAVED;
  test_result = snd_pcm_hw_params_test_access(handle, params, access);
  while ( (test_result < 0) 
	  && (access <= SND_PCM_ACCESS_LAST))
  {
    ++access;
    test_result = snd_pcm_hw_params_test_access(handle, params, access);
  }

  if (test_result >= 0)
  {
    fts_log("[alsaaudioport] found a better choice (%s)\n",
	    get_alsa_access_type_name(access));
  }
  return access;
}


/** 
 * Get name of ALSA PCM Format
 * 
 * @param format ALSA PCM Format
 * 
 * @return name of ALSA PCM Format
 */
static const char* get_alsa_sample_format_name(snd_pcm_format_t format)
{
  const char* sample_format_name;
  switch(format)
  {
  case SND_PCM_FORMAT_S8:
    sample_format_name = "Signed 8 bit ";
    break;
  case SND_PCM_FORMAT_U8:
    sample_format_name = "Unsigned 8 bit ";
    break;
  case SND_PCM_FORMAT_S16:	
    sample_format_name = "Signed 16 bit CPU endian ";
    break;
  case SND_PCM_FORMAT_U16:
    sample_format_name = "Unsigned 16 bit CPU endian ";
    break;
  case SND_PCM_FORMAT_S24:
    sample_format_name = "Signed 24 bit CPU endian ";
    break;
  case SND_PCM_FORMAT_U24:
    sample_format_name = "Unsigned 24 bit CPU endian ";
    break;
  case SND_PCM_FORMAT_S32:	
    sample_format_name = "Signed 32 bit CPU endian ";
    break;
  case SND_PCM_FORMAT_U32:	
    sample_format_name = "Unsigned 32 bit CPU endian ";
    break;
  case SND_PCM_FORMAT_FLOAT:	
    sample_format_name = "Float 32 bit CPU endian ";
    break;
  case SND_PCM_FORMAT_FLOAT64:	
    sample_format_name = "Float 64 bit CPU endian ";
    break;
  case SND_PCM_FORMAT_IEC958_SUBFRAME:	
    sample_format_name = "IEC-958 CPU Endian ";
    break;
  default:
    sample_format_name = "Unknown Format ";
    break;
  }
  return sample_format_name;
}

/** 
 * Check if PCM format is available for given handle
 * If not try to find a better choice 
 *
 * @param handle handle of PCM device
 * @param params hw_params of PCM device
 * @param format wanted PCM format
 * 
 * @return new PCM format
 */
static int check_better_sample_format(snd_pcm_t* handle,
				      snd_pcm_hw_params_t* params,
				      int format)
{
  int test_result = -1;

  if (snd_pcm_hw_params_test_format(handle, params, format) >= 0)
  {
    return format;
  }

  fts_log("[alsaaudioport] sample format (%s) not available, try to find a better choice \n",
	  get_alsa_sample_format_name(format));

  /* loop initialization */
  format = SND_PCM_FORMAT_IEC958_SUBFRAME;
  test_result = snd_pcm_hw_params_test_format(handle, params, format);

  while ( (test_result < 0)
	  && (format >= SND_PCM_FORMAT_S8))
  {
    --format;
    test_result = snd_pcm_hw_params_test_format(handle, params, format);	
  }

  if (test_result >= 0)
  {
    fts_log("[alsaaudioport] found a better choice (%s)\n",
	    get_alsa_sample_format_name(format));

  }

  return format;
}



static void
alsaaudioport_sample_rate_change(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  alsaaudioport_t* self = (alsaaudioport_t*)o;
  int err;
  alsastream_t* stream;
  int format_is_32;

  if (fts_is_number(at))
  {
    int sampling_rate = (int)fts_get_number_float(at);
    /* close playback handle */
    if (NULL != self->playback.handle)
    {
      /* we change only if device was previously opened */
      snd_pcm_close(self->playback.handle);
      
      stream = &self->playback;
      err = alsastream_open(stream, self->device_name, stream->stream, &stream->format, stream->channels, sampling_rate, stream->fifo_size, &stream->access);
      
      if (err < 0)
      {
	post("[alsaaudioport] sample rate change: failed (%s)\n", snd_strerror(err));
	fts_log("[alsaaudioport] sample rate change: failed (%s)\n", snd_strerror(err));
      }
      else
      {
	fts_log("[alsaaudioport] sample rate change: success \n");	
	/* update output functions */
	alsaaudioport_update_audioport_output_functions(self, stream);
      }
    }
    
    if (NULL != self->capture.handle)
    {
      /* we change only if device was previously opened */
      snd_pcm_close(self->capture.handle);
      
      stream = &self->capture;
      err = alsastream_open(stream, self->device_name, stream->stream, &stream->format, stream->channels, sampling_rate, stream->fifo_size, &stream->access);
      
      if (err < 0)
      {
	post("[alsaaudioport] sample rate change: failed (%s)\n", snd_strerror(err));
	fts_log("[alsaaudioport] sample rate change: failed (%s)\n", snd_strerror(err));
      }
      else
      {
	fts_log("[alsaaudioport] sample rate change: success \n");	
	/* update input functions */
	alsaaudioport_update_audioport_input_functions(self, stream);
      }
    }
  }

}

/* ---------------------------------------------------------------------- */
/* alsatream_open: opens a stream                                         */
/* ---------------------------------------------------------------------- */

static int alsastream_open( alsastream_t *stream, const char *pcm_name, int which_stream, int* format, int channels, int sampling_rate, int fifo_size, snd_pcm_access_t* access)
{
/*     snd_pcm_access_mask_t *mask; */
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;

  int err = 0;
  int open_mode = 0;
  int dir;


  snd_pcm_hw_params_alloca( &hwparams);
  snd_pcm_sw_params_alloca( &swparams);

  open_mode |= SND_PCM_NONBLOCK;
  /*
   * Open the PCM device
   */
  if ( (err = snd_pcm_open( &stream->handle, pcm_name, which_stream, open_mode)) < 0)
  {
    fts_log("[alsaaudioport] cannot open ALSA PCM device %s (%s)\n", pcm_name, snd_strerror(err));
    return err;
  }
  stream->stream = which_stream;
  stream->mode = open_mode;

  if ((err = snd_pcm_hw_params_any( stream->handle, hwparams)) < 0)
  {
    fts_log("[alsaaudioport] broken configuration for handle: no configuration available (%s)\n", 
	    snd_strerror(err));
    return err;
  }

  /*
   * Set the access mode:
   */
  /* 
     Check if acces type is available,
     return a better choice if not 
  */
  *access = check_better_access_type(stream->handle, hwparams, *access);
  if ((err = snd_pcm_hw_params_set_access( stream->handle, hwparams, *access)) < 0)
  {
    fts_log("[alsaaudioport] acces type not available for handle: (%s)\n", snd_strerror(err));
    return err;
  }
  stream->access = *access;

  /*
   * Set the sample format
   */
  *format = check_better_sample_format(stream->handle, hwparams, *format);
  if ((err = snd_pcm_hw_params_set_format( stream->handle, hwparams, *format)) < 0)
  {
    fts_log("[alsaaudioport] sample format not available: (%s)\n", snd_strerror(err));
    return err;
  }
  stream->format = *format;

  /*
   * Set the number of channels
   */  
  if ( (snd_pcm_hw_params_get_channels_max(hwparams) < channels)
       || (snd_pcm_hw_params_get_channels_min(hwparams) > channels)
    )
  {
    fts_log("[alsaaudioport] channels count (%i) not available \n", channels);
    channels = snd_pcm_hw_params_get_channels_max(hwparams);
    fts_log("[alsaaudioport] get max number of channels (%i)\n", channels);
  }
      
  if ((err = snd_pcm_hw_params_set_channels( stream->handle, hwparams, channels)) < 0) 
  {
    fts_log("[alsaaudioport] channels count (%i) not available (%s)\n", channels, snd_strerror(err));
    return err;
  }

  stream->channels = snd_pcm_hw_params_get_channels( hwparams);

  /*
   * Set the sampling rate
   */
  if ((err = snd_pcm_hw_params_set_rate_near( stream->handle, hwparams, sampling_rate, 0)) < 0)
  {
    fts_log("[alsaaudioport] rate %iHz not available for handle: (%s)\n", sampling_rate, snd_strerror(err));
    return err;
  }
  if (err != sampling_rate)
  {
    fts_log("[alsaaudioport] rate doesn't match (requested %iHz, get %iHz)\n", sampling_rate, err);
    post("[alsaaudioport] rate doesn't match (requested %iHz, get %iHz)\n", sampling_rate, err);
  }
    
  /*
   * Set buffer size
   */
  if ((err = snd_pcm_hw_params_set_buffer_size_near( stream->handle, hwparams, fifo_size)) < 0)
  {
    fts_log("[alsaaudioport] buffer size not available (%s)\n", snd_strerror(err));
    return err;
  }
  stream->fifo_size = fifo_size;

  if (err != fifo_size)
  {
    fts_log("[alsaaudioport] buffer size doesn't match (requested %i, get %i)\n", fifo_size, err);
    fifo_size = err;
  }
  
  /*
   * Set period size: the period size is the fifo size divided by the number of periods, here 2
   */
  if ((err = snd_pcm_hw_params_set_period_size_near( stream->handle, hwparams, fifo_size / 2, 0)) < 0) 
  {      
    fts_log("[alsaaudioport] period size not available (%s)\n", snd_strerror(err));
    return err;
  }

  stream->period_size = snd_pcm_hw_params_get_period_size( hwparams, &dir);

  /*
   * Set hardware parameters
   */
  if ((err = snd_pcm_hw_params( stream->handle, hwparams)) < 0)
  {
    snd_pcm_hw_params_dump( hwparams, get_post_log());
/*     post_log(); */
    return err;
  }

  snd_pcm_sw_params_current( stream->handle, swparams);

  if (SND_PCM_STREAM_PLAYBACK == which_stream)
  {
    /* start transfer when the buffer is full */
    if ((err = snd_pcm_sw_params_set_start_threshold( stream->handle, swparams, fifo_size)) < 0)
      return err;
  }

  if ((err = snd_pcm_sw_params_set_avail_min( stream->handle, swparams, stream->period_size)) < 0) 
    return err;

  /* align all transfers to 1 samples */
  if ((err = snd_pcm_sw_params_set_xfer_align( stream->handle, swparams, 1)) < 0)
    return err;

  if ((err = snd_pcm_sw_params( stream->handle, swparams)) < 0) 
  {
    snd_pcm_sw_params_dump(swparams, get_post_log());
    post_log();
    return err;
  }

  stream->bytes_per_sample = (snd_pcm_format_physical_width(*format) / 8);
  stream->bytes_per_frame = stream->bytes_per_sample * stream->channels;

  return 0;
}


/* ********************************************************************** */
/* I/O functions                                                          */
/* ********************************************************************** */

/* ---------------------------------------------------------------------- */
/* I/O functions for mmap or rw mode                                      */
/* ---------------------------------------------------------------------- */

static int xrun( alsaaudioport_t *port, snd_pcm_t *handle)
{
  snd_pcm_status_t *status;
  int err;
  

/*   post("[alsaaudioport] XRUN !!!! \n"); */
/*   return 0; */

  snd_pcm_status_alloca(&status);
  if ( (err = snd_pcm_status( handle, status)) < 0)
    return err;

  if ( snd_pcm_status_get_state( status) == SND_PCM_STATE_XRUN)
  {
    port->xrun = 1;
  }

  if (( err = snd_pcm_prepare( handle)) < 0)
    return err;

  return 0;
}


#define INPUT_COPY_FUN_INTERLEAVED(FUN_NAME, TYPE, SHIFT, CONV)  \
static void FUN_NAME(fts_audioport_t* port, float* buf, int buffsize, int channel)  \
{  \
  alsaaudioport_t* alsa_port = (alsaaudioport_t*)port;  \
  int ch;  \
  int channels = fts_audioport_get_max_channels(port, FTS_AUDIO_INPUT);  \
  int i;  \
  int j; \
  TYPE* buffer = (TYPE*)alsa_port->input_buffer; \
 \
  for (ch = 0; ch < channels; ++ch) \
  { \
    j = ch; \
    for (i = 0; i < buffsize; ++i) \
    { \
      buf[i] = (buffer[j] >> SHIFT) / CONV; \
      j += channels; \
    } \
  } \
} 


#define INPUT_COPY_FUN_NONINTERLEAVED(FUN_NAME, TYPE, SHIFT, CONV) \
static void FUN_NAME(fts_audioport_t* port, float* buf, int buffsize, int channel) \
{ \
  alsaaudioport_t* alsa_port = (alsaaudioport_t*)port; \
  int i; \
  TYPE* buffer; \
 \
  buffer = ((TYPE**)alsa_port->input_buffer)[channel]; \
  for (i = 0; i < buffsize; ++i) \
  { \
    buf[i] = (buffer[i] >> SHIFT) / CONV; \
  } \
}


#define INPUT_FUN_INTERLEAVED(FUN_NAME, SND_PCM_FUN) \
static void FUN_NAME(fts_audioport_t* port) \
{ \
  alsaaudioport_t* alsa_port = (alsaaudioport_t*)port; \
  ssize_t r; \
  size_t count; \
  u_char* data; \
     \
  data = (u_char*)alsa_port->input_buffer; \
  /* @@@@@ change this @@@@@ */ \
  count = fts_dsp_get_tick_size(); \
 \
  while (count > 0) \
  { \
    r = SND_PCM_FUN(alsa_port->capture.handle, data, count); \
    if (r == -EAGAIN || (r >= 0 && r < (ssize_t)count)) \
      snd_pcm_wait(alsa_port->capture.handle, 1000); \
    else if (r == -EPIPE) \
      xrun(alsa_port, alsa_port->capture.handle); \
    else if (r < 0) \
      post("%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r)); \
 \
    if (r > 0) \
    { \
      count -= r; \
      data += r * alsa_port->capture.bytes_per_frame; \
    } \
  } \
}

#define INPUT_FUN_NONINTERLEAVED(FUN_NAME, SND_PCM_FUN) \
static void FUN_NAME(fts_audioport_t* port) \
{ \
  alsaaudioport_t* alsa_port = (alsaaudioport_t*)port; \
  ssize_t r; \
  size_t count, result; \
  int channels = fts_audioport_get_max_channels(port, FTS_AUDIO_INPUT); \
  int ch; \
 \
  u_char** buffers; \
  buffers = (u_char**)alloca(channels * sizeof(u_char*)); \
 \
  /* @@@@@ change this @@@@@ */     \
  count = fts_dsp_get_tick_size(); \
  result = 0; \
 \
  while (count > 0) \
  { \
    for (ch = 0; ch < channels; ++ch) \
      buffers[ch] = ((u_char**)alsa_port->input_buffer)[ch] + result * alsa_port->capture.bytes_per_sample; \
    r = SND_PCM_FUN(alsa_port->capture.handle, (void**)buffers, count); \
    if (r == -EAGAIN || (r >= 0 && r < (ssize_t)count)) \
      snd_pcm_wait(alsa_port->capture.handle, 1000); \
    else if (r == -EPIPE) \
      xrun(alsa_port, alsa_port->capture.handle); \
    else if (r < 0) \
      post("%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r)); \
    if (r > 0) \
    { \
      result += r; \
      count -= r; \
    } \
  } \
}


#define OUTPUT_COPY_FUN_INTERLEAVED(FUN_NAME, TYPE, SHIFT, CONV) \
static void FUN_NAME(fts_audioport_t* port, float* buf, int buffsize, int channel) \
{ \
  alsaaudioport_t* alsa_port = (alsaaudioport_t*)port; \
  int channels = fts_audioport_get_max_channels(port, FTS_AUDIO_OUTPUT); \
  int i; \
  int j = channel; \
  TYPE* buffer; \
   \
  buffer = (TYPE*)alsa_port->output_buffer; \
 \
  for (i = 0; i < buffsize; ++i) \
  { \
    buffer[j] = ((TYPE) (CONV * buf[i])) << SHIFT; \
    j += channels; \
  } \
   \
  alsa_port->to_output = buffsize; \
} 

#define OUTPUT_COPY_FUN_NONINTERLEAVED(FUN_NAME, TYPE, SHIFT, CONV) \
static void FUN_NAME(fts_audioport_t* port, float* buff, int buffsize, int channel) \
{ \
  alsaaudioport_t* alsa_port = (alsaaudioport_t*)port; \
  int i; \
  TYPE* buffer = ((TYPE**)alsa_port->output_buffer)[channel]; \
 \
  for (i = 0; i < buffsize; ++i) \
  { \
    buffer[i] = ((TYPE) (CONV * buff[i])) << SHIFT; \
  } \
 \
  alsa_port->to_output = buffsize; \
}


#define OUTPUT_FUN_INTERLEAVED(FUN_NAME, SND_PCM_FUN) \
static void FUN_NAME(fts_audioport_t* port) \
{ \
  alsaaudioport_t* alsa_port = (alsaaudioport_t*)port; \
  ssize_t r; \
  size_t count; \
  u_char* data; \
   \
  data = (u_char*)alsa_port->output_buffer; \
  count = alsa_port->to_output; \
  while (count > 0) \
  { \
    r = SND_PCM_FUN(alsa_port->playback.handle, data, count); \
        if (r == -EAGAIN || (r >= 0 && r < (ssize_t)count))  \
      snd_pcm_wait( alsa_port->playback.handle, 1000);  \
    else if ( r == -EPIPE )  \
      xrun( alsa_port, alsa_port->playback.handle);  \
    else if (r < 0)  \
      post( "%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r));  \
    if ( r > 0)  \
    {  \
      count -= r;  \
      data += r * alsa_port->playback.bytes_per_frame;  \
    }  \
     \
  } \
  alsa_port->to_output = count; \
}


#define OUTPUT_FUN_NONINTERLEAVED(FUN_NAME, SND_PCM_FUN) \
static void FUN_NAME(fts_audioport_t* port) \
{ \
  alsaaudioport_t* alsa_port = (alsaaudioport_t*)port; \
  ssize_t r; \
  size_t count, result; \
  u_char** buffers; \
  int channels = fts_audioport_get_max_channels(port, FTS_AUDIO_OUTPUT); \
  int ch; \
 \
  buffers = (u_char**)alloca(channels * sizeof(u_char*)); \
  count = alsa_port->to_output; \
  result = 0; \
 \
  while (count > 0) \
  { \
    for (ch = 0; ch < channels; ++ch) \
    { \
      buffers[ch] = ((u_char**)alsa_port->output_buffer)[ch] + result*alsa_port->playback.bytes_per_sample; \
    } \
 \
    r = SND_PCM_FUN(alsa_port->playback.handle, (void**)buffers, count); \
    if (r == -EAGAIN || (r >= 0 && r < (ssize_t)count)) \
      snd_pcm_wait(alsa_port->playback.handle, 1000); \
    else if (r == -EPIPE) \
      xrun(alsa_port, alsa_port->playback.handle); \
    else if (r < 0) \
      post("%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r)); \
    if (r > 0) \
    { \
      result += r; \
      count -= r; \
    } \
 \
  } \
}


/*
  Conversion factors:
  16 bits: 32767
  24 bits: 8388607
*/


/* /\* to access this table: functions_table[mode][format][inout] *\/ */
/*   static ftl_wrapper_t functions_table[4][2][2] = { */
/*     { { alsa_input_16_mmap_n, alsa_output_16_mmap_n}, { alsa_input_32_mmap_n, alsa_output_32_mmap_n} },  */
/*     { { alsa_input_16_mmap_i, alsa_output_16_mmap_i}, { alsa_input_32_mmap_i, alsa_output_32_mmap_i} },  */
/*     { { alsa_input_16_rw_n, alsa_output_16_rw_n}, { alsa_input_32_rw_n, alsa_output_32_rw_n} },  */
/*     { { alsa_input_16_rw_i, alsa_output_16_rw_i}, { alsa_input_32_rw_i, alsa_output_32_rw_i} } */
/*   }; */


INPUT_COPY_FUN_INTERLEAVED( alsa_input_copy_fun_16_rw_i, short, 0, 32767.0f)
INPUT_COPY_FUN_INTERLEAVED( alsa_input_copy_fun_32_rw_i, long, 8, 8388607.0f)
INPUT_COPY_FUN_INTERLEAVED( alsa_input_copy_fun_16_mmap_i, short, 0, 32767.0f)
INPUT_COPY_FUN_INTERLEAVED( alsa_input_copy_fun_32_mmap_i, long, 8, 8388607.0f)
INPUT_COPY_FUN_NONINTERLEAVED( alsa_input_copy_fun_16_rw_n, short, 0, 32767.0f)
INPUT_COPY_FUN_NONINTERLEAVED( alsa_input_copy_fun_32_rw_n, long, 8, 8388607.0f)
INPUT_COPY_FUN_NONINTERLEAVED( alsa_input_copy_fun_16_mmap_n, short, 0, 32767.0f)
INPUT_COPY_FUN_NONINTERLEAVED( alsa_input_copy_fun_32_mmap_n, long, 8, 8388607.0f)

INPUT_FUN_INTERLEAVED( alsa_input_fun_16_rw_i, snd_pcm_readi)
INPUT_FUN_INTERLEAVED( alsa_input_fun_32_rw_i, snd_pcm_readi)
INPUT_FUN_INTERLEAVED( alsa_input_fun_16_mmap_i, snd_pcm_mmap_readi)
INPUT_FUN_INTERLEAVED( alsa_input_fun_32_mmap_i, snd_pcm_mmap_readi)
INPUT_FUN_NONINTERLEAVED( alsa_input_fun_16_rw_n, snd_pcm_readn)
INPUT_FUN_NONINTERLEAVED( alsa_input_fun_32_rw_n, snd_pcm_readn)
INPUT_FUN_NONINTERLEAVED( alsa_input_fun_16_mmap_n, snd_pcm_mmap_readn)
INPUT_FUN_NONINTERLEAVED( alsa_input_fun_32_mmap_n, snd_pcm_mmap_readn)

OUTPUT_COPY_FUN_INTERLEAVED( alsa_output_copy_fun_16_rw_i, short, 0, 32767.0f)
OUTPUT_COPY_FUN_INTERLEAVED( alsa_output_copy_fun_32_rw_i, long, 8, 8388607.0f)
OUTPUT_COPY_FUN_INTERLEAVED( alsa_output_copy_fun_16_mmap_i, short, 0, 32767.0f)
OUTPUT_COPY_FUN_INTERLEAVED( alsa_output_copy_fun_32_mmap_i, long, 8, 8388607.0f)
OUTPUT_COPY_FUN_NONINTERLEAVED( alsa_output_copy_fun_16_rw_n, short, 0, 32767.0f)
OUTPUT_COPY_FUN_NONINTERLEAVED( alsa_output_copy_fun_32_rw_n, long, 8, 8388607.0f)
OUTPUT_COPY_FUN_NONINTERLEAVED( alsa_output_copy_fun_16_mmap_n, short, 0, 32767.0f)
OUTPUT_COPY_FUN_NONINTERLEAVED( alsa_output_copy_fun_32_mmap_n, long, 8, 8388607.0f)

OUTPUT_FUN_INTERLEAVED( alsa_output_fun_16_rw_i, snd_pcm_writei)
OUTPUT_FUN_INTERLEAVED( alsa_output_fun_32_rw_i, snd_pcm_writei)
OUTPUT_FUN_INTERLEAVED( alsa_output_fun_16_mmap_i, snd_pcm_mmap_writei)
OUTPUT_FUN_INTERLEAVED( alsa_output_fun_32_mmap_i, snd_pcm_mmap_writei)
OUTPUT_FUN_NONINTERLEAVED( alsa_output_fun_16_rw_n, snd_pcm_writen)
OUTPUT_FUN_NONINTERLEAVED( alsa_output_fun_32_rw_n, snd_pcm_writen)
OUTPUT_FUN_NONINTERLEAVED( alsa_output_fun_16_mmap_n, snd_pcm_mmap_writen)
OUTPUT_FUN_NONINTERLEAVED( alsa_output_fun_32_mmap_n, snd_pcm_mmap_writen)



/* to acess to this table: io_copy_functions_table[mode][format][inout] */
/* static fts_audioport_copy_fun_t io_copy_functions_table */
static fts_audioport_copy_fun_t input_copy_functions_table[4][2] = {
  {
    alsa_input_copy_fun_16_mmap_n,
    alsa_input_copy_fun_32_mmap_n
  },
  {
    alsa_input_copy_fun_16_mmap_i,
    alsa_input_copy_fun_32_mmap_i
  },
  {
    alsa_input_copy_fun_16_rw_n,
    alsa_input_copy_fun_32_rw_n
  },
  {
    alsa_input_copy_fun_16_rw_i,
    alsa_input_copy_fun_32_rw_i
  }
};

/* to access to this table: io_functions_table[mode][format][inout] */
/* static fts_audioport_io_fun_t io_functions_table */
static fts_audioport_io_fun_t input_functions_table[4][2] = {
  {
    alsa_input_fun_16_mmap_n,
    alsa_input_fun_32_mmap_n
  },
  {
    alsa_input_fun_16_mmap_i,
    alsa_input_fun_32_mmap_i
  },
  {
    alsa_input_fun_16_rw_n,
    alsa_input_fun_32_rw_n
  },
  {
    alsa_input_fun_16_rw_i,
    alsa_input_fun_32_rw_i
  }
};

/* to acess to this table: io_copy_functions_table[mode][format][inout] */
/* static fts_audioport_copy_fun_t io_copy_functions_table */
static fts_audioport_copy_fun_t output_copy_functions_table[4][2] = {
  {
    alsa_output_copy_fun_16_mmap_n,
    alsa_output_copy_fun_32_mmap_n
  },
  {
    alsa_output_copy_fun_16_mmap_i,
    alsa_output_copy_fun_32_mmap_i
  },
  {
    alsa_output_copy_fun_16_rw_n,
    alsa_output_copy_fun_32_rw_n
  },
  {
    alsa_output_copy_fun_16_rw_i,
    alsa_output_copy_fun_32_rw_i
  }
};

/* to access to this table: io_functions_table[mode][format][inout] */
/* static fts_audioport_io_fun_t io_functions_table */
static fts_audioport_io_fun_t output_functions_table[4][2] = {
  {
    alsa_output_fun_16_mmap_n,
    alsa_output_fun_32_mmap_n
  },
  {
    alsa_output_fun_16_mmap_i,
    alsa_output_fun_32_mmap_i
  },
  {
    alsa_output_fun_16_rw_n,
    alsa_output_fun_32_rw_n
  },
  {
    alsa_output_fun_16_rw_i,
    alsa_output_fun_32_rw_i
  }
};

/* ********************************************************************** */
/* alsaaudioport methods                                                  */
/* ********************************************************************** */

static void *alsaaudioport_allocate_buffer( snd_pcm_access_t access, int channels, int tick_size, int sample_bytes)
{
  void *buffer;

  if ( access == SND_PCM_ACCESS_MMAP_NONINTERLEAVED || access == SND_PCM_ACCESS_RW_NONINTERLEAVED)
  {
    int ch;
    void **buffers;

    buffer = fts_malloc( channels * sizeof( void *));

    buffers = (void **)buffer;
    for ( ch = 0; ch < channels; ch++)
      buffers[ch] = fts_malloc( tick_size * sample_bytes);
  }
  else
  {
    buffer = fts_malloc( channels * tick_size * sample_bytes);
  }

  return buffer;
}


static void
alsaaudioport_update_audioport_input_functions(alsaaudioport_t* self, alsastream_t* stream)
{
  int format_is_32;
  /* convert access type to function array index */
  int access_index = convert_alsa_access_to_access_index(stream->access);
  if (-1 == access_index)
  {
    post("[alsaaudioport] playback access type unknown \n");
    fts_object_error((fts_object_t*)self,"playback access type unknown \n");
    return;
  }
  format_is_32 = (stream->format == SND_PCM_FORMAT_S32_LE);
    
  if (self->input_buffer)
  {
    fts_free(self->input_buffer);
  }
  self->input_buffer = alsaaudioport_allocate_buffer(stream->access, stream->channels, 
						     fts_dsp_get_tick_size(), snd_pcm_format_physical_width(stream->format)/8);

  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, input_functions_table[access_index][format_is_32]);
  fts_audioport_set_copy_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, input_copy_functions_table[access_index][format_is_32]);

}

static void
alsaaudioport_update_audioport_output_functions(alsaaudioport_t* self, alsastream_t* stream)
{
  int format_is_32;
  /* convert access type to function array index */
  int access_index = convert_alsa_access_to_access_index(stream->access);
  if (-1 == access_index)
  {
    post("[alsaaudioport] playback access type unknown \n");
    fts_object_error((fts_object_t*)self,"playback access type unknown \n");
    return;
  }
  format_is_32 = (stream->format == SND_PCM_FORMAT_S32_LE);


  if (self->output_buffer)
  {
    fts_free(self->output_buffer);
  }
  self->output_buffer = alsaaudioport_allocate_buffer( stream->access, stream->channels, fts_dsp_get_tick_size(), snd_pcm_format_physical_width(stream->format)/8);


  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, output_functions_table[access_index][format_is_32]);
  fts_audioport_set_copy_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, output_copy_functions_table[access_index][format_is_32]);
}

static int alsaaudioport_xrun_function( fts_audioport_t *port)
{
  alsaaudioport_t *ap = (alsaaudioport_t *)port;
  int xrun;

  xrun = ap->xrun;
  ap->xrun = 0;

  return xrun;
}

static void
alsaaudioport_open_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  alsaaudioport_t* self = (alsaaudioport_t*)o;

  int err;
  int format  = snd_pcm_format_value(DEFAULT_FORMAT);
  snd_pcm_access_t access = alsaaudiomanager_convert_jmax_symbol_to_alsa_access(DEFAULT_ACCESS);
  int sampling_rate = (int)fts_dsp_get_sample_rate();
  int fifo_size = DEFAULT_FIFO_SIZE;

  err = alsastream_open(&self->capture, self->device_name, 
			SND_PCM_STREAM_CAPTURE, &format, self->capture.channels, 
			sampling_rate, fifo_size, &access);

  if (err != 0)
  {
    fts_object_error(o, "Error when trying to open alsastream \n");
    post("[alsaaudioport_open_input] err: %s \n", snd_strerror(err));
    fts_log("[alsaaudioport_open_input] err: %s \n", snd_strerror(err));
    return;
  }
  /* 
     set fts_audioport io_fun and copy fun 
     and 
     memory allocation for buffers 
   */
  alsaaudioport_update_audioport_input_functions(self, &self->capture);

  fts_audioport_set_open((fts_audioport_t*)self, FTS_AUDIO_INPUT);

  fts_dsp_sample_rate_add_listener(o, alsaaudioport_sample_rate_change);
}

static void
alsaaudioport_open_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  alsaaudioport_t* self = (alsaaudioport_t*)o;

  int err;
  int format  = snd_pcm_format_value(DEFAULT_FORMAT);
  snd_pcm_access_t access = alsaaudiomanager_convert_jmax_symbol_to_alsa_access(DEFAULT_ACCESS);
  int sampling_rate = (int)fts_dsp_get_sample_rate();
  int fifo_size = DEFAULT_FIFO_SIZE;

  err = alsastream_open(&self->playback, self->device_name, 
			SND_PCM_STREAM_PLAYBACK, &format, self->playback.channels, 
			sampling_rate, fifo_size, &access);

  if (err != 0)
  {
    fts_object_error(o, "Error when trying to open alsastream \n");
    post("[alsaaudioport_open_output] err: %s \n", snd_strerror(err));
    fts_log("[alsaaudioport_open_output] err: %s \n", snd_strerror(err));
    return;
  }
  /* set fts_audioport io_fun and copy fun */
  alsaaudioport_update_audioport_output_functions(self, &self->playback);
  
  fts_audioport_set_open((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);
  
  fts_dsp_sample_rate_add_listener(o, alsaaudioport_sample_rate_change);
}

static void alsaaudioport_close_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  alsaaudioport_t* self = (alsaaudioport_t*)o;

  post("[alsaaudioport_close_input]\n");

  fts_audioport_unset_open((fts_audioport_t*)self, FTS_AUDIO_INPUT);

  if (self->capture.handle != 0)
    snd_pcm_close(self->capture.handle);

  fts_dsp_sample_rate_remove_listener(o);
}

static void alsaaudioport_close_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  alsaaudioport_t* self = (alsaaudioport_t*)o;

  post("[alsaaudioport_close_output]\n");

  fts_audioport_unset_open((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);

  if (self->playback.handle != 0)
    snd_pcm_close(self->playback.handle);

  fts_dsp_sample_rate_remove_listener(o);
}

static void alsaaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int sampling_rate, fifo_size, format, format_is_32, capture_channels, playback_channels, err;
  float sr;
  char pcm_name[256];
  const char *format_name;
  snd_pcm_access_t access;
  fts_symbol_t s_access;
  int access_index;
  alsaaudioport_t *self = (alsaaudioport_t *)o;

  int in_max_channels;
  int out_max_channels;

  fts_audioport_init((fts_audioport_t*)self);

  self->capture.handle = NULL;
  self->playback.handle = NULL;
    
  sr = fts_dsp_get_sample_rate();
  sampling_rate = (int)sr ;
  fifo_size = DEFAULT_FIFO_SIZE;
    
  self->device_name = fts_get_symbol(at);
  
  /* check for capture */
  in_max_channels = alsaaudiomanager_get_channels_max(self->device_name, SND_PCM_STREAM_CAPTURE);
  if (0 < in_max_channels)
  {
    fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_INPUT);
    fts_audioport_set_max_channels((fts_audioport_t*)self, FTS_AUDIO_INPUT, in_max_channels);
    self->capture.channels = in_max_channels;
  }

  /* check for playback */
  out_max_channels = alsaaudiomanager_get_channels_max(self->device_name, SND_PCM_STREAM_PLAYBACK);
  if (0 < out_max_channels)
  {
    fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);
    fts_audioport_set_max_channels((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, out_max_channels);
    self->playback.channels = out_max_channels;
  }

  self->to_output = 0;
}

static void alsaaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsaaudioport_t *self = (alsaaudioport_t *)o;

  fts_audioport_delete((fts_audioport_t*)self);


  if (self->input_buffer)
    fts_free( self->input_buffer);
  if (self->output_buffer)
    fts_free( self->output_buffer);
  if (self->capture.handle)
    snd_pcm_close(self->capture.handle);
  if (self->playback.handle)
    snd_pcm_close(self->playback.handle);
}

static void
alsaaudioport_print_all_device(alsaaudioport_t* self)
{
  snd_output_t *out;
  int err;

  err = snd_output_buffer_open(&out);
  if (err < 0)
  {
    fts_log("[alsaaudioport] cannot create config output buffer \n");
    return;
  }

  if (NULL != self->capture.handle)
  {

    if (snd_pcm_dump(self->capture.handle, out) < 0)
    {
      fts_log("[alsaaudioport] error when dumping capture handle (%s)\n", 
	      snd_strerror(err));
    }
    else
    {
      post("Capture Stream \n");
      post_snd_output_buffer(out);
    }
  }
  snd_output_flush(out);

  if (NULL != self->playback.handle)
  {
    if (snd_pcm_dump(self->playback.handle, out) < 0)
    {	    
      fts_log("[alsaaudioport] error when dumping capture handle (%s)\n", 
	      snd_strerror(err));
    }
    else
    {
      post("Playback Stream \n");
      post_snd_output_buffer(out);
    }
    snd_output_flush(out);
  }

  snd_output_close(out);    
}


static void alsaaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( alsaaudioport_t), alsaaudioport_init, alsaaudioport_delete);

  fts_class_message_varargs(cl, fts_s_open_input, alsaaudioport_open_input);
  fts_class_message_varargs(cl, fts_s_open_output, alsaaudioport_open_output);
  
  fts_class_message_varargs(cl, fts_s_close_input, alsaaudioport_close_input);
  fts_class_message_varargs(cl, fts_s_close_output, alsaaudioport_close_output);
}

/***********************************************************************
 *
 * Config
 *
 */
void alsaaudioport_config( void)
{
  fts_symbol_t s = fts_new_symbol("alsaaudioport");

  s_s32_le = fts_new_symbol( "S32_LE");
  s_s16_le = fts_new_symbol( "S16_LE");
  s_mmap_noninterleaved = fts_new_symbol( "mmap_noninterleaved");
  s_mmap_interleaved = fts_new_symbol( "mmap_interleaved");
  s_rw_noninterleaved = fts_new_symbol( "rw_noninterleaved");
  s_rw_interleaved = fts_new_symbol( "rw_interleaved");

  alsaaudioport_type = fts_class_install( s, alsaaudioport_instantiate);

  alsaaudiomanager_scan_devices();
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
