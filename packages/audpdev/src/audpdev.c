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
 * Author: FranÅÁois DÅÈchelle (dechelle@ircam.fr)
 *
 */


/* This file include the FTS device for using AUDP (Audio-UDP) transport:
   The devices are:
    - an audp input device
    - an audp output device

    A device class is defined for DACs and ADCs

    Warning: unlike the audio device this network device is not synchronised.  
*/

/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>


#include "audp.h"
#include "audp_fifo.h"
#include <fts/fts.h>


/******************************************************************************/
/*                                                                            */
/* Audp devices                                                               */
/*                                                                            */
/******************************************************************************/

/*
  This structure will be stored in the "device_data" and will be
  available in any device function using fts_dev_get_device_data.

  It can be used to store device specific datas as well as
  handle to the platform specific audio datas.
*/

typedef short platform_sample_t;


typedef struct audp_stats {
  int dropped_packets;
  int lost_packets;
  int fifo_high, fifo_low, fifo_empty;
} audp_stats_t;

typedef struct audp_dev_data {
  audp_t *handle;
  int buffer_size; /* in samples */
  platform_sample_t *samples_buffer;
  audp_fifo_t in_fifo;
  int go; /* kind of starter */
  int packet_id;
  int get_count;
  audp_stats_t in_stats;
} audp_dev_data_t;

#define DEFAULT_N_CHANNELS 2
#define DEFAULT_SAMPLING_RATE 44100
#define DEFAULT_PACKET_SIZE 256

#define IN_FIFO_SAMPLE_SIZE 4096
#define IN_FIFO_SAMPLE_LOW 256
#define IN_FIFO_SAMPLE_HIGH 3840

#define DEFAULT_IN_PORT 3001
#define DEFAULT_OUT_PORT 3002
#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_MCAST ""
#define DEFAULT_TTL 16

/*----------------------------------------------------------------------------*/
/* Audio output device                                                        */
/*----------------------------------------------------------------------------*/

/* Forward declarations of device and device class functions */

static fts_status_t audp_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t audp_dac_close(fts_dev_t *dev);
static void         audp_dac_put(fts_word_t *args);

static int          audp_dac_get_nchans(fts_dev_t *dev);
static int          audp_dac_get_nerrors(fts_dev_t *dev);


/* Init function */

