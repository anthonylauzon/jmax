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

#include <stdio.h>
#include <Carbon/Carbon.h>
#include <CoreAudio/AudioHardware.h>
#include <unistd.h>
#include <string.h>

#include <fts/fts.h>

typedef struct {
  fts_audioport_t head;
  AudioDeviceID device;
  int samples_per_tick;
  float *input_buffer;
  float *output_buffer;
  int xrun;
  int halted;
} halaudioport_t;


static void halaudioport_input( fts_word_t *argv)
{
  halaudioport_t *this;
  int n, channels, ch, i, j;

  this = (halaudioport_t *)fts_word_get_pointer( argv+0);

  if ( !this->output_buffer)
    return;
    
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_input_channels( (fts_audioport_t *)this);

  for ( ch = 0; ch < channels; ch++)
    {
      float *out = (float *) fts_word_get_pointer( argv + 2 + ch);
    
      j = ch;
      for ( i = 0; i < n; i++)
	{
	  out[i] = this->input_buffer[j];
	  j += channels;
	}
    }
}

static void halaudioport_output( fts_word_t *argv)
{
  halaudioport_t *this;
  int n, channels, ch, i, j;

  this = (halaudioport_t *)fts_word_get_pointer( argv+0);

  if ( !this->output_buffer)
    return;
    
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_output_channels( (fts_audioport_t *)this);

  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_pointer( argv + 2 + ch);
    
      j = ch;
      for ( i = 0; i < n; i++)
	{
	  this->output_buffer[j] = in[i];
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
  int n, samples_per_buffer;

  samples_per_buffer = outOutputData->mBuffers[0].mDataByteSize / sizeof( float);

  for ( n = 0; n < samples_per_buffer; n += this->samples_per_tick)
    {
      this->input_buffer = (float *)inInputData->mBuffers[0].mData + n;
      this->output_buffer = (float *)outOutputData->mBuffers[0].mData + n;
      fts_sched_run_one_tick();
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

  fts_sched_remove( o);

  if (this->halted)
    {
      fts_log( "Halted twice !!!\n");
      return;
    }

  this->halted = 1;

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
  UInt32 buffer_size;
  AudioStreamBasicDescription format;
  int channels;

  fts_audioport_init( &this->head);

  /* Get the default device */
  count = sizeof( this->device);
  err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &count, (void *)&this->device);
  if (err != noErr)
    {
      fts_object_set_error( o, "cannot get default device");
      return;
    }

  /* Get the buffer size (in bytes) */
  count = sizeof( buffer_size);
  err = AudioDeviceGetProperty( this->device, 0, false, kAudioDevicePropertyBufferSize, &count, &buffer_size);
  if (err != noErr)
    {
      fts_object_set_error( o, "cannot get device buffer size");
      return;
    }

  /* Get the device data format */
  count = sizeof( format);
  err = AudioDeviceGetProperty( this->device, 0, false, kAudioDevicePropertyStreamFormat, &count, &format);
  if (err != noErr)
    {
      fts_object_set_error( o, "cannot get device data format");
      return;
    }

  channels = format.mChannelsPerFrame;
  fts_audioport_set_input_channels( (fts_audioport_t *)this, channels);
  fts_audioport_set_input_function( (fts_audioport_t *)this, halaudioport_input);
  fts_audioport_set_output_channels( (fts_audioport_t *)this, channels);
  fts_audioport_set_output_function( (fts_audioport_t *)this, halaudioport_output);

  this->samples_per_tick = fts_dsp_get_tick_size() * channels;

#if 1
  fts_log( "Buffer size %d\n", buffer_size);
  fts_log( "sampleRate %g\n", format.mSampleRate);
  fts_log( "mFormatFlags %08X\n", format.mFormatFlags);
  fts_log( "mBytesPerPacket %d\n", format.mBytesPerPacket);
  fts_log( "mFramesPerPacket %d\n", format.mFramesPerPacket);
  fts_log( "mChannelsPerFrame %d\n", format.mChannelsPerFrame);
  fts_log( "mBytesPerFrame %d\n", format.mBytesPerFrame);
  fts_log( "mBitsPerChannel %d\n", format.mBitsPerChannel);
  fts_log( "samples_per_tick %d\n", this->samples_per_tick);
#endif

  fts_sched_add( o, FTS_SCHED_ALWAYS);
}

static void halaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;
  OSStatus err;

  fts_audioport_delete( &this->head);

  err = AudioDeviceStop( this->device, halaudioport_ioproc);
  if (err != noErr)
    /* ??? */;

  err = AudioDeviceRemoveIOProc( this->device, halaudioport_ioproc);
  if (err != noErr)
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




