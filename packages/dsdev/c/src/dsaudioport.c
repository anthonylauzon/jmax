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

#define INITGUID

#define CAPTURE 0

#include <fts/fts.h>
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

static HINSTANCE dsdev_instance = NULL;
static HWND fts_wnd = NULL;
static LPDIRECTSOUND fts_direct_sound = NULL;
static LPDIRECTSOUNDCAPTURE fts_direct_sound_capture = NULL;
static LPDIRECTSOUNDBUFFER fts_primary_buffer = NULL;

fts_class_t* dsaudioport_type = NULL;

int fts_open_direct_sound(char *device);
int fts_close_direct_sound();
int fts_open_direct_sound_capture(char *device);
int fts_close_direct_sound_capture();
int fts_win32_create_window();
int fts_win32_destroy_window();
long FAR PASCAL fts_win32_wndproc(HWND hWnd, unsigned message, WPARAM wParam, LPARAM lParam);
char* fts_win32_error(HRESULT hr);

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 2048
#define DEFAULT_CHANNELS 2

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
  int buffer_byte_size;
  int buffer_sample_size;
  int num_buffers;
  int no_xrun_message_already_posted;
  
  LPGUID guid;
  LPDIRECTSOUND direct_sound;
  LPDIRECTSOUNDCAPTURE direct_sound_capture;
  LPDIRECTSOUNDBUFFER primary_buffer;

  /* sound output */
  LPDIRECTSOUNDBUFFER dsBuffer;
  LPDIRECTSOUNDNOTIFY notify;
  DSBPOSITIONNOTIFY* position;
  HANDLE* event;
  int cur_buffer;

  /* sound input */
  LPDIRECTSOUNDCAPTUREBUFFER dscBuffer;
  LPDIRECTSOUNDNOTIFY cnotify;
  DSBPOSITIONNOTIFY* cposition;
  HANDLE* cevent;
  int cur_cbuffer;

} dsaudioport_t;

static void dsaudioport_input( fts_audioport_t* port, float** buffers, int buffsize);
static void dsaudioport_output(fts_audioport_t* port, float** buffers, int buffsize);
static int dsaudioport_xrun(fts_audioport_t *port);

static void dsaudioport_open_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_open_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_close_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_close_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);


static void dsaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_cleanup(dsaudioport_t *dev);
static void dsaudioport_instantiate(fts_class_t *cl);



