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
  NOTE:
  How to fix the multiple devices problem ?
  The problem is that if you open > 1 device, you have 2 IOProc running and
  only one must call the FTS scheduler.
  Fix:
   - add a 'master' field in the audioport
   - only the master audioport runs the FTS scheduler. This will trigger the call
   of ***all*** the audioport input/output functions
   - the input/output functions must differentiate the case where they are called
   inside the IOProc of their own port or not (i.e. if the port is master or not)
   - if port is master, do as now
   - if not, copy from/to the buffers that are passed as arguments to a temporary buffer
   that is allocated in the port_init (or open) of size the device buffer size
   - the IOProc must also differentiate if master or not
   - if master, do as now
   - if not, copy the port input/output buffers to the buffers that are passed to the 
   IOProc 
   - when opening/closing ports, if closed port was master, you must find a new one
   by going through the list of opened ports and select the first one as master port. This must
   probably be done at the same time as restarting the scheduler, which should only happen
   when all IOProc has been removed (i.e. all ports have been closed)
 */

/*
  NOTE 2:
  But it does not work yet, probably because IOProc are called in different threads, 
  resulting in race conditions. 
  Current situation (as of release 4.0.2), is that selecting a different device makes
  FTS crash. The fix is that the audiomanager defines only the default output device
  This is done by the following #define:
*/
#define ONLY_DEFAULT_OUTPUT_DEVICE

#include <Carbon/Carbon.h>
#include <CoreAudio/AudioHardware.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <fts/fts.h>

typedef struct {
  fts_audioport_t head;
  AudioDeviceID device;
  fts_symbol_t name;
  struct _buffer_info {
    int buffer_number;
    int start_offset;
    int n_channels_in_buffer;
  } *input_buffer_info, *output_buffer_info;
  AudioBufferList *input_buffer_list, *output_buffer_list;
  AudioBufferList *store_input_buffer_list, *store_output_buffer_list;
  int master;
} macosxaudioport_t;

static fts_class_t *macosxaudioport_class;

static int sched_pipe_des[2];

static int opened_ports_count;
static int samples_count;

static void
fzero( float *dst, int dst_jump, int size)
{
  int i;

  for ( i = 0; i < size; i++)
    {
      *dst = 0.0;
      dst += dst_jump;
    }
}

static void
fcopy( float *dst, int dst_jump, const float *src, int src_jump, int size)
{
  int i;

  for ( i = 0; i < size; i ++)
    {
      *dst = *src;
      dst += dst_jump;
      src += src_jump;
    }
}

static void
macosxaudioport_input(fts_audioport_t* port, float **buffers, int buffsize)
{
  macosxaudioport_t* self = (macosxaudioport_t*)port;
  int channel;
  AudioBufferList *buffer_list;

  buffer_list = (self->input_buffer_list) ? self->input_buffer_list : self->store_input_buffer_list;

  for ( channel = 0; channel < fts_audioport_get_channels( port, FTS_AUDIO_INPUT); channel++)
    {
      float *src, *dst;
      int buffer_number, n_channels_in_buffer;

      buffer_number = self->input_buffer_info[ channel ].buffer_number;

      n_channels_in_buffer = self->input_buffer_info[ channel ].n_channels_in_buffer;

      src = (float *)buffer_list->mBuffers[ buffer_number ].mData
	+ samples_count * n_channels_in_buffer
	+ self->input_buffer_info[ channel ].start_offset;

      dst = *buffers++;

      if (fts_audioport_is_channel_used( port, FTS_AUDIO_INPUT, channel))
	fcopy( dst, 1, src, n_channels_in_buffer, buffsize);
      else
	fzero( dst, 1, buffsize);
    }
}


static void
macosxaudioport_output(fts_audioport_t* port, float **buffers, int buffsize)
{
  macosxaudioport_t *self = (macosxaudioport_t*)port;
  int channel;
  AudioBufferList *buffer_list;

  buffer_list = (self->output_buffer_list) ? self->output_buffer_list : self->store_output_buffer_list;

  for ( channel = 0; channel < fts_audioport_get_channels( port, FTS_AUDIO_OUTPUT); channel++)
    {
      float *src, *dst;
      int buffer_number, n_channels_in_buffer;

      buffer_number = self->output_buffer_info[ channel ].buffer_number;

      n_channels_in_buffer = self->output_buffer_info[ channel ].n_channels_in_buffer;

      dst = (float *)buffer_list->mBuffers[ buffer_number ].mData
	+ samples_count * n_channels_in_buffer
	+ self->output_buffer_info[ channel ].start_offset;

      src = *buffers++;

      if (fts_audioport_is_channel_used( port, FTS_AUDIO_OUTPUT, channel))
	fcopy( dst, n_channels_in_buffer, src, 1, buffsize);
      else
	fzero( dst, n_channels_in_buffer, buffsize);
    }
}

