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
 * Authors: Guenter Geiger with some copy/paste from ossdev.c
 *
 */


/* 
 * This file includes the device specifics for the ALSA architecture.
 *
 */

#define DEF_DEVICE_NAME "/dev/audio"
#define DEF_SAMPLING_RATE ((float)44100.0f)
#define DEF_FIFO_SIZE 256
#define DEF_CHANNELS 2

#undef ALSADEV_DEBUG
/* #define DEBUGMESSAGE post(__FUNCTION__"\n");  */
#define DEBUGMESSAGE 

/* #define PDEBUG(x) x */
 #define PDEBUG(x) 


/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/asoundlib.h>
#include <linux/asound.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "fts.h"

extern void fts_dsp_set_dac_slip_dev(fts_dev_t *dev);

/* forward declarations */

#define MAXDEVICES 8

/******************************************************************************/
/*                                                                            */
/* Audio descriptor: keeps file descriptor and parameters                     */
/*                                                                            */
/******************************************************************************/


typedef struct _alsa_handle {
  snd_pcm_t* handle;
  int channels;
} alsa_handle_t;


typedef struct _audio_desc
{
  fts_symbol_t device_name;

  /* status */
  int dac_open;
  int adc_open;

  /* the device handles */
  alsa_handle_t dac_handles[MAXDEVICES];
  alsa_handle_t adc_handles[MAXDEVICES];

  /* parameters */
  int sampling_rate;
  int fifo_size;
  int inchannels;
  int outchannels;
  int devices;
  /* formating buffers */
  short *dac_fmtbuf;
  short *adc_fmtbuf;

  /* output bytes count, for dac slip detection */
  long bytes_count;
} audio_desc_t;



static fts_hash_table_t *audio_desc_table;






static void alsa_detect_devices( audio_desc_t *aud)
{
  int err;
  snd_ctl_t* handle;
  snd_ctl_hw_info_t hw_info;
  int i;
  int card = snd_defaults_pcm_card();

  snd_pcm_info_t pcm_info;
  snd_pcm_playback_info_t pcm_playback_info;
  snd_pcm_capture_info_t pcm_capture_info;



  err = snd_ctl_open(&handle, card);

  err = snd_ctl_hw_info(handle, &hw_info);
  aud->devices = hw_info.pcmdevs;

  /*
  err = snd_ctl_pcm_info(handle, card, &pcm_info);
  aud->devices = pcm_info.playback;
  */

  PDEBUG(post("devices %d\n",aud->devices));

  for (i=0;i<aud->devices;i++) {
    err = snd_ctl_pcm_playback_info(handle, card, i, &pcm_playback_info);
    err = snd_ctl_pcm_capture_info(handle, card, i, &pcm_capture_info);

    aud->dac_handles[i].channels = pcm_playback_info.max_channels;
    aud->adc_handles[i].channels = pcm_capture_info.max_channels;
    PDEBUG(post("Device %d, input %d, output %d\n",i,aud->dac_handles[i].channels, 
	 aud->adc_handles[i].channels));
  }

}
  




static void audio_desc_init( audio_desc_t *aud, fts_symbol_t device_name)
{
  int i;
  DEBUGMESSAGE;

  aud->device_name = device_name;

  aud->dac_open = 0;
  aud->adc_open = 0;
  
  for (i=0;i<MAXDEVICES;i++) {
    aud->dac_handles[i].handle = NULL;
    aud->adc_handles[i].handle = NULL;
  }

  aud->sampling_rate = 0;
  aud->fifo_size = 0;
  aud->inchannels = 0;
  aud->outchannels = 0;
  aud->devices = 0;
  aud->dac_fmtbuf = 0;
  aud->adc_fmtbuf = 0;

  aud->bytes_count = 0;
  alsa_detect_devices( aud);
}



static audio_desc_t *audio_desc_new( fts_symbol_t device_name)
{
  audio_desc_t *p;
  DEBUGMESSAGE;

  p = (audio_desc_t *)fts_malloc( sizeof( audio_desc_t));

  assert( p != 0);

  audio_desc_init( p, device_name);

  return p;
}



