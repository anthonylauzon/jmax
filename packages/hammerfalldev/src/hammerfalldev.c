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
 * Authors: Guenter Geiger <geiger@debian.org> aka <geiger@epy.co.at>
 *
 */


/* 
 * This file implements the device specifics for the RME Hammerfall architecture.
 *
 */

/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <errno.h>
/*#include <string.h>*/
#include <assert.h>

#include <fts/fts.h>




typedef int32_t t_hammerfall_sample;
#define HAMMERFALL_SAMPLEWIDTH sizeof(t_hammerfall_sample)
#define RME9652_MAX_CHANNELS 26

static char rme9652_dsp_dac[] = "/dev/rme9652/C0da%d"; 
static char rme9652_dsp_adc[] = "/dev/rme9652/C0ad%d"; 

/* taken form ossdev.c */

#define DEF_DEVICE_NAME "hammerfall"
#define DEF_SAMPLING_RATE ((float)44100.0f)
#define DEF_FIFO_SIZE 256
#define DEF_CHANNELS 8


/* debugging */

//#define DEBUG

#ifdef DEBUG
#define DEBUGMESSAGE post(__FUNCTION__"\n");  
#define PDEBUG(x) x 
#else
#define DEBUGMESSAGE 
#define PDEBUG(x) 
#endif


static fts_hash_table_t *audio_desc_table;


/***************************************************************************
 *                                                                         *
 *        The audio_desc_t a description of the devices used               *
 *                                                                         *
 ***************************************************************************/