static void
copy_buffers( AudioBufferList *dst, const AudioBufferList *src)
{
  int n;

  for ( n = 0; n < dst->mNumberBuffers; n++)
    fcopy( (float *)dst->mBuffers[n].mData, 1, (float *)src->mBuffers[n].mData, 1, dst->mBuffers[n].mDataByteSize / sizeof( float));
}

OSStatus
macosxaudioport_ioproc( AudioDeviceID inDevice, 
			const AudioTimeStamp *inNow, 
			const AudioBufferList *inInputData, 
			const AudioTimeStamp *inInputTime, 
			AudioBufferList *outOutputData,
			const AudioTimeStamp *inOutputTime,
			void *inClientData)
{
  macosxaudioport_t *self = (macosxaudioport_t *)inClientData;

  /* set buffers */
  self->input_buffer_list = inInputData;
  self->output_buffer_list = outOutputData;

  if (self->master)
    {
      /* run FTS scheduler */
      int n_samples = outOutputData->mBuffers[0].mDataByteSize / (outOutputData->mBuffers[0].mNumberChannels * sizeof( float));
      for ( samples_count = 0; samples_count < n_samples; samples_count += fts_dsp_get_tick_size())
	fts_sched_run_one_tick();
    }
  else
    {
      copy_buffers( self->store_input_buffer_list, inInputData);
      copy_buffers( outOutputData, self->store_output_buffer_list);
    }

  /* unset buffers */
  self->input_buffer_list = NULL;
  self->output_buffer_list = NULL;

  return noErr;
}

static void
find_master_port( void)
{
  fts_iterator_t iter;
  fts_atom_t a;
  macosxaudioport_t *port;

  /* If we already got one, there is no need to search more. */
  for ( fts_audioport_get_ports( &iter); fts_iterator_has_more( &iter); )
    {
      fts_iterator_next( &iter, &a);

      if (fts_object_get_class( fts_get_object( &a)) != macosxaudioport_class)
	continue;

      port = (macosxaudioport_t *)fts_get_object( &a);

      if (port->master 
	  && (fts_audioport_is_open( (fts_audioport_t *)port, FTS_AUDIO_INPUT) 
	      || fts_audioport_is_open( (fts_audioport_t *)port, FTS_AUDIO_OUTPUT)))
	return;
    }

  /* No master port yet, we choose the first one that is opened. */
  for ( fts_audioport_get_ports( &iter); fts_iterator_has_more( &iter); )
    {
      fts_iterator_next( &iter, &a);

      if (fts_object_get_class( fts_get_object( &a)) != macosxaudioport_class)
	continue;

      port = (macosxaudioport_t *)fts_get_object( &a);

      if (fts_audioport_is_open( (fts_audioport_t *)port, FTS_AUDIO_INPUT) 
	  || fts_audioport_is_open( (fts_audioport_t *)port, FTS_AUDIO_OUTPUT))
	{
	  port->master = 1;
	  fts_log( "[macosxaudioport] master port is \"%s\"\n", port->name);
	  return;
	}
    }
}

static void 
macosxaudioport_halt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxaudioport_t *self = (macosxaudioport_t *)o;
  OSStatus err;
  fd_set rfds;

  fts_sched_remove( o);

  find_master_port();

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

  /* when opening first port, halt the standard scheduler */
  if (opened_ports_count == 1)
    {
      FD_ZERO(&rfds);
      FD_SET(sched_pipe_des[0], &rfds);

      fts_log( "[macosxaudioport] halting FTS scheduler \n");

      if (select( sched_pipe_des[0] + 1 , &rfds, NULL, NULL, NULL) < 0)
	fts_log( "[macosxaudioport] select() failed, error=\"%s\"\n", strerror( errno));

      if (FD_ISSET(sched_pipe_des[0], &rfds))
	{
	  int val;
	  read(sched_pipe_des[0], &val, sizeof(int));
	}

      fts_log( "[macosxaudioport] restarted FTS scheduler \n");
    }
}

static void
macosxaudioport_open( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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

  opened_ports_count++;

  fts_sched_add( o, FTS_SCHED_ALWAYS);
}

