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

/* 
 * This file include the jMax ALSA audio port.
 * Based on Ardour's alsa_device.cc by Paul Barton-Davis
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/asoundlib.h>
#include <linux/asound.h>

#include <fts/fts.h>

static fts_symbol_t s_plug_0_0;
static fts_symbol_t s_s16_le, s_s32_le;
static fts_symbol_t s_mmap_ardour, s_mmap_noninterleaved, s_mmap_interleaved, s_rw_noninterleaved, s_rw_interleaved;

#define GUESS_CHANNELS -1
#define DEFAULT_PCM_NAME s_plug_0_0
#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 256
#define DEFAULT_CHANNELS 2

enum transfer_mode_t { MMAP_ARDOUR, MMAP_NONINTERLEAVED, MMAP_INTERLEAVED, RW_NONINTERLEAVED, RW_INTERLEAVED};

/* ---------------------------------------------------------------------- */
/* Structure used for both capture and playback                           */
/* ---------------------------------------------------------------------- */
typedef struct _alsastream_t {
  snd_pcm_t *handle;
  int period_size;
  int periods;
  size_t bytes_per_sample, bytes_per_frame;
  int channels;
  int current_period;
  int count;
  char ***addr;
  int fd;
  int xrun;
  struct _alsastream_t *link; /* link to playback stream for capture stream */
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
} alsaaudioport_t;

/* ********************************************************************** */
/* alsastream_t functions                                                 */
/* ********************************************************************** */

#define ERROR(FUN,ERR) do {post( "Error: %s() failed: %s\n", FUN, snd_strerror( ERR )); return ERR;} while (0)

/* ---------------------------------------------------------------------- */
/* alsatream_open: opens a stream                                         */
/* ---------------------------------------------------------------------- */