/* destroy closes devices and free's allocated buffers */

static void audio_desc_destroy( audio_desc_t *aud)
{
  int i;
  DEBUGMESSAGE;

  for (i=0;i< MAXDEVICES;i++)
    if ( aud->dac_handles[i].handle != NULL)
      {
	snd_pcm_close( aud->dac_handles[i].handle);
	PDEBUG(post("ALSA closed dac device %d\n",i));
	aud->dac_handles[i].handle = NULL;
      }
    else
      break;

  for (i=0;i< MAXDEVICES;i++)
    if ( aud->adc_handles[i].handle != NULL)
      {
	snd_pcm_close( aud->adc_handles[i].handle);
	PDEBUG(post("ALSA closed adc device %d\n",i));
	aud->adc_handles[i].handle = NULL;
      }
    else
      break;

  if (aud->dac_fmtbuf)
    {
      fts_free( aud->dac_fmtbuf);
      aud->dac_fmtbuf = 0;
    }

  if (aud->adc_fmtbuf)
    {
      fts_free( aud->adc_fmtbuf);
      aud->adc_fmtbuf = 0;
    }
}

static void audio_desc_free( audio_desc_t *aud)
{
  DEBUGMESSAGE;
  audio_desc_destroy( aud);
  fts_free( aud);
}


static int alsa_set_fragmentsize(audio_desc_t *aud)
{
  int fragment_size;
  int err;
  int i;
  struct snd_pcm_playback_params pcm_playback_params;
  struct snd_pcm_capture_params pcm_capture_params;


  DEBUGMESSAGE;
  /* Set fragment size */
  /* HACK !!! Vector size is not known when this function is called */

  fragment_size = 2 * sizeof( short) * 64;

  for( i = 0; i < 16; i++)
    if (fragment_size & (1<<i))
      break;

  PDEBUG(post("Setting fragment size to %d\n",fragment_size)); 
  pcm_capture_params.fragment_size = pcm_playback_params.fragment_size = fragment_size;
  pcm_capture_params.fragments_min =  1; /* hmm if this is a good value ? */
  pcm_playback_params.fragments_max = -1; /* (trick ?? from Jaroslav aplay.c) aud->fifo_size / 64;*/


  for (i=0;i< MAXDEVICES;i++) {
    if (aud->dac_handles[i].handle)
      if( (err = snd_pcm_playback_params(aud->dac_handles[i].handle,&pcm_playback_params))< 0){ 
	fprintf(stderr,"ERROR pcm_playback_params failed: %s\n", 
		snd_strerror(err));
	return -1;
      }
    if (aud->adc_handles[i].handle)
      if( (err = snd_pcm_capture_params(aud->adc_handles[i].handle,&pcm_capture_params))< 0){ 
	fprintf(stderr,"ERROR pcm_playback_params failed: %s\n", 
		snd_strerror(err));
	return -1;
      }
  }    

  return 0;
}


static int alsa_set_format_rate_channels(audio_desc_t *aud)
{
  struct snd_pcm_format pcm_format;
  int err;
  int i;

  DEBUGMESSAGE;
  for (i=0;i< MAXDEVICES;i++) {
    if (aud->dac_handles[i].handle) {
      pcm_format.format = SND_PCM_SFMT_S16_LE ; /* 16 bit little endian */
      pcm_format.rate = aud->sampling_rate; 
      pcm_format.channels = 2;
      
      if( (err = snd_pcm_playback_format(aud->dac_handles[i].handle, &pcm_format)) < 0){ 
	post("ERROR pcm_format (DAC) failed: %s\n", snd_strerror(err));
	return -1;
      }
    }

    if (aud->adc_handles[i].handle) {
      pcm_format.format = SND_PCM_SFMT_S16_LE ; /* 16 bit little endian */
      pcm_format.rate =  aud->sampling_rate; 
      pcm_format.channels = 2;
      
      if( (err = snd_pcm_capture_format(aud->adc_handles[i].handle, &pcm_format)) < 0){ 
	post("ERROR pcm_format (ADC) failed: %s\n", snd_strerror(err));
	return -1;
      }
    }
  }
  return 0;
}