static void
macosxaudioport_close(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  macosxaudioport_t *self = (macosxaudioport_t *)o;
  OSStatus err;
  int dummy = 0;


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

  fts_audioport_unset_open((fts_audioport_t*)self, FTS_AUDIO_INPUT);
  fts_audioport_unset_open((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);

  if (self->master)
    {
      self->master = 0;
      find_master_port();
    }
  
  opened_ports_count--;

  /* when closing last port, restart scheduler */
  if (opened_ports_count <= 0)
    write(sched_pipe_des[1], &dummy, sizeof(int));
}

static void
log_buffer_info( macosxaudioport_t *self, int channels, int direction)
{
  struct _buffer_info *info;
  int ch;

  info = (direction == FTS_AUDIO_INPUT) ? self->input_buffer_info : self->output_buffer_info;
  fts_log( "%s %s buffer info:\n", self->name, (direction == FTS_AUDIO_INPUT) ? "input" : "output");
  fts_log( "      %-20s %-20s %-20s\n", "buffer_number", "start_offset", "n_channels_in_buffer");
  for ( ch = 0; ch < channels; ch++)
    {
      fts_log( " [%2d] %-20d %-20d %-20d\n", ch, info[ch].buffer_number, info[ch].start_offset, info[ch].n_channels_in_buffer);
    }
}

static void
log_buffer_list( macosxaudioport_t *self, int direction)
{
  int i;
  AudioBufferList *buffer_list;

  buffer_list = (direction == FTS_AUDIO_INPUT) ?  self->store_input_buffer_list : self->store_output_buffer_list;
  fts_log( "%s %s buffer list:\n", self->name, (direction == FTS_AUDIO_INPUT) ? "input" : "output");
  fts_log( "      %-20s %-20s\n", "mNumberChannels", "mDataByteSize");
  for ( i = 0; i < buffer_list->mNumberBuffers; i++)
    {
      fts_log( " [%2d] %-20d %-20d\n", i, buffer_list->mBuffers[i].mNumberChannels, buffer_list->mBuffers[i].mDataByteSize);
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
  AudioBufferList *buffer_list, *store_buffer_list;
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

  fts_audioport_set_channels( (fts_audioport_t *)self, direction, channels);

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
      info[ch].start_offset = channel_in_buffer;
      info[ch].n_channels_in_buffer = buffer_list->mBuffers[i].mNumberChannels;
    }

#if 1
  log_buffer_info( self, channels, direction);
#endif

  store_buffer_list = (AudioBufferList *)fts_malloc( sizeof( AudioBufferList) + (buffer_list->mNumberBuffers-1)*sizeof( AudioBuffer));

  store_buffer_list->mNumberBuffers = buffer_list->mNumberBuffers;
  for ( i = 0; i < buffer_list->mNumberBuffers; i++)
    {
      store_buffer_list->mBuffers[i].mNumberChannels = buffer_list->mBuffers[i].mNumberChannels;
      store_buffer_list->mBuffers[i].mDataByteSize = buffer_list->mBuffers[i].mDataByteSize;
      store_buffer_list->mBuffers[i].mData = fts_malloc( store_buffer_list->mBuffers[i].mDataByteSize);

      fzero( store_buffer_list->mBuffers[i].mData, 1, buffer_list->mBuffers[i].mDataByteSize / sizeof( float));
    }

  if (direction == FTS_AUDIO_INPUT)
    self->store_input_buffer_list = store_buffer_list;
  else
    self->store_output_buffer_list = store_buffer_list;

#if 1
  log_buffer_list( self, direction);
#endif

  return channels;
}

static void 
macosxaudioport_sched_listener(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{  
  int dummy = 0;
  
  if (!fts_sched_is_running())
  {
    fts_log("[macosxaudioport] trying to restart FTS scheduler\n");

    /* restart FTS scheduler */
    write(sched_pipe_des[1], &dummy, sizeof(int));

    /* @@@@@ delete audio port  @@@@ */
    /* (fd) why ????? */
    /*     fts_object_release(o); */
  }
}

static void
macosxaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxaudioport_t *self = (macosxaudioport_t *)o;

  fts_audioport_init( (fts_audioport_t *)self);

  self->device = (AudioDeviceID)fts_get_int( at);
  self->name = fts_get_symbol( at+1);

  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, macosxaudioport_input);
  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, macosxaudioport_output);

  if ((get_channels( self, FTS_AUDIO_INPUT)) < 0)
    fts_object_error( (fts_object_t *)self, "cannot get device configuration");

  if ((get_channels( self, FTS_AUDIO_OUTPUT)) < 0)
    fts_object_error( (fts_object_t *)self, "cannot get device configuration");

  fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_INPUT);
  fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);

  fts_sched_running_add_listener(o, macosxaudioport_sched_listener);
}

