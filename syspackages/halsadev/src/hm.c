/*
    Copyright (C) 2000 François Déchelle (dechelle@ircam.fr)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
  Based on ardour (http://ardour.sourceforge.net/) by Paul Barton-Davis 
*/

#include <stdlib.h>
#include <sys/asoundlib.h>
#include <linux/asoundid.h>
#include "hm.h"

static int switch_configure( snd_ctl_t *ctl_handle, int sample_rate, int fragment_samples)
{
  int err;
  snd_switch_t sw;

  /* SAMPLE RATE */

  strcpy ((char *) sw.name, "sample rate");

  if ((err = snd_ctl_switch_read(ctl_handle, &sw)) < 0)
    return err;

  sw.value.data32[0] = sample_rate;

  if ((err = snd_ctl_switch_write (ctl_handle, &sw)) < 0)
    return err;

  /* LATENCY */

  strcpy ((char *) sw.name, "latency");

  if ((err = snd_ctl_switch_read (ctl_handle, &sw)) < 0)
    return err;

  sw.value.data16[0] = fragment_samples;

  if ((err = snd_ctl_switch_write (ctl_handle, &sw)) < 0)
    return err;

  return 0;
}

static int hm_channel_open( hm_channel_t *channel, int card, int dev, int subdev, int which, int sample_rate, int frag_size)
{
  int err;
  int mode;
  snd_pcm_channel_params_t params;

  mode = ( which == SND_PCM_CHANNEL_PLAYBACK) ? SND_PCM_OPEN_PLAYBACK : SND_PCM_OPEN_CAPTURE;

  if ((err = snd_pcm_open_subdevice( &channel->handle, card, dev, subdev, mode)) < 0)
    return err;

  memset( &params, 0, sizeof(params));

  params.format.format = SND_PCM_SFMT_S32_LE; /* XXX HAMMERFALL DEPENDENT */
  params.format.rate = sample_rate;
  params.format.voices = 1; /* XXX HAMMERFALL DEPENDENT */
  params.format.interleave = 0; /* XXX HAMMERFALL DEPENDENT */

  params.mode = SND_PCM_MODE_BLOCK;
  params.channel = which;
  params.start_mode = SND_PCM_START_DATA;
  params.stop_mode = SND_PCM_STOP_ROLLOVER;
  params.buf.block.frag_size = frag_size;
  params.buf.block.frags_max = 2;  /* XXX HAMMERFALL DEPENDENT */
  params.buf.block.frags_min = 1;

  snd_pcm_channel_flush ( channel->handle, which);

  if ((err = snd_pcm_channel_params( channel->handle, &params)) < 0)
    return err;

  if ((err = snd_pcm_mmap( channel->handle, 
			   which, 
			   &channel->mmap_control, 
			   (void **) &channel->mmap_buffer)) < 0)
    return err;

  if ((err = snd_pcm_channel_prepare( channel->handle, which)) < 0)
    return err;

  return 0;
}

static int hm_channel_close( hm_channel_t *channel, int which)
{
  int err;

  if ((err = snd_pcm_munmap( channel->handle, which)) < 0)
    return err;

  if ((err = snd_pcm_close( channel->handle)) < 0)
    return err;

  return 0;
}

