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
 * This file include the jMax HAL audio port for Mac OS X
 *
 * Current implementation restrictions:
 * - only interleaved
 * - only output
 * - don't know how to get (and set?) the number of channels
 */

#undef PRINT_A_LOT
/*  #define PRINT_A_LOT */

#include <stdio.h>
#include <Carbon/Carbon.h>
#include <CoreAudio/AudioHardware.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <mach/mach.h>
#include <sys/sysctl.h>

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 1024
#define DEFAULT_CHANNELS 2

typedef struct {
  pthread_mutex_t mutex;        /* synchronisation mutex to access the fifo */
  pthread_cond_t awaiting_cond; /* to signal fts waiting to put data in the fifo */  

  volatile int fts_is_waiting;            /*flag to signal fts is waiting*/

  float *data;
  int length; /* in floats */

  volatile int read_index;  
  volatile int write_index;
} halaudioport_fifo_t;

typedef struct {
  fts_audioport_t head;
  halaudioport_fifo_t *fifo;
  AudioDeviceID device;
  int xrun;
} halaudioport_t;


static void fzero( float *buf, int n)
{
  int i;

  for ( i = 0; i < n; i++)
    buf[i] = 0.0;
}

static void fcpy( float *dst, const float *src, int n)
{
  int i;

  for ( i = 0; i < n; i++)
    dst[i] = src[i];
}


/***********************************************************************
 *
 * Fifo
 *
 */

#define FIFO_READ_LEVEL(FIFO)						\
(((FIFO)->write_index - (FIFO)->read_index < 0)				\
 ? (FIFO)->write_index - (FIFO)->read_index + (FIFO)->length	\
 : (FIFO)->write_index - (FIFO)->read_index)

#define FIFO_WRITE_LEVEL(FIFO)						\
(((FIFO)->read_index - (FIFO)->write_index - 1 < 0)			\
 ? (FIFO)->read_index - (FIFO)->write_index - 1 + (FIFO)->length	\
 : (FIFO)->read_index - (FIFO)->write_index - 1)

#define FIFO_INCR_READ_INDEX(FIFO, INCR) 					\
(FIFO)->read_index = ((FIFO)->read_index + (INCR)) % (FIFO)->length

#define FIFO_INCR_WRITE_INDEX(FIFO, INCR) 					\
(FIFO)->write_index = ((FIFO)->write_index + (INCR)) % (FIFO)->length

#define FIFO_READ_LOCATION( FIFO) ((FIFO)->data + (FIFO)->read_index)
#define FIFO_WRITE_LOCATION( FIFO) ((FIFO)->data + (FIFO)->write_index)


halaudioport_fifo_t *halaudioport_fifo_new( int length)
{
  halaudioport_fifo_t *fifo;

  fifo = (halaudioport_fifo_t *) fts_malloc( sizeof(halaudioport_fifo_t));

  fifo->length = length;
  fifo->data = (float *)fts_malloc( length * sizeof( float));
  fzero( fifo->data, length);
  
  fifo->read_index = 0;
  fifo->write_index = 0;

  pthread_mutex_init( &fifo->mutex, NULL);
  pthread_cond_init( &fifo->awaiting_cond, NULL);

  fifo->fts_is_waiting = 0;

  return fifo;
}

void halaudioport_fifo_delete(halaudioport_fifo_t *fifo)
{
  fts_free(fifo->data);
  pthread_mutex_destroy( &fifo->mutex);
  pthread_cond_destroy( &fifo->awaiting_cond);
  fts_free(fifo);
}

void halaudioport_fifo_debug( halaudioport_fifo_t *fifo, const char *msg)
{
  fprintf( stderr, "%s: ri=%-5d wi=%-5d rl=%-5d wl=%-5d w=%-1d\n", 
	   msg,
	   fifo->read_index,
	   fifo->write_index,
	   FIFO_READ_LEVEL( fifo),
	   FIFO_WRITE_LEVEL( fifo),
	   fifo->fts_is_waiting);

  assert( FIFO_READ_LEVEL( fifo) >= 0 && FIFO_READ_LEVEL( fifo) < fifo->length);
  assert( FIFO_WRITE_LEVEL( fifo) >= 0 && FIFO_WRITE_LEVEL( fifo) < fifo->length);
}

/***********************************************************************
 *
 * The audioport itself
 *
 */

static void halaudioport_input( fts_word_t *argv)
{
}

