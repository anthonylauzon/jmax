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

static void dsaudioport_input( fts_word_t *argv);
static void dsaudioport_output(fts_word_t *argv);
static int dsaudioport_xrun(fts_audioport_t *port);
static void dsaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void dsaudioport_cleanup(dsaudioport_t *dev);
static void dsaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value);
static fts_status_t dsaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);



static void 
dsaudioport_input( fts_word_t *argv)
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

 
  dev = (dsaudioport_t *) fts_word_get_ptr(argv+0);
  if (dev->state != dsaudioport_running) {
    /* FIXME */
    return;
  }

  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_input_channels(dev);

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
    log = fopen("C:\\dsaudioport.txt", "a");
    fprintf(log, "cur=%d\n", dev->cur_cbuffer);
  }

  /* Lock */
  hr = IDirectSoundCaptureBuffer_Lock(dev->dscBuffer, boffset, dev->buffer_byte_size, 
				      (void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);

  for (ch = 0; ch < channels; ch++) {
    float *out = (float *) fts_word_get_ptr(argv + 2 + ch);
    
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
    float *out = (float *) fts_word_get_ptr(argv + 2 + ch);
    
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
dsaudioport_output(fts_word_t *argv)
{
  dsaudioport_t *dev;
  short *buf1, *buf2;
  DWORD bytes1, bytes2;    
  int boffset, soffset, n, channels, ch, i, j;

  dev = (dsaudioport_t *) fts_word_get_ptr(argv+0);
  if (dev->state != dsaudioport_running) {
    /* FIXME */
    return;
  }

  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_output_channels(dev);

  /* Wait till the next buffer becomes available */
  WaitForSingleObject(dev->event[dev->cur_buffer], INFINITE);

  /* Calculate the byte and sample offset in the fifo buffer */
  if (dev->cur_buffer == 0) {
    boffset = (dev->num_buffers - 1) * dev->buffer_byte_size;    
    soffset = (dev->num_buffers - 1) * dev->buffer_sample_size;
  } else {
    boffset = (dev->cur_buffer - 1) * dev->buffer_byte_size;    
    soffset = (dev->cur_buffer - 1) * dev->buffer_sample_size;    
  }

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

static int 
dsaudioport_xrun(fts_audioport_t *port)
{
  dsaudioport_t *dev = (dsaudioport_t *)port;
  
  if (!dev->no_xrun_message_already_posted) {
    post( "Warning: the audio device does not yet support out-of-sync detection\n");
    post( "         Synchronisation errors (\"dac slip\") will not be reported\n");
    dev->no_xrun_message_already_posted = 1;
  }
  
  return 0;
}

static void 
dsaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int channels, sample_rate, i, fifo_size, frames;
  dsaudioport_t *dev = (dsaudioport_t *)o;
  DSBUFFERDESC desc;
  HRESULT hr;
#if CAPTURE
  DSCBUFFERDESC cdesc;
#endif

  ac--;
  at++;

  fts_log("[dsaudioport]: Opening audio port\n");

  /* initialize the audioport */
  fts_audioport_init(&dev->head);

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


  /* make sure that the module was initialized correctly */
  if (fts_direct_sound == NULL) {
    fts_object_set_error(o, "Error opening DirectSound device (failed to create the DirectSound object)");
    return;
  }
  if (fts_primary_buffer == NULL) {
    fts_object_set_error(o, "Error opening DirectSound device (failed to create the primary buffer object)");
    return;
  }
#if CAPTURE
  if (fts_direct_sound_capture == NULL) {
    fts_object_set_error(o, "Error opening DirectSound device (failed to create the DirectSoundCapture object)");
    return;
  }
#endif

  fts_audioport_set_xrun_function((fts_audioport_t *) dev, dsaudioport_xrun);

  /*************************** settings and format ********************************/

  /* get the basic audio settings */
  sample_rate = (int) fts_dsp_get_sample_rate();
  channels = fts_get_int_arg(ac, at, 0, DEFAULT_CHANNELS);

  frames = fts_dsp_get_tick_size();
  dev->buffer_sample_size = channels * frames;
  dev->buffer_byte_size = dev->buffer_sample_size * sizeof(short);

  fifo_size = fts_param_get_int(fts_s_fifo_size, DEFAULT_FIFO_SIZE);

  dev->num_buffers = fifo_size / frames;
  if (fifo_size % frames) {
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

  /* set the primary sound buffer to this format */
  hr = IDirectSoundBuffer_SetFormat(fts_primary_buffer, dev->format);
  if (hr != DS_OK) {
    /* just print a warning. this error is not fatal. */
    post("Warning: dsaudioport: can't set format of primary sound buffer: %s\n", fts_win32_error(hr));
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
  hr = IDirectSound_CreateSoundBuffer(fts_direct_sound, &desc, &dev->dsBuffer, NULL);
  if (hr != DS_OK) {
    fts_object_set_error(o, "Error opening DirectSound device (failed to create the secondary buffer)");
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
      fts_object_set_error(o, "Error opening DirectSound device (failed to create the notifications events)");
      goto error_recovery;
    }
    dev->position[i].dwOffset = i * dev->buffer_byte_size;
    dev->position[i].hEventNotify = dev->event[i];
  }

  /* set the notification position for the synchronisation with the audio device */
  hr = IDirectSoundBuffer_QueryInterface(dev->dsBuffer, &IID_IDirectSoundNotify, (LPVOID*) &dev->notify);
  if (hr != S_OK) {
    dev->notify = NULL;
    fts_object_set_error(o, "Error opening DirectSound device (failed to create notify interface: %s)", fts_win32_error(hr));
    goto error_recovery;
  }
  hr = IDirectSoundNotify_SetNotificationPositions(dev->notify, dev->num_buffers, dev->position);
  if (hr != S_OK) {
    fts_object_set_error(o, "Error opening DirectSound device (failed to set notify positions: %s)", fts_win32_error(hr));
    goto error_recovery;
  }

  fts_audioport_set_output_channels((fts_audioport_t *) dev, channels);
  fts_audioport_set_output_function((fts_audioport_t *) dev, dsaudioport_output);

  /*************************** input ********************************/
#if CAPTURE

  /* initialize the input buffer description */
  ZeroMemory(&cdesc, sizeof(DSCBUFFERDESC));
  cdesc.dwSize = sizeof(DSCBUFFERDESC);
  cdesc.dwFlags = DSCBCAPS_WAVEMAPPED;
  cdesc.lpwfxFormat = dev->format;
  cdesc.dwBufferBytes = dev->buffer_byte_size * dev->num_buffers;  
  cdesc.dwReserved = 0; 

  /* create the sound input buffer */
  hr = IDirectSoundCapture_CreateCaptureBuffer(fts_direct_sound_capture, &cdesc, &dev->dscBuffer, NULL);
  if (hr != DS_OK) {
    fts_object_set_error(o, "Error opening DirectSound device (failed to create the capture buffer)");
    goto error_recovery;
  }

  /* allocate an array for the notification events */
  dev->cevent = (HANDLE*) fts_malloc(dev->num_buffers * sizeof(HANDLE));
  ZeroMemory(dev->cevent, dev->num_buffers * sizeof(HANDLE));

  /* allocate an array for the notification positions */
  dev->cposition = (DSBPOSITIONNOTIFY*) fts_malloc(dev->num_buffers * sizeof(DSBPOSITIONNOTIFY));

  /* create and initialize the notification events and positions */
  ZeroMemory(dev->cposition, dev->num_buffers * sizeof(DSBPOSITIONNOTIFY));
  for (i = 0; i < dev->num_buffers; i++) {
    dev->cevent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (dev->cevent[i] == NULL) {
      fts_object_set_error(o, "Error opening DirectSound device (failed to create the notifications events)");
      goto error_recovery;
    }
    dev->cposition[i].dwOffset = i * dev->buffer_byte_size;
    dev->cposition[i].hEventNotify = dev->cevent[i];
  }

  /* set the notification position for the synchronisation with the audio device */
  hr = IDirectSoundBuffer_QueryInterface(dev->dscBuffer, &IID_IDirectSoundNotify, (LPVOID*) &dev->cnotify);
  if (hr != S_OK) {
    dev->cnotify = NULL;
    fts_object_set_error(o, "Error opening DirectSound device (failed to create notify interface: %s)", fts_win32_error(hr));
    goto error_recovery;
  }
  hr = IDirectSoundNotify_SetNotificationPositions(dev->cnotify, dev->num_buffers, dev->cposition);
  if (hr != S_OK) {
    fts_object_set_error(o, "Error opening DirectSound device (failed to set notify positions: %s)", fts_win32_error(hr));
    goto error_recovery;
  }

  fts_audioport_set_input_channels((fts_audioport_t *) dev, channels);
  fts_audioport_set_input_function((fts_audioport_t *) dev, dsaudioport_input);
#endif

  /*************************** finish and start ********************************/

#if CAPTURE
  hr = IDirectSoundCaptureBuffer_Start(dev->dscBuffer, DSCBSTART_LOOPING);
  if (hr != S_OK) {
    /* FIXME */
  }
#endif

  hr = IDirectSoundBuffer_Play(dev->dsBuffer, 0, 0, DSBPLAY_LOOPING);
  if (hr != S_OK) {
    /* FIXME */
  }

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
    post( "Warning: dsaudioport: failed to create direct sound\n");
    fts_direct_sound = NULL;
    return -2;
  }

  hr = IDirectSound_SetCooperativeLevel(fts_direct_sound, fts_wnd, DSSCL_PRIORITY);
  if (hr != DS_OK) {
    post( "Warning: dsaudioport: failed to create set the cooperative level\n");
    IDirectSound_Release(fts_direct_sound); 
    fts_direct_sound = NULL;
    return -3;
  }
  ZeroMemory(&desc, sizeof(DSBUFFERDESC));

  desc.dwSize = sizeof(DSBUFFERDESC);
  desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

  hr = IDirectSound_CreateSoundBuffer(fts_direct_sound, &desc, &fts_primary_buffer, NULL);
  if (hr != DS_OK) {
    post( "Warning: dsaudioport: failed to create set the primary sound buffer\n");
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
    post( "Warning: dsaudioport: failed to create direct sound capture\n");
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

  /* open DirectSound */
  if ((fts_direct_sound == NULL) 
      && (fts_open_direct_sound(NULL) != 0)) {
    return;
  }

  /* open DirectSoundCapture */
  if ((fts_direct_sound_capture == NULL) 
      && (fts_open_direct_sound_capture(NULL) != 0)) {
    return;
  }

  fts_class_install( fts_new_symbol("dsaudioport"), dsaudioport_instantiate);
}
