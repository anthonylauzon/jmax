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
 */

#define INITGUID

#include <fts/fts.h>
#include <windows.h>
#include <mmsystem.h> 
#include <dsound.h>

/* These macros are used to test and debug possible synchronization
   and latency configurations */
#define DSDEV_SYNC_ON_OUTPUT 0
#define DSDEV_NUM_FRAGMENTS 32
#define DEFAULT_FIFO_SIZE 8192
#define DEFAULT_SAMPLING_RATE (44100.0f)
#define DEFAULT_CHANNELS 2


/* for some fsck'ing reason, direct sound needs a window! */
static HWND fts_wnd = NULL;

/* the handle to this dll instance to create the fsck'ing window */
static HINSTANCE dsdev_instance = NULL;


static fts_symbol_t fts_s_read_only = NULL;
static fts_symbol_t fts_s_write_only = NULL;
static fts_symbol_t fts_s_read_write = NULL;

int fts_win32_create_window();
int fts_win32_destroy_window();
long FAR PASCAL fts_win32_wndproc(HWND hWnd, unsigned message, WPARAM wParam, LPARAM lParam);
char* fts_win32_error(HRESULT hr);


enum {
  dsaudioport_clean = 0,
  dsaudioport_running,
  dsaudioport_stopping,
  dsaudioport_stopped,
  dsaudioport_corrupted
};

/*
 * the dsaudioport structure
 */
typedef struct {
  fts_audioport_t head;

  WAVEFORMATEX* format;
  int state; 
  unsigned int buffer_byte_size;
  unsigned int buffer_sample_size;
  fts_symbol_t mode;                /* write only, read only or read & write */
  LPGUID guid;

  /* the number of buffers (fts ticks) that go a the fifo buffer */
  unsigned int num_buffers;

  /*  
      the current implementation devides the driver's fifo buffer in a
      number of fragments. the fragment size is always a multiple of
      FTS tick size. the use of fragments avoids a synchronization
      at every tick but rather on a multiple of the tick
      size. hopefully it will make things more efficient. huh...  
  */
  unsigned int num_fragments;
  unsigned int fragment_size;

  int no_xrun_message_already_posted;

  /* sound output */
  LPDIRECTSOUND direct_sound;
  LPDIRECTSOUNDBUFFER primary_buffer;
  LPDIRECTSOUNDBUFFER dsBuffer;
  LPDIRECTSOUNDNOTIFY notify;
  DSBPOSITIONNOTIFY* position;
  HANDLE* event;
  unsigned int cur_buffer;
  unsigned int cur_fragment;

  /* sound input */
  LPDIRECTSOUNDCAPTURE direct_sound_capture;
  LPDIRECTSOUNDCAPTUREBUFFER dscBuffer;
  LPDIRECTSOUNDNOTIFY cnotify;
  DSBPOSITIONNOTIFY* cposition;
  HANDLE* cevent;
  unsigned int cur_cbuffer;
  unsigned int cur_cfragment;

  char* default_device;
  int default_channels;
  int default_fragments;
  int default_fifo_size;
  fts_symbol_t default_mode;

  int sync_on_output;

} dsaudioport_t;

static void dsaudioport_input( fts_word_t *argv);
static void dsaudioport_output(fts_word_t *argv);
static int dsaudioport_xrun(fts_audioport_t *port);
static void dsaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_cleanup(dsaudioport_t *dev);
static void dsaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value);
static fts_status_t dsaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);



