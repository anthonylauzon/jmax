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

static HINSTANCE dsdev_instance = NULL;
static HWND fts_wnd = NULL;
static LPDIRECTSOUND fts_direct_sound = NULL;
static LPDIRECTSOUNDBUFFER fts_primary_buffer = NULL;

int fts_open_direct_sound(char *device);
int fts_close_direct_sound();
int fts_win32_create_window();
int fts_win32_destroy_window();
long FAR PASCAL fts_win32_wndproc(HWND hWnd, unsigned message, WPARAM wParam, LPARAM lParam);
char* fts_win32_error(HRESULT hr);

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 2048
#define DEFAULT_CHANNELS 2

enum {
  dsaudioport_clean,
  dsaudioport_running,
  dsaudioport_stopping,
  dsaudioport_stopped,
  dsaudioport_corrupted
};

typedef struct {
  fts_audioport_t head;
  LPDIRECTSOUNDBUFFER dsBuffer;
  LPDIRECTSOUNDNOTIFY notify;
  WAVEFORMATEX* format;
  DSBUFFERDESC desc;
  DSBPOSITIONNOTIFY* position;
  HANDLE* event;
  int state; 
  int buffer_byte_size;
  int buffer_sample_size;
  int num_buffers;
  int cur_buffer;
  int no_xrun_message_already_posted;
} dsaudioport_t;

static void 
dsaudioport_input( fts_word_t *argv)
{
}

