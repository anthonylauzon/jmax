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
 * Authors: Francois Dechelle, Riccardo Borghesi, Norbert Schnell, .
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
#include <pthread.h>

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 1024
#define DEFAULT_CHANNELS 2

typedef struct {
  pthread_mutex_t *mutex;        /* synchronisation mutex to access the fifo */
  pthread_cond_t *awaiting_cond; /* to signal fts waiting to put data in the fifo */  
  volatile int fts_is_waiting;            /*flag to signal fts is waiting*/

  float *data;
  int fifo_length;

  volatile int read_index;  
  volatile int write_index;
} halaudioport_fifo_t;

#define FIFO_READ_LEVEL(FIFO)						\
(((FIFO)->write_index - (FIFO)->read_index < 0)				\
 ? (FIFO)->write_index - (FIFO)->read_index + (FIFO)->fifo_length	\
 : (FIFO)->write_index - (FIFO)->read_index)

#define FIFO_WRITE_LEVEL(FIFO)						\
(((FIFO)->read_index - (FIFO)->write_index - 1 < 0)			\
 ? (FIFO)->read_index - (FIFO)->write_index - 1 + (FIFO)->fifo_length	\
 : (FIFO)->read_index - (FIFO)->write_index - 1)

#define FIFO_INCR_READ_INDEX(FIFO, INCR) 					\
(FIFO)->read_index = ((FIFO)->read_index + (INCR)) % (FIFO)->fifo_length

#define FIFO_INCR_WRITE_INDEX(FIFO, INCR) 					\
(FIFO)->write_index = ((FIFO)->write_index + (INCR)) % (FIFO)->fifo_length

halaudioport_fifo_t *halaudioport_fifo_new(int channels, int fifo_length)
{
  halaudioport_fifo_t *fifo;

  fifo = (halaudioport_fifo_t *) malloc(sizeof(halaudioport_fifo_t));

  if (!fifo)
    return 0;
  memset(fifo, 0, sizeof(halaudioport_fifo_t));

  fifo->fifo_length = fifo_length*channels*sizeof(float);

  fifo->data = (float*) malloc(channels * fifo->fifo_length * sizeof(float));
  memset(fifo->data, 0, channels * fifo->fifo_length * sizeof(float));
  
  fifo->mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(fifo->mutex, NULL);
  
  fifo->awaiting_cond = malloc(sizeof(pthread_cond_t));
  pthread_cond_init(fifo->awaiting_cond, NULL);
  fifo->fts_is_waiting = 0;

  fifo->read_index = 0;
  fifo->write_index = 0;

  return fifo;
}

void halaudioport_fifo_delete(halaudioport_fifo_t *fifo)
{
  if (fifo == NULL) {
    return;
  }
  if (fifo->data != NULL) {
    fts_free(fifo->data);
  }
  if (fifo->mutex != NULL) {
    pthread_mutex_destroy(fifo->mutex);
  }
  if (fifo->awaiting_cond != NULL) {
    pthread_cond_destroy(fifo->awaiting_cond);
  }
  fts_free(fifo);
}

typedef struct {
  fts_audioport_t head;
  halaudioport_fifo_t *fifo;
  AudioDeviceID device;
} halaudioport_t;

static void halaudioport_input( fts_word_t *argv)
{
}

static void halaudioport_output( fts_word_t *argv)
{
  halaudioport_t *this;
  int n, channels, ch, i, j;

  this = (halaudioport_t *)fts_word_get_ptr( argv+0);
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_output_channels( (fts_audioport_t *)this);

  /* first pthread mutex lock */
  pthread_mutex_lock(this->fifo->mutex);

  /* wait until there's the right space available */
  while (FIFO_WRITE_LEVEL(this->fifo) < n*channels*sizeof(float)) {
    this->fifo->fts_is_waiting = 1;
    pthread_cond_wait(this->fifo->awaiting_cond, this->fifo->mutex);    
  }

  /* then store the data and increment the write pointer */
  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_ptr( argv + 2 + ch);
    
      j = this->fifo->write_index + ch;
      for ( i = 0; i < n; i++)
	{
	  this->fifo->data[j] = in[i];/*?????*/
	  j += channels;
	}
    }

  FIFO_INCR_WRITE_INDEX(this->fifo, n*channels*sizeof(float));

  pthread_mutex_unlock(this->fifo->mutex);
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
  int n, aval, rest;
  /* first pthread mutex lock */
  pthread_mutex_lock(this->fifo->mutex);

  aval = FIFO_READ_LEVEL(this->fifo);
  if(aval >= outOutputData->mBuffers[0].mDataByteSize)
    { 
      memcpy( outOutputData->mBuffers[0].mData, 
	      this->fifo->data + this->fifo->read_index, 
	      outOutputData->mBuffers[0].mDataByteSize);
    
      FIFO_INCR_READ_INDEX(this->fifo, outOutputData->mBuffers[0].mDataByteSize);
    }
  else
    {
      rest = outOutputData->mBuffers[0].mDataByteSize - aval;
      memcpy( outOutputData->mBuffers[0].mData, 
	      this->fifo->data + this->fifo->read_index,
	      aval);
      memset( outOutputData->mBuffers[0].mData, 0, rest);
      FIFO_INCR_READ_INDEX(this->fifo, aval);
    }

  if(this->fifo->fts_is_waiting)
    {
      pthread_cond_signal(this->fifo->awaiting_cond);
      this->fifo->fts_is_waiting = 0;
    }

  pthread_mutex_unlock(this->fifo->mutex);

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
  buffer_size = fts_get_int_arg( ac, at, 2, fts_dsp_get_tick_size());

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
  this->fifo = halaudioport_fifo_new(channels, fifo_size);

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
  fts_method_define( cl, fts_SystemInlet, fts_s_delete, halaudioport_delete, 0, 0);
  /* define variable */
  fts_class_add_daemon( cl, obj_property_get, fts_s_state, halaudioport_get_state);

  return fts_Success;
}

void halaudioport_config( void)
{
  fts_class_install( fts_new_symbol("halaudioport"), halaudioport_instantiate);
}
