/*
 *
 *
 * Based on Open Source API proposed by Ross Bencina
 * and Port Audio ASIO by Stephane Letz
 */

#ifndef JMAX_ASIO_PORT_H_
#define JMAX_ASIO_PORT_H_ 1

#include <fts/fts.h>
#include "jmax_asio.h"

#ifdef WIN32
#include <windows.h>
#include <asiosys.h>
#include <asio.h>
#include <iasiodrv.h>
/* max length for a driver name */
#define MAX_DRIVER_NAME_LENGTH 128

/* max length for a path */
#define MAX_PATH_LEN 512

typedef struct 
{
  int ID;
  CLSID clsid;
  char name[MAX_DRIVER_NAME_LENGTH];
  char dllpath[MAX_PATH_LEN];
  LPVOID asiodrv;
  IASIO* driver_interface;
  bool isStarted;
} asio_driver_t;


#endif  /* WIN32 */
typedef struct
{
  fts_audioport_t head;
  
  /* driver */
  asio_driver_t* driver;

  /* Channels */
  long inputChannels;
  long outputChannels;
  
  /* Buffer size */
  /* ASIO configuration */
  long minSize;
  long maxSize;
  long preferredSize;
  long granularity;

  double sampleRate;
  
  ASIOBufferInfo* bufferInfos;
  ASIOChannelInfo* channelInfos;


  // Information from ASIOGetSamplePosition()
  double nanoSeconds;
  double samples;
  double tcSamples; // time code samples
  
  // bufferSwitchTimeInfo
  ASIOTime tInfo; // time info state
  unsigned long sysRefTime; // system reference time, when bufferSwitch() was called


  // driver is running ?
  bool stopped;
  // need to call AsioOutputReady() ?
  bool postOutput;


  // offset in asio input buffer
  int asio_input_buffer_offset;
  // offset in asio output buffer
  int asio_output_buffer_offset;
  // offset in port input buffer
  int input_buffer_offset;
  // offset in port output buffer
  int output_buffer_offset;

  void** input_buffers;
  void** output_buffers;

  int asio_buffer_size;
  int inout_buffer_size;

  int size_of_asio_type;

} asio_audioport_t;


extern fts_class_t* asio_audioport_type;

#endif /* JMAX_ASIO_PORT_H_ */


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
