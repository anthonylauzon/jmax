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
#define DEFAULT_FIFO_SIZE 8192
#define DEFAULT_SAMPLING_RATE (44100.0f)
#define DEFAULT_CHANNELS 2

typedef struct _dsdevice_t dsdevice_t;

struct _dsdevice_t {
  dsdevice_t* next;
  LPGUID guid;
  char* description;  
};

/* for some fsck'ing reason, direct sound needs a window! */
static HWND fts_wnd = NULL;

/* the handle to this dll instance to create the fsck'ing window */
static HINSTANCE dsdev_instance = NULL;

/* the list of DirectSound devices */
static dsdevice_t* dsdevice_list = NULL;

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

  LPGUID guid;
  fts_symbol_t device;

  int state; 
  
  /* write only, read only or read & write */
  fts_symbol_t mode;                

  /* the number of buffers (fts ticks) that go a the fifo buffer */
  unsigned int num_buffers;
  unsigned int buffer_byte_size;
  unsigned int cur_read_position;
  unsigned int cur_write_position;
  unsigned int frame_size;
  unsigned int fifo_frame_size;
  unsigned int fifo_byte_size;

  int no_xrun_message_already_posted;

  /* sound output */
  LPDIRECTSOUND direct_sound;
  LPDIRECTSOUNDBUFFER primary_buffer;
  LPDIRECTSOUNDBUFFER dsBuffer;

  /* sound input */
  LPDIRECTSOUNDCAPTURE direct_sound_capture;
  LPDIRECTSOUNDCAPTUREBUFFER dscBuffer;

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
  DWORD n, channels, ch, i, j, k;
  short *buf1, *buf2;
  DWORD bytes1, bytes2;

  DWORD cur_position, frames, capture_position, read_position, bytes;
  DWORD offset = 0;
  HRESULT res;

  dev = (dsaudioport_t *) fts_word_get_ptr(argv+0);
  if (dev->state != dsaudioport_running) {
    /* FIXME */
    return;
  }

  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_input_channels(dev);
  
  if ((dev->mode == fts_s_read) || (dev->mode == fts_s_read_write)) {

    while (offset < n) {

      cur_position = dev->cur_read_position * dev->frame_size;
      IDirectSoundCaptureBuffer_GetCurrentPosition(dev->dscBuffer, &capture_position, &read_position);

      if (cur_position <= read_position) {
	bytes = read_position - cur_position;
      } else if ((read_position < cur_position) && (capture_position <= cur_position)) {
	bytes = dev->fifo_byte_size + read_position - cur_position;      
      } else {
	/* xrun! */
	bytes = 0;
      }

      if (bytes > 0) {
	
	frames = bytes / dev->frame_size;
	
	if (offset + frames > n) {
	  frames = n - offset;
	  bytes = frames * dev->frame_size;
	}

	/* Lock */
	res = IDirectSoundCaptureBuffer_Lock(dev->dscBuffer, cur_position, bytes, (void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);

	if ((res != DS_OK) || (buf1 == NULL)) {
	  fts_log("[dsaudioport]: Failed to lock the audio capture buffer. System lockup might follow. Exiting. (curpos=%u, bytes=%u)\n", 
		  cur_position, bytes);
	  ExitProcess(0);
	}

	/* Interleave the sample buffer into the output buffer */
	for (ch = 0; ch < channels; ch++) {
	  
	  float *in = (float *) fts_word_get_ptr(argv + 2 + ch);
	  
	  i = 0;
	  j = ch;
	  k = offset;
	  
	  while (i < frames) { 
	    in[k++] = buf1[j] / 32768.0f;
	    j += channels;
	    i++;
	  }
	}

	/* Unlock */
	IDirectSoundCaptureBuffer_Unlock(dev->dscBuffer, buf1, bytes1, buf2, bytes2);

	offset += frames;
	dev->cur_read_position += frames;
	
	if (dev->cur_read_position >= dev->fifo_frame_size) {
	  dev->cur_read_position -= dev->fifo_frame_size;
	}
      }
      
      if (offset < n) {
	Sleep(1);
      }
    }
  }
}
    