static void
dsaudioport_input(fts_audioport_t* port, float** buffers, int buffsize)
{
  dsaudioport_t *dev;
  int n, channels, ch, i, j;

#if CAPTURE
  short *buf1, *buf2;
  DWORD bytes1, bytes2;    
  int boffset, soffset;
  HRESULT hr;
  FILE* log; /* FIXME */
  int print_log = 0;
#endif

 
  dev = (dsaudioport_t *)port;
  if (dev->state != dsaudioport_running) 
  {
    /* FIXME */
    return;
  }

  n = buffsize;
  channels = fts_audioport_get_channels(port, FTS_AUDIO_INPUT);

#if CAPTURE

  /* Wait till the next buffer becomes available */
  WaitForSingleObject(dev->cevent[dev->cur_cbuffer], INFINITE);

  /* Calculate the byte and sample offset in the fifo buffer */
  if (dev->cur_cbuffer == 0) {
    boffset = (dev->num_buffers - 1) * dev->buffer_byte_size;    
    soffset = (dev->num_buffers - 1) * dev->buffer_sample_size;
  } else {
    boffset = (dev->cur_cbuffer - 1) * dev->buffer_byte_size;    
    soffset = (dev->cur_cbuffer - 1) * dev->buffer_sample_size;    
  }

  if (print_log) { /* FIXME */
    fts_log ("[dsaudioport] cur=%d\n", dev->cur_cbuffer);
  }

  /* Lock */
  hr = IDirectSoundCaptureBuffer_Lock(dev->dscBuffer, boffset, dev->buffer_byte_size, 
				      (void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);

  for (ch = 0; ch < channels; ch++) {
    float *out = buffers[ch];
    
    for (i = 0, j = ch; i < n; i++, j += channels) {
      out[i] = (float) buf1[j] / 32767.0f;

      if (print_log) { /* FIXME */
	fprintf(log, "[%d,%d,%d]=%d\n", ch, i, j, buf1[j]);
      }
    }
  }
  
  /* Unlock */
  IDirectSoundCaptureBuffer_Unlock(dev->dscBuffer, buf1, bytes1, buf2, bytes2);

  if (print_log) { /* FIXME */
    fclose(log);
  }

#else

  for (ch = 0; ch < channels; ch++) {
    float *out = buffers[ch];
    
    for (i = 0, j = ch; i < n; i++, j += channels) {
      out[i] = 0.0f;
    }
  }
#endif
  
  dev->cur_cbuffer++;
  if (dev->cur_cbuffer == dev->num_buffers) {
    dev->cur_cbuffer = 0;
  }
}
    

static void
dsaudioport_output(fts_audioport_t* port, float** buffers, int buffsize)
{
  dsaudioport_t *dev;
  short *buf1, *buf2;
  DWORD bytes1, bytes2;    
  int boffset, soffset, n, channels, ch, i, j;

  dev = (dsaudioport_t *)port;
  if (dev->state != dsaudioport_running) 
  {
    fts_log("[dsaudioport] output fun but dev->state != dsaudioport_running\n");
    /* FIXME */
    return;
  }

  channels = fts_audioport_get_channels(port, FTS_AUDIO_OUTPUT);

  /* Wait till the next buffer becomes available */
  WaitForSingleObject(dev->event[dev->cur_buffer], INFINITE);

  /* Calculate the byte and sample offset in the fifo buffer */
  if (dev->cur_buffer == 0) 
  {
    boffset = (dev->num_buffers - 1) * dev->buffer_byte_size;    
    soffset = (dev->num_buffers - 1) * dev->buffer_sample_size;
  } 
  else 
  {
    boffset = (dev->cur_buffer - 1) * dev->buffer_byte_size;    
    soffset = (dev->cur_buffer - 1) * dev->buffer_sample_size;    
  }

  /* Lock */
  IDirectSoundBuffer_Lock(dev->dsBuffer, boffset, dev->buffer_byte_size, 
			  (void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);

  /* Interleave the sample buffer into the output buffer */
  for (ch = 0; ch < channels; ch++) 
  {
    float *in = buffers[ch];
    
    for (i = 0, j = ch; i < buffsize; i++, j += channels) 
    {
      buf1[j] = (short) (32767.0f * in[i]);
    }
  }

  /* Unlock */
  IDirectSoundBuffer_Unlock(dev->dsBuffer, buf1, bytes1, buf2, bytes2);
  
  dev->cur_buffer++;
  if (dev->cur_buffer == dev->num_buffers) 
  {
    dev->cur_buffer = 0;
  }
}


static int 
dsaudioport_xrun(fts_audioport_t *port)
{
  dsaudioport_t *dev = (dsaudioport_t *)port;
  
  if (!dev->no_xrun_message_already_posted) 
  {
    fts_post( "Warning: the audio device does not yet support out-of-sync detection\n");
    fts_post( "         Synchronisation errors (\"dac slip\") will not be reported\n");
    dev->no_xrun_message_already_posted = 1;
  }
  
  return 0;
}


static BOOL
dsaudioport_scan_input_device(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
  fts_object_t* device;
  int ac = 3;
  fts_atom_t at[3];
  fts_symbol_t device_name;

  fts_log("[dsaudioport]: try to add %s \n", lpcstrDescription);
  device_name = fts_new_symbol(lpcstrDescription);
  
  fts_set_symbol(at, device_name);
  fts_set_pointer(at + 1, (void*)lpGuid);
  fts_set_symbol(at + 2, fts_s_input);

  device = fts_object_create(dsaudioport_type, ac, at);
  if (device != NULL)
  {
    fts_object_refer(device);
    fts_audiomanager_put_port(device_name, (fts_audioport_t*)device);
  }
  else
  {
    fts_log("[dsaudioport] cannot add %s \n", lpcstrDescription);
  }
  return 1;
}

static BOOL
dsaudioport_scan_output_device(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
  fts_object_t* device;
  int ac = 3;
  fts_atom_t at[3];
  fts_symbol_t device_name;

  fts_log("[dsaudioport]: try to add %s \n", lpcstrDescription);
  device_name = fts_new_symbol(lpcstrDescription);
  
  fts_set_symbol(at, device_name);
  fts_set_pointer(at + 1, (void*)lpGuid);
  fts_set_symbol(at + 2, fts_s_output);
  device = fts_object_create(dsaudioport_type, ac, at);
  if (device != NULL)
  {
    fts_object_refer(device);
    fts_audiomanager_put_port(device_name, (fts_audioport_t*)device);
  }
  else
  {
    fts_log("[dsaudioport] cannot add %s \n", lpcstrDescription);
  }
  return 1;
}

static BOOL 
dsaudioport_enum_callback(LPGUID lpGuid, LPCSTR lpcstrDescription, 
			  LPCSTR lpcstrModule, LPVOID lpContext)
{
  fts_log("[dsaudioport]: %s\n", lpcstrDescription);
  return 1;
}

static void
dsaudioport_open_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  dsaudioport_t* dev = (dsaudioport_t*)o;
  HRESULT hr;

  /* do something when when you want to open input */
  fts_log("[dsaudioport] dsaudioport_open_input\n");

  hr = IDirectSoundCaptureBuffer_Start(dev->dscBuffer, DSCBSTART_LOOPING);
  fts_audioport_set_open((fts_audioport_t*)o, FTS_AUDIO_INPUT);
}

static void
dsaudioport_open_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  dsaudioport_t* dev = (dsaudioport_t*)o;
  HRESULT hr;
  /* do something when when you want to open output */
  fts_log("[dsaudioport] dsaudioport_open_output\n");

  hr = IDirectSoundBuffer_Play(dev->dsBuffer, 0, 0, DSBPLAY_LOOPING);
  if (hr != S_OK) {
    
    /* FIXME */
  }

  dev->state = dsaudioport_running;


  fts_audioport_set_open((fts_audioport_t*)o, FTS_AUDIO_OUTPUT);
}

