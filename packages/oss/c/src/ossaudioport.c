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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <linux/soundcard.h>

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 256
#define DEFAULT_CHANNELS 2

#define MAX_NUMBER_OF_DEVICE 100 /* if you change this, change in memory allocation in ossaudiomanager_scan_dev_name */
/* by the way, I think if you have more than 100 audiocards, it's a bit strange .... */

typedef struct {
  fts_audioport_t head;
  fts_symbol_t device_name;
  int fd;
  int fifo_size;
  short *dac_fmtbuf;
  short *adc_fmtbuf;
  int buff_size;
  /* output bytes count, for dac slip detection */
  long bytes_count;
  int no_xrun_message_already_posted;
} ossaudioport_t;


static fts_symbol_t s_read_only;
static fts_symbol_t s_write_only;

fts_class_t* ossaudioport_type = NULL;

static void
ossaudiomanager_scan_dev_name(const char* devname)
{
  fts_audioport_t* port;
  fts_atom_t at;
  fts_symbol_t devicename = 0;
  int count = 0;

  int newdevlength=  strlen(devname) + 1 + 2; /* length devname + \0 + "[0-9][0-9] " */
  char* newdevname = (char*)malloc(newdevlength * sizeof(char));
  
  newdevname = strncpy(newdevname, devname, newdevlength);

  devicename = fts_new_symbol(newdevname);

  while ((fts_file_exists(devicename))
	 && (count < MAX_NUMBER_OF_DEVICE))
  {
    fts_set_symbol(&at, devicename);
    port = (fts_audioport_t*)fts_object_create(ossaudioport_type, 1, &at);
    fts_audiomanager_put_port(devicename, port);
    snprintf(newdevname + strlen(devname), 3, "%d", count);
    devicename = fts_new_symbol(newdevname);
    count++;
  }

  free(newdevname);
}

static void
ossaudiomanager_scan_devices()
{
  ossaudiomanager_scan_dev_name("/dev/dsp");
  ossaudiomanager_scan_dev_name("/dev/audio");
}

/**
 * The audioport IO function calls the native audio layer to read/write a buffer
 * of samples in the native format.
 * Its argument is the audioport. Buffers are allocated by the port.
 */
static void ossaudioport_input_fun(fts_audioport_t* port, float** buffers, int buffsize)
{
  ossaudioport_t* oss_port = (ossaudioport_t*)port;
  int channels = fts_audioport_get_channels( port, FTS_AUDIO_INPUT);
  int i;
  int j; 
  int ch;

  read(oss_port->fd, oss_port->adc_fmtbuf, oss_port->buff_size);
  
  for (ch = 0; ch < channels; ++ch)
  {
    j = ch;
    if (fts_audioport_is_channel_used(port, FTS_AUDIO_INPUT, ch))
    {
      for (i = 0; i < buffsize; ++i)
      {
	short s0 = oss_port->adc_fmtbuf[j];
	buffers[ch][i] = (float)s0/32767.0f;
	j += channels;
      }
    }
    else
    {
      for (i = 0; i < buffsize; ++i)
      {
	buffers[ch][i] = 0.0f;
      }
    }
  }
}


static void ossaudioport_output_fun(fts_audioport_t* port, float** buffers, int buffsize)
{
  ossaudioport_t* oss_port = (ossaudioport_t*)port;
  int channels = fts_audioport_get_channels( port, FTS_AUDIO_OUTPUT);
  int i;
  int j;
  int ch;

  for (ch = 0; ch < channels; ++ch)
  {
    j = ch;
    if (fts_audioport_is_channel_used(port, FTS_AUDIO_OUTPUT, ch))
    {
      for (i = 0; i < buffsize; ++i)
      {
	short s0 = (short)(32767.0f * buffers[ch][i]);
	oss_port->dac_fmtbuf[j] = s0;
	j += channels;
      }
    }
    else
    {
      for (i = 0; i < buffsize; ++i)
      {
	oss_port->dac_fmtbuf[j] = 0;
	j += channels;
      }
    }
  }
  write(oss_port->fd, oss_port->dac_fmtbuf, oss_port->buff_size);
}

