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

#undef PRINT_A_LOT

#include <Carbon/Carbon.h>
#include <CoreAudio/AudioHardware.h>
#include <unistd.h>
#include <string.h>

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 1024
#define DEFAULT_CHANNELS 2

typedef struct {
  fts_audioport_t head;
  AudioDeviceID device;
  fts_symbol_t name;
  int samples_per_tick;
  float *input_buffer;
  float *output_buffer;
  int xrun;
  int halted;
} macosxaudioport_t;

static fts_class_t *macosxaudioport_class;

static void 
fzero( float *buf, int n)
{
  int i;

  for ( i = 0; i < n; i++)
    buf[i] = 0.0f;
}

static void 
fcpy( float *dst, const float *src, int n)
{
  int i;

  for ( i = 0; i < n; i++)
    dst[i] = src[i];
}

/**
 * The audioport IO function calls the native audio layer to read/write a buffer
 * of samples in the native format.
 * Its argument is the audioport. Buffers are allocated by the port.
 */
static void macosxaudioport_input(fts_audioport_t* port)
{
/*   macosxaudioport_t* macosx_port = (macosxaudioport_t*)port; */
}


static void macosxaudioport_output(fts_audioport_t* port)
{
/*   macosxaudioport_t* macosx_port = (macosxaudioport_t*)port; */
}

/** 
 * The audioport copy function copies the samples in the native format to a float buffer
 * for a given channel.
 */
static void macosxaudioport_input_copy_fun( fts_audioport_t *port, float *buff, int buffsize, int channel)
{
  macosxaudioport_t* macosx_port = (macosxaudioport_t*)port;
  int channels = fts_audioport_get_max_channels( port, FTS_AUDIO_INPUT);
  int i, j;

  j = channel;
  for (i = 0; i < buffsize; i++)
    {
      buff[i] = macosx_port->input_buffer[j];
      j += channels;
    }
}

static void macosxaudioport_output_copy_fun( fts_audioport_t *port, float *buff, int buffsize, int channel)
{
  macosxaudioport_t* macosx_port = (macosxaudioport_t*)port;
  int channels = fts_audioport_get_max_channels( port, FTS_AUDIO_OUTPUT);
  int i, j;

  j = channel;

  for (i = 0; i < buffsize; i++)
    {
      macosx_port->output_buffer[j] = buff[i];
      j += channels;
    }
}

OSStatus macosxaudioport_ioproc( AudioDeviceID inDevice, 
			      const AudioTimeStamp *inNow, 
			      const AudioBufferList *inInputData, 
			      const AudioTimeStamp *inInputTime, 
			      AudioBufferList *outOutputData,
			      const AudioTimeStamp *inOutputTime,
			      void *inClientData)
{
  macosxaudioport_t *self = (macosxaudioport_t *)inClientData;
  int i, samples_per_buffer;

  samples_per_buffer = outOutputData->mBuffers[0].mDataByteSize / sizeof( float);

  for ( i = 0; i < samples_per_buffer; i += self->samples_per_tick)
    {
      self->input_buffer = (float *)inInputData->mBuffers[0].mData + i;
      self->output_buffer = (float *)outOutputData->mBuffers[0].mData + i;
      fts_sched_run_one_tick();
    }

  return noErr;
}

static int macosxaudioport_xrun( fts_audioport_t *port)
{
  macosxaudioport_t *self = (macosxaudioport_t *)port;
  int xrun = self->xrun;

  self->xrun = 0;

  return xrun;
}

static void macosxaudioport_halt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fd_set rfds;
  macosxaudioport_t *self = (macosxaudioport_t *)o;
  OSStatus err;

  fts_sched_remove( o);

  if (self->halted)
    {
      fts_log( "Halted twice !!!\n");
      return;
    }

  self->halted = 1;

  err = AudioDeviceAddIOProc( self->device, macosxaudioport_ioproc, self);
  if (err != noErr)
    {
      fts_object_error( o, "Cannot set IO proc");
      return;
    }
  
  if ((err = AudioDeviceStart( self->device, macosxaudioport_ioproc)) != noErr)
    {
      fts_object_error( o, "cannot start device");
      return;
    }

  /* halt scheduler main loop */
  FD_ZERO( &rfds);
  FD_SET( 0, &rfds);

  if (select( 1, &rfds, NULL, NULL, NULL) < 0)
    fts_log( "select() failed\n");

  fts_log( "After select() ????????\n");
}

static int
macosxaudioport_set_channels( macosxaudioport_t *self, int direction)
{
  OSStatus status;
  AudioBufferList *buffer_list;
  UInt32 count, channels;
  Boolean isInput = (direction == FTS_AUDIO_INPUT) ? true : false;

  /* Get the number of channels */
  if ((status = AudioDeviceGetPropertyInfo( self->device, 0, isInput, kAudioDevicePropertyStreamConfiguration, &count, NULL)) != noErr)
    {
      fts_object_error( (fts_object_t *)self, "cannot get device configuration");
      return -1;
    }

  buffer_list = (AudioBufferList *)alloca( count);
  if ((status = AudioDeviceGetProperty( self->device, 0, isInput, kAudioDevicePropertyStreamConfiguration, &count, buffer_list)) != noErr)
    {
      fts_object_error( (fts_object_t *)self, "cannot get device configuration");
      return -1;
    }

  /* We assume that there is only one buffer */
  if ( buffer_list->mNumberBuffers != 1)
    {
      fts_object_error( (fts_object_t *)self, "buffer_list->mNumberBuffers != 1");
      return -1;
    }

  channels = buffer_list->mBuffers[0].mNumberChannels;

  fts_audioport_set_max_channels( (fts_audioport_t *)self, direction, channels);

  return channels;
}