static void
dsaudioport_close_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  dsaudioport_t* dev = (dsaudioport_t*)o;
  /* do something when you want to close input */
  IDirectSoundCaptureBuffer_Stop(dev->dscBuffer);

  fts_audioport_unset_open((fts_audioport_t*)o, FTS_AUDIO_INPUT);
}

static void
dsaudioport_close_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  dsaudioport_t* dev = (dsaudioport_t*)o;
  short* buf1;
  short* buf2;
  DWORD bytes1;
  DWORD bytes2;
  int boffset, soffset;
  int i;

  /* Calculate the byte and sample offset in the fifo buffer */
  if (dev->cur_buffer == 0) 
  {
    boffset = (dev->num_buffers - 1) * dev->buffer_byte_size;    
    soffset = (dev->num_buffers - 1) * dev->buffer_sample_size;
  } 
  else 
  {
    boffset = (dev->cur_buffer - 1) * dev->buffer_byte_size;    
    soffset = (dev->cur_buffer - 1) * dev->buffer_sample_size;    
  }

  /* Lock buffer */
  IDirectSoundBuffer_Lock(dev->dsBuffer, boffset, dev->buffer_byte_size, (void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);
  /* clear buffer */
  for (i = 0; i < dev->buffer_byte_size; ++i)
    {
      buf1[i] = 0;
    }
  /* Unlock */
  IDirectSoundBuffer_Unlock(dev->dsBuffer, buf1, bytes1, buf2, bytes2);
  /* do something when you want to close output */
  IDirectSoundBuffer_Stop(dev->dsBuffer);

  fts_audioport_unset_open((fts_audioport_t*)o, FTS_AUDIO_OUTPUT);
}