static int ossaudioport_xrun( fts_audioport_t *port)
{
/*   ossaudioport_t *ossport = (ossaudioport_t *)port; */
/*   count_info count; */
/*   int size; */
  
/*   if (ioctl( ossport->fd, SNDCTL_DSP_GETOPTR, &count) < 0) */
/*     { */
/*       if ( !ossport->no_xrun_message_already_posted) */
/* 	{ */
/* 	  post( "Warning: this device does not support SNDCTL_DSP_GETOPTR\n"); */
/* 	  post( "         Synchronisation errors (\"dac slip\") will not be reported\n"); */

/* 	  ossport->no_xrun_message_already_posted = 1; */
/* 	} */

/*       return 0; */
/*     } */

/*   size = fts_audioport_get_output_channels( port) * sizeof( short) * ossport->fifo_size; */
/*   if (count.bytes > (ossport->bytes_count + size)) */
/*     { */
/*       ossport->bytes_count = count.bytes; */
/*       return 1; */
/*     } */

  return 0;
}

#ifdef DEBUG
static void ossaudioport_debug( int fd)
{
  audio_buf_info info;

  if ( ioctl( fd, SNDCTL_DSP_GETOSPACE, &info) == -1)
    fts_post( "SNDCTL_DSP_GETOSPACE\n");
  fts_post( "fragments: %d\n", info.fragments);
  fts_post( "total number of fragments: %d\n", info.fragstotal);
  fts_post( "fragment size: %d bytes\n", info.fragsize);
  fts_post( "bytes: %d\n", info.bytes);
}
#endif



static void ossaudioport_open (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ossaudioport_t* self = (ossaudioport_t*)o;
  int sample_rate, p_sample_rate, fragparam, fragment_size, channels, p_channels, flags, format, wanted_format, i;
  float sr;
  fts_symbol_t device_name;

  sr = fts_dsp_get_sample_rate();
  sample_rate = (int)sr ;

  /* FIXME */
  device_name = self->device_name;
  channels = DEFAULT_CHANNELS;
  flags = O_RDWR;

  if (-1 != self->fd)
  {
    /* close previously opened device */
    if (close(self->fd) < 0)
    {
      fts_object_error(o, "cannot close device (%s)\n", strerror(errno));
      fts_post("[ossaudioport:] cannot close device (%s)\n", strerror(errno));
      return;
    }
    self->fd = -1;
    fts_log("[ossaudioport] oss audioport is closed \n");
  }

  if ( (self->fd = open( device_name, flags, 0)) < 0 )
    {
      fts_object_error( o, "cannot open device \"%s\" (%s)", device_name, strerror( errno));
      fts_post("[ossaudioport:] cannot open device \"%s\" (%s)", device_name, strerror( errno));
      return;
    }

  /* Set fragment size */
  fragment_size = 2 * sizeof( short) * fts_dsp_get_tick_size();

  for( i = 0; i < 16; i++)
    if (fragment_size & (1<<i))
      break;

  fragparam = ((self->fifo_size / fts_dsp_get_tick_size()) <<16) | (i);

  if (ioctl( self->fd, SNDCTL_DSP_SETFRAGMENT, &fragparam))
    {
      fts_object_error( o, "cannot set fragment size or number of fragment (%s)", strerror( errno));
      fts_post("ossaudioport: cannot set fragment size or number of fragment (%s)\n", strerror( errno));
      return;
    }

#ifdef DEBUG
  ossaudioport_debug( self->fd);
#endif

  /* Set 16 bit format */
#if WORDS_BIGENDIAN
  format = AFMT_S16_BE;
#else
  format = AFMT_S16_LE;
#endif

  wanted_format = format;
  if ( (ioctl( self->fd, SNDCTL_DSP_SETFMT, &format) == -1) || (format != wanted_format))
    {
      fts_object_error( o, "cannot set sample format to signed 16 bits little endian (%s)", strerror( errno));
      fts_post("ossaudioport: cannot set sample format to signed 16 bits little endian (%s)\n", strerror( errno));
      return;
    }

  /* Set number of channels */
  p_channels = channels;
  if ( (ioctl( self->fd, SNDCTL_DSP_CHANNELS, &p_channels) == -1) || (channels != p_channels) )
    {
      fts_object_error( o, "cannot set number of channels (%s)", strerror( errno));
      fts_post("ossaudioport: cannot set number of channels (%s)\n", strerror( errno));
      return;
    }

  /* Set sample rate */
  p_sample_rate = sample_rate;
  if (ioctl( self->fd, SNDCTL_DSP_SPEED, &p_sample_rate) == -1)
    {
      fts_object_error( o, "cannot set sample rate (%s)", strerror( errno));
      fts_post("ossaudioport: cannot set sample rate (%s)\n", strerror( errno));
      return;
    }

  if (sample_rate != p_sample_rate)
    {
      fts_post("[ossaudioport]: cannot set to wanted sample rate (%d), get (%d)\n",
	   sample_rate, p_sample_rate);
      fts_log("[ossaudioport]: cannot set to wanted sample rate (%d), get (%d)\n",
	      sample_rate, p_sample_rate);
    }

  fts_audioport_set_open( (fts_audioport_t *)self, FTS_AUDIO_INPUT);
  fts_audioport_set_open( (fts_audioport_t *)self, FTS_AUDIO_OUTPUT);
  
  fts_log("[ossaudioport] oss audioport is open \n");
}

