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

#include <fts/fts.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include "jmax_asio_port.h"

/* Define this if you want to post debug message */
#undef WANT_TO_DEBUG_ASIO_PACKAGES

/* 
   ASIO Callback:
   - bufferSwith: 
   indicates that both input and output are to be porcessed.
     
   - sampleRateDidChange:
   indicats a sample rate change 
*/
ASIOCallbacks asioCallbacks;

fts_class_t* asio_audioport_type;

/* 
   it seems that ASIO allows only one driver processing ,
   so we need this global variable to know which driver we use 
*/
static asio_audioport_t* current_port = 0;
static int nb_port_opened = 0;

/*
  Needed ?
*/
unsigned long get_sys_reference_time();

/*
  Used to stop FTS scheduler
*/
#define PIPE_DEFAULT_SIZE 256
int pipe_handles[2];

// Utilities for buffer size alignment computation
int PGCD(int a, int b)
{
  return (b == 0) ? a : PGCD(b, a%b);
}

int PPCM(int a, int b)
{
  return (a*b)/PGCD(a,b);
}

/* int max(int a, int b)
{
  return a > b ? a : b;
}

int min(int a, int b)
{
  return a < b ? a : b;
}
*/
// compute number of frames needed for buffer alignment
int compute_frame_shift(int asio_buffer_size, int port_buffer_size)
{
  int res = 0;
  int i ;
  for (i = asio_buffer_size; i < PPCM(asio_buffer_size, port_buffer_size); i += asio_buffer_size)
  {
    res = max(res, i % port_buffer_size);
  }
  return res;
}

void copy_from_fts_to_asio(asio_audioport_t* port, int frames, int index)
{
  int i;
  /* copy output buffer to ASIO driver */
  for (i = 0; i < port->inputChannels + port->outputChannels; ++i)
  {
    if (port->bufferInfos[i].isInput == ASIOFalse)
    {
      int buffIndex = port->bufferInfos[i].channelNum;
      char* asio_output_buffer = (char*)port->bufferInfos[i].buffers[index];
      char* port_output_buffer = (char*)port->output_buffers[buffIndex];

      // move to offset
      asio_output_buffer += port->asio_output_buffer_offset * port->size_of_asio_type;
      port_output_buffer += port->output_buffer_offset * port->size_of_asio_type;
      /* copy buffsize sample to audio card buffer */
      memcpy(asio_output_buffer, port_output_buffer, frames * port->size_of_asio_type);
    }
  }
}

void copy_from_asio_to_fts(asio_audioport_t* port, int frames, int index)
{
  int i;
  /* copy output buffer to ASIO driver */
  for (i = 0; i < port->inputChannels + port->outputChannels; ++i)
  {
    if (port->bufferInfos[i].isInput == ASIOTrue)
    {
      int buffIndex = port->bufferInfos[i].channelNum;
      char* asio_input_buffer = (char*)port->bufferInfos[i].buffers[index];
      char* port_input_buffer = (char*)port->input_buffers[buffIndex];
      // move to offset
      asio_input_buffer += port->asio_input_buffer_offset * port->size_of_asio_type;
      port_input_buffer += port->input_buffer_offset * port->size_of_asio_type;

      /* copy buffsize sample to audio card buffer */
      memcpy(port_input_buffer, asio_input_buffer, frames * port->size_of_asio_type);
    }
  }

}


void output_fts_to_asio(asio_audioport_t* port, int frames_number, int index)
{
  if (frames_number > 0)
  {
    copy_from_fts_to_asio(port, frames_number, index);
    port->asio_output_buffer_offset += frames_number;
    port->output_buffer_offset += frames_number;
  }
}

// FROM ASIO SDK
//----------------------------------------------------------------------------------
// conversion from 64 bit ASIOSample/ASIOTimeStamp to double float
#if NATIVE_INT64
#define ASIO64toDouble(a)  (a)
#else
const double twoRaisedTo32 = 4294967296.;
#define ASIO64toDouble(a)  ((a).lo + (a).hi * twoRaisedTo32)
#endif

ASIOTime *bufferSwitchTimeInfo(ASIOTime *timeInfo, long index, ASIOBool processNow)
{	
  long frames_per_buffer = current_port->asio_buffer_size;
  long samples_per_tick = fts_dsp_get_tick_size();
  int i;
  long tmp;
  
  // number of frames available into the asio input buffer
  long frames_input_asio_buffer = frames_per_buffer;
  // number of frames needed to complete the port input buffer
  long frames_input_port_buffer;
  // number of frames needed to complete the asio output buffer
  long frames_output_asio_buffer;
  // number of frames available into the port output buffer
  long frames_output_port_buffer;
  
  // reinit asio input buffer offset  
  current_port->asio_input_buffer_offset = 0;

  // fill asio output with remaining frames in port output 
  frames_output_asio_buffer = frames_per_buffer - current_port->asio_output_buffer_offset;

  frames_output_port_buffer = current_port->inout_buffer_size - current_port->output_buffer_offset;

  tmp = min(frames_output_asio_buffer, frames_output_port_buffer);
  output_fts_to_asio(current_port, tmp, index);

  // remove consumed asio output frame
  frames_output_asio_buffer -= tmp;      

  while (frames_input_asio_buffer > 0)
  {
    // number of frame needed to fill port input buffer
    frames_input_port_buffer = current_port->inout_buffer_size - current_port->input_buffer_offset;
    if (frames_input_asio_buffer >= frames_input_port_buffer)
    {
      // read value from asio input buffer
      copy_from_asio_to_fts(current_port, frames_input_port_buffer, index);
      
      // run scheduler 
      fts_sched_run_one_tick();
      
      // port output buffer is full
      current_port->output_buffer_offset = 0;
      
      // port input buffer is empty
      current_port->input_buffer_offset = 0;
      
      // number of frame to write on sound card
      tmp = min(current_port->inout_buffer_size, frames_output_asio_buffer);

      // write value to asio output buffer
      output_fts_to_asio(current_port, tmp, index);

      // number of frame still available on asio output 
      frames_output_asio_buffer -= tmp;
      // number of frame stil available on asio input 
      frames_input_asio_buffer -= frames_input_port_buffer;      
      // offset for asio input buffer
      current_port->asio_input_buffer_offset += frames_input_port_buffer;
    }
    else
    {
      // read value from asio input buffer
      copy_from_asio_to_fts(current_port, frames_input_asio_buffer, index);
      // offset for port input 
      current_port->input_buffer_offset += frames_input_asio_buffer;
      // offset for asio input buffer
      current_port->asio_input_buffer_offset += frames_input_asio_buffer;
      // we have consumed all frames available for reading
      frames_input_asio_buffer = 0;
    }
  }

  // reinit asio output buffer offset
  current_port->asio_output_buffer_offset = 0;

  // finally if the driver supports the ASIOOutputReady() optimization, do it here, all data are in place
  if (current_port->postOutput)
  {
    current_port->driver->driver_interface->outputReady();
  }

  return 0L;
}

// FROM ASIO SDK
//----------------------------------------------------------------------------------
void bufferSwitch(long index, ASIOBool processNow)
{	
  // the actual processing callback.
  // Beware that this is normally in a seperate thread, hence be sure that you take care
  // about thread synchronization. This is omitted here for simplicity.
    
  // as this is a "back door" into the bufferSwitchTimeInfo a timeInfo needs to be created
  // though it will only set the timeInfo.samplePosition and timeInfo.systemTime fields and the according flags
  ASIOTime  timeInfo;
  memset (&timeInfo, 0, sizeof (timeInfo));
    
  // get the time stamp of the buffer, not necessary if no
  // synchronization to other media is required
  if(ASE_OK == current_port->driver->driver_interface->getSamplePosition(&timeInfo.timeInfo.samplePosition, &timeInfo.timeInfo.systemTime))
    timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
    
  bufferSwitchTimeInfo (&timeInfo, index, processNow);
}


// FROM ASIO SDK
//----------------------------------------------------------------------------------
void sampleRateChanged(ASIOSampleRate sRate)
{
  // do whatever you need to do if the sample rate changed
  // usually this only happens during external sync.
  // Audio processing is not stopped by the driver, actual sample rate
  // might not have even changed, maybe only the sample rate status of an
  // AES/EBU or S/PDIF digital input at the audio device.
  // You might have to update time/sample related conversion routines, etc.
}

// FROM ASIO SDK
//----------------------------------------------------------------------------------
long asioMessages(long selector, long value, void* message, double* opt)
{
  // currently the parameters "value", "message" and "opt" are not used.
  long ret = 0;
  switch(selector)
  {
  case kAsioSelectorSupported:
    if(value == kAsioResetRequest
       || value == kAsioEngineVersion
       || value == kAsioResyncRequest
       || value == kAsioLatenciesChanged
       // the following three were added for ASIO 2.0, you don't necessarily have to support them
       || value == kAsioSupportsTimeInfo
       || value == kAsioSupportsTimeCode
       || value == kAsioSupportsInputMonitor)
      ret = 1L;
    break;
  case kAsioResetRequest:
    // defer the task and perform the reset of the driver during the next "safe" situation
    // You cannot reset the driver right now, as this code is called from the driver.
    // Reset the driver is done by completely destruct is. I.e. ASIOStop(), ASIODisposeBuffers(), Destruction
    // Afterwards you initialize the driver again.
    current_port->stopped;  // In this sample the processing will just stop
    ret = 1L;
    break;
  case kAsioResyncRequest:
    // This informs the application, that the driver encountered some non fatal data loss.
    // It is used for synchronization purposes of different media.
    // Added mainly to work around the Win16Mutex problems in Windows 95/98 with the
    // Windows Multimedia system, which could loose data because the Mutex was hold too long
    // by another thread.
    // However a driver can issue it in other situations, too.
    ret = 1L;
    break;
  case kAsioLatenciesChanged:
    // This will inform the host application that the drivers were latencies changed.
    // Beware, it this does not mean that the buffer sizes have changed!
    // You might need to update internal delay data.
    ret = 1L;
    break;
  case kAsioEngineVersion:
    // return the supported ASIO version of the host application
    // If a host applications does not implement this selector, ASIO 1.0 is assumed
    // by the driver
    ret = 2L;
    break;
  case kAsioSupportsTimeInfo:
    // informs the driver wether the asioCallbacks.bufferSwitchTimeInfo() callback
    // is supported.
    // For compatibility with ASIO 1.0 drivers the host application should always support
    // the "old" bufferSwitch method, too.
    ret = 1;
    break;
  case kAsioSupportsTimeCode:
    // informs the driver wether application is interested in time code info.
    // If an application does not need to know about time code, the driver has less work
    // to do.
    ret = 0;
    break;
  }
  return ret;
}

// FROM ASIO SDK
//----------------------------------------------------------------------------------
unsigned long get_sys_reference_time()
{       
  // get the system reference time
  return timeGetTime();
}



ASIOError
asio_audioport_create_buffers(asio_audioport_t* port)
{
  ASIOBufferInfo* info;
  ASIOError result;
  int i;
  // Hack to use short .....
  int size_of_asio_type = 2;


  asioCallbacks.bufferSwitch = &bufferSwitch;
  asioCallbacks.sampleRateDidChange = &sampleRateChanged;
  asioCallbacks.asioMessage = &asioMessages;
  asioCallbacks.bufferSwitchTimeInfo = &bufferSwitchTimeInfo;

  port->bufferInfos = new ASIOBufferInfo[port->inputChannels + port->outputChannels];
  port->channelInfos = new ASIOChannelInfo[port->inputChannels + port->outputChannels];

  info = port->bufferInfos;
  /* prepare inputs */
  for (i = 0; i < port->inputChannels; i++, info++)
  {
    info->isInput = ASIOTrue;
    info->channelNum = i;
    info->buffers[0] = info->buffers[1] = 0;
  }
  /* prepare outputs */
  for (i = 0; i < port->outputChannels; i++, info++)
  {
    info->isInput = ASIOFalse;
    info->channelNum = i;
    info->buffers[0] = info->buffers[1] = 0;
  }
  /* create and activate buffers */
  result = 
    port->driver->driver_interface->createBuffers(port->bufferInfos, 
						  port->inputChannels + port->outputChannels,
						  port->preferredSize, &asioCallbacks);


  if (ASE_OK == result)
  {
    port->asio_buffer_size = port->preferredSize;
    /* allocate memory for fts buffers */
    port->input_buffers = (void**)fts_malloc(port->inputChannels * sizeof(void*));      
    port->output_buffers = (void**)fts_malloc(port->outputChannels * sizeof(void*));      

    for (i = 0; i < port->inputChannels + port->outputChannels; i++)
    {
      port->channelInfos[i].channel = port->bufferInfos[i].channelNum;
      port->channelInfos[i].isInput = port->bufferInfos[i].isInput;
      result = port->driver->driver_interface->getChannelInfo(&port->channelInfos[i]);	  
      if (ASE_OK != result)
      {
	break;
      }
#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
      if (port->channelInfos[i].isInput == ASIOTrue)
	{
	  if (port->channelInfos[i].isActive == ASIOTrue)
	    {
	      fts_post("Input Channel %d is active\n", port->bufferInfos[i].channelNum);
	    }
	  else
	    {
	      fts_post("Input Channel %d is not active\n", port->bufferInfos[i].channelNum);
	    }
	}
      else
	{
	  if (port->channelInfos[i].isActive == ASIOTrue)
	    {
	      fts_post("Output Channel %d is active\n", port->bufferInfos[i].channelNum);
	    }
	  else
	    {
	      fts_post("Output Channel %d is not active\n", port->bufferInfos[i].channelNum);
	    }
	}
#endif // WANT_TO_DEBUG_ASIO_PACKAGES
    }

    int dsp_tick_size = fts_dsp_get_tick_size();
    int j;

    for (i = 0; i < port->inputChannels; ++i)
    {
      /* create fts buffers */
      port->input_buffers[i] = (void*)fts_malloc(dsp_tick_size * size_of_asio_type);
      /* clear value */
      port->input_buffers[i] = memset(port->input_buffers[i], 0, dsp_tick_size * size_of_asio_type);
    }
    for (i = 0; i < port->outputChannels; ++i)
    {
      /* create fts buffers */
      port->output_buffers[i] = (void*)fts_malloc(dsp_tick_size * size_of_asio_type);
      /* clear value */
      port->output_buffers[i] = memset(port->output_buffers[i], 0, dsp_tick_size * size_of_asio_type);
    }


    port->inout_buffer_size = dsp_tick_size;
    port->size_of_asio_type = size_of_asio_type;

    int frame_shift = compute_frame_shift(port->asio_buffer_size, port->inout_buffer_size);

#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
    fts_post("Frame shift : %d\n", frame_shift);
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */
    if (port->inout_buffer_size <= port->asio_buffer_size)
    {
      port->asio_input_buffer_offset = 0;
      port->asio_output_buffer_offset = frame_shift;
      port->input_buffer_offset = 0;
      port->output_buffer_offset = port->inout_buffer_size;
    }
    else
    {
      port->asio_input_buffer_offset = 0;
      port->asio_output_buffer_offset = 0;
      port->input_buffer_offset = frame_shift;
      port->output_buffer_offset = port->inout_buffer_size;
    }
  }

  return result;    
}




static void
asio_audioport_input(fts_audioport_t* port, float** buffers, int buffsize)
{
  // WE CAN ONLY HAVE ONE ASIO DRIVER AT THE SAME TIME
  // So we use current_port instead of port 
  int i, j;
  int nbAsioChannels = current_port->inputChannels + current_port->outputChannels;

  for (i = 0; i <nbAsioChannels; ++i)
  {
    if (current_port->bufferInfos[i].isInput == ASIOFalse)
    {      
      // ASIO -> jMax type conversion 
      switch (current_port->channelInfos[0].type)
      {
      case ASIOSTInt16LSB:
	// get FTS channel index
	int channelIndex = current_port->bufferInfos[i].channelNum;
	// get corret fts buffer 
	float* buff = buffers[channelIndex];
	// get asio buffer
	short* input_buffers = (short*)current_port->input_buffers[channelIndex];
	for (j = 0; j < buffsize; ++j)
	{
	  // convert samples
	  buff[j] = (float)(input_buffers[j]) / 32767.0f;
	}
	break;
#if 0
      case ASIOSTInt24LSB: // not implemented
	break;	
      case ASIOSTInt32LSB: // not implemented
	break;	
#endif
      }
    }
  }
}

static void
asio_audioport_output(fts_audioport_t* port, float** buffers, int buffsize)
{
  // WE CAN ONLY HAVE ONE ASIO DRIVER AT THE SAME TIME
  // So we use current_port instead of port 
  int i, j;
  int nbAsioChannels = current_port->inputChannels + current_port->outputChannels;

  for (i = 0; i <nbAsioChannels; ++i)
  {
    if (current_port->bufferInfos[i].isInput == ASIOFalse)
    {      
      // ASIO -> jMax type conversion 
      switch (current_port->channelInfos[0].type)
      {
      case ASIOSTInt16LSB:
	// get FTS channel index
	int channelIndex = current_port->bufferInfos[i].channelNum;
	// get corret fts buffer 
	float* buff = buffers[channelIndex];
	// get asio buffer
	short* output_buffers = (short*)current_port->output_buffers[channelIndex];
	if (fts_audioport_is_channel_used((fts_audioport_t*)current_port, FTS_AUDIO_OUTPUT, channelIndex))
	  {
	    for (j = 0; j < buffsize; ++j)
	      {
		// convert samples 
		output_buffers[j] = (short)(buff[j] * 32767.0f);
	      }
	  }
	else
	  {
	    for (j = 0; j < buffsize; ++j)
	      {
		output_buffers[j] = 0;
	      }
	  }
	break;
#if 0
      case ASIOSTInt24LSB: // not implemented
	break;	
      case ASIOSTInt32LSB: // not implemented
	break;	
#endif
      }
    }
  }
}

static void asio_restart_fts_scheduler()
{
  // write in named pipe
  int val = 0;
  fts_post("[asio] Restart FTS scheduler \n");
  fts_log("[asio] Restart FTS scheduler \n");
  write(pipe_handles[1], &val, sizeof(int));
}

static void asio_stop_fts_scheduler()
{
  // Create pipe
  if (0 != _pipe(pipe_handles, PIPE_DEFAULT_SIZE, O_BINARY))
    {
      fts_log("[asio] Cannot create a pipe for stopping FTS scheduler\n");
      fts_post("[asio] Cannot create pipe\n");
    }
  else
    {
      int val;
      fts_post("[asio] FTS scheduler stopped \n");
      fts_log("[asio] FTS scheduler stopped \n");
      read(pipe_handles[0], &val, sizeof(int));
      fts_post("[asio] FTS scheduler restarted \n");
      fts_log("[asio] FTS scheduler restarted \n");
    }
  
}

static int
asio_audioport_open(asio_audioport_t* port, int input_or_output)
{
  int result = 0;

  if (nb_port_opened == 0)
    {
      // set current port
      current_port = port;
      nb_port_opened++;
      // set port open
      fts_audioport_set_open((fts_audioport_t*)port, input_or_output);
      // start ASIO callback
      if (!current_port->driver->isStarted)
	{
	  fts_post("Start ASIO callback \n");
	  current_port->driver->driver_interface->start();
	  result = 1;
	}

      // stop fts scheduler 
      asio_stop_fts_scheduler();
    }
  else
    {
      fts_post("ASIO allows only to use one driver (%s)\n", current_port->driver->name);
      // set port open
      fts_audioport_set_open((fts_audioport_t*)port, input_or_output);
      nb_port_opened++;
    }
#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
  fts_post("[asio] open (%d)\n", nb_port_opened);
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */
  return result;
}

static void
asio_audioport_close()
{
  int result = 0;
  if (nb_port_opened == 1)
    {
      // stop ASIO callback
      current_port->driver->driver_interface->stop();
      fts_post("ASIO Callback stop\n");
      // restart fts scheduler
      asio_restart_fts_scheduler();
      fts_post("FTS scheduler restarted\n");      
      nb_port_opened = 0;
    }
  else
    {
      nb_port_opened--;
    }
#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
  fts_post("[asio] close (%d)\n", nb_port_opened);
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */
}

static void 
asio_audioport_open_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  /* ASIO support only one callback, so we don't want to open other ports */
  /* set current_port to opened port */
  asio_audioport_t* port = (asio_audioport_t*)o;
  asio_audioport_open(port, FTS_AUDIO_INPUT);
}

static void 
asio_audioport_open_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  /* ASIO support only one callback, so we don't want to open other ports */
  /* set current_port to opened port */
  asio_audioport_t* port = (asio_audioport_t*)o;
  asio_audioport_open(port, FTS_AUDIO_OUTPUT);
}

static void
asio_audioport_close_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioport_unset_open((fts_audioport_t*)o, FTS_AUDIO_INPUT);
  // dont forget to restart FTS scheduler 
  asio_audioport_close();
}

static void
asio_audioport_close_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioport_unset_open((fts_audioport_t*)o, FTS_AUDIO_OUTPUT);
  // dont forget to restart FTS scheduler  
  asio_audioport_close();
}

static void
asio_audioport_sched_listener(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  asio_audioport_t* self = (asio_audioport_t*)o;
  
  if (!fts_sched_is_running())
  {
    /* stop process ... */
    self->driver->driver_interface->stop();

    /* dispose buffer ... */
    self->driver->driver_interface->disposeBuffers();
      
    /* exit asio ... */
    self->driver->driver_interface->Release();
  }
}

static void 
asio_audioport_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  ASIODriverInfo driverInfo;
  ASIOSampleRate sampleRate;

  asio_audioport_t* self = (asio_audioport_t*)o;
  asio_driver_t* driver;
  fts_audioport_init((fts_audioport_t*)self);
  

  driver = (asio_driver_t*)fts_get_pointer(at);
  self->driver = driver;
  self->driver->isStarted = false;

  /* initialize driver */
  driver->driver_interface->init(driverInfo.sysRef);
  
  /* set number of channels */
  driver->driver_interface->getChannels(&self->inputChannels, &self->outputChannels);
#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
  fts_post("Inputs Channels: %d \n"
       "Outputs Channels: %d \n"
       , self->inputChannels, self->outputChannels);
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */

  /* set buffer size */
  driver->driver_interface->getBufferSize(&self->minSize, &self->maxSize, &self->preferredSize, &self->granularity);
#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
  fts_post("Min Buffer Size: %d\n"
       "Max Buffer Size: %d\n"
       "Preferred Buffer Size: %d\n"
       "Buffer Size Granularity: %d\n",
       self->minSize, self->maxSize, self->preferredSize, self->granularity);
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */

  /* set current sample rate */
  driver->driver_interface->getSampleRate(&sampleRate);
  self->sampleRate = (double)sampleRate;
#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
  fts_post("Current SampleRate : %f\n", self->sampleRate);
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */
  if (ASE_OK == driver->driver_interface->outputReady())
  {
    self->postOutput = true;
  }
  else
  {
    self->postOutput = false;
  }
#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
  fts_post("ASIOOuputReady() - %s \n", self->postOutput ? "Supported" : "Not Supported");
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */
  /* create buffers */
  asio_audioport_create_buffers(self);

  /* set for playback */
  if (0 < self->inputChannels)
  {
    fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_INPUT);
    fts_audioport_set_channels((fts_audioport_t*)self, FTS_AUDIO_INPUT, self->inputChannels);
    fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, asio_audioport_input);
  }
  /* set for capture */
  if (0 < self->outputChannels)
  {
    fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);
    fts_audioport_set_channels((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, self->outputChannels);
    fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, asio_audioport_output);
  }
  
  fts_sched_running_add_listener(o, asio_audioport_sched_listener);
}


static void 
asio_audioport_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  asio_audioport_t* self = (asio_audioport_t*)o;

  fts_sched_running_remove_listener(o);

  fts_audioport_delete((fts_audioport_t*)self);

  // release ASIO buffers 
  self->driver->driver_interface->disposeBuffers();

  // exit ASIO drivers
  self->driver->driver_interface->Release();
  self->driver->asiodrv = 0;
}

static void asio_audioport_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(asio_audioport_t), asio_audioport_init, asio_audioport_delete);
  
  fts_class_message_varargs(cl, fts_s_open_input, asio_audioport_open_input); 
  fts_class_message_varargs(cl, fts_s_open_output, asio_audioport_open_output);

  fts_class_message_varargs(cl, fts_s_close_input, asio_audioport_close_input);
  fts_class_message_varargs(cl, fts_s_close_output, asio_audioport_close_output);
}

extern "C"
{
  void asio_audioport_config()
  {
    fts_symbol_t s = fts_new_symbol("asio_audioport");
    asio_audioport_type = fts_class_install(s, asio_audioport_instantiate);
  }
}


/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset:2
 * End:
 */
