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

#define COUNTDOWN 512

typedef struct {
  fts_audioport_t head;
  AudioDeviceID device;
  float *buffer;
  int countdown;
} halaudioport_t;

/*  #define LOTS_OF_PRINTOUT */

static void halaudioport_input( fts_word_t *argv)
{
}

static void halaudioport_output( fts_word_t *argv)
{
  halaudioport_t *this;
  int n, channels, ch, i, j;

  this = (halaudioport_t *)fts_word_get_ptr( argv+0);
  if (!this->buffer)
    return;

  n = fts_word_get_long( argv + 1);
  channels = fts_audioport_get_output_channels( (fts_audioport_t *)this);

  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_ptr( argv + 2 + ch);

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

#ifdef LOTS_OF_PRINTOUT
  post( "*** Running scheduler for %d bytes\n", outOutputData->mBuffers[0].mDataByteSize);
#endif

  this->buffer = (float *)outOutputData->mBuffers[0].mData;
  samples_per_tick = outOutputData->mBuffers[0].mNumberChannels * fts_get_tick_size();
  samples_per_buffer = outOutputData->mBuffers[0].mDataByteSize / sizeof( float);

  for ( n = 0; n < samples_per_buffer; n += samples_per_tick)
    {
#ifdef LOTS_OF_PRINTOUT
      post( "*** Running scheduler for 1 tick (%d bytes)\n", fts_get_tick_size()*2*sizeof( float));
#endif
      fts_sched_run_one_tick_without_select();

      this->buffer += samples_per_tick;
    }

  return noErr;
}

static int halaudioport_xrun( fts_audioport_t *port)
{
  halaudioport_t *halport = (halaudioport_t *)port;

  return 0;
}

static void hal_halt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fd_set rfds;
  halaudioport_t *this = (halaudioport_t *)o;
  OSStatus err;

  this->countdown--;

  if (this->countdown > 0)
    return;

  fprintf( stderr, "hal_halt\n");

  err = AudioDeviceAddIOProc( this->device, halaudioport_ioproc, this);
  if (err != noErr)
    {
      fprintf( stderr, "Cannot set IO proc\n");

      fts_object_set_error( o, "Cannot set IO proc");
      return;
    }
  
  err = AudioDeviceStart( this->device, halaudioport_ioproc);
  if (err != noErr)
    {
      fprintf( stderr, "Cannot start device\n");

      fts_object_set_error( o, "Cannot start device");
      return;
    }

  /* halt scheduler main loop */
  FD_ZERO( &rfds);
  FD_SET( 0, &rfds);

  fprintf( stderr, "About to select()\n");

  if (select( 1, &rfds, NULL, NULL, NULL) < 0)
    fprintf( stderr, "select() failed\n");

  fprintf( stderr, "After select() ????????\n");
}

static void halaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;
  int fifo_size, channels;
  OSStatus err;
  UInt32 count;
  UInt32 bufferSizeProperty;

  fts_audioport_init( &this->head);

  ac--;
  at++;
  fifo_size = fts_param_get_int( fts_s_fifo_size, DEFAULT_FIFO_SIZE);

  channels = fts_get_int_arg( ac, at, 1, DEFAULT_CHANNELS);

  count = sizeof( this->device);
  err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &count, (void *)&this->device);
  if (err != noErr)
    {
      fts_object_set_error( o, "Cannot get default device");
      return;
    }

  count = sizeof( bufferSizeProperty);
  err = AudioDeviceGetProperty( this->device, 0, false, kAudioDevicePropertyBufferSize, &count, &bufferSizeProperty);
  if (err != noErr)
    {
      fts_object_set_error( o, "Cannot get buffer size");
      return;
    }

  fts_audioport_set_output_channels( (fts_audioport_t *)this, channels);

  fts_audioport_set_output_function( (fts_audioport_t *)this, halaudioport_output);

  post( "*** Buffer size %d\n", bufferSizeProperty);

  this->buffer = 0;
  this->countdown = COUNTDOWN;

  fts_sched_add( fts_sched_get_current(), hal_halt, o);
}

static void halaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;
  OSStatus err;

  fts_audioport_delete( &this->head);

  err = AudioDeviceStop( this->device, halaudioport_ioproc);
  if (err != noErr)
    ;

  err = AudioDeviceRemoveIOProc( this->device, halaudioport_ioproc);
  if (err != noErr)
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