static void audp_dac_init(void)
{
  fts_dev_class_t *audp_dac_class;
    
  /* Declaration of the device class */
  audp_dac_class = fts_dev_class_new( fts_sig_dev, fts_new_symbol("audp_dac"));

  /* Definition of the device class `open' function */
  fts_dev_class_set_open_fun( audp_dac_class, audp_dac_open);
  /* Definition of the device class `close' function */
  fts_dev_class_set_close_fun( audp_dac_class, audp_dac_close);
  /* Definition of the device class `put' function */
  fts_dev_class_sig_set_put_fun( audp_dac_class, audp_dac_put);
  /* Definition of the device class `get_nerrors' function
     This function is used to get the number of I/O errors (for instance,
     synchronization errors 
  */
  fts_dev_class_sig_set_get_nerrors_fun( audp_dac_class, audp_dac_get_nerrors);
  /* Definition of the device class `get_nchans' function
     This function returns the number of channels of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( audp_dac_class, audp_dac_get_nchans);
}

/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.
  For instance, the arguments may be:
  [0] (symbol) "channels"
  [1] (int)    2
  [2] (symbol) "port"
  [3] (int) 2001
  [4] (symbol) "hostname"
  [5] (symbol) "ravi.ircam.fr"
  The functions fts_get_<TYPE>_by_name can be used to retrieve the values.
  
  Usually, a device `open' function:
   - parses its arguments
   - allocate a structure to store the values
   - store the pointer to this structure as the "dev_data" using fts_dev_set_device_data
   - calls the platform specific function to open the audio port
*/

static fts_status_t audp_dac_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int n_channels;
  int port;
  fts_symbol_t dest;
  fts_symbol_t mcast_addr_s;
  char* addr =(char*)fts_malloc(256);
  audp_dev_data_t *data;
  int packet_size; /* audp packet size in samples */
  int ttl;

  /* Allocation of device data */
  data = (audp_dev_data_t *)fts_malloc( sizeof( audp_dev_data_t));
  fts_dev_set_device_data( dev, data);

  /* Parameter parsing */
  n_channels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEFAULT_N_CHANNELS);
  port = fts_get_int_by_name( nargs, args, fts_new_symbol("port"), DEFAULT_OUT_PORT);
  dest = fts_get_symbol_by_name( nargs, args, fts_new_symbol("dest"), fts_new_symbol(DEFAULT_HOSTNAME));
  packet_size = DEFAULT_PACKET_SIZE;
  /*  mcast_addr_s = fts_get_symbol_by_name(nargs, args, fts_new_symbol("mcast_addr"), fts_new_symbol(DEFAULT_MCAST)); */
  ttl = fts_get_int_by_name(nargs, args, fts_new_symbol("ttl"), DEFAULT_TTL);

  strcpy(addr, dest);
  if((data->handle = audp_send_open(addr, port, ttl))==NULL)
    {
      post( "Error: audp_send_open() failed\n");
      return &fts_dev_open_error;
    }

  /* 
  else
    {
      post("MULTICAST\n");
      if((data->handle = audp_send_open(addr, port, ttl))==NULL)
	{
	  post( "Error: audp_send_open() failed\n");
	  return &fts_dev_open_error;
	}
	
  */

  data->buffer_size = 0;
  
  /* No header option */
  if(fts_get_int_by_name(nargs, args, fts_new_symbol("header"), 1) == 0 )
    {
      data->handle->head = 0;
      post("AUDP audp out: no header");
    }
  /* Channels setting */
  if(audp_set_channels(data->handle, n_channels) != n_channels)
    {
      post( "Error: audp_set_channels() failed\n");
      return &fts_dev_open_error;
    }
  /* Sampling Rate setting */
  if(audp_set_sampling_rate(data->handle, DEFAULT_SAMPLING_RATE) != DEFAULT_SAMPLING_RATE)
    {
      post("Error: audp_set_sampling_rate() failed\n");
      return &fts_dev_open_error;
    }
  /* Audio format setting */
  if(audp_set_format(data->handle, audp_int16) != audp_int16)
    {
      post("Error: audp_set_format() failed\n");
      return &fts_dev_open_error;
    }
  data->handle->audp_header->packet_size = packet_size * n_channels * sizeof(platform_sample_t);
  data->samples_buffer = (platform_sample_t *)fts_malloc(packet_size*n_channels*sizeof(platform_sample_t));
  data->buffer_size = 0;

   /* This is to inform the scheduler that it should use this device to
     check for I/O errors */
  fts_dsp_set_dac_slip_dev( dev);
  post("AUDP audio out open\n");

  return fts_ok;
}


/*
  Device `close' function

  It can use the platform specific datas stored in the `device_data'
*/
static fts_status_t audp_dac_close(fts_dev_t *dev)
{
  audp_dev_data_t *data;

  data = (audp_dev_data_t *)fts_dev_get_device_data( dev); 
  audp_close(data->handle);
  return fts_ok;
}

static int audp_dac_get_nchans( fts_dev_t *dev)
{
  audp_dev_data_t *data;

  data = (audp_dev_data_t *)fts_dev_get_device_data( dev);
  return (int)data->handle->audp_header->channels;
}

static int audp_dac_get_nerrors( fts_dev_t *dev)
{
  audp_dev_data_t *data;

  data = (audp_dev_data_t *)fts_dev_get_device_data( dev);

  /* Platform specific code should go here */


  return 0;
}


/*
  Device `put' function

  It gets its arguments in an array of fts_word_t, the arguments being
  organized as:
  [0] fts_dev_t **pdev
  [1] int nchans
  [2] int n
  [3] float *buf_1
  [4] float *buf_2
  etc

  It can use the platform specific datas stored in the `device_data'
*/

static void audp_dac_put( fts_word_t *argv)
{
  fts_dev_t *dev;
  audp_dev_data_t *data;
  int n_channels, channel, n, i, j, ret,r;

  dev = *((fts_dev_t **)fts_word_get_pointer( argv));
  data = (audp_dev_data_t *)fts_dev_get_device_data( dev);
  n_channels = fts_word_get_int(argv + 1);
  n = fts_word_get_int(argv + 2);
 
  /* Platform specific code should go here */

  /* This is an example how to format a buffer of interleaved short samples
     from the passed arguments.
     This buffer will then be send to the audio port
  */
  for ( channel = 0; channel < n_channels; channel++)
    {
      float *in;
  
      in = (float *) fts_word_get_pointer( argv + 3 + channel);
      j = channel;
      for ( i = 0; i < n; i++)
	{	  
	      data->samples_buffer[j+(data->buffer_size * n_channels)] = (short) ( 32767.0f * in[i]);
	      j += n_channels;	    
	}
    }
  data->buffer_size += n;

  /* output the buffer */
  if(data->buffer_size*n_channels*sizeof(platform_sample_t) >= data->handle->audp_header->packet_size)
    {
      ret = audp_send(data->handle, (unsigned char*)data->samples_buffer, data->handle->audp_header->packet_size);
      data->buffer_size = 0;
    }
}