static int audio_desc_set_parameters( audio_desc_t *aud)
{

  DEBUGMESSAGE;
  if (alsa_set_fragmentsize(aud) < 0) return -1;
  if (alsa_set_format_rate_channels(aud) < 0) return -1;

  return 0;
}




/* 
   To put the audio device in the right configuration,
   set the dac_open/adc_open fields in the right
   status and call the following function; work also
   to close the unix device.

   Return 0 in case of success, -1 in case of errors in opening.
  */

static int audio_desc_check_full_duplex_caps( audio_desc_t *aud)
{
  DEBUGMESSAGE;
  return 0;
}


static int alsa_real_open( audio_desc_t *aud, int flags)
{

  int i=0;
  int inchannels=0;
  int outchannels=0;
  int card = 0;
  int max = 8; /* try 8 devices maximum */

  DEBUGMESSAGE;
  #ifdef ALSA_DEBUG
  fprintf(stderr,"ALSA: inchannels = %d\n",aud->inchannels);
  fprintf(stderr,"ALSA: outchannels = %d\n",aud->outchannels);
  fprintf(stderr,"ALSA: rate = %d\n",aud->sampling_rate);
  fprintf(stderr,"ALSA: dac = %d, adc = %d\n",aud->dac_open,aud->adc_open);
  fprintf(stderr,"ALSA: fifo = %d\n",aud->fifo_size);
  #endif
  inchannels = aud->inchannels;
  outchannels = aud->outchannels;

  for(i=0; (inchannels || outchannels) &&  (i < aud->devices);i++)
    {
    if (flags == SND_PCM_OPEN_PLAYBACK || !inchannels) {
      if (snd_pcm_open(&aud->dac_handles[i].handle, card, i, flags) < 0) {
	post("unable to open card %d, device %d in playback mode \n",card,i,flags);
	continue;
      }
      PDEBUG(post("opened playback\n"));
      outchannels -= aud->dac_handles[i].channels;
    }
    else if (flags == SND_PCM_OPEN_CAPTURE || !outchannels) {
      if (snd_pcm_open(&aud->adc_handles[i].handle, card, i, flags) < 0) {
	post("unable to open card %d, device %d in capture mode \n",card,i,flags);
	continue;
      }
      inchannels -= aud->adc_handles[i].channels;
      PDEBUG(post("opened capture\n"));
    }
    else {
      if (snd_pcm_open(&aud->dac_handles[i].handle, card, i, flags) < 0) {
	post("unable to open card %d, device %d in duplex mode \n",card,i,flags);
	continue;
      }
      aud->adc_handles[i].handle = aud->dac_handles[i].handle;
      outchannels -= aud->dac_handles[i].channels;
      inchannels  -= aud->adc_handles[i].channels;
      PDEBUG(post("opened duplex\n"));
    }
  }  

  aud->inchannels -= inchannels; 
  aud->outchannels -= outchannels;
  

  return 0;
}


/* 
 * Called by alsa_dac_open, calls in turn the alsa_real_open function
 */

#define CMD_OPEN_DAC    1
#define CMD_OPEN_ADC    2
#define CMD_CLOSE_DAC   3
#define CMD_CLOSE_ADC   4