static void
dsaudioport_input_init(dsaudioport_t* dev, fts_symbol_t device_name, LPGUID guid)
{
  int channels;
  int sample_rate;
  int frames;
  int fifo_size;
  int i;
  fts_object_t* o = (fts_object_t*)dev;
  HRESULT hr;
  int err = 0;
  DSCBUFFERDESC dscbd;

  /* Create Sound Capture Object */
  hr = DirectSoundCaptureCreate(guid, &dev->direct_sound_capture, NULL);
  if (DS_OK != err)
    {
      fts_object_error(o, "Cannot create the DirectSoundCapture object");
      fts_log("[dsaudioport] Cannot create the DirectSoundCapture object\n");
      return;
    }

  /* get the basic audio settings */
  sample_rate = (int)fts_dsp_get_sample_rate();
  channels = DEFAULT_CHANNELS;
  frames = fts_dsp_get_tick_size();
  dev->buffer_sample_size = channels * frames;
  dev->buffer_byte_size = dev->buffer_sample_size * sizeof(short);

  fifo_size = DEFAULT_FIFO_SIZE;
  dev->num_buffers = 0;
  dev->num_buffers = fifo_size / frames;
  if (fifo_size % frames) 
    {
      dev->num_buffers++;
    }

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
  
  /* Create Sound Capture Buffer */
  ZeroMemory(&dscbd, sizeof(dscbd));
  dscbd.dwSize = sizeof(dscbd);
  dscbd.lpwfxFormat = dev->format;
  dscbd.dwBufferBytes = dev->buffer_byte_size * dev->num_buffers;

  err = IDirectSoundCapture_CreateCaptureBuffer(dev->direct_sound_capture, &dscbd, &dev->dscBuffer, NULL);
  if (DS_OK != err)
    {
      fts_object_error(o, "Cannot create the DirectSoundCapture Buffer");
      fts_log("[dsaudioport] Cannot create the DirectSoundCapture Buffer\n");
      switch(err)
	{
	case DSERR_INVALIDPARAM:
	  fts_log("[dsaudioport] An invalid parameter was passed \n");
	  break;
	case DSERR_BADFORMAT:
	  fts_log("[dsaudioport] The specified wave format is not supported\n");
	  break;
	case DSERR_GENERIC:
	  fts_log("[dsaudioport] Undetermined error occured \n");
	  break;
	case DSERR_NODRIVER:
	  fts_log("[dsaudioport] No sound driver available \n");
	  break;
	case DSERR_OUTOFMEMORY:
	  fts_log("[dsaudioport] The DirectSound subsystem could not allocate sufficient "
		  " memory \n");
	  break;
	case DSERR_UNINITIALIZED:
	  fts_log("[dsaudioport] The Initialize method has not been called successfully \n");
	  break;
	default:
	  break;
	}
      return;
    }

  /* allocate an array for the notification events */
  dev->cevent = (HANDLE*) fts_malloc(dev->num_buffers * sizeof(HANDLE));
  ZeroMemory(dev->cevent, dev->num_buffers * sizeof(HANDLE));
  
  /* allocated an array for the notification positions */
  dev->cposition = (DSBPOSITIONNOTIFY*) fts_malloc(dev->num_buffers * sizeof(DSBPOSITIONNOTIFY));
  /* create and initialize the notification events and positions */
  ZeroMemory(dev->cposition, dev->num_buffers * sizeof(DSBPOSITIONNOTIFY));
  for (i = 0; i < dev->num_buffers; i++) {
    dev->cevent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (dev->cevent[i] == NULL) {
      fts_object_error(o, "error opening DirectSound device (failed to create the notifications events)");
      goto error_recovery;
    }
    dev->cposition[i].dwOffset = i * dev->buffer_byte_size;
    dev->cposition[i].hEventNotify = dev->cevent[i];
  }
  
  /* set the notification position for the synchronisation with the audio device */
  hr = IDirectSoundBuffer_QueryInterface(dev->dscBuffer, &IID_IDirectSoundNotify, (LPVOID*) &dev->cnotify);
  if (hr != S_OK) {
    dev->cnotify = NULL;
    fts_object_error(o, "error opening DirectSound device (failed to create notify interface: %s)", fts_win32_error(hr));
    goto error_recovery;
  }
  hr = IDirectSoundNotify_SetNotificationPositions(dev->cnotify, dev->num_buffers, dev->cposition);
  if (hr != S_OK) {
    fts_object_error(o, "error opening DirectSound device (failed to set notify positions: %s)", fts_win32_error(hr));
    goto error_recovery;
  }
error_recovery:
  /* dsaudioport_cleanup(dev); */

  fts_audioport_set_channels((fts_audioport_t*)dev, FTS_AUDIO_INPUT, channels);
  fts_audioport_set_io_fun((fts_audioport_t*)dev, FTS_AUDIO_INPUT, dsaudioport_input);
  fts_audioport_set_valid((fts_audioport_t*)dev, FTS_AUDIO_INPUT);
}