static void 
dsaudioport_output(fts_word_t *argv)
{
  dsaudioport_t *port;
  short *buf1, *buf2;
  DWORD bytes1, bytes2;    
  int boffset, soffset, n, channels, ch, i, j;

  port = (dsaudioport_t *) fts_word_get_ptr(argv+0);
  if (port->state != dsaudioport_running) {
    /* FIXME */
    return;
  }

  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_output_channels(port);

  /* Wait till the next buffer becomes available */
  WaitForSingleObject(port->event[port->cur_buffer], INFINITE);

  /* Calculate the byte and sample offset in the fifo buffer */
  if (port->cur_buffer == 0) {
    boffset = (port->num_buffers - 1) * port->buffer_byte_size;    
    soffset = (port->num_buffers - 1) * port->buffer_sample_size;
  } else {
    boffset = (port->cur_buffer - 1) * port->buffer_byte_size;    
    soffset = (port->cur_buffer - 1) * port->buffer_sample_size;    
  }

  /* Lock */
  IDirectSoundBuffer_Lock(port->dsBuffer, boffset, port->buffer_byte_size, (void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);

  /* Interleave the sample buffer into the output buffer */
  for (ch = 0; ch < channels; ch++) {
    float *in = (float *) fts_word_get_ptr(argv + 2 + ch);

    for (i = 0, j = ch; i < n; i++, j += channels) {
      buf1[j] = (short) (32767.0f * in[i]);
    }
  }
  
  /* Unlock */
  IDirectSoundBuffer_Unlock(port->dsBuffer, buf1, bytes1, buf2, bytes2);
  
  port->cur_buffer++;
  if (port->cur_buffer == port->num_buffers) {
    port->cur_buffer = 0;
  }
}

static int 
dsaudioport_xrun(fts_audioport_t *port)
{
  dsaudioport_t *dsport = (dsaudioport_t *)port;
  
  if (!dsport->no_xrun_message_already_posted) {
    post( "Warning: this device does not support SNDCTL_DSP_GETOPTR\n");
    post( "         Synchronisation errors (\"dac slip\") will not be reported\n");
    dsport->no_xrun_message_already_posted = 1;
  }
  
  return 0;
}

static void 
dsaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int channels, sample_rate, i, fifo_size, frames;
  dsaudioport_t *this = (dsaudioport_t *)o;
  HRESULT hr;

  ac--;
  at++;

  fts_audioport_init( &this->head);

  /* initialize everything to null */
  this->state = dsaudioport_clean;
  this->dsBuffer = NULL;
  this->notify = NULL;
  this->format = NULL;
  this->position = NULL;
  this->event = NULL;
  this->no_xrun_message_already_posted = 0;

  /* get the basic audio settings */
  sample_rate = (int) fts_dsp_get_sample_rate();
  channels = fts_get_int_arg( ac, at, 0, DEFAULT_CHANNELS);

  frames = fts_dsp_get_tick_size();
  this->buffer_sample_size = channels * frames;
  this->buffer_byte_size = this->buffer_sample_size * sizeof(short);
  this->cur_buffer = 0;

  fifo_size = fts_param_get_int(fts_s_fifo_size, DEFAULT_FIFO_SIZE);

  this->num_buffers = fifo_size / frames;
  if (fifo_size % frames) {
    this->num_buffers++;
  }

  /* create and initialize the buffer format */
  this->format = (WAVEFORMATEX*) malloc(sizeof(WAVEFORMATEX));
  ZeroMemory(this->format, sizeof(WAVEFORMATEX));
  this->format->wFormatTag = WAVE_FORMAT_PCM;
  this->format->nChannels = (unsigned short) channels;
  this->format->nSamplesPerSec = (DWORD) sample_rate;
  this->format->nBlockAlign = channels * sizeof(short);
  this->format->nAvgBytesPerSec = this->format->nSamplesPerSec * this->format->nBlockAlign;
  this->format->wBitsPerSample = sizeof(short) * 8; 
  this->format->cbSize = 0; 

  /* initialize the buffer description */
  ZeroMemory(&this->desc, sizeof(DSBUFFERDESC));
  this->desc.dwSize = sizeof(DSBUFFERDESC);
  this->desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY;
  this->desc.lpwfxFormat = this->format;
  this->desc.dwBufferBytes = this->buffer_byte_size * this->num_buffers;  

  /* set the primary sound buffer to this format */
/*    hr = IDirectSoundBuffer_SetFormat(fts_primary_buffer, this->format); */
/*    if (hr != DS_OK) { */
/*      post("Warning: dsaudioport: can't set format of primary sound buffer: %s\n", fts_win32_error(hr)); */
/*      goto error_recovery;     */
/*    } */

  /* create the sound buffer */
  hr = IDirectSound_CreateSoundBuffer(fts_direct_sound, &this->desc, &this->dsBuffer, NULL);
  if (hr != DS_OK) {
    post("Warning: dsaudioport: can't create sound buffer: %s\n", fts_win32_error(hr));
    goto error_recovery;
  }

  /* allocate an array for the notification events */
  this->event = (HANDLE*) malloc(this->num_buffers * sizeof(HANDLE));
  ZeroMemory(this->event, this->num_buffers * sizeof(HANDLE));

  /* allocate an array for the notification positions */
  this->position = (DSBPOSITIONNOTIFY*) malloc(this->num_buffers * sizeof(DSBPOSITIONNOTIFY));

  /* create and initialize the notification events and positions */
  ZeroMemory(this->position, this->num_buffers * sizeof(DSBPOSITIONNOTIFY));
  for (i = 0; i < this->num_buffers; i++) {
    this->event[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (this->event[i] == NULL) {
      post("Warning: dsaudioport: can't create the notifications events\n");
      goto error_recovery;
    }
    this->position[i].dwOffset = i * this->buffer_byte_size;
    this->position[i].hEventNotify = this->event[i];
  }

  /* set the notification position for the synchronisation with the audio device */
  hr = IDirectSoundBuffer_QueryInterface(this->dsBuffer, &IID_IDirectSoundNotify, (LPVOID*) &this->notify);
  if (hr != S_OK) {
    this->notify = NULL;
    post("Warning: dsaudioport: can't get notify interface: %s\n", fts_win32_error(hr));
    goto error_recovery;
  }
  hr = IDirectSoundNotify_SetNotificationPositions(this->notify, this->num_buffers, this->position);
  if (hr != S_OK) {
    post("Warning: dsaudioport: can't set notify positions: %s\n", fts_win32_error(hr));
    goto error_recovery;
  }

  fts_audioport_set_input_channels( (fts_audioport_t *)this, 0);
  fts_audioport_set_input_function( (fts_audioport_t *)this, NULL);
  fts_audioport_set_output_channels( (fts_audioport_t *)this, channels);
  fts_audioport_set_output_function( (fts_audioport_t *)this, dsaudioport_output);
  fts_audioport_set_xrun_function( (fts_audioport_t *)this, dsaudioport_xrun);

  IDirectSoundBuffer_Play(this->dsBuffer, 0, 0, DSBPLAY_LOOPING);

  this->state = dsaudioport_running;
  
  return;

 error_recovery:

  fts_audioport_set_input_channels( (fts_audioport_t *)this, 0);
  fts_audioport_set_input_function( (fts_audioport_t *)this, NULL);
  fts_audioport_set_output_channels( (fts_audioport_t *)this, 0);
  fts_audioport_set_output_function( (fts_audioport_t *)this, NULL);
  fts_audioport_set_xrun_function( (fts_audioport_t *)this, NULL);

  this->state = dsaudioport_corrupted;
}

static void
dsaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  dsaudioport_t *dev = (dsaudioport_t *)o;

  if (dev->event != NULL) {
    for (i = 0; i < dev->num_buffers; i++) {
      if (dev->event[i] != NULL) {
	CloseHandle(dev->event[i]);
      }
    }
    free(dev->event);
  }

  if (dev->position != NULL) {
    free(dev->position);
  }

  if (dev->format != NULL) {
    free(dev->format);
  }

  if (dev->notify != NULL) {
    IDirectSoundNotify_Release(dev->notify);
  }

  if (dev->dsBuffer != NULL) {
    IDirectSoundBuffer_Release(dev->dsBuffer);
  }
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
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, dsaudioport_delete, 0, 0);

  /* define variable */
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

  if (dsdev_instance == NULL) {
    return -1;
  }

  err = fts_win32_create_window();
  if (err != 0) {
    return err;
  }

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
  IDirectSoundBuffer_Release(fts_primary_buffer); 
  fts_primary_buffer = NULL;
  IDirectSound_Release(fts_direct_sound); 
  fts_direct_sound = NULL;
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
  return 0;
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  dsdev_instance = (HINSTANCE) hModule;
  return TRUE;
}

void 
dsaudioport_config(void)
{
  /* open DirectSound */
  if ((fts_direct_sound == NULL) && 
      (fts_open_direct_sound(NULL) != 0)) {
    return;
  }

  fts_class_install( fts_new_symbol("dsaudioport"), dsaudioport_instantiate);
}