/*----------------------------------------------------------------------------*/
/* Audio input device                                                         */
/*----------------------------------------------------------------------------*/

/* Forward declarations of device and device class functions */

static fts_status_t audp_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t audp_adc_close(fts_dev_t *dev);
static void         audp_adc_get(fts_word_t *args);

static int          audp_adc_get_nchans(fts_dev_t *dev);


/* Init function */

static void audp_adc_init(void)
{
  fts_dev_class_t *audp_adc_class;

  /* Declaration of the device class */
  audp_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("audp_adc"));

  /* Definition of the device class `open' function */
  fts_dev_class_set_open_fun( audp_adc_class, audp_adc_open);
  /* Definition of the device class `close' function */
  fts_dev_class_set_close_fun( audp_adc_class, audp_adc_close);
  /* Definition of the device class `get' function */
  fts_dev_class_sig_set_get_fun( audp_adc_class, audp_adc_get);

  /* Definition of the device class `get_nchans' function
     This function returns the number of channels of the device
  */
  fts_dev_class_sig_set_get_nchans_fun( audp_adc_class, audp_adc_get_nchans);
}



/*
  Device `open' function

  It gets as arguments an array of atoms (as an object...) with a passing by name.

  See similar function for `adc' device
*/

static fts_status_t audp_adc_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  int n_channels;
  fts_symbol_t mcast;
  int port;
  audp_dev_data_t *data;
  int ret;
  int buf_size;
  int packet_size;
  char* mcast_addr =(char*)fts_malloc(256);


  /* Allocation of device data */
  data = (audp_dev_data_t *)fts_malloc( sizeof(audp_dev_data_t));
  fts_dev_set_device_data( dev, data);

  /* Parameter parsing */
  n_channels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEFAULT_N_CHANNELS);
  port = fts_get_int_by_name( nargs, args, fts_new_symbol("port"), DEFAULT_IN_PORT);
  packet_size = fts_get_int_by_name(nargs, args, fts_new_symbol("packetsize"), DEFAULT_PACKET_SIZE);
  mcast = fts_get_symbol_by_name(nargs, args, fts_new_symbol("mcast"), fts_new_symbol(DEFAULT_MCAST));
  strcpy(mcast_addr, mcast);
  buf_size = packet_size*n_channels*sizeof(platform_sample_t) + HEADER_LEN;
  data->buffer_size = 0;

  /* audp handle opening */
  if((data->handle = audp_recv_open(mcast_addr, port))==NULL)
    {
      post( "Error: audp_recv_open() failed\n");
      return &fts_dev_open_error;
    }

  /*   setsockopt(data->handle->sockd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)); */
  /* No header option */
  if(fts_get_int_by_name(nargs, args, fts_new_symbol("header"), 1) == 0 )
    {
      data->handle->head = 0;
      post("AUDP audp out: no header");
    }
  /* Channels setting */
  if(audp_set_channels(data->handle, n_channels) != n_channels)
    {
      post( "Error: audp_set_channels() failed\n");
      return &fts_dev_open_error;
    }
  /* Sampling Rate setting */
  if(audp_set_sampling_rate(data->handle, DEFAULT_SAMPLING_RATE) != DEFAULT_SAMPLING_RATE)
    {
      post("Error: audp_set_sampling_rate() failed\n");
      return &fts_dev_open_error;
    }
  /* Audio format setting */
  if(audp_set_format(data->handle, audp_int16) != audp_int16)
    {
      post("Error: audp_set_format() failed\n");
      return &fts_dev_open_error;
    }
  data->samples_buffer = (platform_sample_t *)fts_malloc(MAX_AUDIO_SIZE);


  /* fifo initializing */
  audp_fifo_init(&data->in_fifo,
		 IN_FIFO_SAMPLE_SIZE * n_channels * sizeof(platform_sample_t),
		 IN_FIFO_SAMPLE_LOW * n_channels * sizeof(platform_sample_t),
		 IN_FIFO_SAMPLE_HIGH * n_channels * sizeof(platform_sample_t));

  data->go = 0;
  data->get_count = 0;
  memset(&data->in_stats, 0, sizeof(data->in_stats));

  post("AUDP audio in open\n");
  return fts_ok;
}