static int audio_desc_update( audio_desc_t *aud, int cmd)
{
  int flags;
  int err;

  DEBUGMESSAGE;

  flags = -1;

  switch (cmd) {
  case CMD_OPEN_DAC:
    flags = ( aud->adc_open ) ? SND_PCM_OPEN_DUPLEX : SND_PCM_OPEN_PLAYBACK;
    aud->dac_open = 1;
    break;
  case CMD_OPEN_ADC:
    flags = ( aud->dac_open ) ? SND_PCM_OPEN_DUPLEX : SND_PCM_OPEN_CAPTURE;
    aud->adc_open = 1;
    break;
  case CMD_CLOSE_DAC:
    if ( aud->adc_open )
      flags = O_RDONLY;
    aud->dac_open = 0;
    break;
  case CMD_CLOSE_ADC:
    if ( aud->dac_open )
      flags = O_WRONLY;
    aud->adc_open = 0;
    break;
  }

  if (flags < 0)
    {
      fts_symbol_t device_name;

      device_name = aud->device_name;

      audio_desc_free( aud);

      assert( fts_hash_table_remove( audio_desc_table, device_name) != 0);

      return 0;
    }

  audio_desc_destroy( aud);



  if ((err = alsa_real_open(aud,flags)) < 0)
    return err;

  /* 
     If the device is opened read and write, check 
     if the device supports full duplex.
     If it does not support full duplex, then return error 
  */
  /* currently noop with alsa */

  if ( aud->dac_open && aud->adc_open)
    {
      audio_desc_check_full_duplex_caps( aud);
    }


  if ( audio_desc_set_parameters( aud) )
    {
      PDEBUG(post("audio_desc_set_parameters failed\n"));
      audio_desc_destroy( aud);
      return -1;
    }


  /* Allocate the formatting buffers */
  if (aud->dac_open)
    aud->dac_fmtbuf = (short *) fts_malloc( MAXVS * aud->outchannels * sizeof(short));
  if (aud->adc_open)
    aud->adc_fmtbuf = (short *) fts_malloc( MAXVS * aud->inchannels * sizeof(short));

  return 0;
}

static audio_desc_t *audio_desc_get( fts_symbol_t s)
{
  fts_atom_t a;
  audio_desc_t *aud;

  DEBUGMESSAGE;
  if ( fts_hash_table_lookup( audio_desc_table, s, &a))
    {
      aud = (audio_desc_t *)fts_get_ptr( &a);
    }
  else
    {
      aud = audio_desc_new( s);

      assert( aud != 0);

      fts_set_ptr( &a, aud);
      assert( fts_hash_table_insert( audio_desc_table, s, &a) != 0);
    }

  return aud;
}


/******************************************************************************/
/*                                                                            */
/*                              DAC Devices                                   */
/*                                                                            */
/******************************************************************************/


/* Forward declarations of DAC dev and dev class static functions */

static fts_status_t alsa_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t alsa_dac_close(fts_dev_t *dev);
static void         alsa_dac_put(fts_word_t *args);

static int          alsa_dac_get_nchans(fts_dev_t *dev);
static int alsa_dac_get_nerrors(fts_dev_t *dev);

/* Init and shutdown functions */

static void alsa_dac_init(void)
{
  fts_dev_class_t *alsa_dac_class;
    
  /* ALSA DAC class  */

  DEBUGMESSAGE;
  alsa_dac_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("alsa_dac"));

  fts_dev_class_set_open_fun(alsa_dac_class, alsa_dac_open);
  fts_dev_class_set_close_fun(alsa_dac_class, alsa_dac_close);
  fts_dev_class_sig_set_put_fun(alsa_dac_class, alsa_dac_put);
  fts_dev_class_sig_set_get_nerrors_fun( alsa_dac_class, alsa_dac_get_nerrors);
  fts_dev_class_sig_set_get_nchans_fun(alsa_dac_class, alsa_dac_get_nchans);
}

/* ALSA DAC control/options functions */

/*
   The dev_ctrl operation for the alsa_dac
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the alsa dac.

   NOTE that for the ALSA you *have* to set the sample_rate using
   the dedicated UCS command (audio-set sample-rate) , not the alsa_dac.

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in samples frame units

   sample_rate  sampling frequency     int      44100     ANY (as long as it is supported by the machine)
 
*/