typedef struct _audio_desc
{
  fts_symbol_t device_name; /* do we really need this ?? */

  /* the device handles */

  int dac_handles[RME9652_MAX_CHANNELS];
  int adc_handles[RME9652_MAX_CHANNELS];

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



static void audio_desc_init( audio_desc_t *aud, fts_symbol_t device_name)
{
  int i;
  DEBUGMESSAGE;

  aud->device_name = device_name;

  for (i=0;i<RME9652_MAX_CHANNELS;i++) {
    aud->dac_handles[i] = -1;
    aud->adc_handles[i] = -1;
  }

  aud->sampling_rate = 0;
  aud->fifo_size = 0;  /* fifo size can be set from RME ctrl program */
  aud->inchannels = 0;
  aud->outchannels = 0;
  aud->devices = 0;  /* devices is number of inchannels and outchannels ? */
  aud->dac_fmtbuf = 0;
  aud->adc_fmtbuf = 0;
  aud->bytes_count = 0;
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

  for (i=0;i< RME9652_MAX_CHANNELS;i++)
    if ( aud->dac_handles[i] != -1)
      {
        close(aud->dac_handles[i]);
        PDEBUG(post("RME closed dac device %d\n",i));
        aud->dac_handles[i]= -1;
      }
    else
      break;

  for (i=0;i< RME9652_MAX_CHANNELS;i++)
    if ( aud->adc_handles[i] != -1)
      {
        close(aud->adc_handles[i]);
        PDEBUG(post("RME closed adc device %d\n",i));
        aud->adc_handles[i] = -1;
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



/***************************************************************************
 *                                                                         *
 *         D A C                                                           *
 *                                                                         *
 ***************************************************************************/


/* Forward declarations of DAC dev and dev class static functions */

static fts_status_t hammerfall_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t hammerfall_dac_close(fts_dev_t *dev);
static void         hammerfall_dac_put(fts_word_t *args);

static int          hammerfall_dac_get_nchans(fts_dev_t *dev);
static int          hammerfall_dac_get_nerrors(fts_dev_t *dev);


static void hammerfall_dac_init(void)
{
  fts_dev_class_t *hammerfall_dac_class;
    
  /* RME DAC class  */

  DEBUGMESSAGE;
  hammerfall_dac_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("hammerfall_dac"));

  fts_dev_class_set_open_fun(hammerfall_dac_class, hammerfall_dac_open);
  fts_dev_class_set_close_fun(hammerfall_dac_class, hammerfall_dac_close);
  fts_dev_class_sig_set_put_fun(hammerfall_dac_class, hammerfall_dac_put);
  fts_dev_class_sig_set_get_nerrors_fun( hammerfall_dac_class, hammerfall_dac_get_nerrors);
  fts_dev_class_sig_set_get_nchans_fun(hammerfall_dac_class, hammerfall_dac_get_nchans);
}



static fts_status_t hammerfall_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_symbol_t s;
  audio_desc_t *aud;
  int channels_wanted;
  int channels_open;
  char devname[24];
  int startdevice = 0;
  int num_devs;
  float ftmp;

  DEBUGMESSAGE;


  s = fts_get_symbol_by_name( nargs, args, fts_new_symbol("device"), fts_new_symbol(DEF_DEVICE_NAME));

  aud = audio_desc_get( s);

  /* Parameter parsing  */

  ftmp = fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);
  aud->sampling_rate = (int) ftmp; 
  aud->fifo_size = fts_param_get_int(fts_s_fifo_size, DEF_FIFO_SIZE);
  aud->outchannels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEF_CHANNELS);

  PDEBUG(post("HAMMERFALL dac channels =%d\n",aud->outchannels));

  /* Open the devices */

  channels_open = 0;
  channels_wanted = aud->outchannels;

  for (num_devs=startdevice; channels_open < channels_wanted; num_devs++)
    {
      int tmp;
      sprintf(devname, rme9652_dsp_dac, num_devs+1);
      if ((tmp = open(devname,O_WRONLY)) == -1) 
	{
	  PDEBUG(post("RME9652: failed to open %s writeonly\n",
		      devname);)
	    aud->outchannels = channels_open;
	  return &fts_dev_open_error;
	}
      PDEBUG(post("RME9652: out device Nr. %d (%d) on %s\n",
		  channels_open+1,tmp,devname));
 
      aud->dac_handles[channels_open] = tmp;
      channels_open++;
    }

  aud->outchannels = channels_open;

  /* Now, what is left to do is to configure the devices ....
     we don't do it, because we can set it with the rme ctrl program, 
     and anyhow I don't know if the ioctls are implemented correctly. */

  aud->dac_fmtbuf = (short *) fts_malloc(fts_get_tick_size() * HAMMERFALL_SAMPLEWIDTH);

  fts_dev_set_device_data(dev, aud);

  fts_dsp_set_dac_slip_dev( dev);

  /*   if (aud->outchannels < channels->wanted) return fts_Failure; */

  return fts_Success;
}

static fts_status_t hammerfall_dac_close(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);
  int startdevice = 0;
  int i;

  DEBUGMESSAGE;

  for (i=startdevice;i<aud->outchannels;i++) {
    if (aud->dac_handles[i] != -1) {
      close(aud->dac_handles[i]);
      aud->dac_handles[i] = -1;
    }
     
  }

  return fts_Success;

}



#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif


//#define CLIP(x) (((x)>INT32_MAX)?INT32_MAX:((x) < -INT32_MAX)?-INT32_MAX:(x))
#define CLIP(x) x