static fts_status_t audp_adc_close(fts_dev_t *dev)
{
  audp_dev_data_t *data;

  data = (audp_dev_data_t *)fts_dev_get_device_data( dev);
  audp_close(data->handle);
  /* Platform specific code should go here */

  return fts_ok;
}

static int audp_adc_get_nchans(fts_dev_t *dev)
{
  audp_dev_data_t *data;

  data = (audp_dev_data_t *)fts_dev_get_device_data( dev);

  return (int)data->handle->audp_header->channels;
}

/*
  Device `get' function

  It gets its arguments in an array of fts_word_t, the arguments being
  organized as:
  [0] fts_dev_t **pdev
  [1] int nchans
  [2] int n
  [3] float *buf_1
  [4] float *buf_2
  etc

  It can use the platform specific datas stored in the `device_data'
*/

static void audp_adc_get( fts_word_t *args)
{
  fts_dev_t *dev;
  audp_dev_data_t *data;
  int n_channels, channel, n, i, j, sz;

 
  dev = *((fts_dev_t **)fts_word_get_pointer( args));

  data = (audp_dev_data_t *)fts_dev_get_device_data( dev);
  n_channels = fts_word_get_int(args + 1);
  n = fts_word_get_int(args + 2);

  /* if(n_channels != data->handle->audp_header->channels)
    {
      data->handle->audp_header->channels = n_channels;
      post("Wrong channel number\n");
      } */ 
  /*
    if ( data->buffer_size != n || !data->samples_buffer)
    {
    int new_size;
    
    data->buffer_size = n;
    new_size = data->buffer_size * n_channels * sizeof( platform_sample_t);
    data->samples_buffer = (platform_sample_t *)fts_realloc( data->samples_buffer, new_size);
    }
  */


  /*******************************************************/
  /**/

  /* let's see if we have received some packets */ 

  while ((sz = audp_recv(data->handle, (unsigned char*)data->samples_buffer)) > 0) {
    unsigned long bufsz = data->in_fifo.end - data->in_fifo.begin;

    if (audp_fifo_write(&data->in_fifo, sz, data->samples_buffer) < 0) {
      data->in_stats.dropped_packets++;
    }

    if ((data->go) && (data->handle->audp_header->packet_id != data->packet_id)) {
      data->in_stats.lost_packets++;
    }
    data->packet_id = data->handle->audp_header->packet_id+1; 

    if (!data->go) data->go = 1;

  } 

  if (sz < 0) {
    perror("audp_recv");
    return;
  }

  if (data->in_fifo.end - data->in_fifo.begin > data->in_fifo.high) 
    data->in_stats.fifo_high++;

  /* pop the vector */
  sz = n * n_channels * sizeof(platform_sample_t);

  if (audp_fifo_read(&data->in_fifo, sz, data->samples_buffer) < 0) {
    data->in_stats.fifo_empty++;
    memset(data->samples_buffer, 0, sz); 
  }

  if (data->in_fifo.end - data->in_fifo.begin < data->in_fifo.high)
    data->in_stats.fifo_low++;

  /* feed the vector to fts */
  for ( channel = 0; channel < n_channels; channel++)
    {
      float *out;
      
      out = (float *) fts_word_get_pointer( args + 3 + channel);
      
      j = channel;
      for ( i = 0; i < n; i++)
	{
	  out[i] = (float)data->samples_buffer[j] / 32767.0f;
	  j += n_channels;
	}
      
    }

  if (data->get_count % 1024 == 0) {
    post("AUDP stats: %d lost packets, %d fifo empty, %d fifo low, %d fifo high, %d dropped packets\n",
	 data->in_stats.lost_packets, data->in_stats.fifo_empty, data->in_stats.fifo_low,
	 data->in_stats.fifo_high, data->in_stats.dropped_packets);
    memset(&data->in_stats, 0, sizeof(data->in_stats));
  }

  data->get_count++;
}




/******************************************************************************/
/*                                                                            */
/* Module declaration                                                         */
/*                                                                            */
/******************************************************************************/

static void audpdev_init(void);

fts_module_t audpdev_module = { "audpdev", "AUDP devices", audpdev_init, 0, 0};

static void audpdev_init(void)
{
  audp_adc_init();
  audp_dac_init();
}