static void 
dsaudioport_output_init(dsaudioport_t* dev, fts_symbol_t device_name, LPGUID guid)
{
  int channels, sample_rate, i, fifo_size, frames;
  fts_object_t* o = (fts_object_t*)dev;
  HWND wnd = fts_wnd;
  HRESULT hr;
  DSCAPS caps;
  DSBUFFERDESC desc;
  DSBUFFERDESC desc_primary;

  /* allocate space for the audio port object: DONE in instantiate */
  /* we can't use memset(0) since the object's head is already
     initialized */
  dev->format = NULL;
  dev->state = 0; 
  dev->buffer_byte_size = 0;
  dev->buffer_sample_size = 0;
  dev->num_buffers = 0;
  dev->no_xrun_message_already_posted = 0;
  dev->dsBuffer = NULL;
  dev->notify = NULL;
  dev->position = NULL;
  dev->event = NULL;
  dev->cur_buffer = 0;
  dev->dscBuffer = NULL;
  dev->cnotify = NULL;
  dev->cposition = NULL;
  dev->cevent = NULL;
  dev->cur_cbuffer = 1;
  
  /* initialize the audio port object */
  dev->guid = guid;
  dev->direct_sound = NULL;
  dev->direct_sound_capture = NULL;
  
    
  /* Create the DirectSound object with the given GUID */
  hr = DirectSoundCreate(dev->guid, &dev->direct_sound, NULL);
  if (hr != DS_OK)
  {
    fts_object_error(o, "Cannot create the DirectSound object");
    fts_log("[dsaudioport] Cannot create the DirectSound object\n");
    return;
  }
  
  /* Set the cooperative level */
  hr = IDirectSound_SetCooperativeLevel(dev->direct_sound, wnd, DSSCL_PRIORITY);
  if (hr != DS_OK)
  {
    fts_object_error(o, "Cannot set cooperative level");
    fts_log("[dsaudioport] Cannot set cooperative level\n");
    return;
  }
  
  /* Get the capabilites of the device */
  caps.dwSize = sizeof(caps);
  hr = IDirectSound_GetCaps(dev->direct_sound, &caps);
  if (DS_OK == hr)
  {
    /* Use the information about the device capabilities */
  }
  
  /* get the basic audio settings */
  sample_rate = (int) fts_dsp_get_sample_rate();
  channels = DEFAULT_CHANNELS;
  
  frames = fts_dsp_get_tick_size();
  dev->buffer_sample_size = channels * frames;
  dev->buffer_byte_size = dev->buffer_sample_size * sizeof(short);
  
  fifo_size = DEFAULT_FIFO_SIZE;
  
  dev->num_buffers = fifo_size / frames;
  if (fifo_size % frames) {
    dev->num_buffers++;
  }
  /* create the primary buffer */
  /* initialize the primary buffer description */
  ZeroMemory(&desc_primary, sizeof(DSBUFFERDESC));
  desc_primary.dwSize = sizeof(DSBUFFERDESC);
  desc_primary.dwFlags = DSBCAPS_PRIMARYBUFFER;
  
  /* create the sound output buffer */
  hr = IDirectSound_CreateSoundBuffer(dev->direct_sound, &desc_primary, &dev->primary_buffer, NULL);
  if (hr != DS_OK) {
    fts_object_error(o, "error opening DirectSound device (failed to create the secondary buffer)");
    goto error_recovery;
  }
  
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
  
  /* set the primary sound buffer to this format */
  hr = IDirectSoundBuffer_SetFormat(dev->primary_buffer, dev->format);
  if (hr != DS_OK) {
    /* just print a warning. this error is not fatal. */
    fts_post("Warning: dsaudioport: can't set format of primary sound buffer: %s\n", fts_win32_error(hr));
  }
  

  /*************************** output ********************************/
  
  /* initialize the output buffer description */
  ZeroMemory(&desc, sizeof(DSBUFFERDESC));
  desc.dwSize = sizeof(DSBUFFERDESC);
  desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY;
  desc.lpwfxFormat = dev->format;
  desc.dwBufferBytes = dev->buffer_byte_size * dev->num_buffers;  
  desc.dwReserved = 0; 
  
  /* create the sound output buffer */
  hr = IDirectSound_CreateSoundBuffer(dev->direct_sound, &desc, &dev->dsBuffer, NULL);
  if (hr != DS_OK) {
    fts_object_error(o, "error opening DirectSound device (failed to create the secondary buffer)");
    goto error_recovery;
  }
  
  /* allocate an array for the notification events */
  dev->event = (HANDLE*) fts_malloc(dev->num_buffers * sizeof(HANDLE));
  ZeroMemory(dev->event, dev->num_buffers * sizeof(HANDLE));
  
  /* allocate an array for the notification positions */
  dev->position = (DSBPOSITIONNOTIFY*) fts_malloc(dev->num_buffers * sizeof(DSBPOSITIONNOTIFY));

  /* create and initialize the notification events and positions */
  ZeroMemory(dev->position, dev->num_buffers * sizeof(DSBPOSITIONNOTIFY));
  for (i = 0; i < dev->num_buffers; i++) {
    dev->event[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (dev->event[i] == NULL) {
      fts_object_error(o, "error opening DirectSound device (failed to create the notifications events)");
      goto error_recovery;
    }
    dev->position[i].dwOffset = i * dev->buffer_byte_size;
    dev->position[i].hEventNotify = dev->event[i];
  }
  
  /* set the notification position for the synchronisation with the audio device */
  hr = IDirectSoundBuffer_QueryInterface(dev->dsBuffer, &IID_IDirectSoundNotify, (LPVOID*) &dev->notify);
  if (hr != S_OK) {
    dev->notify = NULL;
    fts_object_error(o, "error opening DirectSound device (failed to create notify interface: %s)", fts_win32_error(hr));
    goto error_recovery;
  }
  hr = IDirectSoundNotify_SetNotificationPositions(dev->notify, dev->num_buffers, dev->position);
  if (hr != S_OK) {
    fts_object_error(o, "error opening DirectSound device (failed to set notify positions: %s)", fts_win32_error(hr));
    goto error_recovery;
  }
error_recovery:
  /* dsaudioport_cleanup(dev); */
  
  
  fts_audioport_set_channels((fts_audioport_t*)dev, FTS_AUDIO_OUTPUT, channels);
  fts_audioport_set_io_fun((fts_audioport_t*)dev, FTS_AUDIO_OUTPUT, dsaudioport_output);
  fts_audioport_set_valid((fts_audioport_t*)dev, FTS_AUDIO_OUTPUT);

}


static void 
dsaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsaudioport_t* dev = (dsaudioport_t*)o;
  fts_symbol_t device_name = fts_get_symbol(at);
  LPGUID guid = (LPGUID)fts_get_pointer(at + 1);
  fts_symbol_t input_or_output = fts_get_symbol(at + 2);

  /* initialize the audioport */
  fts_audioport_init((fts_audioport_t*)dev);

  /* we don't support yet input device */
  if (fts_s_input == input_or_output)
  {
    dsaudioport_input_init(dev, device_name, guid);
  }
  else
  {
    dsaudioport_output_init(dev, device_name, guid);
  }

}


