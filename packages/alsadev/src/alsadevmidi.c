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
 * Authors: François Déchelle (dechelle@ircam.fr)
 *
 */

/* 
 * This file include the FTS ALSA raw midi device.
 */

#include <sys/asoundlib.h>
#include <linux/asound.h>

#include <fts/fts.h>

typedef struct {
  snd_rawmidi_t *handle;
  int fd;
} alsa_midi_dev_data_t;

static fts_status_t alsa_midi_open( fts_dev_t *dev, int ac, const fts_atom_t *at)
{
  int r, card, device, mode;
  alsa_midi_dev_data_t *dev_data;
  snd_rawmidi_params_t params;

  dev_data = (alsa_midi_dev_data_t *)fts_malloc( sizeof( alsa_midi_dev_data_t));

  card = fts_get_int_by_name( ac, at, fts_new_symbol( "card"), 0);
  device = fts_get_int_by_name( ac, at, fts_new_symbol( "device"), 0);

  mode = SND_RAWMIDI_OPEN_DUPLEX;
  if ( (r = snd_rawmidi_open( &dev_data->handle, card, device, mode)) < 0)
    {
      post( "Error: snd_rawmidi_open failed (%s)\n", snd_strerror( r));
      return &fts_dev_open_error;
    }

  if ( (r = snd_rawmidi_block_mode( dev_data->handle, 0)) < 0)
    {
      post( "Error: snd_rawmidi_block_mode failed (%s)\n", snd_strerror( r));
      return &fts_dev_open_error;
    }

  if ( (r = snd_rawmidi_file_descriptor( dev_data->handle)) < 0)
    {
      post( "Error: snd_rawmidi_file_descriptor failed (%s)\n", snd_strerror( r));
      return &fts_dev_open_error;
    }
  else
    dev_data->fd = r;

  memset( &params, 0, sizeof( params));

  params.channel = SND_RAWMIDI_CHANNEL_OUTPUT;
  params.size = 4096;
  params.max = 4096;
  params.room = 1;
  if ( (r = snd_rawmidi_channel_params( dev_data->handle, &params)) < 0)
    {
      post( "Error: snd_rawmidi_channel_params failed (%s)\n", snd_strerror( r));
      return &fts_dev_open_error;
    }

  params.channel = SND_RAWMIDI_CHANNEL_INPUT;
  params.size = 4096;
  params.min = 1;
  if ( (r = snd_rawmidi_channel_params( dev_data->handle, &params)) < 0)
    {
      post( "Error: snd_rawmidi_channel_params failed (%s)\n", snd_strerror( r));
      return &fts_dev_open_error;
    }

  return fts_Success;
}

static fts_status_t alsa_midi_close( fts_dev_t *dev)
{
  alsa_midi_dev_data_t *dev_data = (alsa_midi_dev_data_t *)fts_dev_get_device_data( dev);
  int r;

  if ( (r = snd_rawmidi_close( dev_data->handle)) < 0)
    {
      post( "Error: snd_rawmidi_close failed (%s)\n", snd_strerror( r));
      return &fts_dev_open_error;
    }
  
  return fts_Success;
}

fts_status_t alsa_midi_get( fts_dev_t *dev, unsigned char *cp)
{
  alsa_midi_dev_data_t *dev_data = (alsa_midi_dev_data_t *)fts_dev_get_device_data( dev);
  int r;

  /* reading 1 character at a time is *very* inefficient */
  if( ( r = snd_rawmidi_read( dev_data->handle, cp, 1)) < 0)
    return &fts_dev_io_error;

  if ( r == 0)
    return &fts_data_not_ready;

  return fts_Success;
}

static fts_status_t alsa_midi_put( fts_dev_t *dev, unsigned char c)
{
  alsa_midi_dev_data_t *dev_data = (alsa_midi_dev_data_t *)fts_dev_get_device_data( dev);
  int r;

  /* Writing 1 character at a time is *very* inefficient !!! */
  if ( ( r = snd_rawmidi_write( dev_data->handle, &c, 1)) < 0)
    return &fts_dev_io_error;

  return fts_Success;
}

static fts_status_t alsa_midi_flush( fts_dev_t *dev)
{
  return fts_Success;
}

/*
 * Init function
 */
void alsa_midi_init( void)
{
  fts_dev_class_t *alsa_midi_class;

  alsa_midi_class = fts_dev_class_new( fts_char_dev, fts_new_symbol( "alsa_midi"));

  fts_dev_class_set_open_fun( alsa_midi_class, alsa_midi_open);
  fts_dev_class_set_close_fun( alsa_midi_class, alsa_midi_close);
  fts_dev_class_char_set_get_fun( alsa_midi_class, alsa_midi_get);
  fts_dev_class_char_set_put_fun( alsa_midi_class, alsa_midi_put);
  fts_dev_class_char_set_flush_fun( alsa_midi_class, alsa_midi_flush);
}
