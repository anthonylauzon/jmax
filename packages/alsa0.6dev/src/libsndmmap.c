#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <assert.h>
#include <sys/asoundlib.h>

#include "libsndmmap.h"
#if 0
#include "alsabash.h"
#endif

static void error( const char *fun, int err)
{
  fprintf( stderr, "Error: snd_pcm_%s() failed: %s\n", fun, snd_strerror( err ));
}

static char ***snd_get_addr( mmapdev_t *d, snd_pcm_t *handle, int format)
{
  snd_pcm_channel_area_t *areas, *stopped_areas;
  char ***addr;
  int err, channel, frag;
  
  areas = (snd_pcm_channel_area_t *)malloc( sizeof( snd_pcm_channel_area_t) * d->n_channels);
  stopped_areas = (snd_pcm_channel_area_t *)malloc( sizeof( snd_pcm_channel_area_t) * d->n_channels);
	
  if ((err = snd_pcm_mmap_get_areas( handle, stopped_areas, areas)) < 0)
    {
      error( "mmap_get_areas", err);
      snd_pcm_close( handle ); 
      return 0;
    }

  addr = (char ***)malloc( d->n_channels * sizeof( char *));

  for (channel = 0; channel < d->n_channels; channel++)
    {
      addr[channel] = (char **)malloc( d->n_fragments * sizeof( char *));

      memset( addr[channel], 0, d->n_fragments * sizeof( char *));

      for ( frag = 0; frag < d->n_fragments; frag++) 
	{
	  int channel_fragment_bytes = (snd_pcm_format_physical_width(format) / 8) * d->frag_size;

	  addr[channel][frag] = (char *) areas[channel].addr 
	    + (areas[channel].first / 8)    /* offset to first sample in bits */
	    + frag * channel_fragment_bytes;
	}
    }

  return addr;
}

static void silence( mmapdev_t *d)
{
  int channel;

  /* Clear the fragments */
  for (channel = 0; channel < d->n_channels; channel++)
    {
      int frag;

      for ( frag = 0; frag < d->n_fragments; frag++)
	memset( d->playback_addr[channel][frag], 0, d->frag_size * sizeof( long));
    }
}

static int snd_configure( snd_pcm_t *handle, int format, int sampling_rate, int n_channels, int frag_size)
{
  int err;
  snd_pcm_params_t params;

  memset( &params, 0, sizeof( params));

  params.format.sfmt = format;
  params.format.rate = sampling_rate;
  params.format.channels = n_channels;
  params.ready_mode = SND_PCM_READY_FRAGMENT;
  params.start_mode = SND_PCM_START_EXPLICIT;
  params.xfer_mode = SND_PCM_XFER_NONINTERLEAVED;
  params.mmap_shape = SND_PCM_MMAP_NONINTERLEAVED;
  params.xrun_mode = SND_PCM_XRUN_NONE;

  params.frag_size = frag_size;
  params.avail_min = params.frag_size;
  params.buffer_size = 3*params.frag_size; /* Ask for 3 fragments */

  if ((err = snd_pcm_params( handle, &params)) < 0)
    { 
      error( "params", err);
      snd_pcm_close( handle ); 

      return -1;
    } 

  if ((err = snd_pcm_mmap( handle)) < 0)
    {
      error( "mmap", err);
      snd_pcm_close( handle ); 

      return -1;
    }

  return 0;
}

