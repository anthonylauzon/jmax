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

#include <Carbon/Carbon.h>
#include <CoreAudio/AudioHardware.h>
#include <unistd.h>

#include <fts/fts.h>

typedef struct {
  fts_audioport_t head;
  AudioDeviceID device;
  fts_symbol_t name;
  struct _buffer_info {
    int buffer_number;
    int offset;
    int jump;
    float *buffer;
  } *input_buffer_info, *output_buffer_info;
  int halted;
} macosxaudioport_t;

static fts_class_t *macosxaudioport_class;

/**
 * The audioport IO function calls the native audio layer to read/write a buffer
 * of samples in the native format.
 * Its argument is the audioport. Buffers are allocated by the port.
 */
static void macosxaudioport_input(fts_audioport_t* port)
{
}


static void macosxaudioport_output(fts_audioport_t* port)
{
}

/** 
 * The audioport copy functions copies the samples in the native format to a float buffer
 * for a given channel.
 */

static void macosxaudioport_input_copy( fts_audioport_t *port, float *buff, int buffsize, int channel)
{
  macosxaudioport_t* macosx_port = (macosxaudioport_t*)port;
  float *src;
  int i, src_jump;

  src = macosx_port->input_buffer_info[channel].buffer;
  src_jump = macosx_port->input_buffer_info[channel].jump;

  for ( i = 0; i < buffsize; i += 4)
    {
      *buff = *src;
      src += src_jump;
      buff++;
      *buff = *src;
      src += src_jump;
      buff++;
      *buff = *src;
      src += src_jump;
      buff++;
      *buff = *src;
      src += src_jump;
      buff++;
    }

  macosx_port->input_buffer_info[channel].buffer = src;
}