static int alsastream_open( alsastream_t *st, char *pcm_name, int which_stream, int format, int channels, int sampling_rate, int fifo_size, enum transfer_mode_t transfer_mode)
{
  int COUNT = 0;
#define HERE fprintf( stderr, "-> %d\n", COUNT++);

  snd_pcm_access_mask_t *mask;
  snd_output_t *log;
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  char *outbuf;
  int err, periods;
  int open_mode = 0;
  int dir;

  err = snd_output_stdio_attach( &log, stderr, 0);
  snd_pcm_hw_params_alloca( &hwparams);
  snd_pcm_sw_params_alloca( &swparams);

  open_mode |= SND_PCM_NONBLOCK;
  /*
   * Open the PCM device
   */
  if ( (err = snd_pcm_open( &st->handle, (char *)pcm_name, which_stream, open_mode)) < 0)
    ERROR( "snd_pcm_open", err);

  if ((err = snd_pcm_hw_params_any( st->handle, hwparams)) < 0)
    ERROR( "snd_pcm_hw_params_any", err);

  /*
   * Set the access mode:
   */
  switch ( transfer_mode) {
  case MMAP_ARDOUR:
    if ((err = snd_pcm_hw_params_set_access( st->handle, hwparams, SND_PCM_ACCESS_MMAP_NONINTERLEAVED)) < 0)
      ERROR( "snd_pcm_hw_params_set_access", err);
    break;
  case MMAP_NONINTERLEAVED:
    if ((err = snd_pcm_hw_params_set_access( st->handle, hwparams, SND_PCM_ACCESS_MMAP_NONINTERLEAVED)) < 0)
      ERROR( "snd_pcm_hw_params_set_access", err);
    break;
  case MMAP_INTERLEAVED:
    if ((err = snd_pcm_hw_params_set_access( st->handle, hwparams, SND_PCM_ACCESS_MMAP_INTERLEAVED)) < 0)
      ERROR( "snd_pcm_hw_params_set_access", err);
    break;
  case RW_NONINTERLEAVED:
    if ((err = snd_pcm_hw_params_set_access( st->handle, hwparams, SND_PCM_ACCESS_RW_NONINTERLEAVED)) < 0)
      ERROR( "snd_pcm_hw_params_set_access", err);
    break;
  case RW_INTERLEAVED:
    if ((err = snd_pcm_hw_params_set_access( st->handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
      ERROR( "snd_pcm_hw_params_set_access", err);
    break;
  }

  /*
   * Set the sample format
   */
  if ((err = snd_pcm_hw_params_set_format( st->handle, hwparams, format)) < 0)
    ERROR( "snd_pcm_hw_params_set_format", err);

  /*
   * Set the number of channels
   */
#if 0
  if (st->channels == GUESS_CHANNELS)
    st->channels = snd_pcm_hw_params_get_channels_max (hwparams);
#endif
  if ((err = snd_pcm_hw_params_set_channels( st->handle, hwparams, channels)) < 0) 
    ERROR( "snd_pcm_hw_params_set_channels", err);

  st->channels = snd_pcm_hw_params_get_channels( hwparams);

  /*
   * Set the sampling rate
   */
  if ((err = snd_pcm_hw_params_set_rate_near( st->handle, hwparams, sampling_rate, 0)) < 0)
    ERROR( "snd_pcm_hw_params_set_rate_near", err);

  /*
   * Set periods: we ask for 2 periods
   */
  periods = 2;
  if ((err = snd_pcm_hw_params_set_periods( st->handle, hwparams, periods, 0) < 0))
    ERROR( "snd_pcm_hw_params_set_periods", err);

  st->periods = snd_pcm_hw_params_get_periods( hwparams, 0);

  /*
   * Set period size: the period size is the fifo size divided by the number of periods
   */
  if ((err = snd_pcm_hw_params_set_period_size( st->handle, hwparams, fifo_size / periods, 0)) < 0) 
    ERROR( "snd_pcm_hw_params_set_period_size", err);

  st->period_size = snd_pcm_hw_params_get_period_size( hwparams, &dir);

  /*
   * Set buffer size
   */
  if ((err = snd_pcm_hw_params_set_buffer_size( st->handle, hwparams, fifo_size)) < 0)
    ERROR( "snd_pcm_hw_params_buffer_size", err);

  /*
   * Set hardware parameters
   */
  if ((err = snd_pcm_hw_params( st->handle, hwparams)) < 0)
    {
      snd_pcm_hw_params_dump( hwparams, log);
      ERROR( "snd_pcm_hw_params", err);
    }

  snd_pcm_sw_params_current( st->handle, swparams);

  if ((err = snd_pcm_sw_params_set_period_step( st->handle, swparams, 1)) < 0)
    ERROR( "snd_pcm_sw_params_set_period_step", err);

  if ( transfer_mode == MMAP_ARDOUR)
    {
      if ((err = snd_pcm_sw_params_set_xrun_mode( st->handle, swparams, SND_PCM_XRUN_NONE)) < 0) 
	ERROR( "snd_pcm_sw_params_set_xrun_mode", err);

      if ((err = snd_pcm_sw_params_set_start_mode( st->handle, swparams, SND_PCM_START_EXPLICIT)) < 0)
	ERROR( "snd_pcm_sw_params_set_start_mode", err);
    }

  if ((err = snd_pcm_sw_params_set_avail_min( st->handle, swparams, fifo_size / periods)) < 0) 
    ERROR( "snd_pcm_sw_params_set_avail_min", err);

  if ((err = snd_pcm_sw_params( st->handle, swparams)) < 0) 
    {
      snd_pcm_sw_params_dump(swparams, log);
      ERROR( "snd_pcm_sw_params", err);
    }

  st->bytes_per_sample = (snd_pcm_format_physical_width(format) / 8);
  st->bytes_per_frame = st->bytes_per_sample * st->channels;

  st->link = 0;

  return 1;
}


/* ---------------------------------------------------------------------- */
/* alsatream_ functions that are used only in ardour mode                 */
/* ---------------------------------------------------------------------- */
static int alsastream_get_addr( alsastream_t *st)
{
  const snd_pcm_channel_area_t *areas;
  char ***addr;
  int ch, per;
  
  areas = (snd_pcm_channel_area_t *)malloc( sizeof( snd_pcm_channel_area_t) * st->channels);
	
  if ((areas = snd_pcm_mmap_running_areas( st->handle)) == 0)
    {
      post( "Error: snd_pcm_mmap_get_areas() failed\n");
      snd_pcm_close( st->handle );
      st->handle = 0;
      return -1;
    }

  addr = (char ***)malloc( st->channels * sizeof( char *));

  for (ch = 0; ch < st->channels; ch++)
    {
      addr[ch] = (char **)fts_malloc( st->periods * sizeof( char *));
      memset( addr[ch], 0, st->periods * sizeof( char *));

      for ( per = 0; per < st->periods; per++) 
	{
	  int channel_fragment_bytes = st->bytes_per_sample * st->period_size;

	  addr[ch][per] = (char *) areas[ch].addr 
	    + (areas[ch].first / 8)    /* offset to first sample in bits */
	    + per * channel_fragment_bytes;
	}
    }

  st->addr = addr;
  st->current_period = 0;
  st->count = 0;

  return 0;
}

static int alsastream_link( alsastream_t *capture, alsastream_t *playback)
{
  snd_pcm_link( capture->handle, playback->handle);

  capture->link = playback;

  return 0;
}

static int alsastream_start( alsastream_t *st)
{
  int err, ch, per;

  if ( (err = snd_pcm_prepare( st->handle)) < 0)
    return err;

  for (ch = 0; ch < st->channels; ch++)
    for ( per = 0; per < st->periods; per++) 
      memset( st->addr[ch][per], 0, st->bytes_per_sample * st->period_size);

  snd_pcm_mmap_forward( st->handle, st->periods * st->period_size);

  if ( (err = snd_pcm_start( st->handle)) < 0)
    return err;

  return 0;
}

static void alsastream_stop( alsastream_t *st)
{
  if (st->handle)
    snd_pcm_drop( st->handle);
}

static int alsastream_xrun_recovery_ardour( alsastream_t *capture)
{
  snd_pcm_sframes_t capture_delay;
  int err;

  if ((err = snd_pcm_delay( capture->handle, &capture_delay)))
    ERROR( "snd_pcm_delay", err);

  post( "xrun of %d frames\n", capture_delay);
	
  alsastream_stop( capture->link);
  alsastream_start( capture->link);

  return 0;
}

static int alsastream_poll( alsastream_t *st)
{
  struct pollfd pfd;
  int r, frames_avail, xrun;

  if (!st->fd)
    {
      snd_pcm_poll_descriptors( st->handle, &pfd, 1);
      st->fd = pfd.fd;
    }

  pfd.fd = st->fd;
  pfd.events = POLLIN | POLLERR;

  if ((r = poll ( &pfd, 1, 1000)) < 0)
    {
      if (errno == EINTR)
	{
	  /* this happens mostly when run */
	  /* under gdb, or when exiting due to a signal */
	}

      ERROR( "poll", r);
      return -1;
    }
  else if (r == 0)
    return 0;
  else if (pfd.revents & POLLERR)
    return -1;

  xrun = 0;

  if ((frames_avail = snd_pcm_avail_update( st->handle)) < 0)
    {
      if (frames_avail == -EPIPE)
	xrun = 1;
      else 
	{
	  ERROR( "snd_pcm_avail_update", frames_avail);
	  return -1;
	}
    }

  if (st->link)
    {
      if ((frames_avail = snd_pcm_avail_update( st->link->handle)) < 0)
	{
	  if (frames_avail == -EPIPE)
	    xrun = 1;
	  else 
	    {
	      ERROR( "snd_pcm_avail_update", frames_avail);
	      return -1;
	    }
	}
    }

  if (xrun)
    alsastream_xrun_recovery_ardour( st);

  return 1;
}

/* ********************************************************************** */
/* I/O functions                                                          */
/* ********************************************************************** */

/* ---------------------------------------------------------------------- */
/* I/O functions for ardour mmap mode                                     */
/* ---------------------------------------------------------------------- */
static void alsastream_forward( alsastream_t *st, int n)
{
  st->count += n;

  if ( st->count % st->period_size == 0)
    {
      st->current_period = (st->current_period + 1) % st->periods;
      snd_pcm_mmap_forward( st->handle, st->period_size);
      st->count = 0;
    }
}

static void alsa_input_32_mmap_ardour( fts_word_t *argv)
{
  alsaaudioport_t *port;
  int n, channels, ch, i;
  
  port = (alsaaudioport_t *)fts_word_get_ptr( argv+0);
  n = fts_word_get_long(argv + 1);
  channels = fts_audioport_get_input_channels( port);

  if (port->capture.handle && port->capture.count == 0)
    alsastream_poll( &port->capture);

  for ( ch = 0; ch < channels; ch++)
    {
      float *out = (float *) fts_word_get_ptr( argv + 2 + ch);
      long *mmap_buffer = (long *)port->capture.addr[ch][port->capture.current_period] + port->capture.count;

#define DIV ((float)((1<<23) - 1))
      for ( i = 0; i < n; i++)
	out[i] = (float)(mmap_buffer[i] >> 8) / DIV;
    }

  alsastream_forward( &port->capture, n);
}

static void alsa_output_32_mmap_ardour( fts_word_t *argv)
{
  alsaaudioport_t *port;
  int n, channels, ch, i;
  
  port = (alsaaudioport_t *)fts_word_get_ptr( argv+0);
  n = fts_word_get_long(argv + 1);
  channels = fts_audioport_get_output_channels( port);

  if (!port->capture.handle && port->playback.handle && port->playback.count == 0)
    alsastream_poll( &port->playback);

  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_ptr( argv + 2 + ch);
      long *mmap_buffer = (long *)port->playback.addr[ch][port->playback.current_period] + port->playback.count;

      for ( i = 0; i < n; i++)
	mmap_buffer[i] = (long) (((1<<23) - 1) * in[i]) << 8;
    }

  alsastream_forward( &port->playback, n);
}


/* ---------------------------------------------------------------------- */
/* I/O functions for mmap mode                                            */
/* ---------------------------------------------------------------------- */

static int xrun( snd_pcm_t *handle)
{
  snd_pcm_status_t *status;
  int err;
	
  snd_pcm_status_alloca(&status);
  if ( (err = snd_pcm_status( handle, status)) < 0)
    ERROR( "snd_pcm_status", err);

  if ( snd_pcm_status_get_state( status) == SND_PCM_STATE_XRUN)
    {
/*        snd_pcm_sframes_t delay; */

/*        if ((err = snd_pcm_delay( handle, &delay))) */
/*  	ERROR( "snd_pcm_delay", err); */

/*        post( "xrun of %d frames\n", delay); */

#if 0
      post( "xrun\n");
#endif
    }

  if (( err = snd_pcm_prepare( handle)) < 0)
    ERROR( "snd_pcm_prepare", err);

  return 0;
}

#define INPUT_FUN_INTERLEAVED(FUN_NAME,TYPE,SND_PCM_FUN,SHIFT,CONV)	\
static void FUN_NAME( fts_word_t *argv)					\
{									\
  alsaaudioport_t *port;						\
  int n, channels, ch, i, j;						\
  ssize_t r;								\
  size_t count;								\
  TYPE *buffer;								\
  u_char *data;								\
									\
  port = (alsaaudioport_t *)fts_word_get_ptr( argv+0);			\
  n = fts_word_get_long(argv + 1);					\
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
	xrun( port->capture.handle);					\
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
      float *out = (float *) fts_word_get_ptr( argv + 2 + ch);		\
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
  port = (alsaaudioport_t *)fts_word_get_ptr( argv+0);			\
  n = fts_word_get_long(argv + 1);					\
  channels = fts_audioport_get_input_channels( port);			\
  buffer = (TYPE *)port->output_buffer;					\
									\
  for ( ch = 0; ch < channels; ch++)					\
    {									\
      float *in = (float *) fts_word_get_ptr( argv + 2 + ch);		\
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
	snd_pcm_wait( port->capture.handle, 1000);			\
      else if ( r == -EPIPE )						\
	xrun( port->playback.handle);					\
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


#define INPUT_FUN_NONINTERLEAVED(FUN_NAME,TYPE,SND_PCM_FUN,SHIFT,CONV)	\
static void FUN_NAME( fts_word_t *argv)					\
{									\
  alsaaudioport_t *port;						\
  int n, channels, ch, i, r;						\
  TYPE *buffer;								\
  void **buffers;							\
									\
  port = (alsaaudioport_t *)fts_word_get_ptr( argv+0);			\
  n = fts_word_get_long(argv + 1);					\
  channels = fts_audioport_get_input_channels( port);			\
  buffers = (void **)port->input_buffer;				\
									\
  if ( (r = SND_PCM_FUN( port->capture.handle, buffers, n)) == -EPIPE)	\
    xrun( port->capture.handle);					\
  else if (r < 0)							\
    post( "%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r));	\
									\
  for ( ch = 0; ch < channels; ch++)					\
    {									\
      long *buffer = (long *)buffers[ch];				\
      float *out = (float *) fts_word_get_ptr( argv + 2 + ch);		\
									\
      for ( i = 0; i < n; i++)						\
        out[i] = (buffer[i] >> SHIFT) / CONV;				\
    }									\
}

#define OUTPUT_FUN_NONINTERLEAVED(FUN_NAME,TYPE,SND_PCM_FUN,SHIFT,CONV)	\
static void FUN_NAME( fts_word_t *argv)					\
{									\
  alsaaudioport_t *port;						\
  int n, channels, ch, i, r;						\
  TYPE *buffer;								\
  void **buffers;							\
									\
  port = (alsaaudioport_t *)fts_word_get_ptr( argv+0);			\
  n = fts_word_get_long(argv + 1);					\
  channels = fts_audioport_get_output_channels( port);			\
  buffers = (void **)port->output_buffer;				\
									\
  for ( ch = 0; ch < channels; ch++)					\
    {									\
      long *buffer = (long *)buffers[ch];				\
      float *in = (float *) fts_word_get_ptr( argv + 2 + ch);		\
									\
      for ( i = 0; i < n; i++)						\
	buffer[i] = ((TYPE) (CONV * in[i])) << SHIFT;			\
    }									\
									\
  if ( (r = SND_PCM_FUN( port->playback.handle, buffers, n)) == -EPIPE)	\
    xrun( port->playback.handle);					\
  else if (r < 0)							\
    post( "%s error: %d, %s\n", #SND_PCM_FUN, r, snd_strerror(r));	\
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
static ftl_wrapper_t functions_table[5][2][2] = {
  { { 0, 0}, { alsa_input_32_mmap_ardour, alsa_output_32_mmap_ardour} }, 
  { { alsa_input_16_mmap_n, alsa_output_16_mmap_n}, { alsa_input_32_mmap_n, alsa_output_32_mmap_n} }, 
  { { alsa_input_16_mmap_i, alsa_output_16_mmap_i}, { alsa_input_32_mmap_i, alsa_output_32_mmap_i} }, 
  { { alsa_input_16_rw_n, alsa_output_16_rw_n}, { alsa_input_32_rw_n, alsa_output_32_rw_n} }, 
  { { alsa_input_16_rw_i, alsa_output_16_rw_i}, { alsa_input_32_rw_i, alsa_output_32_rw_i} }
};


/* ********************************************************************** */
/* alsaaudioport methods                                                  */
/* ********************************************************************** */

static void *alsaaudioport_allocate_buffer( int transfer_mode, int channels, int tick_size, int sample_bytes)
{
  void *buffer;

  if ( transfer_mode == MMAP_NONINTERLEAVED || transfer_mode == RW_NONINTERLEAVED)
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

static void alsaaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int sampling_rate, fifo_size, format, format_is_32, capture_channels, playback_channels, transfer_mode;
  float sr;
  char pcm_name[256];
  const char *format_name;
  fts_symbol_t s_transfer_mode;
  alsaaudioport_t *this = (alsaaudioport_t *)o;
  snd_output_t *log;

  fts_audioport_init( &this->head);

  ac--;
  at++;
  sr = fts_param_get_float( fts_s_sampling_rate, DEFAULT_SAMPLING_RATE);
  sampling_rate = (int)sr ;
  fifo_size = fts_param_get_int(fts_s_fifo_size, DEFAULT_FIFO_SIZE);

  strcpy( pcm_name, fts_symbol_name( fts_get_symbol_arg( ac, at, 0, s_plug_0_0)) );

  capture_channels = fts_get_int_arg( ac, at, 1, DEFAULT_CHANNELS);
  playback_channels = fts_get_int_arg( ac, at, 2, DEFAULT_CHANNELS);

  format_name = fts_symbol_name( fts_get_symbol_arg( ac, at, 3, s_s16_le));
  format = snd_pcm_format_value( format_name);
  format_is_32 = (format == SND_PCM_FORMAT_S32_LE);

  s_transfer_mode = fts_get_symbol_arg( ac, at, 4, s_rw_interleaved);
  if (s_transfer_mode == s_mmap_ardour)
    transfer_mode = MMAP_ARDOUR;
  else if (s_transfer_mode == s_mmap_noninterleaved)
    transfer_mode = MMAP_NONINTERLEAVED;
  else if (s_transfer_mode == s_mmap_interleaved)
    transfer_mode = MMAP_INTERLEAVED;
  else if (s_transfer_mode == s_rw_noninterleaved)
    transfer_mode = RW_NONINTERLEAVED;
  else if (s_transfer_mode == s_rw_interleaved)
    transfer_mode = RW_INTERLEAVED;

  if ( capture_channels != 0)
    {
      if ( alsastream_open( &this->capture, pcm_name, SND_PCM_STREAM_CAPTURE, format, capture_channels, sampling_rate, fifo_size, transfer_mode) < 0)
	{
	  fts_object_set_error(o, "Error opening ALSA device");
	  return;
	}

      fts_audioport_set_input_channels( (fts_audioport_t *)this, this->capture.channels);
      fts_audioport_set_input_fun( (fts_audioport_t *)this, functions_table[transfer_mode][format_is_32][0]);

      this->input_buffer = alsaaudioport_allocate_buffer( transfer_mode, this->capture.channels, fts_get_tick_size(), snd_pcm_format_physical_width(format)/8);
    }

  if ( playback_channels != 0)
    {
      if ( alsastream_open( &this->playback, pcm_name, SND_PCM_STREAM_PLAYBACK, format, playback_channels, sampling_rate, fifo_size, transfer_mode) < 0)
	{
	  fts_object_set_error(o, "Error opening ALSA device");
	  return;
	}

      fts_audioport_set_output_channels( (fts_audioport_t *)this, this->playback.channels);
      fts_audioport_set_output_fun( (fts_audioport_t *)this, functions_table[transfer_mode][format_is_32][1]);

      this->output_buffer = alsaaudioport_allocate_buffer( transfer_mode, this->playback.channels, fts_get_tick_size(), snd_pcm_format_physical_width(format)/8);
  }

  if (transfer_mode == MMAP_ARDOUR)
    {
      if (this->capture.handle)
	alsastream_get_addr( &this->capture);
      if (this->playback.handle)
	alsastream_get_addr( &this->playback);

      alsastream_link( &this->capture, &this->playback);

      if ( alsastream_start( &this->playback) < 0)
	{
	  alsastream_stop( &this->playback);
	  fts_object_set_error(o, "Error opening ALSA device");
	  return;
	}
    }

  {
    snd_output_t *log;

    snd_output_stdio_attach( &log, stderr, 0);

    if (this->capture.handle)
      snd_pcm_dump( this->capture.handle, log);
    if (this->playback.handle)
      snd_pcm_dump( this->playback.handle, log);
  }
}

static void alsaaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsaaudioport_t *this = (alsaaudioport_t *)o;

  fts_audioport_delete( &this->head);

  alsastream_stop( &this->playback);
}

static void alsaaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static fts_status_t alsaaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( alsaaudioport_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, alsaaudioport_init);
  fts_method_define( cl, fts_SystemInlet, fts_s_delete, alsaaudioport_delete, 0, 0);
  /* define variable */
  fts_class_add_daemon( cl, obj_property_get, fts_s_state, alsaaudioport_get_state);

  return fts_Success;
}

/* ********************************************************************** */
/* init function                                                          */
/* ********************************************************************** */

void alsaaudioport_config( void)
{
  fts_class_install( fts_new_symbol("alsaaudioport"), alsaaudioport_instantiate);

  s_plug_0_0 = fts_new_symbol( "plug:0,0");
  s_s32_le = fts_new_symbol( "S32_LE");
  s_s16_le = fts_new_symbol( "S16_LE");

  s_mmap_ardour = fts_new_symbol( "mmap_ardour");
  s_mmap_noninterleaved = fts_new_symbol( "mmap_noninterleaved");
  s_mmap_interleaved = fts_new_symbol( "mmap_interleaved");
  s_rw_noninterleaved = fts_new_symbol( "rw_noninterleaved");
  s_rw_interleaved = fts_new_symbol( "rw_interleaved");
}