int snd_open( mmapdev_t *d, char *pcm_name, int format, int sampling_rate, int frag_size, int init_frag)
{
  snd_pcm_params_info_t params_info;
  snd_pcm_setup_t setup;
  int n_channels, channel, err;

  if ((err = snd_pcm_open( &d->capture_handle, pcm_name, SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
      error( "snd_pcm_open", err);
      return -1;
    }

  if ((err = snd_pcm_subdevice( &d->playback_handle, pcm_name, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
      error( "snd_pcm_open", err);
      return -1;
    }

  memset( &params_info, 0, sizeof (params_info));
  if ((err = snd_pcm_params_info( d->capture_handle, &params_info)) < 0)
    {
      error( "params_info", err);
      return;
    }
  n_channels = params_info.max_channels;

  if ((err = snd_configure( d->capture_handle, format, sampling_rate, n_channels, frag_size)) < 0)
    {
      error( "snd_configure", err);
      return -1;
    }

  if ((err = snd_configure( d->playback_handle, format, sampling_rate, n_channels, frag_size)) < 0)
    {
      error( "snd_configure", err);
      return -1;
    }

  if ((err = snd_pcm_setup( d->capture_handle, &setup)) < 0)
    {
      error( "setup", err);
      return -1;
    } 

  if (setup.frag_size != frag_size)
    {
      fprintf( stderr, "Error: wanted frag_size %d got %d\n", frag_size, setup.frag_size);
      return -1;
    }

  d->frag_size = setup.frag_size;
  d->n_fragments = setup.frags;
  d->n_channels = n_channels;

#if 0
  d->fragment = 0; /* As noted by Paul, may be not the right initial value */
#else
  d->fragment = init_frag; /* As noted by Paul, may be not the right initial value */
#endif

  d->capture_addr = snd_get_addr( d, d->capture_handle, format);
  d->playback_addr = snd_get_addr( d, d->playback_handle, format);
  d->xrun = 0;

  snd_pcm_link( d->capture_handle, d->playback_handle);

  return 0;
}

int snd_start( mmapdev_t *d)
{
  int err;

  if ((err = snd_pcm_prepare ( d->playback_handle)) < 0)
    {
      error( "prepare", err);
      return -1;
    } 

  /* Only on playback */
  silence( d);
  snd_pcm_mmap_forward( d->playback_handle, d->n_fragments * d->frag_size);

  if ((err = snd_pcm_start( d->playback_handle)) < 0)
    {
      error( "start", err);
      return -1;
    }

  d->fd = snd_pcm_poll_descriptor( d->capture_handle);

  snd_pcm_dump( d->capture_handle, stderr);
  snd_pcm_dump( d->playback_handle, stderr);

  return 0;
}

int snd_close( mmapdev_t *d)
{
  int err;

  if ((err = snd_pcm_drop( d->playback_handle)) < 0)
    {
      error( "snd_pcm_drop", err);
      return -1;
    }

  snd_pcm_close( d->playback_handle);
  snd_pcm_close( d->capture_handle);

  return 0;
}

int snd_get_n_fragments( mmapdev_t *d)
{
  return d->n_fragments;
}

int snd_get_n_channels( mmapdev_t *d)
{
  return d->n_channels;
}

int snd_get_frag_size( mmapdev_t *d)
{
  return d->frag_size;
}

int snd_get_xrun( mmapdev_t *d)
{
  return d->xrun;
}

int snd_clear_xrun( mmapdev_t *d)
{
  d->xrun = 0;
}

/* I am very suspicious about these functions */

#if 0
static void xrun_recovery( mmapdev_t *d)
{
  size_t chn;
  size_t frames;

  silence( d);

  frames = *((snd_pcm_t *)d->capture_handle)->hw_ptr - *((snd_pcm_t *)d->capture_handle)->appl_ptr;
	
  fprintf( stderr, "xrun of %d frames\n", frames);

  *((snd_pcm_t *)d->capture_handle)->appl_ptr = *((snd_pcm_t *)d->capture_handle)->hw_ptr;

  *((snd_pcm_t *)d->playback_handle)->appl_ptr = 
    *((snd_pcm_t *)d->playback_handle)->hw_ptr +
    d->n_fragments * d->frag_size;
}
#endif

#ifdef _CODE_THAT_DOES_NOT_WORK_
static void xrun_recovery( mmapdev_t *d)
{
  int frames, playback_avail, capture_avail, buffer_size;

  buffer_size = d->n_fragments * d->frag_size;

  silence( d);

  frames = snd_pcm_mmap_hw_offset( d->capture_handle) - snd_pcm_mmap_offset( d->capture_handle); /* ??? */

  if (frames < 0)
    frames += buffer_size;

  snd_pcm_mmap_appl_forward( d->capture_handle, frames);

  /* 
     This does not work: it crashed with failed assertion:
     mythrough: pcm.c:695: snd_pcm_mmap_forward: Assertion `size <= snd_pcm_mmap_avail(pcm)' failed.
     This explains may be why Paul copied in alsa_device.cc the definition of the snd_pcm structure
     and garbles the pointers by hand ?
  */
#if 0
  playback_avail = snd_pcm_avail_update( d->playback_handle);
  snd_pcm_mmap_appl_forward( d->playback_handle, buffer_size);
#endif

  fprintf( stderr, "xrun of %d frames\n", frames);
}
#endif


int snd_do_poll( mmapdev_t *d)
{
  int r, channel, frames_avail;
  struct pollfd pfd;

  pfd.fd = d->fd;
  pfd.events = POLLIN | POLLERR;

  if ((r = poll ( &pfd, 1, 1000)) < 0)
    {
      if (errno == EINTR)
	{
	  /* this happens mostly when run */
	  /* under gdb, or when exiting due to a signal */
	}

      error( "poll", r);
      return -1;
    }
  else if (r == 0)
    return 0;
  else if (pfd.revents & POLLERR)
    return -1;

  if ((frames_avail = snd_pcm_avail_update( d->capture_handle)) < 0)
    {
      if (frames_avail == -EPIPE)
	{
	  d->xrun = 1;
#if 0
  	  xrun_recovery( d);
#endif
	} 
      else 
	{
	  error( "avail_update", frames_avail);
	  return -1;
	}
    }
}

void *snd_get_capture_fragment( mmapdev_t *d, int channel)
{
  return d->capture_addr[channel][d->fragment];
}

void *snd_get_playback_fragment( mmapdev_t *d, int channel)
{
  return d->playback_addr[channel][d->fragment];
}

void snd_done_fragment( mmapdev_t *d)
{
  d->fragment = (d->fragment + 1) % d->n_fragments;

  snd_pcm_mmap_forward( d->capture_handle, d->frag_size);
  snd_pcm_mmap_forward( d->playback_handle, d->frag_size);
}