static void 
dsaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsaudioport_t* dev = (dsaudioport_t*)o;
  if (dev->direct_sound != NULL)
  {
    IDirectSound_Release(dev->direct_sound);
  }
  fts_audioport_delete((fts_audioport_t*)o);
  /* object allocated memery is freed by destructor */
}


static void dsaudioport_cleanup(dsaudioport_t *dev)
{
  int i;

#if CAPTURE

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
    for (i = 0; i < dev->num_buffers; i++) {
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
#endif


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
    for (i = 0; i < dev->num_buffers; i++) {
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

  if (dev->format != NULL) {
    fts_free(dev->format);
    dev->format = NULL;
  }
}


static void 
dsaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dsaudioport_t), dsaudioport_init, dsaudioport_delete);

  /* open messages */
  fts_class_message_varargs(cl, fts_s_open_input, dsaudioport_open_input);
  fts_class_message_varargs(cl, fts_s_open_output, dsaudioport_open_output);

  /* close messages */
  fts_class_message_varargs(cl, fts_s_close_input, dsaudioport_close_input);
  fts_class_message_varargs(cl, fts_s_close_output, dsaudioport_close_output);

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

static int 
fts_open_direct_sound(char *device) 
{
  HRESULT hr;
  DSBUFFERDESC desc;
  int err = 0;

  /*
   * create DirectSound object for sound output
   */

  hr = DirectSoundCreate(NULL, &fts_direct_sound, NULL);
  if (hr != DS_OK) {
    fts_post( "Warning: dsaudioport: failed to create direct sound\n");
    fts_direct_sound = NULL;
    return -2;
  }

  hr = IDirectSound_SetCooperativeLevel(fts_direct_sound, fts_wnd, DSSCL_PRIORITY);
  if (hr != DS_OK) {
    fts_post( "Warning: dsaudioport: failed to create set the cooperative level\n");
    IDirectSound_Release(fts_direct_sound); 
    fts_direct_sound = NULL;
    return -3;
  }
  ZeroMemory(&desc, sizeof(DSBUFFERDESC));

  desc.dwSize = sizeof(DSBUFFERDESC);
  desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

  hr = IDirectSound_CreateSoundBuffer(fts_direct_sound, &desc, &fts_primary_buffer, NULL);
  if (hr != DS_OK) {
    fts_post( "Warning: dsaudioport: failed to create set the primary sound buffer\n");
    IDirectSound_Release(fts_direct_sound); 
    fts_direct_sound = NULL;
    fts_primary_buffer = NULL;
    return -4;
  }

  return 0;  
}