static void         hammerfall_dac_put(fts_word_t *args)
{
  int n, nchannels, device;
  int i;
  audio_desc_t *aud;
  t_hammerfall_sample* buf;


  aud = (audio_desc_t *)fts_dev_get_device_data(*((fts_dev_t **)fts_word_get_ptr( args) ));

  /* no output devices at all --> return immediately */
  
  if (aud->dac_handles[0] == -1)
    return;

  /* 
     this seems to be called with channels at argv[1] and bufsize
     at argv[2].
  */

  nchannels = fts_word_get_long(args + 1);
  n = fts_word_get_long(args + 2);

  aud->bytes_count += (nchannels * n * HAMMERFALL_SAMPLEWIDTH);

  /* Translation from float to 32 bit */

  buf = (t_hammerfall_sample*) aud->dac_fmtbuf;


  for (device = 0; device < aud->outchannels; device++) {  
    t_hammerfall_sample *a,*b,*c,*d;
    float *fp1,*fp2,*fp3,*fp4;
    int num = n>>2;

    fp1 = (float *) fts_word_get_ptr( args + (3 + device) );
    fp2 = fp1+1;
    fp3 = fp1+2;
    fp4 = fp1+3;

    a = buf;
    b = a+1;
    c = a+2;
    d = a+3;
     
    for (i = num;i--;fp1+=4,fp2+=4,fp3+=4,fp4+=4,a+=4,b+=4,c+=4,d+=4)
      { 
        float intmax = (float) INT32_MAX;
        float s1 =  *fp1*intmax;
        float s2 =  *fp2*intmax;
        float s3 =  *fp3*intmax;
        float s4 =  *fp4*intmax;
        
        *(a) = CLIP(s1);
        *(b) = CLIP(s2);
        *(c) = CLIP(s3);
        *(d) = CLIP(s4);
      } 
    write(aud->dac_handles[device],buf,n*HAMMERFALL_SAMPLEWIDTH);
  }

}



static int hammerfall_dac_get_nchans(fts_dev_t *dev)
{
  audio_desc_t *aud;

  aud = (audio_desc_t *)fts_dev_get_device_data(dev);
  return aud->outchannels; 
}



static int hammerfall_dac_get_nerrors(fts_dev_t *dev)
{
  return 0; /* no error detection up to now */
}





/***************************************************************************
 *                                                                         *
 *         A D C                                                           *
 *                                                                         *
 ***************************************************************************/

/* Forward declarations of ADC dev and dev class static functions */

static fts_status_t hammerfall_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t hammerfall_adc_close(fts_dev_t *dev);
static void         hammerfall_adc_get(fts_word_t *args);

static int          hammerfall_adc_get_nchans(fts_dev_t *dev);




static void hammerfall_adc_init(void)
{
  fts_dev_class_t *hammerfall_adc_class;

  /* RME ADC class  */

  DEBUGMESSAGE;
  hammerfall_adc_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("hammerfall_adc"));

  /* device functions */

  fts_dev_class_set_open_fun(hammerfall_adc_class, hammerfall_adc_open);
  fts_dev_class_set_close_fun(hammerfall_adc_class, hammerfall_adc_close);
  fts_dev_class_sig_set_get_fun(hammerfall_adc_class, hammerfall_adc_get);

  fts_dev_class_sig_set_get_nchans_fun(hammerfall_adc_class, hammerfall_adc_get_nchans);
}




static fts_status_t hammerfall_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_symbol_t s;
  audio_desc_t *aud;
  int channels_wanted;
  int channels_open;
  char devname[24];
  int startdevice = 0;
  int num_devs;
  float ftmp;

  DEBUGMESSAGE;
  s = fts_get_symbol_by_name( nargs, args, fts_new_symbol("device"), fts_new_symbol(DEF_DEVICE_NAME));

  aud = audio_desc_get( s);

  /* Parameter parsing  */

  ftmp = fts_param_get_float( fts_s_sampling_rate, DEF_SAMPLING_RATE);
  aud->sampling_rate = (int) ftmp;
  aud->fifo_size = fts_param_get_int(fts_s_fifo_size, DEF_FIFO_SIZE);
  aud->inchannels = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), DEF_CHANNELS);

  PDEBUG(post("HAMMERFALL adc channels =%d\n",aud->outchannels));

  /* Open the devices */

  channels_open = 0;
  channels_wanted = aud->inchannels;
  for (num_devs=startdevice; channels_open < channels_wanted; num_devs++)
    {
      int tmp;
      sprintf(devname, rme9652_dsp_adc, num_devs+1);
      if ((tmp = open(devname,O_RDONLY)) == -1) 
	{
	  PDEBUG(post("RME9652: failed to open %s readonly\n",
		      devname);)
	    aud->inchannels = channels_open;
	  return &fts_dev_open_error;

	}
      PDEBUG(post("RME9652: in device Nr. %d (%d) on %s\n",
		  channels_open+1,tmp,devname));
 
      if (channels_wanted > channels_open)
        {
	  aud->adc_handles[channels_open] = tmp;
	  channels_open++;
        }
      else close(tmp);
    }

  aud->inchannels = channels_open;

  /* Now, what is left to do is to configure the devices ....
     we don't do it, because we can set it with the rme ctrl program, 
     and anyhow I don't know if the ioctls are implemented correctly. */

  aud->adc_fmtbuf = (short *) fts_malloc(fts_get_tick_size() * HAMMERFALL_SAMPLEWIDTH);

  fts_dev_set_device_data(dev, aud);

  fts_dsp_set_dac_slip_dev( dev);

  /*   if (aud->outchannels < channels->wanted) return fts_Failure; */
  return fts_Success;
}