static void macosxaudioport_open( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxaudioport_t* self = (macosxaudioport_t*)o;
  OSStatus err;
  UInt32 count, buffer_size;
  int fifo_size = 4096;

  if (macosxaudioport_set_channels( self, FTS_AUDIO_INPUT) < 0)
    return;
  if (macosxaudioport_set_channels( self, FTS_AUDIO_OUTPUT) < 0)
    return;

  buffer_size = 2 * sizeof( float) * fifo_size;
  count = sizeof( buffer_size);
  
  if ((err = AudioDeviceSetProperty( self->device, NULL, 0, false, kAudioDevicePropertyBufferSize, count, &buffer_size)) != noErr)
    {
      post( "cannot set buffer size");
    }

  fts_audioport_set_io_fun( (fts_audioport_t *)self, FTS_AUDIO_INPUT, macosxaudioport_input);
  fts_audioport_set_io_fun( (fts_audioport_t *)self, FTS_AUDIO_OUTPUT, macosxaudioport_output);

  fts_sched_add( o, FTS_SCHED_ALWAYS);
}

static void macosxaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxaudioport_t *self = (macosxaudioport_t *)o;

  fts_audioport_init( (fts_audioport_t *)self);

  self->device = (AudioDeviceID)fts_get_int( at);
  self->name = fts_get_symbol( at+1);
}

static void macosxaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxaudioport_t *self = (macosxaudioport_t *)o;
  OSStatus err;

  fts_audioport_delete( (fts_audioport_t *)self);

  if ((err = AudioDeviceStop( self->device, macosxaudioport_ioproc)) != noErr)
    {
      post( "cannot stop device\n");
      return;
    }

  if ((err = AudioDeviceRemoveIOProc( self->device, macosxaudioport_ioproc)) != noErr)
    {
      post( "cannot remove IOProc\n");
      return;
    }
}

static void macosxaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( macosxaudioport_t), macosxaudioport_init, macosxaudioport_delete);

  fts_class_message_varargs( cl, fts_s_open_input, macosxaudioport_open);
  fts_class_message_varargs( cl, fts_s_open_output, macosxaudioport_open);

  fts_class_message_varargs( cl, fts_s_sched_ready, macosxaudioport_halt);
}

#define print_error( err, fun) post( "[Mac OS X audio port] error %d in function %s\n", err, #fun);

static void
macosxaudiomanager_scan_devices( void)
{
  UInt32 size, i, buffsize, count;
  char *buff;
  AudioDeviceID *device_list;
  AudioDeviceID default_device;
  OSStatus status;

  if ((status = AudioHardwareGetPropertyInfo( kAudioHardwarePropertyDevices, &size, NULL)) != noErr)
    {
      print_error( status, AudioHardwareGetPropertyInfo);
      return;
    }

  device_list = (AudioDeviceID *)malloc( size); 

  if ((status = AudioHardwareGetProperty( kAudioHardwarePropertyDevices, &size, device_list)) != noErr )
    {
      print_error( status, AudioHardwareGetProperty);
      return;
    }

  buffsize = 128;
  buff = (char *)fts_malloc( buffsize);

  fts_log( "scanning %d devices\n", size / sizeof(AudioDeviceID));

  for ( i = 0; i < size / sizeof(AudioDeviceID); i++)
    {
      fts_atom_t at[2];
      fts_audioport_t *port;
      fts_symbol_t name;

      if ((status = AudioDeviceGetPropertyInfo( device_list[i], 0, false, kAudioDevicePropertyDeviceName, &count, NULL)) != noErr)
	{
	  print_error( status, AudioDeviceGetPropertyInfo);
	  return;
	}

      if ( count >= buffsize)
	{
	  buffsize = count;
	  buff = (char *)fts_realloc( buff, buffsize);
	}

      if ((status = AudioDeviceGetProperty( device_list[i], 0, false, kAudioDevicePropertyDeviceName, &count, buff)) != noErr)
	{
	  print_error( status, AudioDeviceGetProperty);
	  return;
	}

      name = fts_new_symbol( buff);

      fts_set_int( at, device_list[i]);
      fts_set_symbol( at+1, name);
      port = (fts_audioport_t *)fts_object_create( macosxaudioport_class, NULL, 2, at);
      fts_audiomanager_put_port( name, port);

      fts_log( "registering device %d %s\n", i, name);
    }

  fts_free( buff);

  /* Get the default device */
  count = sizeof( default_device);
  if ((status = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &count, (void *)&default_device)) != noErr)
    {
      print_error( status, AudioHardwareGetProperty);
      return;
    }
}


void macosxaudioport_config( void)
{
  macosxaudioport_class = fts_class_install( fts_new_symbol("macosxaudioport"), macosxaudioport_instantiate);

  macosxaudiomanager_scan_devices();
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */



