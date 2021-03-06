/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
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
#include "sample_conversions.h"


/* Define this if you want to post debug messages */
/* #define WANT_TO_DEBUG_ASIO_PACKAGES */

/* 
   ASIO Callback:
   - bufferSwith: 
   indicates that both input and output are to be porcessed.
     
   - sampleRateDidChange:
   indicates a sample rate change 
*/
static ASIOCallbacks asioCallbacks;

fts_class_t* asio_audioport_type;

/* 
   ASIO allows only one driver to be active at a time ,
   so we need this global variable to know which driver we use 
*/
static asio_audioport_t* current_port = NULL;
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

/* Utilities for buffer size alignment computation */
int PGCD(int a, int b)
{
  return (b == 0) ? a : PGCD(b, a%b);
}

int PPCM(int a, int b)
{
  return (a*b)/PGCD(a,b);
}

/* compute number of frames needed for buffer alignment */
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

      /* move to offset */
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

  // finally if the driver supports the ASIOOutputReady() optimization, do it here, all data is in place
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

  fts_dsp_set_sample_rate((double)sRate);
  current_port->sampleRate = sRate;
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
  int size_of_asio_type = 2;

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
  result = port->driver->driver_interface->createBuffers(port->bufferInfos, 
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

    switch(port->channelInfos[0].type)
    {
          case ASIOSTInt16LSB :
          case ASIOSTInt16MSB : size_of_asio_type = 2; break;
          case ASIOSTInt24MSB :
          case ASIOSTInt24LSB : size_of_asio_type = 3; break;
          case ASIOSTInt32MSB :
          case ASIOSTFloat32MSB :
	        case ASIOSTInt32MSB16 :
	        case ASIOSTInt32MSB18 :
	        case ASIOSTInt32MSB20 :
	        case ASIOSTInt32MSB24 :
          case ASIOSTInt32LSB :
          case ASIOSTFloat32LSB :
	        case ASIOSTInt32LSB16 :
          case ASIOSTInt32LSB18 :
          case ASIOSTInt32LSB20 :
	        case ASIOSTInt32LSB24 : size_of_asio_type = 4; break;
          case ASIOSTFloat64MSB :
          case ASIOSTFloat64LSB : size_of_asio_type = 8; break;
    }

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
    fts_post("Data type : %d\n",port->channelInfos[0].type);
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
  int i;
  int nbAsioChannels = current_port->inputChannels + current_port->outputChannels;

  for (i = 0; i <nbAsioChannels; ++i)
  {
    if (current_port->bufferInfos[i].isInput == ASIOTrue)
    {      
	      int channelIndex = current_port->bufferInfos[i].channelNum; // get FTS channel index
	      float* buff = buffers[channelIndex]; // get correct fts buffer 

        // ASIO -> jMax type conversion 
        switch (current_port->channelInfos[0].type)
        {

          case ASIOSTInt16LSB:
            fts_convert_int16_to_float32(buffsize,(short*)current_port->input_buffers[channelIndex],buff);
    	      break;

          case ASIOSTInt24LSB: 
            fts_convert_int24P_to_float32(buffsize,(char*)current_port->input_buffers[channelIndex],buff);
            break;	

          case ASIOSTInt32LSB: 
            fts_convert_int32_to_float32(buffsize,(long*)current_port->input_buffers[channelIndex],buff);
            break;	

          case ASIOSTInt32LSB24:
            fts_convert_int24R_to_float32(buffsize,(long*)current_port->input_buffers[channelIndex],buff);
            break;	

          case ASIOSTInt32MSB24:
            fts_convert_int24L_to_float32(buffsize,(long*)current_port->input_buffers[channelIndex],buff);
            break;	

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

  for(i = 0; i <nbAsioChannels; ++i)
  {
    if(current_port->bufferInfos[i].isInput == ASIOFalse)
    {      
      int channelIndex = current_port->bufferInfos[i].channelNum; // get FTS channel index
      float* buff = buffers[channelIndex]; // get correct fts buffer 

      if(fts_audioport_is_channel_used((fts_audioport_t*)current_port, FTS_AUDIO_OUTPUT, channelIndex))
      {
          //fts_post("Type %d Channel Index: %d\n",current_port->channelInfos[0].type,channelIndex);

        // ASIO -> jMax type conversion 
        switch (current_port->channelInfos[0].type)
        {
          case ASIOSTInt16LSB:
            fts_convert_float32_to_int16(buffsize,buff,(short*)current_port->output_buffers[channelIndex]);
    	      break;

          case ASIOSTInt24LSB: 
          case ASIOSTInt24MSB: 

            fts_convert_float32_to_int24P(buffsize,buff,(char*)current_port->output_buffers[channelIndex]);
    	      break;

          case ASIOSTInt32LSB:
            fts_convert_float32_to_int32(buffsize,buff,(long*)current_port->output_buffers[channelIndex]);
    	      break;

          case ASIOSTInt32LSB24:
            fts_convert_float32_to_int24R(buffsize,buff,(long*)current_port->output_buffers[channelIndex]);
    	      break;

          case ASIOSTInt32MSB24:
            fts_convert_float32_to_int24L(buffsize,buff,(long*)current_port->output_buffers[channelIndex]);
    	      break;
	      }
      }
      else /* output silence if channel is not used */
	    {
        switch (current_port->channelInfos[0].type)
        {
          case ASIOSTInt16LSB:
	          for (j = 0; j < buffsize; ++j)
              *((short*)current_port->output_buffers[channelIndex]+j) = 0;
    	      break;

          case ASIOSTInt24LSB: 
	          for (j = 0; j < buffsize * 3; ++j)
              *((char*)current_port->output_buffers[channelIndex]+j) = 0;
    	      break;

          case ASIOSTInt32LSB:
          case ASIOSTInt32LSB24:
          case ASIOSTInt32MSB24:
	          for (j = 0; j < buffsize; ++j)
              *((long*)current_port->output_buffers[channelIndex]+j) = 0;
    	      break;
	      }
	    }
    }    
  }
}




static void asio_audioport_halt(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  /* 
     This is the most complex part of the ASIO code :          
 
     When the port is opened, it adds this method to the FTS scheduler.
     When we are called by the FTS code, we block on the following read() 
     call, so the FTS thread is in conditional wait state.

     Meanwhile, we have started the ASIO driver, and the driver code begins
     to call our BufferSwitch method, from an interrupt or from another
     thread. From this point, this is the ASIO callback which manually drives
     the FTS scheduler and the DSP chain.

     When the port is closed, a write() call is made from the ASIO context,
     which releases the FTS thread. We then stop the ASIO driver from here so 
     the callback is not called anymore, and we return to the FTS scheduler 
     code, which continues its infinite loop (Run allways).
  */

  int val;

  /* Remove object from FTS scheduler, so we are not called again when the */
  /* FTS scheduler wakes up after the blocking read()                      */
  fts_sched_remove(o);

  /* This is the blocking read() */
  fts_post("[asio] FTS scheduler stopped \n");
  fts_log("[asio] FTS scheduler stopped \n");
  read(pipe_handles[0], &val, sizeof(int));
 
  /* When we reach this point, the ASIO port is being closed */ 
  fts_post("[asio] FTS scheduler restarted \n");
  fts_log("[asio] FTS scheduler restarted \n");

  /* Stop the ASIO driver */
  current_port->driver->driver_interface->stop();
  fts_post("[asio] driver callback stoped\n");

  /* Release ASIO buffers */
  current_port->driver->driver_interface->disposeBuffers();

  /* We now return the hand to the FTS scheduler loop */

  current_port = NULL;
}



static void asio_restart_fts_scheduler()
{
  /* write in named pipe */
  int val = 0;
  fts_post("[asio] Restarting FTS scheduler \n");
  fts_log("[asio] Restarting FTS scheduler \n");
  write(pipe_handles[1], &val, sizeof(int));
}



/* this method is called when the scheduler quits. */
static void
asio_audioport_sched_listener(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  asio_audioport_t* self = (asio_audioport_t*)o;
  
  if(!fts_sched_is_running())
  {
    fts_log("[asio] sched not running, stop, dispose and release\n");

    /* force close the audioport */
    if(self)
    {
      fts_audioport_unset_open((fts_audioport_t*)self, FTS_AUDIO_INPUT);
      fts_audioport_unset_open((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);
    }

    asio_restart_fts_scheduler();
  }
}



static int
asio_audioport_open(asio_audioport_t* port, int input_or_output)
{

  if(nb_port_opened==0)
  {
    /* set current port pointer (only one open port at a given time) */
    current_port = port;
    
    /* set port open */
    fts_audioport_set_open((fts_audioport_t*)current_port, input_or_output);

    /* create buffers */
    if(asio_audioport_create_buffers(current_port)!=ASE_OK)
    {
      fts_post("[asio] Error : could not prepare audio buffers\n");

      current_port = NULL;
      return 0; /* error */
    }

    fts_sched_running_add_listener((fts_object_t*)current_port, asio_audioport_sched_listener);

    /* stop fts scheduler */
    fts_sched_add((fts_object_t*)current_port,FTS_SCHED_ALWAYS);

    /* start ASIO callback */
    if(!current_port->driver->isStarted)
    {
        fts_post("[asio] starting driver callback\n");

        fts_dsp_set_sample_rate((double)current_port->sampleRate);

        current_port->driver->driver_interface->start();
    }

    nb_port_opened++;
  }
  else if((current_port)&&(port!=current_port)) /* user is trying to open a different port */
  {
     fts_post("ASIO allows to use only one driver at a time\n");
     fts_post("Current driver is : %s\n",current_port->driver->name);
     nb_port_opened++;

     return 0; /* error */
  }
  else /* user is trying to open another channel of the current port */
  {
     /* set port open */
     fts_audioport_set_open((fts_audioport_t*)port, input_or_output);
     nb_port_opened++;
  }

#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
  fts_post("[asio] open (%d)\n", nb_port_opened);
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */

  return 1; /* success */
}



static void asio_audioport_close()
{
  int result = 0;

  if(nb_port_opened==1)
  {
    fts_post("[asio] effective port close\n");

    /* restart fts scheduler. when the FTS thread wakes up, it stops */
    /* the ASIO driver and disposes the audio buffers                */
    asio_restart_fts_scheduler();
    nb_port_opened = 0;


    fts_sched_running_remove_listener((fts_object_t*)current_port);
  }
  else
  {
      nb_port_opened--;

      if(nb_port_opened<0) nb_port_opened = 0;
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
  asio_audioport_close();
}



static void
asio_audioport_close_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioport_unset_open((fts_audioport_t*)o, FTS_AUDIO_OUTPUT);
  asio_audioport_close();
}




static void 
asio_audioport_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  /* 
     ASIODriverInfo driverInfo;
     driverInfo.sysRef = TODO: get main client window handle !!
  */ 
  ASIOSampleRate sampleRate;

  asio_audioport_t* self = (asio_audioport_t*)o;
  fts_audioport_t* base  = (fts_audioport_t*)self;
  fts_audioport_init(base);
  
  self->driver = (asio_driver_t*)fts_get_pointer(at);
  self->driver->isStarted = false;

  /* initialize driver interface */
  self->driver->driver_interface->init(NULL); /* TODO: ->init(driverInfo.sysRef);
  
  /* retrieve the number of i/o channels */
  self->driver->driver_interface->getChannels(&self->inputChannels, &self->outputChannels);

  /* retrieve the min max and preferred driver buffer sizes */
  self->driver->driver_interface->getBufferSize(&self->minSize, &self->maxSize, &self->preferredSize, &self->granularity);

  /* retrieve the driver's current sampling rate */
  self->driver->driver_interface->getSampleRate(&sampleRate);
  self->sampleRate = (double)sampleRate;

  /* check the state of the OutputReady optimization flag */
  if(ASE_OK == self->driver->driver_interface->outputReady())
    self->postOutput = true;
  else
    self->postOutput = false;

  /* set for capture */
  if (0 < self->inputChannels)
  {
    fts_audioport_set_valid(base, FTS_AUDIO_INPUT);
    fts_audioport_set_channels(base, FTS_AUDIO_INPUT, self->inputChannels);
    fts_audioport_set_io_fun(base, FTS_AUDIO_INPUT, asio_audioport_input);
  }
  /* set for playback */
  if (0 < self->outputChannels)
  {
    fts_audioport_set_valid(base, FTS_AUDIO_OUTPUT);
    fts_audioport_set_channels(base, FTS_AUDIO_OUTPUT, self->outputChannels);
    fts_audioport_set_io_fun(base, FTS_AUDIO_OUTPUT, asio_audioport_output);
  }
  

#ifdef WANT_TO_DEBUG_ASIO_PACKAGES
  fts_post("[asio] Debug - Initializing audio port\n");
  fts_post("Inputs Channels: %d \nOutputs Channels: %d \n",self->inputChannels,self->outputChannels);
  fts_post("Min Buffer Size: %d\n",self->minSize);
  fts_post("Max Buffer Size: %d\n",self->maxSize);
  fts_post("Preferred Buffer Size: %d\n",self->preferredSize);
  fts_post("Buffer Size Granularity: %d\n",self->granularity);
  fts_post("Current SampleRate : %f\n", self->sampleRate);
  fts_post("ASIOOuputReady() - %s \n", self->postOutput ? "Supported" : "Not Supported");
#endif /* WANT_TO_DEBUG_ASIO_PACKAGES */
}



static void 
asio_audioport_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  asio_audioport_t* self = (asio_audioport_t*)o;

  asio_audioport_close();

  /* release the driver interface */
  self->driver->driver_interface->Release();
  self->driver->driver_interface = NULL;
  self->driver->asiodrv = NULL;

  fts_audioport_delete((fts_audioport_t*)self);

  fts_log("[asio] audioport_delete\n");
}



static void asio_audioport_instantiate(fts_class_t* cl)
{
  /* Initialize class and register message handlers */
  fts_class_init(cl, sizeof(asio_audioport_t), asio_audioport_init, asio_audioport_delete);
  
  fts_class_message_varargs(cl, fts_s_open_input, asio_audioport_open_input); 
  fts_class_message_varargs(cl, fts_s_open_output, asio_audioport_open_output);

  fts_class_message_varargs(cl, fts_s_close_input, asio_audioport_close_input);
  fts_class_message_varargs(cl, fts_s_close_output, asio_audioport_close_output);

  fts_class_message_varargs(cl, fts_s_sched_ready, asio_audioport_halt);
}



extern "C"
{
  void asio_audioport_at_exit(void)
  {
    /* Ensure clean release of the pipes */
    close(pipe_handles[0]);
    close(pipe_handles[1]);

    /* Release the COM library */
    CoUninitialize();
  }

  void asio_audioport_config()
  {
    fts_symbol_t s = fts_new_symbol("asio_audioport");
    asio_audioport_type = fts_class_install(s, asio_audioport_instantiate);

    /* initialize the COM library */
    CoInitialize(0);

    /* Let the asioCallbacks structure point to our static methods */
    asioCallbacks.bufferSwitch         = &bufferSwitch;
    asioCallbacks.sampleRateDidChange  = &sampleRateChanged;
    asioCallbacks.asioMessage          = &asioMessages;
    asioCallbacks.bufferSwitchTimeInfo = &bufferSwitchTimeInfo;

    /* Create the communication pipe (bidirectional) */
    if(0 != _pipe(pipe_handles, PIPE_DEFAULT_SIZE, O_BINARY))
    {
        fts_log("[asio] Cannot create communication pipe with FTS scheduler\n");
        fts_post("[asio] Error : Cannot create pipe\n");
        return; /* error */
    }

    /* Register exit method */
    atexit(asio_audioport_at_exit);
  }
}


/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset:2
 * End:
 */