static fts_status_t hammerfall_adc_close(fts_dev_t *dev)
{
  audio_desc_t *aud = (audio_desc_t *)fts_dev_get_device_data( dev);
  int startdevice = 0;
  int i;

  DEBUGMESSAGE;

  for (i=startdevice;i<aud->inchannels;i++) {
    if (aud->adc_handles[i] != -1) {
      close(aud->adc_handles[i]);
      aud->adc_handles[i] = -1;
    }
  }
  return fts_Success;
}


static void         hammerfall_adc_get(fts_word_t *args)
{
  int n, nchannels, device;
  int i;
  audio_desc_t *aud;
  short* buf;


  aud = (audio_desc_t *)fts_dev_get_device_data(*((fts_dev_t **)fts_word_get_ptr( args) ));

  /* no output devices at all --> return immediately */
  
  if (aud->adc_handles[0] == -1)
    return;

  /* 
     this seems to be called with channels at argv[1] and bufsize
     at argv[2].
  */

  nchannels = fts_word_get_long(args + 1);
  n = fts_word_get_long(args + 2);
  /*     aud->bytes_count += (nchannels * n * HAMMERFALL_SAMPLEWIDTH); */

  /* Translation from 32 bit to float */

  buf = aud->adc_fmtbuf;
  for(device=0;device<aud->inchannels;device++){

    read(aud->adc_handles[device], buf, n*HAMMERFALL_SAMPLEWIDTH);
    {
      t_hammerfall_sample *a,*b,*c,*d;
      float *fp1 = (float *) fts_word_get_ptr( args + 3 + device);
      float *fp2 = fp1 + 1;
      float *fp3 = fp1 + 2;
      float *fp4 = fp1 + 3;
      int quartersize = n>>2;

      a = (t_hammerfall_sample*) buf;
      b=a+1;
      c=a+2;
      d=a+3; 
        
      for (i = quartersize;i--;fp1+=4,fp2+=4,fp3+=4,fp4+=4,a+=4,b+=4,c+=4,d+=4)
        { 
	  *(fp1) = *(a) * (float)(1./(float)INT32_MAX);
	  *(fp2) = *(b) * (float)(1./(float)INT32_MAX);
	  *(fp3) = *(c) * (float)(1./(float)INT32_MAX);
	  *(fp4) = *(d) * (float)(1./(float)INT32_MAX);
        }
    }  
  } 
     
}



static int hammerfall_adc_get_nchans(fts_dev_t *dev)
{
  audio_desc_t* aud;
  aud = (audio_desc_t *)fts_dev_get_device_data(dev);
  return aud->inchannels; 
}



/******************************************************************************/
/*                                                                            */
/* Module declaration                                                         */
/*                                                                            */
/******************************************************************************/

static void hammerfalldev_init(void);

fts_module_t hammerfalldev_module = {"hammerfalldev", "HAMMERFALL devices", hammerfalldev_init, 0, 0};

static void hammerfalldev_init(void)
{
  PDEBUG(post("hammerfalldev init\n"));

  audio_desc_table = fts_hash_table_new();

  hammerfall_dac_init();
  hammerfall_adc_init();
}