static void
macosxaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxaudioport_t *self = (macosxaudioport_t *)o;
  int i;
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

  fts_sched_running_remove_listener(o);

  for ( i = 0; i < self->store_input_buffer_list->mNumberBuffers; i++)
    fts_free( self->store_input_buffer_list->mBuffers[i].mData);

  for ( i = 0; i < self->store_output_buffer_list->mNumberBuffers; i++)
    fts_free( self->store_output_buffer_list->mBuffers[i].mData);
}

static void
macosxaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( macosxaudioport_t), macosxaudioport_init, macosxaudioport_delete);

  fts_class_message_varargs( cl, fts_s_open_input, macosxaudioport_open);
  fts_class_message_varargs( cl, fts_s_open_output, macosxaudioport_open);

  fts_class_message_varargs( cl, fts_s_close_input, macosxaudioport_close);
  fts_class_message_varargs( cl, fts_s_close_output, macosxaudioport_close);

  fts_class_message_varargs( cl, fts_s_sched_ready, macosxaudioport_halt);
}

#define print_error( err, fun) post( "[macosxaudioport] error %d in function %s\n", err, #fun);

#ifndef ONLY_DEFAULT_OUTPUT_DEVICE
static void
macosxaudiomanager_scan_devices( void)
{
  UInt32 size, i, buffsize, count;
  char *buff;
  AudioDeviceID *device_list;
  AudioDeviceID default_input_device, default_output_device;
  OSStatus status;

  /* Get the default devices */
  count = sizeof( default_input_device);
  if ((status = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultInputDevice, &count, (void *)&default_input_device)) != noErr)
    {
      print_error( status, AudioHardwareGetProperty);
      return;
    }

  count = sizeof( default_output_device);
  if ((status = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &count, (void *)&default_output_device)) != noErr)
    {
      print_error( status, AudioHardwareGetProperty);
      return;
    }

  /* Get device list */
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
      fts_object_refer((fts_object_t*)port);
      fts_audiomanager_put_port( name, port);

      if(device_list[i] == default_output_device)
      {
        fts_object_refer((fts_object_t*)port);
        fts_audiomanager_put_port( fts_s_default, port);
      }
    }

  fts_free( buff);

}
#endif


#ifdef ONLY_DEFAULT_OUTPUT_DEVICE
static void
macosxaudiomanager_new_device( AudioDeviceID device, fts_symbol_t name)
{
  fts_atom_t at[2];
  fts_audioport_t *port;

  if (name == NULL)
    {
      char *buff;
      UInt32 count;
      OSStatus status;

      if ((status = AudioDeviceGetPropertyInfo( device, 0, false, kAudioDevicePropertyDeviceName, &count, NULL)) != noErr)
	{
	  print_error( status, AudioDeviceGetPropertyInfo);
	  return;
	}

      buff = (char *)fts_malloc( count);

      if ((status = AudioDeviceGetProperty( device, 0, false, kAudioDevicePropertyDeviceName, &count, buff)) != noErr)
	{
	  print_error( status, AudioDeviceGetProperty);
	  return;
	}

      name = fts_new_symbol( buff);

      fts_free( buff);
    }

  fts_set_int( at, device);
  fts_set_symbol( at+1, name);
  port = (fts_audioport_t *)fts_object_create( macosxaudioport_class, 2, at);
  fts_object_refer((fts_object_t*)port);
  fts_audiomanager_put_port( name, port);
}

static void
macosxaudiomanager_scan_devices( void)
{
  UInt32 size, i, count;
  char *buff;
  AudioDeviceID *device_list;
  AudioDeviceID default_output_device;
  OSStatus status;

  /* Get the default output device */
  count = sizeof( default_output_device);
  if ((status = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &count, (void *)&default_output_device)) != noErr)
    {
      print_error( status, AudioHardwareGetProperty);
      return;
    }

  macosxaudiomanager_new_device( default_output_device, fts_s_default);
  macosxaudiomanager_new_device( default_output_device, NULL);
}
#endif

void
macosxaudioport_config( void)
{
  macosxaudioport_class = fts_class_install( fts_new_symbol("macosxaudioport"), macosxaudioport_instantiate);

  /* create pipe */
  if (0 != pipe(sched_pipe_des))
  {
    fts_log("[macosxaudioport] cannot create pipe descriptors \n");
    return;
  }
  macosxaudiomanager_scan_devices();
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */



