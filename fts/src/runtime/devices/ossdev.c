/* This file include the device specific to the OSS/LINUX architecture.
   Initially only audio

   These audio driver work for any number of channels channels, allow access to all
   the OSS/LINUX audio parameters from the FTS UCS system.

   A Device class is defined for DACs and ADCs.

   A central structure keep track of the consistency of the whole business.

   In the Future, a io_ctrl operations at the UCS level will allow access
   to the oss audio paramters from UCS programs, accessing to the linux device
   directly.

   The device *only* support stereo devices.

   All the post should generate an error.
*/

#define OSSDEV_DEBUG

/* Include files */

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <errno.h>
#include <string.h>

#include "sys.h"
#include "lang.h"
#include "runtime/devices/devices.h"
#include "runtime/devices/unixdev.h"
#include "runtime/files.h"

/* Check this is right */

#define AUDIO_DEVICE "/dev/dsp"

/* forward declarations */

static void oss_dac_init(void);
static void oss_adc_init(void);
static void oss_midi_init(void);

/******************************************************************************/
/*                                                                            */
/*           Shared audio control structure                                   */
/*                                                                            */
/******************************************************************************/

/* The control structure is shared because we share the same
   file descriptor for input and output
   */


static struct oss_audio_data_struct
{
  /* status */

  int device_opened;
  int dac_opened;
  int adc_opened;

  /* device */

  int fd;

  int sampling_rate;

  int fragment_size;
  int max_fragments;

  /* buffers  */

  short *dac_fmtbuf;		/* buffer to format stereo sample frames, allocated in the device open */
  short *adc_fmtbuf;		/* buffer to format stereo sample frames, allocated in the device_open */

} oss_audio_data;


static void oss_audio_set_parameters()
{
  int format, stereo, sr, fragparam, fragment_size, i;

  /* Set fragment size */
  fragment_size = oss_audio_data.fragment_size;
  for( i = 0; i < 16; i++)
    if (fragment_size & (1<<i))
      break;

  fragparam = (oss_audio_data.max_fragments<<16) | (i);

  if (ioctl( oss_audio_data.fd, SNDCTL_DSP_SETFRAGMENT, &fragparam))
    {
      post( "Error in ioctl(SNDCTL_DSP_SETFRAGMENT)\n");
      perror( "oss");
      return;
    }

#ifdef OSSDEV_DEBUG
  {
    audio_buf_info info;

    if ( ioctl( oss_audio_data.fd, SNDCTL_DSP_GETOSPACE, &info) == -1)
      post( "SNDCTL_DSP_GETOSPACE\n");
    post( "fragments: %d\n", info.fragments);
    post( "total number of fragments: %d\n", info.fragstotal);
    post( "fragment size: %d bytes\n", info.fragsize);
    post( "bytes: %d\n", info.bytes);
  }
#endif

  /* Set 16 bit format */

  format = AFMT_S16_LE;
  if (ioctl(oss_audio_data.fd, SNDCTL_DSP_SETFMT, &format) == -1)
    post("Error in ioctl(SNDCTL_DSP_SETFMT)\n");

  if (format != AFMT_S16_LE)
    post("Audio device doesn't support 16 bit mode\n");

  /* Set stereo mode */

  stereo = 1;
  if (ioctl(oss_audio_data.fd, SNDCTL_DSP_STEREO, &stereo) == -1)
    post("error in ioctl(SNDCTL_DSP_STEREO)\n");

  if (! stereo)
    post("Audio device doesn't support stereo mode\n");

  /* Set sampling rate */

  sr = oss_audio_data.sampling_rate;
  if (ioctl(oss_audio_data.fd, SNDCTL_DSP_SPEED, &sr) == -1)
    post("error in ioctl(SNDCTL_DSP_SPEED)\n");

  if (sr != oss_audio_data.sampling_rate)
    post("Audio device doesn't support requested sampling rate\n");
}

/* to put the audio device in the right configuration,
   set the dac_opened/adc_opened fields in the right
   status and call the following function; work also
   to close the unix device.

   Return 0 in case of success, -1 in case of errors in opening.
  */


static int oss_audiodev_update_device()
{
  int fd;

  if (oss_audio_data.device_opened)
    {
      close(oss_audio_data.fd);
      oss_audio_data.device_opened = 0;
    }

  if (oss_audio_data.dac_opened && oss_audio_data.adc_opened)
    {
      fd = open(AUDIO_DEVICE, O_RDWR, 0);
    }
  else if (oss_audio_data.dac_opened)
    {
      fd = open(AUDIO_DEVICE, O_WRONLY, 0);
    }
  else if (oss_audio_data.adc_opened)
    {
      fd = open(AUDIO_DEVICE, O_RDONLY, 0);
    }

  if ( fd >= 0 )
    {
      oss_audio_data.fd = fd;
      oss_audio_set_parameters();
      oss_audio_data.device_opened = 1;
      return 0;
    }
  else
    {
      fprintf(stderr, "Error opening OSS device: %s\n", strerror(errno));
      return -1;
    }
}


/******************************************************************************/
/*                                                                            */
/*                              DAC Devices                                   */
/*                                                                            */
/******************************************************************************/


/* Forward declarations of DAC dev and dev class static functions */

static fts_status_t oss_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t oss_dac_close(fts_dev_t *dev);
static void         oss_dac_put(fts_word_t *args);

static int          oss_dac_get_nchans(fts_dev_t *dev);

/*
 * Channels housekeeping structure
 */


/* Init and shutdown functions */

static void oss_dac_init(void)
{
  fts_dev_class_t *oss_dac_class;

    
  /* OSS DAC class  */

  oss_dac_class = fts_dev_class_new(fts_sig_dev);

  /* device functions */

  set_open_fun(oss_dac_class, oss_dac_open);
  set_close_fun(oss_dac_class, oss_dac_close);

  set_sig_dev_put_fun(oss_dac_class, oss_dac_put);

  set_sig_dev_get_nchans_fun(oss_dac_class, oss_dac_get_nchans);

  fts_dev_class_register(fts_new_symbol("oss_dac"), oss_dac_class);
}

/* OSS DAC control/options functions */

/*
   The dev_ctrl operation for the oss_dac
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the oss dac.

   NOTE that for the OSS you *have* to set the sample_rate using
   the dedicated UCS command (audio-set sample-rate) , not the oss_dac.

   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   channels     number of channels;    int      2         ANY (as long as it is supported by the machine)

   fifo_size    the size of the fifo, in samples frame units

   sample_rate  sampling frequency     int      44100     ANY (as long as it is supported by the machine)
 
*/


/* OSS DAC dev class functions */

static fts_status_t
oss_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  if (oss_audio_data.dac_opened)
    return &fts_dev_open_error; /*Error: a device was already opened for the ch */

  /* Parameter parsing  */
  
  oss_audio_data.sampling_rate = (int) fts_param_get_float(fts_s_sampling_rate, 44100.);
  oss_audio_data.fragment_size = fts_get_int_by_name(nargs, args, fts_new_symbol("fragment_size"), 128);
  oss_audio_data.max_fragments = fts_get_int_by_name(nargs, args, fts_new_symbol("max_fragments"), 4);

  /* open the device */

  oss_audio_data.dac_opened = 1;

  if (oss_audiodev_update_device())
    {
      oss_audio_data.dac_opened = 0;
      return &fts_dev_open_error;
    }

  /* Allocate the DAC  formatting buffer */

  oss_audio_data.dac_fmtbuf = (short *) fts_malloc(MAXVS * 2 * sizeof(short));

  return fts_Success;
}


static fts_status_t
oss_dac_close(fts_dev_t *dev)
{
  oss_audio_data.dac_opened = 0;

  oss_audiodev_update_device();

  /* free the audio formatting buffer */

  fts_free(oss_audio_data.dac_fmtbuf);

  return fts_Success;
}

static int
oss_dac_get_nchans(fts_dev_t *dev)
{
  return 2;
}


/* 
   fts_dev_t *dev, int n, float *buf1 ... bufm 
   the device is ignored (only one device for the moment allowed)
*/

static void
oss_dac_put(fts_word_t *argv)
{
  long n = fts_word_get_long(argv + 1);
  int i,j, ch;
  float *in1;
  float *in2;

  in1 = (float *) fts_word_get_ptr(argv + 2);
  in2 = (float *) fts_word_get_ptr(argv + 3);

  for (i = 0, j = 0; i < n; i++, j += 2)
    {
      oss_audio_data.dac_fmtbuf[j + 0] = (short) ( 32767.0f * in1[i]);
      oss_audio_data.dac_fmtbuf[j + 1] = (short) ( 32767.0f * in2[i]);
    }

  write(oss_audio_data.fd, oss_audio_data.dac_fmtbuf, 2 * n * sizeof(short));
}


/******************************************************************************/
/*                                                                            */
/*                              ADC Devices                                   */
/*                                                                            */
/******************************************************************************/

/* Forward declarations of ADC dev and dev class static functions */

static fts_status_t oss_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t oss_adc_close(fts_dev_t *dev);
static void         oss_adc_get(fts_word_t *args);

static int          oss_adc_get_nchans(fts_dev_t *dev);


/* Init and functions */

static void oss_adc_init(void)
{
  fts_dev_class_t *oss_adc_class;

  /* OSS ADC class  */

  oss_adc_class = fts_dev_class_new(fts_sig_dev);

  /* device functions */

  set_open_fun(oss_adc_class, oss_adc_open);
  set_close_fun(oss_adc_class, oss_adc_close);
  set_sig_dev_get_fun(oss_adc_class, oss_adc_get);

  set_sig_dev_get_nchans_fun(oss_adc_class, oss_adc_get_nchans);

  fts_dev_class_register(fts_new_symbol("oss_adc"), oss_adc_class);
}

/* OSS ADC control/options functions: use the same parser as the dac */

/* OSS ADC dev class functions */

static fts_status_t
oss_adc_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  if (oss_audio_data.adc_opened)
    return &fts_dev_open_error; /*Error: a device was already opened for the ch */

  /* Parameter parsing  */

  oss_audio_data.sampling_rate = (int) fts_param_get_float(fts_s_sampling_rate, 44100);  
  oss_audio_data.fragment_size = fts_get_int_by_name(nargs, args, fts_new_symbol("fragment_size"), 128);
  oss_audio_data.max_fragments = fts_get_int_by_name(nargs, args, fts_new_symbol("max_fragments"), 4);

  /* open the device */

  oss_audio_data.adc_opened = 1;

  if (oss_audiodev_update_device())
    {
      oss_audio_data.adc_opened = 0;
      return &fts_dev_open_error;
    }

  /* Allocate the ADC  formatting buffer */

  oss_audio_data.adc_fmtbuf = (short *) fts_malloc(MAXVS * 2 * sizeof(short));

  return fts_Success;
}


static fts_status_t
oss_adc_close(fts_dev_t *dev)
{
  oss_audio_data.adc_opened = 0;

  oss_audiodev_update_device();

  /* free the audio formatting buffer */

  fts_free(oss_audio_data.adc_fmtbuf);

  return fts_Success;
}


static int
oss_adc_get_nchans(fts_dev_t *dev)
{
  return 2;
}


/* fts_dev_t *dev, int n, float *buf1 ... bufm 
   the device is ignored (only one device for the moment allowed)

*/

static void
oss_adc_get(fts_word_t *argv)
{
  long n = fts_word_get_long(argv + 1);
  int i,j;
  int ch;

  /* if it is the first device of the tick, read the frame in the 
     buffer */

  read(oss_audio_data.fd, oss_audio_data.adc_fmtbuf, n * 2);

  /* do the data transfer: unrolled pipelined loop */

  for (ch = 0; ch < 2; ch++)
    {
      float *out;
      
      out = (float *) fts_word_get_ptr(argv + 2 + ch);

      for (i = ch, j = 0; j < n; i = i + 8, j += 4)
	{
	  short f1, f2, f3, f4;

	  f1 = oss_audio_data.adc_fmtbuf[i + 0];
	  f2 = oss_audio_data.adc_fmtbuf[i + 2];
	  f3 = oss_audio_data.adc_fmtbuf[i + 4];
	  f4 = oss_audio_data.adc_fmtbuf[i + 6];

	  out[j + 0] = 32767.0f * f1;
	  out[j + 1] = 32767.0f * f2;
	  out[j + 2] = 32767.0f * f3;
	  out[j + 3] = 32767.0f * f4;
	}
    }
}


/******************************************************************************/
/*                                                                            */
/* MIDI device                                                                */
/*                                                                            */
/******************************************************************************/

#define MIDI_BUFSIZE 256

static fts_status_t oss_midi_open( fts_dev_t *dev, int ac, const fts_atom_t *av)
{
  fts_symbol_t name;
  fd_dev_data_t *p;
  int fd;

  p = fd_data_new( MIDI_BUFSIZE, MIDI_BUFSIZE);
  fts_dev_set_device_data( dev, (void *) p);

  name = fts_get_symbol_by_name( ac, av, fts_new_symbol("device"), fts_new_symbol( "/dev/midi00"));
  fd = open( fts_symbol_name( name), O_RDWR);

  if (fd < 0)
    {
      return &fts_dev_open_error;
    }

  fd_data_set_input_fd( p, fd);
  fd_data_set_output_fd( p, fd);

  return fts_Success;
}

static fts_status_t oss_midi_close( fts_dev_t *dev)
{
  fd_dev_data_t *p = (fd_dev_data_t *)fts_dev_get_device_data( dev);

  close( fd_data_get_input_fd( p));

  fd_data_delete( p);

  return fts_Success;
}

static fts_status_t oss_midi_put( fts_dev_t *dev, unsigned char c)
{
  fts_status_t status;

  status = fd_dev_put( dev, c);
  if (status != fts_Success)
    return status;

  return fd_dev_flush( dev);
}


static void oss_midi_init( void)
{
  fts_dev_class_t *oss_midi_class;

  oss_midi_class = fts_dev_class_new( fts_char_dev);

  set_open_fun( oss_midi_class, oss_midi_open);
  set_close_fun( oss_midi_class, oss_midi_close);
  set_char_dev_get_fun( oss_midi_class, fd_dev_get);
  set_char_dev_put_fun( oss_midi_class, oss_midi_put);

  fts_dev_class_register( fts_new_symbol( "oss_midi"), oss_midi_class);
}

/******************************************************************************/
/*                                                                            */
/*                              Init function                                 */
/*                                                                            */
/******************************************************************************/

void ossdev_init(void)
{
  /* defaults and init for the shared data */

  oss_audio_data.device_opened = 0;
  oss_audio_data.dac_opened = 0;
  oss_audio_data.adc_opened = 0;

  oss_dac_init();
  oss_adc_init();

  oss_midi_init();
}