static void macosxaudioport_output_copy( fts_audioport_t *port, float *buff, int buffsize, int channel)
{
  macosxaudioport_t* macosx_port = (macosxaudioport_t*)port;
  float *dst;
  int i, dst_jump;

  dst = macosx_port->output_buffer_info[channel].buffer;
  dst_jump = macosx_port->output_buffer_info[channel].jump;

  for ( i = 0; i < buffsize; i += 4)
    {
      *dst = *buff;
      dst += dst_jump;
      buff++;
      *dst = *buff;
      dst += dst_jump;
      buff++;
      *dst = *buff;
      dst += dst_jump;
      buff++;
      *dst = *buff;
      dst += dst_jump;
      buff++;
    }

  macosx_port->output_buffer_info[channel].buffer = dst;
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
  int i, ch, n_samples;

  /* setup buffers */
  for ( ch = 0; ch < fts_audioport_get_max_channels( (fts_audioport_t *)self, FTS_AUDIO_INPUT); ch++)
    {
      struct _buffer_info *info = self->input_buffer_info + ch;

      info->buffer = (float *)inInputData->mBuffers[ info->buffer_number ].mData + info->offset;
    }

  for ( ch = 0; ch < fts_audioport_get_max_channels( (fts_audioport_t *)self, FTS_AUDIO_OUTPUT); ch++)
    {
      struct _buffer_info *info = self->output_buffer_info + ch;

      info->buffer = (float *)outOutputData->mBuffers[ info->buffer_number ].mData + info->offset;
    }

  /* run FTS scheduler */
  n_samples = outOutputData->mBuffers[0].mDataByteSize / (outOutputData->mBuffers[0].mNumberChannels * sizeof( float));

  for ( i = 0; i < n_samples; i += fts_dsp_get_tick_size())
    {
      fts_sched_run_one_tick();
    }

  return noErr;
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

static void macosxaudioport_open( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxaudioport_t* self = (macosxaudioport_t*)o;
  OSStatus err;
  UInt32 count, buffer_size;
  int fifo_size = 4096;

  buffer_size = 2 * sizeof( float) * fifo_size;
  count = sizeof( buffer_size);
  
  if ((err = AudioDeviceSetProperty( self->device, NULL, 0, false, kAudioDevicePropertyBufferSize, count, &buffer_size)) != noErr)
    {
      post( "cannot set buffer size");
    }

  fts_audioport_set_open( (fts_audioport_t *)self, FTS_AUDIO_INPUT);
  fts_audioport_set_open( (fts_audioport_t *)self, FTS_AUDIO_OUTPUT);

  fts_sched_add( o, FTS_SCHED_ALWAYS);
}

static void
dump_buffer_info( macosxaudioport_t *self, int channels, int direction)
{
  struct _buffer_info *info;
  int ch;

  info = (direction == FTS_AUDIO_INPUT) ? self->input_buffer_info : self->output_buffer_info;
  post( "%s %s buffer info:\n", self->name, (direction == FTS_AUDIO_INPUT) ? "input" : "output");
  post( "      %-8s %-8s %-8s\n", "buffer", "offset", "jump");
  for ( ch = 0; ch < channels; ch++)
    {
      post( " [%2d] %-8d %-8d %-8d\n", ch, info[ch].buffer_number, info[ch].offset, info[ch].jump);
    }
}

/* 
   Returns the total number of channels of the selected direction for the given device 
   and setups the buffer information
*/
static int
get_channels( macosxaudioport_t *self, int direction)
{
  OSStatus status;
  AudioBufferList *buffer_list;
  UInt32 count, channels, i, ch;
  Boolean isInput;
  struct _buffer_info *info;

  isInput = (direction == FTS_AUDIO_INPUT);

  if ((status = AudioDeviceGetPropertyInfo( self->device, 0, isInput, kAudioDevicePropertyStreamConfiguration, &count, NULL)) != noErr)
    return -1;

  buffer_list = (AudioBufferList *)alloca( count);
  if ((status = AudioDeviceGetProperty( self->device, 0, isInput, kAudioDevicePropertyStreamConfiguration, &count, buffer_list)) != noErr)
    return -1;

  channels = 0;
  for (i = 0; i < buffer_list->mNumberBuffers; i++) 
    channels += buffer_list->mBuffers[i].mNumberChannels;

  fts_audioport_set_max_channels( (fts_audioport_t *)self, direction, channels);

  info = (struct _buffer_info *)fts_malloc( sizeof( struct _buffer_info) * channels);

  if (direction == FTS_AUDIO_INPUT)
    self->input_buffer_info = info;
  else
    self->output_buffer_info = info;

  for ( ch = 0; ch < channels; ch++)
    {
      int channel_in_buffer = ch;

      for ( i = 0; i < buffer_list->mNumberBuffers; i++)
	{
	  if (channel_in_buffer < buffer_list->mBuffers[i].mNumberChannels)
	    break;

	  channel_in_buffer -= buffer_list->mBuffers[i].mNumberChannels;
	}

      info[ch].buffer_number = i;
      info[ch].offset = channel_in_buffer;
      info[ch].jump = buffer_list->mBuffers[i].mNumberChannels;
      info[ch].buffer = NULL;
    }

#if 0
  dump_buffer_info( self, channels, direction);
#endif

  return channels;
}

static void macosxaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxaudioport_t *self = (macosxaudioport_t *)o;

  fts_audioport_init( (fts_audioport_t *)self);

  self->device = (AudioDeviceID)fts_get_int( at);
  self->name = fts_get_symbol( at+1);

  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, macosxaudioport_input);
  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, macosxaudioport_output);

  fts_audioport_set_copy_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, macosxaudioport_input_copy);
  fts_audioport_set_copy_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, macosxaudioport_output_copy);

  if ((get_channels( self, FTS_AUDIO_INPUT)) < 0)
    fts_object_error( (fts_object_t *)self, "cannot get device configuration");

  if ((get_channels( self, FTS_AUDIO_OUTPUT)) < 0)
    fts_object_error( (fts_object_t *)self, "cannot get device configuration");

  fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_INPUT);
  fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);
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
      port = (fts_audioport_t *)fts_object_create( macosxaudioport_class, 2, at);
      fts_audiomanager_put_port( name, port);
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



