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
 * This file include the jMax HAL audio port for Mac OS X
 */

#include <stdio.h>
#include <Carbon/Carbon.h>
#include <CoreAudio/AudioHardware.h>
#include <unistd.h>
#include <string.h>

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 256
#define DEFAULT_CHANNELS 2

typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t awaiting_cond;
  volatile int awaiting_size;

  volatile int read_index;
  volatile int write_index;

  int buffer_size;
  volatile float buffer[1];
} halaudioport_fifo_t;

#define FIFO_READ_LEVEL(FIFO)						\
(((FIFO)->write_index - (FIFO)->read_index < 0)				\
 ? (FIFO)->write_index - (FIFO)->read_index + (FIFO)->buffer_size	\
 : (FIFO)->write_index - (FIFO)->read_index)

#define FIFO_WRITE_LEVEL(FIFO)						\
(((FIFO)->read_index - (FIFO)->write_index - 1 < 0)			\
 ? (FIFO)->read_index - (FIFO)->write_index - 1 + (FIFO)->buffer_size	\
 : (FIFO)->read_index - (FIFO)->write_index - 1)

#define FIFO_INCR_READ_INDEX(FIFO, INCR) 					\
(FIFO)->read_index = ((FIFO)->read_index + (INCR)) % (FIFO)->buffer_size

#define FIFO_INCR_READ_INDEX(FIFO, INCR) 					\
(FIFO)->write_index = ((FIFO)->write_index + (INCR)) % (FIFO)->buffer_size




typedef struct {
  fts_audioport_t head;
  AudioDeviceID device;
  float *buffer;
  int count;
  int buffer_size;
} halaudioport_t;




static void halaudioport_input( fts_word_t *argv)
{
}

static void halaudioport_output( fts_word_t *argv)
{
  halaudioport_t *this;
  int n, channels, ch, i, j;

  this = (halaudioport_t *)fts_word_get_ptr( argv+0);
  n = fts_word_get_long(argv + 1);
  channels = fts_audioport_get_output_channels( (fts_audioport_t *)this);

  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_ptr( argv + 2 + ch);

      j = this->count + ch;
      for ( i = 0; i < n; i++)
	{
	  this->buffer[j] = in[i];
	  j += channels;
	}
    }

  this->count += n * channels;

/*    post( "*** this->count %d\n", this->count); */

  if (this->count >= this->buffer_size)
    {
/*        post( "*** Wrap\n"); */
      this->count = 0;
    }
}

OSStatus halaudioport_ioproc( AudioDeviceID inDevice, 
			      const AudioTimeStamp *inNow, 
			      const AudioBufferList *inInputData, 
			      const AudioTimeStamp *inInputTime, 
			      AudioBufferList *outOutputData,
			      const AudioTimeStamp *inOutputTime,
			      void *inClientData)
{
  halaudioport_t *this = (halaudioport_t *)inClientData;
  int n;

/*    post( "*** Running scheduler for %d bytes\n", outOutputData->mBuffers[0].mDataByteSize); */

  for ( n = 0; n < outOutputData->mBuffers[0].mDataByteSize; n += fts_get_tick_size()*2*sizeof( float))
    {
/*        post( "*** Running scheduler for 1 tick (%d bytes)\n", fts_get_tick_size()*2*sizeof( float)); */
      fts_sched_run_one_tick();
    }

  memcpy( outOutputData->mBuffers[0].mData, this->buffer, outOutputData->mBuffers[0].mDataByteSize);
/*    memset( outOutputData->mBuffers[0].mData, 0, outOutputData->mBuffers[0].mDataByteSize); */

  return noErr;
}

static int halaudioport_xrun( fts_audioport_t *port)
{
  halaudioport_t *halport = (halaudioport_t *)port;

  return 0;
}

/*
 * Current implementation restrictions:
 * - only interleaved
 * - only output
 * - don't know how to get (and set?) the number of channels
 */

static void halaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;
  int fifo_size, channels, buffer_size;
  OSStatus err;
  UInt32 count;
  UInt32 bufferSizeProperty;

  fts_audioport_init( &this->head);

  ac--;
  at++;
  fifo_size = fts_param_get_int( fts_s_fifo_size, DEFAULT_FIFO_SIZE);

  channels = fts_get_int_arg( ac, at, 1, DEFAULT_CHANNELS);
  buffer_size = fts_get_int_arg( ac, at, 2, fts_get_tick_size());

  count = sizeof( this->device);
  if ((err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &count, (void *)&this->device)) != noErr)
    {
      fts_object_set_error( o, "Cannot get default device");
      return;
    }

  bufferSizeProperty = channels * sizeof( float) * buffer_size;
  count = sizeof( bufferSizeProperty);
  if ((err = AudioDeviceSetProperty( this->device, NULL, 0, false, kAudioDevicePropertyBufferSize, count, &bufferSizeProperty)) != noErr)
    {
      fts_object_set_error( o, "Cannot set buffer size");
      return;
    }

  fts_audioport_set_output_channels( (fts_audioport_t *)this, channels);
  fts_audioport_set_output_function( (fts_audioport_t *)this, halaudioport_output);

  /* Create the fifo and fill it */
  /* ... */


  if ((err = AudioDeviceAddIOProc( this->device, halaudioport_ioproc, this)) != noErr)
    {
      fts_object_set_error( o, "Cannot set IO proc");
      return;
    }
  
  if ((err = AudioDeviceStart( this->device, halaudioport_ioproc)) != noErr)
    {
      fts_object_set_error( o, "Cannot start device");
      return;
    }
}

static void halaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;
  OSStatus err;

  fts_audioport_delete( &this->head);

  if ((err = AudioDeviceStop( this->device, halaudioport_ioproc)) != noErr)
    ;

  if ((err = AudioDeviceRemoveIOProc( this->device, halaudioport_ioproc)) != noErr)
    ;
}

static void halaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static fts_status_t halaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( halaudioport_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, halaudioport_init);
  fts_method_define( cl, fts_SystemInlet, fts_s_delete, halaudioport_delete, 0, 0);

  /* define variable */
  fts_class_add_daemon( cl, obj_property_get, fts_s_state, halaudioport_get_state);

  return fts_Success;
}

void halaudioport_config( void)
{
  fts_class_install( fts_new_symbol("halaudioport"), halaudioport_instantiate);
}
