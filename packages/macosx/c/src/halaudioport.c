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
 * This file include the audio port for Mac OS X
 *
 * Current implementation restrictions:
 * - only interleaved
 * - don't know how to get (and set?) the number of channels
 */

#include <stdio.h>
#include <Carbon/Carbon.h>
#include <CoreAudio/AudioHardware.h>
#include <unistd.h>
#include <string.h>
/* #include <pthread.h> */
/* #include <mach/mach.h> */
/* #include <sys/sysctl.h> */

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 1024
#define DEFAULT_CHANNELS 2

typedef struct {
  fts_audioport_t head;
  AudioDeviceID device;
  float *buffer;
  int xrun;
} halaudioport_t;


static void halaudioport_input( fts_word_t *argv)
{
}

static void halaudioport_output( fts_word_t *argv)
{
  halaudioport_t *this;
  int n, channels, ch, i, j;

  this = (halaudioport_t *)fts_word_get_pointer( argv+0);

  if ( !this->buffer)
    return;
    
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_output_channels( (fts_audioport_t *)this);

  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_pointer( argv + 2 + ch);
    
      j = ch;
      for ( i = 0; i < n; i++)
	{
	  this->buffer[j] = in[i];
	  j += channels;
	}
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
  float *buffer;
  int n, samples_per_tick, samples_per_buffer;

  this->buffer = (float *)outOutputData->mBuffers[0].mData;
  samples_per_tick = outOutputData->mBuffers[0].mNumberChannels * fts_dsp_get_tick_size();
  samples_per_buffer = outOutputData->mBuffers[0].mDataByteSize / sizeof( float);

  for ( n = 0; n < samples_per_buffer; n += samples_per_tick)
    {
      fts_sched_run_one_tick();

      this->buffer += samples_per_tick;
    }

  return noErr;
}

static int halaudioport_xrun( fts_audioport_t *port)
{
  halaudioport_t *this = (halaudioport_t *)port;
  int xrun = this->xrun;

  this->xrun = 0;

  return xrun;
}

static void halaudioport_halt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fd_set rfds;
  halaudioport_t *this = (halaudioport_t *)o;
  OSStatus err;

  err = AudioDeviceAddIOProc( this->device, halaudioport_ioproc, this);
  if (err != noErr)
    {
      fprintf( stderr, "Cannot set IO proc\n");

      fts_object_set_error( o, "Cannot set IO proc");
      return;
    }
  
  if ((err = AudioDeviceStart( this->device, halaudioport_ioproc)) != noErr)
    {
      fts_object_set_error( o, "cannot start device");
      return;
    }

  /* halt scheduler main loop */
  FD_ZERO( &rfds);
  FD_SET( 0, &rfds);

  if (select( 1, &rfds, NULL, NULL, NULL) < 0)
    fprintf( stderr, "select() failed\n");

  fprintf( stderr, "After select() ????????\n");
}

static void halaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;
  OSStatus err;
  UInt32 count;
  UInt32 channels;
  Boolean writable;
  AudioBufferList *buffer_list;

  fts_audioport_init( &this->head);

  /* Get the default device */
  count = sizeof( this->device);
  if ((err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &count, (void *)&this->device)) != noErr)
    {
      fts_object_set_error( o, "cannot get default device");
      return;
    }

  /* Get the number of channels and the buffer size */
  if ((err = AudioDeviceGetPropertyInfo( this->device, 0, false, kAudioDevicePropertyStreamConfiguration, &count, &writable)) != noErr)
    {
      fts_object_set_error( o, "cannot get device configuration");
      post( "cannot get device configuration\n");
      return;
    }

  buffer_list = (AudioBufferList *)alloca( count);
  if ((err = AudioDeviceGetProperty( this->device, 0, false, kAudioDevicePropertyStreamConfiguration, &count, buffer_list)) != noErr)
    {
      fts_object_set_error( o, "cannot get device configuration");
      post( "cannot get device configuration\n");
      return;
    }

  /* We assume that there is only one buffer */
  if ( buffer_list->mNumberBuffers != 1)
    {
      fts_log( "buffer_list->mNumberBuffers != 1\n");
      fts_object_set_error( o, "buffer_list->mNumberBuffers != 1");
      return;
    }


#if 0
  /* For now, we don't set the buffer size and we reuse the default one. */
  {
    UInt32 bufferSizeProperty = 2 * sizeof( float) * fifo_size;
    count = sizeof( bufferSizeProperty);
  
    if ((err = AudioDeviceSetProperty( this->device, NULL, 0, false, kAudioDevicePropertyBufferSize, count, &bufferSizeProperty)) != noErr)
      {
	fts_object_set_error( o, "cannot set buffer size");
	return;
      }
  }
#endif

  channels = buffer_list->mBuffers[0].mNumberChannels;

  fts_audioport_set_output_channels( (fts_audioport_t *)this, channels);
  fts_audioport_set_output_function( (fts_audioport_t *)this, halaudioport_output);

  fts_sched_add(o, FTS_SCHED_ALWAYS);
}

static void halaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;
  OSStatus err;

  fts_audioport_delete( &this->head);

  if ((err = AudioDeviceStop( this->device, halaudioport_ioproc)) != noErr)
    /* ??? */;

  if ((err = AudioDeviceRemoveIOProc( this->device, halaudioport_ioproc)) != noErr)
    /* ??? */;
}

static void halaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( halaudioport_t), halaudioport_init, halaudioport_delete);

  fts_class_message_varargs(cl, fts_s_sched_ready, halaudioport_halt);
}

void halaudioport_config( void)
{
  fts_symbol_t halaudioport_symbol = fts_new_symbol("halaudioport");

  fts_class_install( halaudioport_symbol, halaudioport_instantiate);
  fts_audioport_set_default_class(halaudioport_symbol);
}