static int 
fts_close_direct_sound() 
{
  if (fts_primary_buffer != NULL) {
    IDirectSoundBuffer_Release(fts_primary_buffer); 
    fts_primary_buffer = NULL;
  }
  if (fts_direct_sound != NULL) {
    IDirectSound_Release(fts_direct_sound); 
    fts_direct_sound = NULL;
  }
  return 0;
}

int 
fts_open_direct_sound_capture(char *device)
{
#if CAPTURE
  HRESULT hr;
  int err = 0;

  /*
   * create DirectSoundCapture object for sound input
   */

  hr = DirectSoundCaptureCreate(NULL, &fts_direct_sound_capture, NULL);
  if (hr != DS_OK) {
    fts_post( "Warning: dsaudioport: failed to create direct sound capture\n");
    fts_direct_sound_capture = NULL;
    return -1;
  }
#endif

  return 0;
}

int 
fts_close_direct_sound_capture()
{
#if CAPTURE
  if (fts_direct_sound_capture != NULL) {
    IDirectSoundCapture_Release(fts_direct_sound_capture); 
    fts_direct_sound_capture = NULL;
  }
#endif
  return 0;
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
  if (!RegisterClass(&myClass)) 
  {
    fts_post( "Warning: dsaudioport: failed to register the window class\n");
    return -100;
  }
  fts_wnd = CreateWindow((LPSTR) "FtsDsDev", (LPSTR) "FtsDsDev", WS_OVERLAPPEDWINDOW,
			 CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, (HWND) NULL, (HMENU) NULL, 
			 dsdev_instance, (LPSTR) NULL);  
  if (fts_wnd == NULL) 
  {
    fts_post( "Warning: dsaudioport: failed to create the window\n");
    return -101;
  }
  return 0;
}

static int 
fts_win32_destroy_window() 
{
  if (fts_wnd != NULL) 
  {
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
    fts_log("[dsdev]: Cleaning up\n");
    fts_win32_destroy_window();
    fts_close_direct_sound_capture();
    fts_close_direct_sound();
    fts_log("[dsdev]: Done\n");
  }
  return TRUE;
}

void 
dsaudioport_config(void)
{
  fts_symbol_t dsaudioport_symbol;
  fts_object_t* default_dev;

  /* make sure we have a valid instance handle */
  if (dsdev_instance == NULL) 
  {
    fts_post("Warning: dsaudioport: invalid DLL instance handle\n");
    fts_log("[dsaudioport] Warning: dsaudioport: invalid DLL instance handle\n");
    return;
  }

  /* create an invisible window */
  if ((fts_wnd == NULL) 
      && (fts_win32_create_window() != 0)) {
    return;
  }

  dsaudioport_symbol = fts_new_symbol("dsaudioport");
  dsaudioport_type = fts_class_install( dsaudioport_symbol, dsaudioport_instantiate);

  /* scan for input device */
  fts_log("[dsaudioport]: Scan audio output devices\n"); 
  DirectSoundEnumerate((LPDSENUMCALLBACK) dsaudioport_scan_output_device, NULL); 
    
  /* scan for output device */
  fts_log("[dsaudioport]: Scan audio input devices\n"); 
  DirectSoundCaptureEnumerate((LPDSENUMCALLBACK) dsaudioport_scan_input_device, NULL);

}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