int hm_open( hm_t *h, int card, int dev, int sample_rate, int fragment_samples)
{
  int err;
  int subdev, chn;
  int frag_size;
  snd_ctl_t *ctl_handle;
  struct snd_ctl_hw_info ctl_info;
  snd_pcm_info_t pcm_info;

  if ((err = snd_ctl_open(&ctl_handle, card)) < 0)
    return err;

  if ((err = snd_ctl_hw_info( ctl_handle, &ctl_info)) < 0)
    return err;

  if (ctl_info.type != SND_CARD_TYPE_HAMMERFALL
      && ctl_info.type != SND_CARD_TYPE_HAMMERFALL_LIGHT)
    return err;

  if ((err = snd_ctl_pcm_info( ctl_handle, dev, &pcm_info)) < 0)
    return err;

  h->n_channels = pcm_info.capture + 1;
  frag_size = 4 * fragment_samples;
  h->frag_size = frag_size;

  if ( (err = switch_configure( ctl_handle, sample_rate, fragment_samples)) < 0)
    return err;

  if ((err = snd_ctl_close( ctl_handle)) < 0)
    return err;

  h->capture_channels = (hm_channel_t *)malloc( h->n_channels * sizeof( hm_channel_t));
  h->playback_channels = (hm_channel_t *)malloc( h->n_channels * sizeof( hm_channel_t));

  for( subdev = 0; subdev < h->n_channels; subdev++)
    {
      if ( (err = hm_channel_open( &h->capture_channels[subdev], 
				   card, 
				   dev, 
				   subdev, 
				   SND_PCM_CHANNEL_CAPTURE, 
				   sample_rate, 
				   frag_size ) ) < 0)
	return err;

      if ( (err = hm_channel_open( &h->playback_channels[subdev], 
				   card, 
				   dev, 
				   subdev, 
				   SND_PCM_CHANNEL_PLAYBACK, 
				   sample_rate, 
				   frag_size) ) < 0)
	return err;
    }

#define WE_DONT_NEED_DATA   /* ;-)) */

  for (chn = 0; chn < h->n_channels; chn++)
    {
      /* XXX need to use sync-groups here */

      if ((err = snd_pcm_capture_go( h->capture_channels[chn].handle)) < 0)
	return err;


#ifdef WE_NEED_DATA 
      if ((err = snd_pcm_playback_go( h->playback_channels[chn].handle)) < 0)
	return err;
#endif // WE_NEED_DATA 
    }

  h->fd = snd_pcm_file_descriptor( h->capture_channels[0].handle, SND_PCM_CHANNEL_CAPTURE);

  h->capture_mc0 = h->capture_channels[0].mmap_control;
  h->playback_mc0 = h->playback_channels[0].mmap_control;

  h->capture_mc0->status.expblock = h->capture_mc0->status.block;
  h->playback_mc0->status.expblock = h->playback_mc0->status.block;

  h->underrun_count = 0;

  return 0;
}

int hm_close( hm_t *h)
{
  int i, err;

  for ( i = 0; i < h->n_channels; i++)
    {
      if ((err = hm_channel_close( &h->capture_channels[i], SND_PCM_CHANNEL_CAPTURE)) < 0)
	return err;

      if ((err = hm_channel_close( &h->playback_channels[i], SND_PCM_CHANNEL_PLAYBACK)) < 0)
	return err;
    }

  return 0;
}

int hm_select( hm_t *h)
{
  int err;
  fd_set rfds;
  struct timeval timeout;

  FD_ZERO( &rfds);
  FD_SET( h->fd, &rfds);

  timeout.tv_sec = 0;
  timeout.tv_usec = 1000000;

  if ((err = select ( h->fd+1, &rfds, NULL, NULL, &timeout)) <= 0) 
    return err;

  if (h->capture_mc0->status.block != h->capture_mc0->status.expblock)
    {
      h->underrun_count++;

      h->capture_mc0->status.expblock = h->capture_mc0->status.block + 1;
      h->capture_mc0->status.expblock = h->capture_mc0->status.block + 1;

      return 0;
    }

  /* Ensure that expected blocks have caught up with
     current block before we go into select. We only
     need to do this for the channel we will call
     select(2) on.
  */

  h->capture_mc0->status.expblock++;
  h->playback_mc0->status.expblock++;

  h->capture_frag = h->capture_mc0->status.frag_io;
  /* Please, please, please don't get preempted between
     the last line and this one, and if we are, please
     let it be for less than one fragment's temporal
     duration ...
  */
#ifdef SHOULD_BE_LIKE_THAT
  h->playback_frag = h->playback_mc0->status.frag_io;
#else
  h->playback_frag = 1 - h->capture_frag;
#endif

  return err;
}

long *hm_get_input_buffer( hm_t *h, int channel)
{
  snd_pcm_mmap_control_t *mc = h->capture_channels[ channel ].mmap_control;

  return (long *)(h->capture_channels[ channel ].mmap_buffer + mc->fragments[ h->capture_frag ].addr);
}

long *hm_get_output_buffer( hm_t *h, int channel)
{
  snd_pcm_mmap_control_t *mc = h->playback_channels[ channel ].mmap_control;

  return (long *)(h->playback_channels[ channel ].mmap_buffer + mc->fragments[ h->playback_frag ].addr);
}

void hm_done_capture_fragment( hm_t *h, int channel)
{
  snd_pcm_mmap_control_t *mc = h->capture_channels[ channel ].mmap_control;

  mc->fragments[ mc->status.frag_io].data = 0;
}

void hm_done_playback_fragment( hm_t *h, int channel)
{
  snd_pcm_mmap_control_t *mc = h->playback_channels[ channel ].mmap_control;

  mc->fragments[ mc->status.frag_io].data = 0 /*was 1*/;
}

int hm_get_fragment_samples( hm_t *h)
{
  return h->frag_size;
}

int hm_get_n_channels( hm_t *h)
{
  return h->n_channels;
}

int hm_get_underrun_count( hm_t *h)
{
  return h->underrun_count;
}