static void ossaudioport_close (fts_object_t* o, fts_symbol_t s, int ac, const fts_atom_t* at, fts_atom_t *ret)
{
  ossaudioport_t* self = (ossaudioport_t*)o;

  if (self->fd >= 0)
    close( self->fd);

}

static void ossaudioport_init (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ossaudioport_t *self = (ossaudioport_t *)o;
  int channels = 2;
  
  fts_audioport_init( (fts_audioport_t *)self);

  self->fifo_size = DEFAULT_FIFO_SIZE;

  self->device_name = fts_get_symbol(at);

  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, ossaudioport_input_fun);
  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, ossaudioport_output_fun);

  fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_INPUT);
  fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);

  fts_audioport_set_channels((fts_audioport_t*)self, FTS_AUDIO_INPUT, channels);
  fts_audioport_set_channels((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, channels);

/*   fts_audioport_set_xrun_function( (fts_audioport_t *)self, ossaudioport_xrun); */

  self->buff_size =  fts_dsp_get_tick_size() * channels * sizeof(short);
  self->adc_fmtbuf = (short *)fts_malloc(self->buff_size);
  self->dac_fmtbuf = (short *)fts_malloc(self->buff_size);

  self->no_xrun_message_already_posted = 0;

  self->fd = -1;
}

static void ossaudioport_delete (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ossaudioport_t *self = (ossaudioport_t *)o;

  fts_audioport_delete( &self->head);


  fts_free( self->adc_fmtbuf);
  fts_free( self->dac_fmtbuf);
}

static void ossaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( ossaudioport_t), ossaudioport_init, ossaudioport_delete);

  fts_class_message_varargs( cl, fts_s_open_input,  ossaudioport_open);
  fts_class_message_varargs( cl, fts_s_open_output, ossaudioport_open);
}

void ossaudioport_config( void)
{
  fts_symbol_t s = fts_new_symbol("ossaudioport");

  s_read_only = fts_new_symbol( "read_only");
  s_write_only = fts_new_symbol( "write_only");

  ossaudioport_type = fts_class_install( s, ossaudioport_instantiate);

  ossaudiomanager_scan_devices();

}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