/* ALSA DAC dev class functions */
static fts_status_t alsa_dac_open( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_symbol_t s;
  audio_desc_t *aud;
  DEBUGMESSAGE;
  s = fts_get_symbol_by_name( nargs, args, fts_new_symbol("device"), fts_new_symbol(DEF_DEVICE_NAME));

  aud = audio_desc_get( s);

  /* Parameter parsing  */
  aud->sampling_rate = (int) fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);
  aud->fifo_size = fts_param_get_int(fts_s_fifo_size, DEF_FIFO_SIZE);
  aud->outchannels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEF_CHANNELS);

  PDEBUG(post("ALSA dac channels =%d\n",aud->outchannels));

  /* Open the device */
  if ( audio_desc_update( aud, CMD_OPEN_DAC) < 0)
    return &fts_dev_open_error;


  fts_dev_set_device_data(dev, aud);

  fts_dsp_set_dac_slip_dev( dev);

  return fts_Success;
}


static fts_status_t alsa_dac_close(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);

  DEBUGMESSAGE;
  audio_desc_update( aud, CMD_CLOSE_DAC);

  return fts_Success;
}

static int alsa_dac_get_nchans(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);

  DEBUGMESSAGE;
  return aud->outchannels;
}


/*
 *   returns 1 if dac queue is full, else returns 0; ?????
 */

static int alsa_dac_get_nerrors(fts_dev_t *dev)
{
  int size;
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);
  snd_pcm_playback_status_t pcm_stat;
  DEBUGMESSAGE;


  if (snd_pcm_playback_status(aud->dac_handles[0].handle, &pcm_stat) < 0)
    {
      post("Error reading ALSA status\n");
      return 0;
    }

  size = aud->outchannels * sizeof( short) * aud->fifo_size;
  if (pcm_stat.count > (aud->bytes_count + size))
    {
      aud->bytes_count = pcm_stat.count;
      PDEBUG(post("error\n"));
      return 1;
    }
  else
    return 0;
}


/* 
   Arguments: fts_dev_t *dev, int nchans, int n, float *buf1 ... bufm 
*/
static void alsa_dac_put(fts_word_t *argv)
{
  int n, nchannels, device, channel;
  int i,j,k;
  audio_desc_t *aud;
  short* buf;
  static snd_pcm_playback_status_t  status;

  aud = (audio_desc_t *)fts_dev_get_device_data(*((fts_dev_t **)fts_word_get_ptr( argv) ));

  /* no output devices at all --> return immediately */
  if (aud->dac_handles[0].handle == NULL)
    return;

  nchannels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  aud->bytes_count += (nchannels * n * sizeof(short));

  channel = 0;
  buf = aud->dac_fmtbuf;
  for ( device = 0; device < aud->devices; device++)
    {
      float *in;
      
      for (j=0;j< aud->dac_handles[device].channels;j++) 
	{
	  in = (float *) fts_word_get_ptr( argv + 3 + channel + j );
	  k = j;
	  for ( i = 0; i < n; i++)
	    {
	      buf[k] = (short) ( 32767.0f * in[i]);
	      k += aud->dac_handles[device].channels;
	    }
	}

      channel += aud->dac_handles[device].channels; 
      buf += aud->dac_handles[device].channels*n;
    }
  
  
  /* check the state ... maybe we can somehow recover form underruns */


  buf = aud->dac_fmtbuf;
  for (device=0; device < aud->devices;device++)
    {
      snd_pcm_playback_status(aud->dac_handles[device].handle, &status);

      if (status.underrun) {
	/* do some resetting flush is not what we want to do
	   somehow either I'm doing something wrong, or the driver
	   gets into some weird state, where it distorts all date 
	   ... mhhmm, this is really a problem
	 */
	post("dac slip\n");

	snd_pcm_flush_playback(aud->dac_handles[device].handle);
      }

      if (snd_pcm_write( aud->dac_handles[device].handle, 
			 buf, 
			 aud->dac_handles[device].channels * n * sizeof(short)) < 0)
	post("dac write error\n");
      buf += aud->dac_handles[device].channels*n;
    }


}


/******************************************************************************/
/*                                                                            */
/*                              ADC Devices                                   */
/*                                                                            */
/******************************************************************************/

/* Forward declarations of ADC dev and dev class static functions */

static fts_status_t alsa_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t alsa_adc_close(fts_dev_t *dev);
static void         alsa_adc_get(fts_word_t *args);