static void 
dsaudioport_input(fts_word_t *argv)
{
  dsaudioport_t *dev;
  int n, channels, ch, i, j;
  short *buf1, *buf2;
  DWORD bytes1, bytes2, boffset, soffset;
  HRESULT hr;
  FILE* log; /* FIXME */
  int print_log = 0;
 
  dev = (dsaudioport_t *) fts_word_get_ptr(argv+0);
  if (dev->state != dsaudioport_running) {
    /* FIXME */
    return;
  }

  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_input_channels(dev);
  
  if ((dev->mode == fts_s_read_only) || (dev->mode == fts_s_read_write)) {
    
    /* Calculate the byte and sample offset in the fifo buffer */
    boffset = dev->cur_cbuffer * dev->buffer_byte_size;    
    soffset = dev->cur_cbuffer * dev->buffer_sample_size;    
    
    /* If we reached the end of the fragment, wait till the next
       fragment becomes available */
    if (!dev->sync_on_output && (dev->cur_cbuffer % dev->fragment_size) == 0) {
      dev->cur_cfragment = 1 + dev->cur_cbuffer / dev->fragment_size;
      if (dev->cur_cfragment == dev->num_fragments) {
	dev->cur_cfragment = 0;
      }
      WaitForSingleObject(dev->event[dev->cur_cfragment], 1000);
    }
    
#if 0  
    IDirectSoundCaptureBuffer_GetCurrentPosition(dev->dscBuffer, &bytes1, &bytes2);
    log = fopen("C:\\dsaudioport.txt", "a");
    fprintf(log, "cur=%i\tstart=%u\tend=%u\tread=%u\tcapture=%u\n", 
	    dev->cur_cbuffer, boffset, 
	    boffset + dev->buffer_byte_size, bytes2, bytes1);
    fclose(log);
#endif
    
    
    /* Lock */
    hr = IDirectSoundCaptureBuffer_Lock(dev->dscBuffer, boffset, dev->buffer_byte_size, 
					(void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);
    
    for (ch = 0; ch < channels; ch++) {
      float *out = (float *) fts_word_get_ptr(argv + 2 + ch);
      
      for (i = 0, j = ch; i < n; i++, j += channels) {
	out[i] = (float) buf1[j] / 32768.0f;	
      }
    }
    
    /* Unlock */
    IDirectSoundCaptureBuffer_Unlock(dev->dscBuffer, buf1, bytes1, buf2, bytes2);
    
    dev->cur_cbuffer++;
    if (dev->cur_cbuffer == dev->num_buffers) {
      dev->cur_cbuffer = 0;
    }
    
  } else {

    /* normally we should NEVER get here!!! */
    for (ch = 0; ch < channels; ch++) {
      float *out = (float *) fts_word_get_ptr(argv + 2 + ch);
      
      for (i = 0, j = ch; i < n; i++, j += channels) {
	out[i] = 0.0f;
      }
    }
  }
}
    
static void 
dsaudioport_output(fts_word_t *argv)
{
  dsaudioport_t *dev;
  short *buf1, *buf2;
  DWORD bytes1, bytes2, boffset, soffset;    
  int n, channels, ch, i, j;
  FILE* log; /* FIXME */
  DWORD bend;

  dev = (dsaudioport_t *) fts_word_get_ptr(argv+0);
  if (dev->state != dsaudioport_running) {
    /* FIXME */
    return;
  }

  if ((dev->mode == fts_s_write_only) || (dev->mode == fts_s_read_write)) {

    n = fts_word_get_int(argv + 1);
    channels = fts_audioport_get_output_channels(dev);
    
    
    /* Calculate the byte and sample offset in the fifo buffer */
    boffset = dev->cur_buffer * dev->buffer_byte_size;    
    soffset = dev->cur_buffer * dev->buffer_sample_size;    
    bend = boffset + dev->buffer_byte_size;
    
    /* If we reached the end of the fragment, wait till the next
       fragment becomes available */
    if (dev->sync_on_output && (dev->cur_buffer % dev->fragment_size) == 0) {
      dev->cur_fragment = 1 + dev->cur_buffer / dev->fragment_size;
      if (dev->cur_fragment == dev->num_fragments) {
	dev->cur_fragment = 0;
      }
      
      WaitForSingleObject(dev->event[dev->cur_fragment], 1000);
    }
    
#if 0
    IDirectSoundBuffer_GetCurrentPosition(dev->dsBuffer, &bytes1, &bytes2);
    log = fopen("C:\\dsaudioport.txt", "a");
    fprintf(log, "buf=%i\tfrag=%i\tplay=%u\twrite=%u\tstart=%u\tend=%u\toverlap=%i\n", 
	    dev->cur_buffer, dev->cur_fragment, 
	    bytes1, bytes2,
	    boffset, bend,
	    (((bytes1 <= boffset) && (boffset <= bytes2))
	     || (bytes1 <= bend) && (bend <= bytes2)));
    
    fclose(log);
#endif
    
    /* Lock */
    IDirectSoundBuffer_Lock(dev->dsBuffer, boffset, dev->buffer_byte_size, 
			    (void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);
    
    /* Interleave the sample buffer into the output buffer */
    for (ch = 0; ch < channels; ch++) {
      float *in = (float *) fts_word_get_ptr(argv + 2 + ch);
      
      for (i = 0, j = ch; i < n; i++, j += channels) {
	buf1[j] = (short) (32767.0f * in[i]);
      }
    }
    
    /* Unlock */
    IDirectSoundBuffer_Unlock(dev->dsBuffer, buf1, bytes1, buf2, bytes2);
    
    dev->cur_buffer++;
    if (dev->cur_buffer == dev->num_buffers) {
      dev->cur_buffer = 0;
    }
  }
}

static int 
dsaudioport_xrun(fts_audioport_t *port)
{
  dsaudioport_t *dev = (dsaudioport_t *)port;
  
  if (!dev->no_xrun_message_already_posted) {
/*      post( "Warning: the audio device does not yet support out-of-sync detection\n"); */
/*      post( "         Synchronisation errors (\"dac slip\") will not be reported\n"); */
    dev->no_xrun_message_already_posted = 1;
  }
  
  return 0;
}

static BOOL 
dsaudioport_enum_callback(LPGUID guid, LPCSTR description, 
			  LPCSTR module, LPVOID context)
{
  dsaudioport_t *dev = (dsaudioport_t *) context;

  fts_log("[dsaudioport]: audio device \"%s\"\n", description);
  if ((dev->default_device != NULL) && (strcmp(dev->default_device, description) == 0)) {
    fts_log("[dsaudioport]: found default audio device\n");
    dev->guid = guid;
  }
  if (dev->default_device == NULL) {
    dev->guid = guid;
    dev->default_device = strdup(description);
  }
  return 1;
}

static void 
dsaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int channels, sample_rate, fifo_size, frames;
  unsigned int i;
  dsaudioport_t *dev = (dsaudioport_t *)o;
  DSBUFFERDESC desc;
  HRESULT hr;
  char str[256];
  int err = 0;
  DSCBUFFERDESC cdesc;
  DSCAPS caps;

  ac--;
  at++;
 
  fts_log("[dsaudioport]: Opening audio port\n");

  /* initialize the audioport */
  fts_audioport_init(&dev->head);

  /* we can't use memset(0) since the object's head is already
     initialized */
  dev->guid = NULL;
  dev->format = NULL;
  dev->state = 0; 
  dev->buffer_byte_size = 0;
  dev->buffer_sample_size = 0;
  dev->num_buffers = 0;
  dev->no_xrun_message_already_posted = 0;
  dev->direct_sound = NULL;
  dev->primary_buffer = NULL;
  dev->dsBuffer = NULL;
  dev->notify = NULL;
  dev->position = NULL;
  dev->event = NULL;
  dev->cur_buffer = 0;
  dev->cur_fragment = 0;
  dev->direct_sound_capture = NULL;
  dev->dscBuffer = NULL;
  dev->cnotify = NULL;
  dev->cposition = NULL;
  dev->cevent = NULL;
  dev->cur_cbuffer = 0;
  dev->cur_cfragment = 0;
  dev->sync_on_output = 1;


  /*************************** default settings ********************************/

  if (fts_get_regvalue_string("AudioDevice", str, 256) != 0) {
    dev->default_device = NULL;
  } else {
    dev->default_device = strdup(str);
  }

  if (fts_get_regvalue_int("AudioDeviceChannels", &dev->default_channels) != 0) {
    dev->default_channels = DEFAULT_CHANNELS;
  }

  if (fts_get_regvalue_int("AudioDeviceFragments", &dev->default_fragments) != 0) {
    dev->default_fragments = DSDEV_NUM_FRAGMENTS;
  }

  if (fts_get_regvalue_int("AudioDeviceBufferSize", &dev->default_fifo_size) != 0) {
    dev->default_fifo_size = DEFAULT_FIFO_SIZE;
  }

  if (fts_get_regvalue_string("AudioDeviceMode", str, 256) != 0) {
    dev->default_mode = fts_s_read_write;
  } else {
    dev->default_mode = fts_new_symbol_copy(str);    
    if ((dev->default_mode != fts_s_read_write) 
	&& (dev->default_mode != fts_s_read_only) 
	&& (dev->default_mode != fts_s_write_only) ) {
      fts_log("[dsdev]: Unknown device mode '%s'\n", fts_symbol_name(dev->default_mode));
      dev->default_mode = fts_s_read_write;      
    }
  }

  /*************************** settings and format ********************************/

  /* set the basic audio settings */

#if 0
  /* FIXME: for now we use the values of the registry until the FTS
     configuration has been put in place */
  channels = fts_get_int_arg(ac, at, 0, DEFAULT_CHANNELS);
  fifo_size = fts_param_get_int(fts_s_fifo_size, DEFAULT_FIFO_SIZE);
  dev->num_fragments = DSDEV_NUM_FRAGMENTS;
#else
  channels = dev->default_channels;
  fifo_size = dev->default_fifo_size;
  dev->num_fragments = dev->default_fragments;
  dev->mode = dev->default_mode;
#endif

  /* try to find the GUID of the audio device */
  DirectSoundEnumerate((LPDSENUMCALLBACK) dsaudioport_enum_callback, (LPVOID) dev);              


  /************************ store the current setting ***********************************/

  fts_set_regvalue_string("AudioDevice", dev->default_device);
  fts_set_regvalue_int("AudioDeviceChannels", dev->default_channels);
  fts_set_regvalue_int("AudioDeviceFragments", dev->default_fragments);
  fts_set_regvalue_int("AudioDeviceBufferSize", dev->default_fifo_size);
  fts_set_regvalue_string("AudioDeviceMode", fts_symbol_name(dev->default_mode));



  /************************ set basic parameters ***********************************/

  sample_rate = (int) fts_dsp_get_sample_rate();
  frames = fts_dsp_get_tick_size();
  dev->buffer_sample_size = channels * frames;
  dev->buffer_byte_size = dev->buffer_sample_size * sizeof(short);
  dev->num_buffers = fifo_size / frames;
  dev->fragment_size = dev->num_buffers / dev->num_fragments;

  /* create and initialize the buffer format */
  dev->format = (WAVEFORMATEX*) fts_malloc(sizeof(WAVEFORMATEX));
  ZeroMemory(dev->format, sizeof(WAVEFORMATEX));
  dev->format->wFormatTag = WAVE_FORMAT_PCM;
  dev->format->nChannels = (unsigned short) channels;
  dev->format->nSamplesPerSec = (DWORD) sample_rate;
  dev->format->nBlockAlign = channels * sizeof(short);
  dev->format->nAvgBytesPerSec = dev->format->nSamplesPerSec * dev->format->nBlockAlign;
  dev->format->wBitsPerSample = sizeof(short) * 8; 
  dev->format->cbSize = 0; 


  /*************************** output ********************************/

  if ((dev->mode == fts_s_write_only) || (dev->mode == fts_s_read_write)) {

    hr = DirectSoundCreate(dev->guid, &dev->direct_sound, NULL);
    if (hr != DS_OK) {
      fts_object_set_error(o, "Warning: dsaudioport: failed to create direct sound");
      goto error_recovery;
    }
    
    hr = IDirectSound_SetCooperativeLevel(dev->direct_sound, fts_wnd, DSSCL_PRIORITY);
    if (hr != DS_OK) {
      fts_object_set_error(o, "Warning: dsaudioport: failed to create set the cooperative level");
      goto error_recovery;
    }

    caps.dwSize = sizeof(caps); 
    hr = IDirectSound_GetCaps(dev->direct_sound, &caps);
    if (hr == DS_OK) {
      if (caps.dwFlags & DSCAPS_EMULDRIVER) {
	fts_log("[dsdev]: The audio driver runs in emulated mode. Just thought I'd let you know.\n");
      } else {
	fts_log("[dsdev]: Cool, you have a native DirectSound driver installed\n");
      }
    }

    /* create the primary buffer */
    ZeroMemory(&desc, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
/*      desc.lpwfxFormat = dev->format; */
    
    hr = IDirectSound_CreateSoundBuffer(dev->direct_sound, &desc, &dev->primary_buffer, NULL);
    if (hr != DS_OK) {
      fts_object_set_error(o, "Warning: dsaudioport: failed to create set the primary sound buffer");
      goto error_recovery;
    }

    /* set the primary sound buffer to this format. if it fails, just
       print a warning. */
    hr = IDirectSoundBuffer_SetFormat(dev->primary_buffer, dev->format);
    if (hr != DS_OK) {
      post("Warning: dsaudioport: can't set format of primary sound buffer: %s\n", fts_win32_error(hr));
    }

    /* create the secondary sound output buffer */
    ZeroMemory(&desc, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY;
    desc.lpwfxFormat = dev->format;
    desc.dwBufferBytes = dev->buffer_byte_size * dev->num_buffers;  
    desc.dwReserved = 0; 

    hr = IDirectSound_CreateSoundBuffer(dev->direct_sound, &desc, &dev->dsBuffer, NULL);
    if (hr != DS_OK) {
      fts_object_set_error(o, "Error opening DirectSound device (failed to create the secondary buffer)");
      goto error_recovery;
    }

    /* allocate an array for the notification events */
    dev->event = (HANDLE*) fts_malloc(dev->num_fragments * sizeof(HANDLE));
    ZeroMemory(dev->event, dev->num_fragments * sizeof(HANDLE));
    
    /* allocate an array for the notification positions */
    dev->position = (DSBPOSITIONNOTIFY*) fts_malloc(dev->num_fragments * sizeof(DSBPOSITIONNOTIFY));
    
    /* create and initialize the notification events and positions */
    ZeroMemory(dev->position, dev->num_fragments * sizeof(DSBPOSITIONNOTIFY));
    for (i = 0; i < dev->num_fragments; i++) {
      dev->event[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
      if (dev->event[i] == NULL) {
	fts_object_set_error(o, "Error opening DirectSound device (failed to create the notifications events)");
	goto error_recovery;
      }
      dev->position[i].dwOffset = i * dev->buffer_byte_size * dev->fragment_size;
      dev->position[i].hEventNotify = dev->event[i];
    }
    
    /* set the notification position for the synchronisation with the audio device */
    hr = IDirectSoundBuffer_QueryInterface(dev->dsBuffer, &IID_IDirectSoundNotify, (LPVOID*) &dev->notify);
    if (hr != S_OK) {
      dev->notify = NULL;
      fts_object_set_error(o, "Error opening DirectSound device (failed to create notify interface: %s)", fts_win32_error(hr));
      goto error_recovery;
    }
    hr = IDirectSoundNotify_SetNotificationPositions(dev->notify, dev->num_fragments, dev->position);
    if (hr != S_OK) {
      fts_object_set_error(o, "Error opening DirectSound device (failed to set notify positions: %s)", fts_win32_error(hr));
      goto error_recovery;
    }

    fts_audioport_set_output_channels((fts_audioport_t *) dev, channels);
    fts_audioport_set_output_function((fts_audioport_t *) dev, dsaudioport_output);

  } else {

    fts_audioport_set_output_channels((fts_audioport_t *) dev, 0);
    fts_audioport_set_output_function((fts_audioport_t *) dev, NULL);

  }


  /*************************** input ********************************/

  if ((dev->mode == fts_s_read_only) || (dev->mode == fts_s_read_write)) {

    hr = DirectSoundCaptureCreate(dev->guid, &dev->direct_sound_capture, NULL);
    if (hr != DS_OK) {
      fts_object_set_error(o, "Warning: dsaudioport: failed to create direct sound capture\n");
      dev->direct_sound_capture = NULL;
      goto error_recovery;
    }
    
    /* initialize the input buffer description */
    ZeroMemory(&cdesc, sizeof(DSCBUFFERDESC));
    cdesc.dwSize = sizeof(DSCBUFFERDESC);
    cdesc.dwFlags = DSCBCAPS_WAVEMAPPED;
    cdesc.lpwfxFormat = dev->format;
    cdesc.dwBufferBytes = dev->buffer_byte_size * dev->num_buffers;  
    cdesc.dwReserved = 0; 

    /* create the sound input buffer */
    hr = IDirectSoundCapture_CreateCaptureBuffer(dev->direct_sound_capture, &cdesc, &dev->dscBuffer, NULL);
    if (hr != DS_OK) {
      fts_object_set_error(o, "Error opening DirectSound device (failed to create the capture buffer)");
      goto error_recovery;
    }
    
    /* allocate an array for the notification events */
    dev->cevent = (HANDLE*) fts_malloc(dev->num_fragments * sizeof(HANDLE));
    ZeroMemory(dev->cevent, dev->num_fragments * sizeof(HANDLE));
    
    /* allocate an array for the notification positions */
    dev->cposition = (DSBPOSITIONNOTIFY*) fts_malloc(dev->num_fragments * sizeof(DSBPOSITIONNOTIFY));
    
    /* create and initialize the notification events and positions */
    ZeroMemory(dev->cposition, dev->num_fragments * sizeof(DSBPOSITIONNOTIFY));
    for (i = 0; i < dev->num_fragments; i++) {
      dev->cevent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
      if (dev->cevent[i] == NULL) {
	fts_object_set_error(o, "Error opening DirectSound device (failed to create the notifications events)");
	goto error_recovery;
      }
      dev->cposition[i].dwOffset = i * dev->buffer_byte_size * dev->fragment_size;
      dev->cposition[i].hEventNotify = dev->cevent[i];
    }
    
    /* set the notification position for the synchronisation with the audio device */
    hr = IDirectSoundBuffer_QueryInterface(dev->dscBuffer, &IID_IDirectSoundNotify, (LPVOID*) &dev->cnotify);
    if (hr != S_OK) {
      dev->cnotify = NULL;
      fts_object_set_error(o, "Error opening DirectSound device (failed to create notify interface: %s)", fts_win32_error(hr));
      goto error_recovery;
    }
    hr = IDirectSoundNotify_SetNotificationPositions(dev->cnotify, dev->num_fragments, dev->cposition);
    if (hr != S_OK) {
      fts_object_set_error(o, "Error opening DirectSound device (failed to set notify positions: %s)", fts_win32_error(hr));
      goto error_recovery;
    }

    fts_audioport_set_input_channels((fts_audioport_t *) dev, channels);
    fts_audioport_set_input_function((fts_audioport_t *) dev, dsaudioport_input);

  } else {
    
    fts_audioport_set_input_channels((fts_audioport_t *) dev, 0);
    fts_audioport_set_input_function((fts_audioport_t *) dev, NULL);
  }


  /*************************** finish and start ********************************/



  if (dev->mode == fts_s_read_only) {
    dev->sync_on_output = 0;
    hr = IDirectSoundCaptureBuffer_Start(dev->dscBuffer, DSCBSTART_LOOPING);

  } else if  (dev->mode == fts_s_read_write) {
    dev->sync_on_output = DSDEV_SYNC_ON_OUTPUT;
    hr = IDirectSoundCaptureBuffer_Start(dev->dscBuffer, DSCBSTART_LOOPING);
    hr = IDirectSoundBuffer_Play(dev->dsBuffer, 0, 0, DSBPLAY_LOOPING);

  } else {
    dev->sync_on_output = 1;
    hr = IDirectSoundBuffer_Play(dev->dsBuffer, 0, 0, DSBPLAY_LOOPING);
  }

  fts_audioport_set_xrun_function((fts_audioport_t *) dev, dsaudioport_xrun);

  dev->state = dsaudioport_running;

  fts_log("[dsaudioport]: Done\n");
  
  return;

 error_recovery:

  dsaudioport_cleanup(dev);

  fts_audioport_set_input_channels((fts_audioport_t *) dev, 0);
  fts_audioport_set_input_function((fts_audioport_t *) dev, NULL);
  fts_audioport_set_output_channels((fts_audioport_t *) dev, 0);
  fts_audioport_set_output_function((fts_audioport_t *) dev, NULL);
  fts_audioport_set_xrun_function((fts_audioport_t *) dev, NULL);

  dev->state = dsaudioport_corrupted;
}

static void
dsaudioport_cleanup(dsaudioport_t *dev)
{
  unsigned int i;

  /* delete all things related to the input buffer */

  if (dev->dscBuffer != NULL) {
    IDirectSoundCaptureBuffer_Stop(dev->dscBuffer);
  }

  if (dev->cnotify != NULL) {
    IDirectSoundNotify_Release(dev->cnotify);
    dev->cnotify = NULL;
  }

  if (dev->cposition != NULL) {
    fts_free(dev->cposition);
    dev->cposition = NULL;
  }

  if (dev->cevent != NULL) {
    for (i = 0; i < dev->num_fragments; i++) {
      if (dev->cevent[i] != NULL) {
	CloseHandle(dev->cevent[i]);
	dev->cevent[i] = NULL;
      }
    }
    fts_free(dev->cevent);
    dev->cevent = NULL;
  }

  if (dev->dscBuffer != NULL) {
    /* FIXME: windows crashes after a Release() on the capture buffer */
/*      IDirectSoundCaptureBuffer_Release(dev->dscBuffer); */
    dev->dscBuffer = NULL;
  }

  if (dev->direct_sound_capture != NULL) {
    IDirectSoundCapture_Release(dev->direct_sound_capture); 
    dev->direct_sound_capture = NULL;
  }


  /* delete all things related to the output buffer */

  if (dev->dsBuffer != NULL) {
    IDirectSoundBuffer_Stop(dev->dsBuffer);
  }

  if (dev->notify != NULL) {
    IDirectSoundNotify_Release(dev->notify);
    dev->notify = NULL;
  }

  if (dev->position != NULL) {
    fts_free(dev->position);
    dev->position = NULL;
  }

  if (dev->event != NULL) {
    for (i = 0; i < dev->num_fragments; i++) {
      if (dev->event[i] != NULL) {
	CloseHandle(dev->event[i]);
	dev->event[i] = NULL;
      }
    }
    fts_free(dev->event);
    dev->event = NULL;
  }

  if (dev->dsBuffer != NULL) {
    IDirectSoundBuffer_Release(dev->dsBuffer);
    dev->dsBuffer = NULL;
  }

  if (dev->primary_buffer != NULL) {
    IDirectSoundBuffer_Release(dev->primary_buffer); 
    dev->primary_buffer = NULL;
  }

  if (dev->direct_sound != NULL) {
    IDirectSound_Release(dev->direct_sound); 
    dev->direct_sound = NULL;
  }

  if (dev->format != NULL) {
    fts_free(dev->format);
    dev->format = NULL;
  }
}

static void
dsaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsaudioport_t *dev = (dsaudioport_t *)o;

  fts_log("[dsaudioport]: Closing audio port\n");
  dsaudioport_cleanup(dev);
  fts_audioport_delete( (fts_audioport_t *) dev);
  fts_log("[dsaudioport]: Done\n");
}

static void 
dsaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, o);
}

static fts_status_t 
dsaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(dsaudioport_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, dsaudioport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, dsaudioport_delete);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, dsaudioport_get_state);

  return fts_Success;
}

