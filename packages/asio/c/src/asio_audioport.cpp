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
#include "jmax_asio_port.h"

/* 
   ASIO Callback:
   - bufferSwith: 
     indicates that both input and output are to be porcessed.
     
   - sampleRateDidChange:
     indicats a sample rate change 
*/
fts_class_t* asio_audioport_type;
ASIOCallbacks asioCallbacks;

asio_audioport_t* current_port = 0;

unsigned long get_sys_reference_time();

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
  
  // store the timeInfo for later use
  current_port->tInfo = *timeInfo;
  post("[asio] bufferSwitchTimeInfo \n");
  // get the time stamp of the buffer, not necessary if no
  // synchronization to other media is required
  if (timeInfo->timeInfo.flags & kSystemTimeValid)
    current_port->nanoSeconds = ASIO64toDouble(timeInfo->timeInfo.systemTime);
  else
    current_port->nanoSeconds = 0;
  
  if (timeInfo->timeInfo.flags & kSamplePositionValid)
    current_port->samples = ASIO64toDouble(timeInfo->timeInfo.samplePosition);
  else
    current_port->samples = 0;
  
  if (timeInfo->timeCode.flags & kTcValid)
    current_port->tcSamples = ASIO64toDouble(timeInfo->timeCode.timeCodeSamples);
  else
    current_port->tcSamples = 0;
  
  // get the system reference time
  current_port->sysRefTime = get_sys_reference_time();
  
#if WINDOWS && _DEBUG
  // a few debug messages for the Windows device driver developer
  // tells you the time when driver got its interrupt and the delay until the app receives
  // the event notification.
  static double last_samples = 0;
  char tmp[128];
  sprintf (tmp, "diff: %d / %d ms / %d ms / %d samples                 \n", current_port->sysRefTime - (long)(current_port->nanoSeconds / 1000000.0), current_port->sysRefTime, (long)(current_port->nanoSeconds / 1000000.0), (long)(current_port->samples - last_samples));
  OutputDebugString (tmp);
  last_samples = current_port->samples;
#endif

  // reset sample count 
  current_port->processedSamples = 0;

  // buffer size in samples
  long buffSize = current_port->preferredSize;
  current_port->currentIndex = index;
  long n;
  long samples_per_tick = fts_dsp_get_tick_size();
  for (n = 0; n < buffSize; n += samples_per_tick)
  {
      // run fts scheduler 
      fts_sched_run_one_tick();
      current_port->processedSamples += samples_per_tick;
  }

#if 0
  // perform the processing
  for (int i = 0; i < current_port->inputChannels + current_port->outputChannels; i++)
  {
      if (current_port->bufferInfos[i].isInput == false)
      {
	  // OK do processing for the outputs only
	  switch (current_port->channelInfos[i].type)
	  {
	  case ASIOSTInt16LSB:
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 2);
	      break;
	  case ASIOSTInt24LSB:		// used for 20 bits as well
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 3);
	      break;
	  case ASIOSTInt32LSB:
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 4);
	      break;
	  case ASIOSTFloat32LSB:		// IEEE 754 32 bit float, as found on Intel x86 architecture
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 4);
	      break;
	  case ASIOSTFloat64LSB: 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 8);
	      break;
	      
	      // these are used for 32 bit data buffer, with different alignment of the data inside
	      // 32 bit PCI bus systems can more easily used with these
	  case ASIOSTInt32LSB16:		// 32 bit data with 18 bit alignment
	  case ASIOSTInt32LSB18:		// 32 bit data with 18 bit alignment
	  case ASIOSTInt32LSB20:		// 32 bit data with 20 bit alignment
	  case ASIOSTInt32LSB24:		// 32 bit data with 24 bit alignment
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 4);
	      break;
	      
	  case ASIOSTInt16MSB:
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 2);
	      break;
	  case ASIOSTInt24MSB:		// used for 20 bits as well
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 3);
	      break;
	  case ASIOSTInt32MSB:
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 4);
	      break;
	  case ASIOSTFloat32MSB:		// IEEE 754 32 bit float, as found on Intel x86 architecture
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 4);
	      break;
	  case ASIOSTFloat64MSB: 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 8);
	      break;
	      
	      // these are used for 32 bit data buffer, with different alignment of the data inside
	      // 32 bit PCI bus systems can more easily used with these
	  case ASIOSTInt32MSB16:		// 32 bit data with 18 bit alignment
	  case ASIOSTInt32MSB18:		// 32 bit data with 18 bit alignment
	  case ASIOSTInt32MSB20:		// 32 bit data with 20 bit alignment
	  case ASIOSTInt32MSB24:		// 32 bit data with 24 bit alignment
	      memset (current_port->bufferInfos[i].buffers[index], 0, buffSize * 4);
	      break;
	  }
      }
  }
#endif

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
	for (i = 0; i < port->inputChannels + port->outputChannels; i++)
	{
	    port->channelInfos[i].channel = port->bufferInfos[i].channelNum;
	    port->channelInfos[i].isInput = port->bufferInfos[i].isInput;
	    result = port->driver->driver_interface->getChannelInfo(&port->channelInfos[i]);
	    if (ASE_OK != result)
	    {
		break;
	    }
	}
	
    }
    return result;    
}