static void 
dsaudioport_output(fts_word_t *argv)
{
  dsaudioport_t *dev;
  short *buf1, *buf2;
  DWORD bytes1, bytes2;    
  DWORD n, channels, ch, i, j, k;
  DWORD offset = 0;
  DWORD cur_position, frames, play_position, write_position, bytes;
  HRESULT res;

  dev = (dsaudioport_t *) fts_word_get_ptr(argv+0);
  if (dev->state != dsaudioport_running) {
    /* FIXME */
    return;
  }

  if ((dev->mode == fts_s_write) || (dev->mode == fts_s_read_write)) {

    n = fts_word_get_int(argv + 1);
    channels = fts_audioport_get_output_channels(dev);

    while (offset < n) {

      cur_position = dev->cur_write_position * dev->frame_size;
      IDirectSoundBuffer_GetCurrentPosition(dev->dsBuffer, &play_position, &write_position);

      if (cur_position <= play_position) {
	bytes = play_position - cur_position;
      } else if ((play_position < cur_position) && (write_position <= cur_position)) {
	bytes = dev->fifo_byte_size + play_position - cur_position;      
      } else {
	/* xrun! */
	bytes = 0;
      }
      
      if (bytes > 0) {
	
	frames = bytes / dev->frame_size;
	
	if (offset + frames > n) {
	  frames = n - offset;
	  bytes = frames * dev->frame_size;
	}
	
	/* Lock */
	res = IDirectSoundBuffer_Lock(dev->dsBuffer, cur_position, bytes, (void*) &buf1, &bytes1, (void*) &buf2, &bytes2, 0);
	
	if ((res != DS_OK) || (buf1 == NULL)) {
	  fts_log("[dsaudioport]: Failed to lock the audio buffer. System lockup might follow. Exiting. (curpos=%u, bytes=%u)\n", 
		  cur_position, bytes);
	  ExitProcess(0);
	}

	/* Interleave the sample buffer into the output buffer */
	for (ch = 0; ch < channels; ch++) {
	  
	  float *in = (float *) fts_word_get_ptr(argv + 2 + ch);
	  
	  i = 0;
	  j = ch;
	  k = offset;
	  
	  while (i < frames) { 
	    buf1[j] = (short) (32767.0f * in[k++]);
	    j += channels;
	    i++;
	  }
	}
	
	/* Unlock */
	IDirectSoundBuffer_Unlock(dev->dsBuffer, buf1, bytes1, buf2, bytes2);
	
	offset += frames;
	dev->cur_write_position += frames;
	
	if (dev->cur_write_position >= dev->fifo_frame_size) {
	  dev->cur_write_position -= dev->fifo_frame_size;
	}

      }

      if (offset < n) {
	Sleep(1);
      }
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


dsdevice_t* new_dsdevice(LPGUID guid, const char* description)
{
  dsdevice_t* dev = (dsdevice_t*) fts_malloc(sizeof(dsdevice_t));

  dev->next = NULL;
  dev->description = description ? strdup(description) : strdup("unknown device");

  if (guid == NULL) {
    dev->guid = NULL;
  } else {
    dev->guid = fts_malloc(sizeof(GUID));
    memcpy(dev->guid, guid, sizeof(GUID));
  }

  return dev;
}

static BOOL 
dsaudioport_enum_callback(LPGUID guid, LPCSTR description, 
			  LPCSTR module, LPVOID context)
{
  dsdevice_t* dev = new_dsdevice(guid, description);

  dev->next = dsdevice_list;
  dsdevice_list = dev;

  return TRUE;
}

static void 
dsaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int channels, sample_rate, fifo_size, frames;
  dsaudioport_t *dev = (dsaudioport_t *)o;
  DSBUFFERDESC desc;
  HRESULT hr;
  int err = 0;
  DSCBUFFERDESC cdesc;
  DSCAPS caps;
  dsdevice_t* device;
  DSBCAPS dsb_caps;

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
  dev->num_buffers = 0;
  dev->no_xrun_message_already_posted = 0;
  dev->direct_sound = NULL;
  dev->primary_buffer = NULL;
  dev->dsBuffer = NULL;

  dev->direct_sound_capture = NULL;
  dev->dscBuffer = NULL;
  dev->device = NULL;
  dev->mode = NULL;
  dev->cur_read_position = 0;
  dev->cur_write_position = 0;
  dev->frame_size = 0;
  dev->fifo_frame_size = 0;
  dev->fifo_byte_size = 0;

  /*************************** fts settings ********************************/

  fifo_size = fts_param_get_int(fts_s_fifo_size, DEFAULT_FIFO_SIZE);
  sample_rate = (int) fts_dsp_get_sample_rate();
  frames = fts_dsp_get_tick_size();


  /*************************** default settings ********************************/

  /*
    
    dsdev <device> <read_write> <channels> <fragments>

   */
  
  dev->device = fts_get_symbol_arg(ac, at, 0, fts_s_default);
  dev->mode = fts_get_symbol_arg(ac, at, 1, fts_s_write);

/*    dev->mode = fts_s_write; */

  channels = fts_get_int_arg(ac, at, 2, DEFAULT_CHANNELS);

  fts_log("[dsdev]: device=\"%s\", mode=%s, channels=%d, fifo_size=%d, sample_rate=%d\n", 
	  fts_symbol_name(dev->device), fts_symbol_name(dev->mode),
	  channels, fifo_size, sample_rate);
  

  if ((dev->mode != fts_s_read_write) 
      && (dev->mode != fts_s_read) 
      && (dev->mode != fts_s_write) ) {
    fts_log("[dsdev]: Unknown device mode '%s'\n", fts_symbol_name(dev->mode));
    dev->mode = fts_s_write;      
  }

  /* try to find the GUID of the audio device */

  /*

    

   */



  if (dev->device != fts_s_default) {

    if (dsdevice_list == NULL) {
      DirectSoundEnumerate((LPDSENUMCALLBACK) dsaudioport_enum_callback, NULL);              
    }
    
    device = dsdevice_list;
    while (device != NULL) {
      if ((device->guid != NULL) && (strcmp(device->description, fts_symbol_name(dev->device)) == 0)) {
	fts_log("[dsdev]: Opening device '%s'\n", device->description);
	dev->guid = device->guid;
      } else if (device->guid == NULL) {
	fts_log("[dsdev]: Default device '%s'\n", device->description);
	dev->guid = NULL;
      } else {
	fts_log("[dsdev]: Alternative device '%s'\n", device->description);
      }
      device = device->next;
    }

  } else {
    fts_log("[dsdev]: Opening default device\n");
  }

  /************************ set basic parameters ***********************************/

  dev->buffer_byte_size = channels * frames * sizeof(short);
  dev->num_buffers = fifo_size / frames;
  dev->fifo_frame_size = fifo_size;
  dev->frame_size = channels * sizeof(short);
  dev->fifo_byte_size = dev->fifo_frame_size * dev->frame_size;

  /* create and initialize the buffer format */
  dev->format = (WAVEFORMATEX*) fts_malloc(sizeof(WAVEFORMATEX));
  ZeroMemory(dev->format, sizeof(WAVEFORMATEX));
  dev->format->wFormatTag = WAVE_FORMAT_PCM;
  dev->format->nChannels = (unsigned short) channels;
  dev->format->nSamplesPerSec = (DWORD) sample_rate;
  dev->format->nBlockAlign = dev->frame_size;
  dev->format->nAvgBytesPerSec = (DWORD) sample_rate * dev->frame_size;
  dev->format->wBitsPerSample = sizeof(short) * 8; 
  dev->format->cbSize = 0; 


  /*************************** output ********************************/

  if ((dev->mode == fts_s_write) || (dev->mode == fts_s_read_write)) {

    /* create the direct sound interface */

    hr = DirectSoundCreate(dev->guid, &dev->direct_sound, NULL);
    if (hr != DS_OK) {
      fts_object_set_error(o, "Warning: dsaudioport: failed to create direct sound");
      goto error_recovery;
    }

    /* set the cooperative level */
    
    hr = IDirectSound_SetCooperativeLevel(dev->direct_sound, fts_wnd, DSSCL_PRIORITY);
    if (hr != DS_OK) {
      fts_object_set_error(o, "Warning: dsaudioport: failed to create set the cooperative level");
      goto error_recovery;
    }

    caps.dwSize = sizeof(caps); 
    hr = IDirectSound_GetCaps(dev->direct_sound, &caps);
    if (hr == DS_OK) {
      if (caps.dwFlags & DSCAPS_EMULDRIVER) {
	fts_log("[dsdev]: The DirectSound driver runs in emulated mode (MME)\n");
      } else {
	fts_log("[dsdev]: You have a native DirectSound driver installed\n");
      }
    }

    /* create the primary buffer */

    ZeroMemory(&desc, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    if (caps.dwFreeHwMixingStreamingBuffers > 0) {
      desc.dwFlags |= DSBCAPS_LOCHARDWARE;
    }
    
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

    dsb_caps.dwSize = sizeof(dsb_caps);
    hr = IDirectSoundBuffer_GetCaps(dev->primary_buffer, &dsb_caps);
    if (hr == DS_OK) {
      fts_log("[dsdev]: The size of the primary buffer is %u\n", dsb_caps.dwBufferBytes);
    }


    /* create the secondary sound output buffer. Try to allocate it on
       the soundcard if possible. */

    ZeroMemory(&desc, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
    if (caps.dwFreeHwMixingStreamingBuffers > 0) {
      desc.dwFlags |= DSBCAPS_LOCHARDWARE;
    }

    desc.lpwfxFormat = dev->format;
    desc.dwBufferBytes = dev->buffer_byte_size * dev->num_buffers;  
    desc.dwReserved = 0; 

    hr = IDirectSound_CreateSoundBuffer(dev->direct_sound, &desc, &dev->dsBuffer, NULL);
    if (hr != DS_OK) {
      post("Error opening DirectSound device (failed to create the secondary buffer)\n");
      fts_object_set_error(o, "Error opening DirectSound device (failed to create the secondary buffer)");
      goto error_recovery;
    }

    fts_audioport_set_output_channels((fts_audioport_t *) dev, channels);
    fts_audioport_set_output_function((fts_audioport_t *) dev, dsaudioport_output);

  } else {

    fts_audioport_set_output_channels((fts_audioport_t *) dev, 0);
    fts_audioport_set_output_function((fts_audioport_t *) dev, NULL);

  }


  /*************************** input ********************************/

  if ((dev->mode == fts_s_read) || (dev->mode == fts_s_read_write)) {

    hr = DirectSoundCaptureCreate(dev->guid, &dev->direct_sound_capture, NULL);
    if (hr != DS_OK) {
      post("Warning: dsaudioport: failed to create direct sound capture\n");
      fts_object_set_error(o, "Warning: dsaudioport: failed to create direct sound capture");
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
      post("Error opening DirectSound device (failed to create the capture buffer)\n");
      fts_object_set_error(o, "Error opening DirectSound device (failed to create the capture buffer)");
      goto error_recovery;
    }
    
    fts_audioport_set_input_channels((fts_audioport_t *) dev, channels);
    fts_audioport_set_input_function((fts_audioport_t *) dev, dsaudioport_input);

  } else {
    
    fts_audioport_set_input_channels((fts_audioport_t *) dev, 0);
    fts_audioport_set_input_function((fts_audioport_t *) dev, NULL);
  }


  /*************************** finish and start ********************************/



  if (dev->mode == fts_s_read) {
    hr = IDirectSoundCaptureBuffer_Start(dev->dscBuffer, DSCBSTART_LOOPING);

  } else if  (dev->mode == fts_s_read_write) {
    hr = IDirectSoundCaptureBuffer_Start(dev->dscBuffer, DSCBSTART_LOOPING);
    hr = IDirectSoundBuffer_Play(dev->dsBuffer, 0, 0, DSBPLAY_LOOPING);

  } else {
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
  /* delete all things related to the input buffer */

  if (dev->dscBuffer != NULL) {
    IDirectSoundCaptureBuffer_Stop(dev->dscBuffer);
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

  dsaudioport_symbol = fts_new_symbol("dsaudioport");
  fts_class_install( dsaudioport_symbol, dsaudioport_instantiate);
}
