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

static fts_symbol_t s_default;
static fts_symbol_t s_s16_le, s_s32_le;
static fts_symbol_t s_mmap_noninterleaved, s_mmap_interleaved, s_rw_noninterleaved, s_rw_interleaved;

#define GUESS_CHANNELS -1
#define DEFAULT_PCM_NAME s_default
#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 2048
#define DEFAULT_INPUT_CHANNELS 0
#define DEFAULT_OUTPUT_CHANNELS 2
#define DEFAULT_ACCESS s_mmap_interleaved

/* ---------------------------------------------------------------------- */
/* Structure used for both capture and playback                           */
/* ---------------------------------------------------------------------- */
typedef struct _alsastream_t {
  snd_pcm_t *handle;
  int period_size;
  size_t bytes_per_sample, bytes_per_frame;
  int channels;
  int fd;
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
} alsaaudioport_t;

/* ********************************************************************** */
/* debug                                                                  */
/* ********************************************************************** */

static snd_output_t *log_for_post = 0;

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


/* ********************************************************************** */
/* alsastream_t functions                                                 */
/* ********************************************************************** */

/* ---------------------------------------------------------------------- */
/* alsatream_open: opens a stream                                         */
/* ---------------------------------------------------------------------- */

static int alsastream_open( alsastream_t *st, char *pcm_name, int which_stream, int format, int channels, int sampling_rate, int fifo_size, snd_pcm_access_t access)
{
  snd_pcm_access_mask_t *mask;
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  int err;
  int open_mode = 0;
  int dir;

  snd_pcm_hw_params_alloca( &hwparams);
  snd_pcm_sw_params_alloca( &swparams);

  open_mode |= SND_PCM_NONBLOCK;
  /*
   * Open the PCM device
   */
  if ( (err = snd_pcm_open( &st->handle, (char *)pcm_name, which_stream, open_mode)) < 0)
    return err;

  if ((err = snd_pcm_hw_params_any( st->handle, hwparams)) < 0)
    return err;

  /*
   * Set the access mode:
   */
  if ((err = snd_pcm_hw_params_set_access( st->handle, hwparams, access)) < 0)
    return err;

  /*
   * Set the sample format
   */
  if ((err = snd_pcm_hw_params_set_format( st->handle, hwparams, format)) < 0)
    return err;

  /*
   * Set the number of channels
   */
#if 0
  if (st->channels == GUESS_CHANNELS)
    st->channels = snd_pcm_hw_params_get_channels_max (hwparams);
#endif
  if ((err = snd_pcm_hw_params_set_channels( st->handle, hwparams, channels)) < 0) 
    return err;

  st->channels = snd_pcm_hw_params_get_channels( hwparams);

  /*
   * Set the sampling rate
   */
  if ((err = snd_pcm_hw_params_set_rate_near( st->handle, hwparams, sampling_rate, 0)) < 0)
    return err;

  /*
   * Set buffer size
   */
  if ((err = snd_pcm_hw_params_set_buffer_size_near( st->handle, hwparams, fifo_size)) < 0)
    return err;

  /*
   * Set period size: the period size is the fifo size divided by the number of periods, here 2
   */
  if ((err = snd_pcm_hw_params_set_period_size_near( st->handle, hwparams, fifo_size / 2, 0)) < 0) 
    return err;

  st->period_size = snd_pcm_hw_params_get_period_size( hwparams, &dir);

#ifdef DEBUG
  snd_pcm_hw_params_dump( hwparams, get_post_log());
  post_log();
#endif

  /*
   * Set hardware parameters
   */
  if ((err = snd_pcm_hw_params( st->handle, hwparams)) < 0)
    {
      snd_pcm_hw_params_dump( hwparams, get_post_log());
      post_log();
      return err;
    }

  snd_pcm_sw_params_current( st->handle, swparams);

  /* start transfer when the buffer is full */
  if ((err = snd_pcm_sw_params_set_start_threshold( st->handle, swparams, fifo_size)) < 0)
    return err;

  if ((err = snd_pcm_sw_params_set_avail_min( st->handle, swparams, st->period_size)) < 0) 
    return err;

  /* align all transfers to 1 samples */
  if ((err = snd_pcm_sw_params_set_xfer_align( st->handle, swparams, 1)) < 0)
    return err;

#ifdef DEBUG
  snd_pcm_sw_params_dump( swparams, get_post_log());
  post_log();
#endif

  if ((err = snd_pcm_sw_params( st->handle, swparams)) < 0) 
    {
      snd_pcm_sw_params_dump(swparams, get_post_log());
      post_log();
      return err;
    }

  st->bytes_per_sample = (snd_pcm_format_physical_width(format) / 8);
  st->bytes_per_frame = st->bytes_per_sample * st->channels;

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

#define INPUT_FUN_INTERLEAVED(FUN_NAME,TYPE,SND_PCM_FUN,SHIFT,CONV)	\
static void FUN_NAME( fts_word_t *argv)					\
{									\
  alsaaudioport_t *port;						\
  int n, channels, ch, i, j;						\
  ssize_t r;								\
  size_t count;								\
  u_char *data;								\
  TYPE *buffer;								\
									\
  port = (alsaaudioport_t *)fts_word_get_pointer( argv+0);		\
  n = fts_word_get_int(argv + 1);					\
  channels = fts_audioport_get_input_channels( port);			\
  buffer = (TYPE *)port->input_buffer;					\
									\
  data = (u_char *)buffer;						\
  count = n;								\
  while ( count > 0)							\
    {									\
      r = SND_PCM_FUN( port->capture.handle, data, count);		\
									\
      if (r == -EAGAIN || (r >= 0 && r < (ssize_t)count))		\
	snd_pcm_wait( port->capture.handle, 1000);			\
      else if ( r == -EPIPE )						\
	xrun( port, port->capture.handle);				\
      else if (r < 0)							\
	post( "%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r));	\
									\
      if ( r > 0)							\
	{								\
	  count -= r;							\
	  data += r * port->capture.bytes_per_frame;			\
	}								\
    }									\
									\
  for ( ch = 0; ch < channels; ch++)					\
    {									\
      float *out = (float *) fts_word_get_pointer( argv + 2 + ch);	\
									\
      j = ch;								\
      for ( i = 0; i < n; i++)						\
	{								\
	  out[i] = (buffer[j] >> SHIFT) / CONV;				\
	  j += channels;						\
	}								\
    }									\
}


#define OUTPUT_FUN_INTERLEAVED(FUN_NAME,TYPE,SND_PCM_FUN,SHIFT,CONV)	\
static void FUN_NAME( fts_word_t *argv)					\
{									\
  alsaaudioport_t *port;						\
  int n, channels, ch, i, j;						\
  ssize_t r;								\
  size_t count;								\
  u_char *data;								\
  TYPE *buffer;								\
									\
  port = (alsaaudioport_t *)fts_word_get_pointer( argv+0);		\
  n = fts_word_get_int(argv + 1);					\
  channels = fts_audioport_get_output_channels( port);			\
  buffer = (TYPE *)port->output_buffer;					\
									\
  for ( ch = 0; ch < channels; ch++)					\
    {									\
      float *in = (float *) fts_word_get_pointer( argv + 2 + ch);	\
									\
      j = ch;								\
      for ( i = 0; i < n; i++)						\
	{								\
	  buffer[j] = ((TYPE) (CONV * in[i])) << SHIFT;			\
	  j += channels;						\
	}								\
    }									\
									\
  data = (u_char *)buffer;						\
  count = n;								\
  while ( count > 0)							\
    {									\
      r = SND_PCM_FUN( port->playback.handle, data, count);		\
									\
      if (r == -EAGAIN || (r >= 0 && r < (ssize_t)count))		\
	snd_pcm_wait( port->playback.handle, 1000);			\
      else if ( r == -EPIPE )						\
	xrun( port, port->playback.handle);				\
      else if (r < 0)							\
	post( "%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r));	\
									\
      if ( r > 0)							\
	{								\
	  count -= r;							\
	  data += r * port->playback.bytes_per_frame;			\
	}								\
    }									\
}


#define INPUT_FUN_NONINTERLEAVED(FUN_NAME,TYPE,SND_PCM_FUN,SHIFT,CONV)					\
static void FUN_NAME( fts_word_t *argv)									\
{													\
  alsaaudioport_t *port;										\
  int n, channels, ch, i;										\
  ssize_t r;												\
  size_t count, result;											\
  u_char **buffers;											\
													\
  port = (alsaaudioport_t *)fts_word_get_pointer( argv+0);						\
  n = fts_word_get_int(argv + 1);									\
  channels = fts_audioport_get_input_channels( port);							\
													\
  buffers = (u_char **)alloca( channels * sizeof( u_char *));						\
  count = n;												\
  result = 0;												\
  while ( count > 0)											\
    {													\
      for ( ch = 0; ch < channels; ch++)								\
	buffers[ch] = ((u_char **)port->input_buffer)[ch] + result*port->capture.bytes_per_sample;	\
													\
      r = SND_PCM_FUN( port->capture.handle, (void **)buffers, count);					\
													\
      if (r == -EAGAIN || (r >= 0 && r < (ssize_t)count))						\
	snd_pcm_wait( port->capture.handle, 1000);							\
      else if ( r == -EPIPE )										\
	xrun( port, port->capture.handle);								\
      else if (r < 0)											\
	post( "%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r));					\
													\
      if ( r > 0)											\
	{												\
	  result += r;											\
	  count -= r;											\
	}												\
    }													\
													\
  for ( ch = 0; ch < channels; ch++)									\
    {													\
      TYPE *buffer = ((TYPE **)port->input_buffer)[ch];							\
      float *out = (float *) fts_word_get_pointer( argv + 2 + ch);					\
													\
      for ( i = 0; i < n; i++)										\
        out[i] = (buffer[i] >> SHIFT) / CONV;								\
    }													\
}

#define OUTPUT_FUN_NONINTERLEAVED(FUN_NAME,TYPE,SND_PCM_FUN,SHIFT,CONV)					\
static void FUN_NAME( fts_word_t *argv)									\
{													\
  alsaaudioport_t *port;										\
  int n, channels, ch, i;										\
  ssize_t r;												\
  size_t count, result;											\
  u_char **buffers;											\
													\
  port = (alsaaudioport_t *)fts_word_get_pointer( argv+0);						\
  n = fts_word_get_int(argv + 1);									\
  channels = fts_audioport_get_output_channels( port);							\
													\
  for ( ch = 0; ch < channels; ch++)									\
    {													\
      TYPE *buffer = ((TYPE **)port->output_buffer)[ch];						\
      float *in = (float *) fts_word_get_pointer( argv + 2 + ch);					\
													\
      for ( i = 0; i < n; i++)										\
	buffer[i] = ((TYPE) (CONV * in[i])) << SHIFT;							\
    }													\
													\
  buffers = (u_char **)alloca( channels * sizeof( u_char *));						\
  count = n;												\
  result = 0;												\
  while ( count > 0)											\
    {													\
      for ( ch = 0; ch < channels; ch++)								\
	buffers[ch] = ((u_char **)port->output_buffer)[ch] + result*port->playback.bytes_per_sample;	\
													\
      r = SND_PCM_FUN( port->playback.handle, (void **)buffers, count);					\
													\
      if (r == -EAGAIN || (r >= 0 && r < (ssize_t)count))						\
	snd_pcm_wait( port->playback.handle, 1000);							\
      else if ( r == -EPIPE )										\
	xrun( port, port->playback.handle);								\
      else if (r < 0)											\
	post( "%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r));					\
													\
      if ( r > 0)											\
	{												\
	  result += r;											\
	  count -= r;											\
	}												\
    }													\
}

/*
  Conversion factors:
   16 bits: 32767
   24 bits: 8388607
*/

INPUT_FUN_INTERLEAVED( alsa_input_16_rw_i, short, snd_pcm_readi, 0, 32767.0f)
INPUT_FUN_INTERLEAVED( alsa_input_32_rw_i, long, snd_pcm_readi, 8, 8388607.0f)
INPUT_FUN_INTERLEAVED( alsa_input_16_mmap_i, short, snd_pcm_mmap_readi, 0, 32767.0f)
INPUT_FUN_INTERLEAVED( alsa_input_32_mmap_i, long, snd_pcm_mmap_readi, 8, 8388607.0f)
OUTPUT_FUN_INTERLEAVED( alsa_output_16_rw_i, short, snd_pcm_writei, 0, 32767.0f)
OUTPUT_FUN_INTERLEAVED( alsa_output_32_rw_i, long, snd_pcm_writei, 8, 8388607.0f)
OUTPUT_FUN_INTERLEAVED( alsa_output_16_mmap_i, short, snd_pcm_mmap_writei, 0, 32767.0f)
OUTPUT_FUN_INTERLEAVED( alsa_output_32_mmap_i, long, snd_pcm_mmap_writei, 8, 8388607.0f)
INPUT_FUN_NONINTERLEAVED( alsa_input_16_rw_n, short, snd_pcm_readn, 0, 32767.0f)
INPUT_FUN_NONINTERLEAVED( alsa_input_32_rw_n, long, snd_pcm_readn, 8, 8388607.0f)
INPUT_FUN_NONINTERLEAVED( alsa_input_16_mmap_n, short, snd_pcm_mmap_readn, 0, 32767.0f)
INPUT_FUN_NONINTERLEAVED( alsa_input_32_mmap_n, long, snd_pcm_mmap_readn, 8, 8388607.0f)
OUTPUT_FUN_NONINTERLEAVED( alsa_output_16_rw_n, short, snd_pcm_writen, 0, 32767.0f)
OUTPUT_FUN_NONINTERLEAVED( alsa_output_32_rw_n, long, snd_pcm_writen, 8, 8388607.0f)
OUTPUT_FUN_NONINTERLEAVED( alsa_output_16_mmap_n, short, snd_pcm_mmap_writen, 0, 32767.0f)
OUTPUT_FUN_NONINTERLEAVED( alsa_output_32_mmap_n, long, snd_pcm_mmap_writen, 8, 8388607.0f)

/* to access this table: functions_table[mode][format][inout] */
static ftl_wrapper_t functions_table[4][2][2] = {
  { { alsa_input_16_mmap_n, alsa_output_16_mmap_n}, { alsa_input_32_mmap_n, alsa_output_32_mmap_n} }, 
  { { alsa_input_16_mmap_i, alsa_output_16_mmap_i}, { alsa_input_32_mmap_i, alsa_output_32_mmap_i} }, 
  { { alsa_input_16_rw_n, alsa_output_16_rw_n}, { alsa_input_32_rw_n, alsa_output_32_rw_n} }, 
  { { alsa_input_16_rw_i, alsa_output_16_rw_i}, { alsa_input_32_rw_i, alsa_output_32_rw_i} }
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

static int alsaaudioport_xrun_function( fts_audioport_t *port)
{
  alsaaudioport_t *ap = (alsaaudioport_t *)port;
  int xrun;

  xrun = ap->xrun;
  ap->xrun = 0;

  return xrun;
}

#ifdef DEBUG
static void pcm_dump_post( snd_pcm_t *handle)
{
}
#endif

static void alsaaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int sampling_rate, fifo_size, format, format_is_32, capture_channels, playback_channels, err;
  float sr;
  char pcm_name[256];
  const char *format_name;
  snd_pcm_access_t access;
  fts_symbol_t s_access;
  int access_index;
  alsaaudioport_t *this = (alsaaudioport_t *)o;

  fts_audioport_init( &this->head);

  ac--;
  at++;
  sr = fts_dsp_get_sample_rate();
  sampling_rate = (int)sr ;
  fifo_size = fts_param_get_int(fts_s_fifo_size, DEFAULT_FIFO_SIZE);

  strcpy( pcm_name, fts_get_symbol_arg( ac, at, 0, DEFAULT_PCM_NAME) );

  capture_channels = fts_get_int_arg( ac, at, 1, DEFAULT_INPUT_CHANNELS);
  playback_channels = fts_get_int_arg( ac, at, 2, DEFAULT_OUTPUT_CHANNELS);

  format_name = fts_get_symbol_arg( ac, at, 3, s_s16_le);
  format = snd_pcm_format_value( format_name);
  format_is_32 = (format == SND_PCM_FORMAT_S32_LE);

  s_access = fts_get_symbol_arg( ac, at, 4, DEFAULT_ACCESS);

  if (s_access == s_mmap_noninterleaved)
    {
      access = SND_PCM_ACCESS_MMAP_NONINTERLEAVED;
      access_index = 0;
    }
  else if (s_access == s_mmap_interleaved)
    {
      access = SND_PCM_ACCESS_MMAP_INTERLEAVED;
      access_index = 1;
    }
  else if (s_access == s_rw_noninterleaved)
    {
      access = SND_PCM_ACCESS_RW_NONINTERLEAVED;
      access_index = 2;
    }
  else
    {
      access = SND_PCM_ACCESS_RW_INTERLEAVED;
      access_index = 3;
    }

  if ( capture_channels != 0)
    {
      if ( (err = alsastream_open( &this->capture, pcm_name, SND_PCM_STREAM_CAPTURE, format, capture_channels, sampling_rate, fifo_size, access)) < 0)
	{
	  fts_object_set_error(o, "Error opening ALSA device (%s)", snd_strerror( err));
	  post("alsaaudioport: cannot open ALSA device %s (%s)\n", pcm_name, snd_strerror( err));
	  return;
	}

      fts_audioport_set_input_channels( (fts_audioport_t *)this, this->capture.channels);
      fts_audioport_set_input_function( (fts_audioport_t *)this, functions_table[access_index][format_is_32][0]);

      this->input_buffer = alsaaudioport_allocate_buffer( access, this->capture.channels, fts_dsp_get_tick_size(), snd_pcm_format_physical_width(format)/8);
    }

  if ( playback_channels != 0)
    {
      if ( (err = alsastream_open( &this->playback, pcm_name, SND_PCM_STREAM_PLAYBACK, format, playback_channels, sampling_rate, fifo_size, access)) < 0)
	{
	  fts_object_set_error(o, "Error opening ALSA device (%s)", snd_strerror( err));

	  post("alsaaudioport: cannot open ALSA device %s (%s)\n", pcm_name, snd_strerror( err));

	  return;
	}

      fts_audioport_set_output_channels( (fts_audioport_t *)this, this->playback.channels);
      fts_audioport_set_output_function( (fts_audioport_t *)this, functions_table[access_index][format_is_32][1]);

      this->output_buffer = alsaaudioport_allocate_buffer( access, this->playback.channels, fts_dsp_get_tick_size(), snd_pcm_format_physical_width(format)/8);
  }

  fts_audioport_set_xrun_function( (fts_audioport_t *)this, alsaaudioport_xrun_function);

#ifdef DEBUG
  if (this->capture.handle)
    {
      snd_pcm_dump( this->capture.handle, get_post_log());
      post_log();
    }
  if (this->playback.handle)
    {
      snd_pcm_dump( this->playback.handle, get_post_log());
      post_log();
    }
#endif
}

static void alsaaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsaaudioport_t *this = (alsaaudioport_t *)o;

  fts_audioport_delete( &this->head);

  if (this->input_buffer)
    fts_free( this->input_buffer);
  if (this->output_buffer)
    fts_free( this->output_buffer);
}

static void alsaaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static fts_status_t alsaaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( alsaaudioport_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, alsaaudioport_init);
  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_delete, alsaaudioport_delete);

  fts_class_add_daemon( cl, obj_property_get, fts_s_state, alsaaudioport_get_state);

  return fts_Success;
}

/***********************************************************************
 *
 * Config
 *
 */
void alsaaudioport_config( void)
{
  fts_symbol_t s = fts_new_symbol("alsaaudioport");

  fts_class_install( s, alsaaudioport_instantiate);

  fts_audioport_set_default_class( s);

  s_default = fts_new_symbol( "default");
  s_s32_le = fts_new_symbol( "S32_LE");
  s_s16_le = fts_new_symbol( "S16_LE");
  s_mmap_noninterleaved = fts_new_symbol( "mmap_noninterleaved");
  s_mmap_interleaved = fts_new_symbol( "mmap_interleaved");
  s_rw_noninterleaved = fts_new_symbol( "rw_noninterleaved");
  s_rw_interleaved = fts_new_symbol( "rw_interleaved");
}