static int          alsa_adc_get_nchans(fts_dev_t *dev);


/* Init and functions */

static void alsa_adc_init(void)
{
  fts_dev_class_t *alsa_adc_class;

  /* ALSA ADC class  */

  DEBUGMESSAGE;
  alsa_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("alsa_adc"));

  /* device functions */

  fts_dev_class_set_open_fun(alsa_adc_class, alsa_adc_open);
  fts_dev_class_set_close_fun(alsa_adc_class, alsa_adc_close);
  fts_dev_class_sig_set_get_fun(alsa_adc_class, alsa_adc_get);

  fts_dev_class_sig_set_get_nchans_fun(alsa_adc_class, alsa_adc_get_nchans);
}

/* ALSA ADC control/options functions: use the same parser as the dac */

/* ALSA ADC dev class functions */

static fts_status_t alsa_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_symbol_t s;
  audio_desc_t *aud;

  DEBUGMESSAGE;
  s = fts_get_symbol_by_name( nargs, args, fts_new_symbol("device"), fts_new_symbol(DEF_DEVICE_NAME));

  aud = audio_desc_get( s);

  /* Parameter parsing  */
  aud->sampling_rate = (int) fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);
  aud->fifo_size = fts_param_get_int(fts_s_fifo_size, DEF_FIFO_SIZE);
  aud->inchannels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEF_CHANNELS);

  /* Open the device */
  aud->adc_open = 1;
  if ( audio_desc_update( aud, CMD_OPEN_ADC) < 0)
    return &fts_dev_open_error;

  fts_dev_set_device_data(dev, aud);

  return fts_Success;
}


static fts_status_t alsa_adc_close(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);

  DEBUGMESSAGE;
  aud->adc_open = 0;
  audio_desc_update( aud, CMD_CLOSE_ADC);

  return fts_Success;
}

static int alsa_adc_get_nchans(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);

  return aud->inchannels;
}

/* 
   Arguments: fts_dev_t *dev, int n, float *buf1 ... bufm 
*/
static void alsa_adc_get( fts_word_t *argv)
{
  int n, i, nchannels, channel, j;
  audio_desc_t *aud;

  aud = (audio_desc_t *)fts_dev_get_device_data(*((fts_dev_t **)fts_word_get_ptr( argv)));


  /* no input handles at all --> return immediately */
  if (aud->adc_handles[0].handle == NULL)
    return;

  nchannels = fts_word_get_long(argv + 1);
  n = fts_word_get_long(argv + 2);

  /* TODO make this multiple device like in dac_put */

  i = 0;
  snd_pcm_read( aud->adc_handles[i].handle, 
		aud->adc_fmtbuf, 
		aud->adc_handles[i].channels * n * sizeof( short));


  if ( nchannels == 2)
    {
      float *out0, *out1;

      out0 = (float *) fts_word_get_ptr(argv + 3);
      out1 = (float *) fts_word_get_ptr(argv + 4);

      j = 0;
      for ( i = 0; i < n; i++)
	{
	  out0[i] = (float)aud->adc_fmtbuf[j++] / 32767.0f;
	  out1[i] = (float)aud->adc_fmtbuf[j++] / 32767.0f;
	}
    }
  else
    {
      for ( channel = 0; channel < nchannels; channel++)
	{
	  float *out;

	  out = (float *) fts_word_get_ptr( argv + 3 + channel);

	  j = channel;
	  for ( i = 0; i < n; i++)
	    {
	      out[i] = (float)aud->adc_fmtbuf[j] / 32767.0f;
	      j += aud->adc_handles[0].channels;
	    }
	}
    }
}


/******************************************************************************/
/*                                                                            */
/* Module declaration                                                         */
/*                                                                            */
/******************************************************************************/

static void alsadev_init(void);

fts_module_t alsadev_module = {"alsadev", "ALSA devices", alsadev_init, 0, 0, 0};

static void alsadev_init(void)
{
  PDEBUG(post("alsadev init\n"));

  audio_desc_table = fts_hash_table_new();

  alsa_dac_init();
  alsa_adc_init();
}