static void halaudioport_output( fts_word_t *argv)
{
  halaudioport_t *this;
  int n, channels, ch, i, j;

  this = (halaudioport_t *)fts_word_get_pointer( argv+0);
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_output_channels( (fts_audioport_t *)this);

  /* first pthread mutex lock */
  pthread_mutex_lock( &this->fifo->mutex);

  /* wait until there's the right space available */
  while ( FIFO_WRITE_LEVEL(this->fifo) < n * channels)
    {
      this->fifo->fts_is_waiting = 1;
      pthread_cond_wait( &this->fifo->awaiting_cond, &this->fifo->mutex);
    }

  pthread_mutex_unlock( &this->fifo->mutex);

  if (this->fifo->write_index + n*channels > this->fifo->length)
    halaudioport_fifo_debug( this->fifo, "Wraparound in output");

  /* then store the data and increment the write pointer */
  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_pointer( argv + 2 + ch);
    
      j = this->fifo->write_index + ch;
      for ( i = 0; i < n; i++)
	{
	  this->fifo->data[j] = in[i];
	  j += channels;
	}
    }

  FIFO_INCR_WRITE_INDEX( this->fifo, n*channels);
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
  int requested, available, missing, copied;

  requested = outOutputData->mBuffers[0].mDataByteSize / sizeof( float);
  available = FIFO_READ_LEVEL( this->fifo);

  if ( available >= requested)
    copied = requested;
  else
    copied = available;

  if ( this->fifo->read_index + copied > this->fifo->length)
    {
      /* There is a wrap */
      int before_wrap = this->fifo->length - this->fifo->read_index;
      int after_wrap = copied - before_wrap;

      fcpy( (float *)outOutputData->mBuffers[0].mData, FIFO_READ_LOCATION( this->fifo), before_wrap);
      fcpy( (float *)outOutputData->mBuffers[0].mData+before_wrap, this->fifo->data, after_wrap);
    }
  else
    { 
      fcpy( (float *)outOutputData->mBuffers[0].mData, FIFO_READ_LOCATION( this->fifo), copied);
    }

  FIFO_INCR_READ_INDEX( this->fifo, copied);

  if (copied < requested)
    {
      fzero( (float *)outOutputData->mBuffers[0].mData + copied, requested - copied);

      this->xrun = 1;
    }

  pthread_mutex_lock( &this->fifo->mutex);

  if ( this->fifo->fts_is_waiting)
    {
      this->fifo->fts_is_waiting = 0;
      pthread_cond_signal( &this->fifo->awaiting_cond);
    }

  pthread_mutex_unlock( &this->fifo->mutex);

  return noErr;
}

static int halaudioport_xrun( fts_audioport_t *port)
{
  halaudioport_t *this = (halaudioport_t *)port;
  int xrun = this->xrun;

  this->xrun = 0;

  return xrun;
}

static int get_bus_speed( void)
{
  int mib[2];
  unsigned int miblen;
  int busspeed;
  int retval;
  size_t len;
          
  mib[0]=CTL_HW;
  mib[1]=HW_BUS_FREQ;
  miblen=2;
  len=4;
  /* Note: you should really check retval here, see man sysctl for info */
  retval = sysctl( mib, miblen, &busspeed, &len, NULL, 0);

  return busspeed;
}

static void set_real_time( float rate)
{
  thread_t s;
  kern_return_t r;
  integer_t info;
  mach_msg_type_number_t count;
  boolean_t b;
  thread_time_constraint_policy_data_t ttcpolicy;

  s = mach_thread_self();

  /* This is in AbsoluteTime units, which are equal to
     1/4 the bus speed on most machines. */
          
  ttcpolicy.period = (int)((get_bus_speed() / 4) / rate);
  ttcpolicy.computation = (int)(0.8 * ttcpolicy.period);
  ttcpolicy.constraint = (int)(0.9 * ttcpolicy.period);
  ttcpolicy.preemptible = TRUE;
                  
  if ( (r = thread_policy_set( mach_thread_self(), THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&ttcpolicy, THREAD_TIME_CONSTRAINT_POLICY_COUNT)) != KERN_SUCCESS)
    post( "Can't do thread_policy_set (%d)\n", r);
}


static void halaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  halaudioport_t *this = (halaudioport_t *)o;
  OSStatus err;
  UInt32 count;
  UInt32 channels;
  UInt32 buffer_size;
  Boolean writable;
  AudioBufferList *buffer_list;

  fts_audioport_init( &this->head);

  /* Get the default device */
  count = sizeof( this->device);
  if ((err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &count, (void *)&this->device)) != noErr)
    {
      fts_object_set_error( o, "Cannot get default device");
      return;
    }

  /* Get the number of channels and the buffer size */
  if ((err = AudioDeviceGetPropertyInfo( this->device, 0, false, kAudioDevicePropertyStreamConfiguration, &count, &writable)) != noErr)
    {
      fts_object_set_error( o, "Cannot get device configuration");
      post( "Cannot get device configuration\n");
      return;
    }

  buffer_list = (AudioBufferList *)alloca( count);
  if ((err = AudioDeviceGetProperty( this->device, 0, false, kAudioDevicePropertyStreamConfiguration, &count, buffer_list)) != noErr)
    {
      fts_object_set_error( o, "Cannot get device configuration");
      post( "Cannot get device configuration\n");
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
	fts_object_set_error( o, "Cannot set buffer size");
	return;
      }
  }
#endif

  channels = buffer_list->mBuffers[0].mNumberChannels;
  buffer_size = buffer_list->mBuffers[0].mDataByteSize;

  fts_audioport_set_output_channels( (fts_audioport_t *)this, channels);
  fts_audioport_set_output_function( (fts_audioport_t *)this, halaudioport_output);

  set_real_time( fts_dsp_get_sample_rate() / (buffer_size / (sizeof(float)*channels)) );

  /* Create the fifo 
   * We make a fifo of 2 buffers. May be 3 would be better.
   */
  this->fifo = halaudioport_fifo_new( (2 * buffer_size) / sizeof( float));

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

  err = AudioDeviceStop( this->device, halaudioport_ioproc);
  if (err != noErr)
    ;

  err = AudioDeviceRemoveIOProc( this->device, halaudioport_ioproc);
  if (err != noErr)
    ;

  halaudioport_fifo_delete(this->fifo);
}

static void halaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static fts_status_t halaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( halaudioport_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, halaudioport_init);
  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_delete, halaudioport_delete);

  fts_class_add_daemon( cl, obj_property_get, fts_s_state, halaudioport_get_state);

  return fts_Success;
}

void halaudioport_config( void)
{
  fts_symbol_t halaudioport_symbol = fts_new_symbol("halaudioport");

  fts_class_install( halaudioport_symbol, halaudioport_instantiate);
  fts_audioport_set_default_class(halaudioport_symbol);
}