static void
asio_audioport_input(fts_audioport_t* port, float** buffers, int buffsize)
{
    asio_audioport_t* driver = (asio_audioport_t*)port;
    // get value from driver
    //    post("asio_audioport_input\n");
#if 0 
    switch (driver->channelInfos[0].type)
    {
    case ASIOSTInt16LSB:
	// post("[sample type: ] ASIOSTInt16LSB\n");
	// ASIO -> jMax type conversion 
	int i, j;
	int processedSamples = current_port->processedSamples;
	int nbAsioChannels = current_port->inputChannels + current_port->outputChannels;
	int index = current_port->currentIndex;
	for (i = 0; i < nbAsioChannels; ++i)
	{
	    if (current_port->bufferInfos[i].isInput == true)
		{
		for (j = 0; j < buffsize; ++j)
		{
		// convert samples 
		    buffers[i][j] = ((int)current_port->bufferInfos[i].buffers[index]) / 32767.0f;
		}
	    }
	}



	break;
 
      }
#endif 
}

static void
asio_audioport_output(fts_audioport_t* port, float** buffers, int buffsize)
{
    asio_audioport_t* driver = (asio_audioport_t*)port;
    // send value to driver
    post("asio_audioport_output\n");
}

static void 
asio_audioport_open_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  /* ASIO support only one callback, so we don't want to open other ports */
  /* set current_port to opened port */
    current_port = (asio_audioport_t*)o;

    fts_audioport_set_open((fts_audioport_t*)o, FTS_AUDIO_INPUT);
    /* before starting ASIO processing we need to stop FTS scheduler .... */

  /* start ASIO processing */
    current_port->driver->driver_interface->start();

	Sleep(10000);
}

static void 
asio_audioport_open_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  /* ASIO support only one callback, so we don't want to open other ports */
  /* set current_port to opened port */
  post("[asioa audioport] open output \n");
    current_port = (asio_audioport_t*)o;

    fts_audioport_set_open((fts_audioport_t*)o, FTS_AUDIO_OUTPUT);
    /* before starting ASIO processing, we need to stop FTS scheduler */
  /* start ASIO processing */
    current_port->driver->driver_interface->start();

}

static void
asio_audioport_close_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioport_unset_open((fts_audioport_t*)o, FTS_AUDIO_INPUT);
  current_port->driver->driver_interface->stop();
  post("[asio] input closed \n");
}

static void
asio_audioport_close_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{

    /* before stop ASIO processing, we need to restart FTS scheduler */
    fts_audioport_unset_open((fts_audioport_t*)o, FTS_AUDIO_OUTPUT);
    current_port->driver->driver_interface->stop();
}

static void 
asio_audioport_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  ASIODriverInfo driverInfo;
  ASIOSampleRate sampleRate;

  asio_audioport_t* self = (asio_audioport_t*)o;
  asio_driver_t* driver;
  fts_audioport_init((fts_audioport_t*)self);
  
  self->processedSamples = 0;

  driver = (asio_driver_t*)fts_get_pointer(at);
  self->driver = driver;

  /* initialize driver */
  driver->driver_interface->init(driverInfo.sysRef);
  
  /* set number of channels */
  driver->driver_interface->getChannels(&self->inputChannels, &self->outputChannels);
  post("Inputs Channels: %d \n"
       "Outputs Channels: %d \n"
       , self->inputChannels, self->outputChannels);

  /* set buffer size */
  driver->driver_interface->getBufferSize(&self->minSize, &self->maxSize, &self->preferredSize, &self->granularity);
  post("Min Buffer Size: %d\n"
       "Max Buffer Size: %d\n"
       "Preferred Buffer Size: %d\n"
       "Buffer Size Granularity: %d\n",
       self->minSize, self->maxSize, self->preferredSize, self->granularity);

  /* set current sample rate */
  driver->driver_interface->getSampleRate(&sampleRate);
  self->sampleRate = (double)sampleRate;
  post("Current SampleRate : %f\n", self->sampleRate);
  post("ASIOOuputReady() - %s \n", driver->driver_interface->outputReady() == ASE_OK ? "Supported" : "Not Supported");
  
  /* create buffers */
  asio_audioport_create_buffers(self);

  /* set for playback */
  if (0 < self->inputChannels)
  {
      fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_INPUT);
      fts_audioport_set_channels((fts_audioport_t*)self, FTS_AUDIO_INPUT, self->inputChannels);
      fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, asio_audioport_input);
      post("[asio audioport] added to input port \n");
  }
  /* set for capture */
  if (0 < self->outputChannels)
  {
      fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);
      fts_audioport_set_channels((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, self->outputChannels);
      fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, asio_audioport_output);
      post("[asio audioport] added to output port \n");
  }
  

}


static void 
asio_audioport_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  asio_audioport_t* self = (asio_audioport_t*)o;
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