static char* 
fts_win32_error(HRESULT hr) {
  char *s = "Don't know why";
  switch (hr) {
  case E_NOINTERFACE: s = "No such interface"; break;
  case DSERR_GENERIC: s = "Generic error"; break;
  case DSERR_ALLOCATED: s = "Required resources already allocated"; break;
  case DSERR_BADFORMAT: s = "The format is not supported"; break;
  case DSERR_INVALIDPARAM: s = "Invalid parameter"; break;
  case DSERR_NOAGGREGATION: s = "No aggregation"; break;
  case DSERR_OUTOFMEMORY: s = "Out of memory"; break;
  case DSERR_UNINITIALIZED: s = "Uninitialized"; break;
  case DSERR_UNSUPPORTED: s = "Function not supported"; break;
  case DSERR_INVALIDCALL: s = "Invalid call"; break;
  case DSERR_PRIOLEVELNEEDED: s = "Priority level needed"; break;
  }
  return s;
}



static long FAR PASCAL 
fts_win32_wndproc(HWND hWnd, unsigned message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
  case WM_CREATE:
    break;
  case WM_DESTROY:
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
    break;
  } 
  return(0L);
}

static int 
fts_win32_create_window() 
{
  WNDCLASS myClass;
  myClass.hCursor = LoadCursor( NULL, IDC_ARROW );
  myClass.hIcon = NULL; 
  myClass.lpszMenuName = (LPSTR) NULL;
  myClass.lpszClassName = (LPSTR) "FtsDsDev";
  myClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  myClass.hInstance = dsdev_instance;
  myClass.style = CS_GLOBALCLASS;
  myClass.lpfnWndProc = fts_win32_wndproc;
  myClass.cbClsExtra = 0;
  myClass.cbWndExtra = 0;
  if (!RegisterClass(&myClass)) {
    post( "Warning: dsaudioport: failed to register the window class\n");
    return -100;
  }
  fts_wnd = CreateWindow((LPSTR) "FtsDsDev", (LPSTR) "FtsDsDev", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, (HWND) NULL, (HMENU) NULL, 
			dsdev_instance, (LPSTR) NULL);  
  if (fts_wnd == NULL) {
    post( "Warning: dsaudioport: failed to create the window\n");
    return -101;
  }
  return 0;
}

static int 
fts_win32_destroy_window() 
{
  if (fts_wnd != NULL) {
    DestroyWindow(fts_wnd);
    fts_wnd = NULL;
  }
  return 0;
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD reason, LPVOID lpReserved)
{
  switch (reason) {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
      dsdev_instance = (HINSTANCE) hModule;
      break;

  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    /* make sure we clean things up nicely*/
    fts_log("[dsdev]: Cleaning up\n");
    fts_win32_destroy_window();
    fts_log("[dsdev]: Done\n");
  }
  return TRUE;
}

void 
dsaudioport_config(void)
{
  fts_symbol_t dsaudioport_symbol;

  /* make sure we have a valid instance handle */
  if (dsdev_instance == NULL) {
    post("Warning: dsaudioport: invalid DLL instance handle\n");
    return;
  }

  /* create an invisible window */
  if ((fts_wnd == NULL) 
      && (fts_win32_create_window() != 0)) {
    return;
  }

  fts_s_read_only = fts_new_symbol("read_only");
  fts_s_write_only = fts_new_symbol("write_only");
  fts_s_read_write = fts_new_symbol("read_write");

  dsaudioport_symbol = fts_new_symbol("dsaudioport");
  fts_class_install( dsaudioport_symbol, dsaudioport_instantiate);
  fts_audioport_set_default_class(dsaudioport_symbol);

  /* FIXME: force the creation of the audio device */
  fts_audioport_get_default(NULL);
}
