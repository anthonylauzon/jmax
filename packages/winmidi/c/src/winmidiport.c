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

#include <windows.h>
#include <fts/fts.h>

#define WINMIDI_DEBUG 1

#define BUFFER_SIZE 1024
#define SYSEX_BUFFER_SIZE 1024

#define NOTEOFF 0x80
#define NOTEON 0x90
#define KEYPRESSURE 0xa0
#define CONTROLCHANGE 0xb0
#define PROGRAMCHANGE 0xc0
#define CHANNELPRESSURE 0xd0
#define PITCHBEND 0xe0
#define SYSEX 0xf0
#define SYSEX_END 0xf7

static char winmidiport_error_buffer[256];

#define msg_pack(_s,_p1,_p2) ((DWORD) ((_s) | (((_p1) & 0x7f) << 8) | (((_p2) & 0x7f) << 16)))

#define msg_type(_m) ((_m) & 0xf0)
#define msg_type_enum(_m) ((((_m) & 0xf0) - 144) >> 4)
#define msg_chan(_m) ((_m) & 0x0f)
#define msg_p1(_m) (((_m) >> 8) & 0x7f)
#define msg_p2(_m) (((_m) >> 16) & 0x7f)

char*
winmidi_tostring(DWORD midi, char* buf, int len)
{
  switch (msg_type(midi)) {
  case 0:
    _snprintf(buf, len, "none");
    break;
  case NOTEOFF:
    _snprintf(buf, len, "noteoff: %3d %3d", msg_chan(midi), msg_p1(midi));
    break;
  case NOTEON:
    _snprintf(buf, len, "noteon: %3d %3d %3d", msg_chan(midi), msg_p1(midi), msg_p2(midi));
    break;
  case KEYPRESSURE:
    _snprintf(buf, len, "poly: %3d %3d %3d", msg_chan(midi), msg_p1(midi), msg_p2(midi));
    break;
  case CONTROLCHANGE:
    _snprintf(buf, len, "cc: %3d %3d %3d", msg_chan(midi), msg_p1(midi), msg_p2(midi));
    break;
  case PROGRAMCHANGE:
    _snprintf(buf, len, "prog: %3d %3d", msg_chan(midi), msg_p1(midi));
    break;
  case CHANNELPRESSURE:
    _snprintf(buf, len, "press: %3d %3d", msg_chan(midi), msg_p1(midi));
    break;
  case PITCHBEND:
    _snprintf(buf, len, "bend: %3d %3d", msg_chan(midi), msg_p1(midi) + (msg_p2(midi) << 7));
    break;
  case SYSEX:
    _snprintf(buf, len, "sysex");
    break;
  }
  return buf;
}


/*************************************************
 *
 *  Win midi port
 *
 */
typedef struct _winmidiport_t
{
  fts_midiport_t port;
  HMIDIOUT hmidiout;
  HMIDIIN hmidiin;
  MIDIHDR outhdr[2];
  int cur_outhdr;
  MIDIHDR inhdr[2];
  DWORD incoming[BUFFER_SIZE];
  int head;
  int tail;
} winmidiport_t;


#define winmidiport_buffer_full(_this)  ((_this->head == _this->tail - 1) || ((_this->head == BUFFER_SIZE - 1) && (_this->tail == 0)))
#define winmidiport_available(_this)  (_this->head != _this->tail)

static void winmidiport_output(fts_object_t *o, fts_midievent_t *event, double time);
static char* winmidiport_output_error(int no);
static char* winmidiport_input_error(int no);
void CALLBACK winmidiport_callback_in(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);


void CALLBACK 
winmidiport_callback_in(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
  winmidiport_t *this = (winmidiport_t *) dwInstance;
  MIDIHDR* hdr;
  
  switch (wMsg) {
  case MIM_OPEN: 
    break;
    
  case MIM_CLOSE:
    break;
    
  case MIM_DATA:
    if (!winmidiport_buffer_full(this)) {
      this->incoming[this->head++] = dwParam1;
      if (this->head == BUFFER_SIZE) {
	this->head = 0;
      }
    }
    break;
    
  case MIM_LONGDATA:
    hdr = (MIDIHDR*) dwParam1;
    break;
    
  case MIM_ERROR:
    break;
    
  case MIM_LONGERROR:
    break;
    
  case MIM_MOREDATA:
    break;
  }
}

void
winmidiport_dispatch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  winmidiport_t *this = (winmidiport_t *)o;
  int cur_fill, cur_send;
  int i;
  UINT err;
  MMRESULT res;
  char msg[256];

  /* handle the short midi messages */
  while (winmidiport_available(this)) {
    
    fts_midievent_t *event = NULL;

    DWORD msg = this->incoming[this->tail++];
    if (this->tail == BUFFER_SIZE) {
      this->tail = 0;
    }
    
    /* we have to make a distinction between the events that have two
       parameter values and those that take only one. */
    switch (msg_type(msg)) {

    case NOTEOFF:
      event = fts_midievent_channel_message_new(midi_note, msg_chan(msg) + 1, msg_p1(msg), msg_p2(msg));
      break;

    case NOTEON:
    case KEYPRESSURE:
    case CONTROLCHANGE:
    case PITCHBEND:
      event = fts_midievent_channel_message_new(msg_type_enum(msg), msg_chan(msg) + 1, msg_p1(msg), msg_p2(msg));
      break;

    case PROGRAMCHANGE:
    case CHANNELPRESSURE:
      event = fts_midievent_channel_message_new(msg_type_enum(msg), msg_chan(msg) + 1, msg_p1(msg), MIDI_EMPTY_BYTE);
      break;

    case SYSEX:
      break;
    }

    if (event != NULL) {
      fts_object_refer((fts_object_t *)event);
      fts_midiport_input((fts_midiport_t *) this, event, 0.0);
      fts_object_release((fts_object_t *)event);
    }
  }

  /* check for incoming sysex messages */
  for (i = 0; i < 2; i++) {

    /* check for incoming messages */
    if (this->inhdr[i].dwFlags & MHDR_DONE) {
      int size = this->inhdr[i].dwBytesRecorded;
      int j; 

      /* create a new midi event and send the sysex message to the
         midiport. start at position 1 to skip the start-of-sysex
         byte, and skip the end-of-sysex byte */
      if (size > 2) {

	fts_midievent_t *sysex = fts_midievent_system_exclusive_new();

	for (j = 1; j < size - 1; j++) {
	  fts_midievent_system_exclusive_append(sysex, this->inhdr[i].lpData[j]);
	}

	fts_object_refer((fts_object_t *)sysex);
	fts_midiport_input((fts_midiport_t *) this, sysex, 0.0);
	fts_object_release((fts_object_t *)sysex);
      }

      /* unprepare the buffer and flag it as available */
      midiInUnprepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
      this->inhdr[i].dwFlags = 0;

      /* prepare and add the buffer to the driver */
      err = midiInPrepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));

      if (err == MMSYSERR_NOERROR ) {
	
	err = midiInAddBuffer(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
	if (err != MMSYSERR_NOERROR ) {
	  midiInGetErrorText(err, &msg[0], 256);
	  post("Warning: winmidiport: Couldn't add sysex buffer: %s\n", msg);
	  fts_log("Warning: winmidiport: Couldn't add sysex buffer: %s\n", msg);
	}      
	
      } else {
	midiInGetErrorText(err, &msg[0], 256);
	post("Error: winmidiport: Couldn't prepare sysex buffer: %s\n", msg);
	fts_log("Error: winmidiport: Couldn't prepare sysex buffer: %s\n", msg);
      }
    }
  }

  cur_fill = this->cur_outhdr;
  cur_send = 1 - this->cur_outhdr;

  /* check if the outgoing message is finished. If we have new data to
     be send, swap the two sysex buffers. */
  if ((this->outhdr[cur_send].dwFlags & MHDR_DONE) 
      && (this->outhdr[cur_fill].dwBytesRecorded > 0)) {

    midiOutUnprepareHeader(this->hmidiout, &this->outhdr[cur_send], sizeof(MIDIHDR));
    this->outhdr[cur_send].dwFlags = 0;
    this->outhdr[cur_send].dwBytesRecorded = 0;
    this->cur_outhdr = cur_send;

    res = midiOutPrepareHeader(this->hmidiout, &this->outhdr[cur_fill], sizeof(MIDIHDR));
    if (res == MMSYSERR_NOERROR) {
      res = midiOutLongMsg(this->hmidiout, &this->outhdr[cur_fill], sizeof(MIDIHDR));
    }	  

    if (res != MMSYSERR_NOERROR) {
      char msg[256];
      midiOutGetErrorText(res, &msg[0], 256);
      post("Error: winmidiport: Couldn't send MIDI message: %s\n", msg);
      fts_log("Error: winmidiport: Couldn't send MIDI message: %s\n", msg);
    }
  }
}

static char* 
winmidiport_output_error(int no)
{
  midiOutGetErrorText(no, winmidiport_error_buffer, 256);
  return winmidiport_error_buffer;
}

static char* 
winmidiport_input_error(int no)
{
  midiInGetErrorText(no, winmidiport_error_buffer, 256);
  return winmidiport_error_buffer;
}

/************************************************************
 *
 *  MIDI port interface methods
 *
 */


static void
winmidiport_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  MMRESULT res = MMSYSERR_NOERROR;

  if (fts_midievent_is_channel_message(event)) {
    if (fts_midievent_channel_message_has_second_byte(event)) {
      
      res = midiOutShortMsg(this->hmidiout, 
			    msg_pack(fts_midievent_channel_message_get_status_byte(event),
				     fts_midievent_channel_message_get_first(event), 
				     fts_midievent_channel_message_get_second(event)));

    } else {
      
      res = midiOutShortMsg(this->hmidiout, 
			    msg_pack(fts_midievent_channel_message_get_status_byte(event),
				     fts_midievent_channel_message_get_first(event), 
				     0));
    }

  } else {

    switch(fts_midievent_get_type(event)) {
      
    case midi_system_exclusive:
      {
	int i;
	int size = fts_midievent_system_exclusive_get_size(event);
	fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(event);
	int cur = this->cur_outhdr;
	int len = this->outhdr[cur].dwBufferLength;
	unsigned char* buffer = (unsigned char*) this->outhdr[cur].lpData;
	int n = this->outhdr[cur].dwBytesRecorded;

	buffer[n++] = SYSEX;
  	
	for (i = 0; i < size; i++) {
	  buffer[n++] = fts_get_int(atoms + i) & 0x7f;
	  
	  if (n == len) {
	    int newlen = len + SYSEX_BUFFER_SIZE;
	    char* newbuf = fts_malloc(newlen);
	    memcpy(newbuf, this->outhdr[cur].lpData, newlen);
	    fts_free(this->outhdr[cur].lpData);
	    this->outhdr[cur].lpData = newbuf;
	    this->outhdr[cur].dwBufferLength = newlen;
	  }
	}
	
	buffer[n++] = SYSEX_END;
	
	this->outhdr[cur].dwBytesRecorded = n;	
      }
      break;
      
    case midi_real_time:
      res = midiOutShortMsg(this->hmidiout, msg_pack(fts_midievent_channel_message_get_status_byte(event), 0, 0));
      break;
      
    default:
      break;
    }
  }
  
  if (res != MMSYSERR_NOERROR) {
    char msg[256];
    midiOutGetErrorText(res, &msg[0], 256);
    post("Error: winmidiport: Couldn't send MIDI message: %s\n", msg);
    fts_log("Error: winmidiport: Couldn't send MIDI message: %s\n", msg);
  }
}

/************************************************************
 *
 *  class
 *
 */

static void
winmidiport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  winmidiport_t *this = (winmidiport_t *)o;
  MMRESULT res;
  UINT i, err, num;
  MIDIOUTCAPS out_caps;
  MIDIINCAPS in_caps;
  char msg[256];

  fts_midiport_init(&this->port);
  fts_midiport_set_input(&this->port);
  fts_midiport_set_output(&this->port, winmidiport_output);

  this->head = 0;
  this->tail = 0;
  this->hmidiout = NULL;
  this->hmidiin = NULL;
  this->cur_outhdr = 0;

  /* open midi output device */

  /* check if there any midi devices installed */
  num = midiOutGetNumDevs(); 

  if (num == 0) {
    post("Warning: winmidiport: no MIDI out devices found\n");
    fts_log("[winmidiport]: No MIDI out devices found\n");

  } else {

    /* for debugging */
    for (i = 0; i < num; i++) {
      res = midiOutGetDevCaps(i, &out_caps, sizeof(MIDIOUTCAPS));
      if (res == MMSYSERR_NOERROR) {
	fts_log("[winmidiport]: midi out port %d: \"%s\"\n", i, out_caps.szPname);
      }
    }

    /* try opening the default port */
    res = midiOutGetDevCaps(0, &out_caps, sizeof(MIDIOUTCAPS));
    if (res == MMSYSERR_NOERROR) {
      fts_log("[winmidiport]: trying to open midi out port \"%s\"\n", out_caps.szPname);
    }
    err = midiOutOpen(&this->hmidiout, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
    if (err != MMSYSERR_NOERROR) {
      post("Warning: winmidiport: couldn't open default MIDI out device: %s (error %d)\n", winmidiport_output_error(err), err);
      fts_log("[winmidiport]: Couldn't open default MIDI out device: %s (error %d)\n", winmidiport_output_error(err), err);
      this->hmidiout = NULL;
    } 
    
    /* if the default midi mapper failed, try opening a hardware port */
    if (this->hmidiout == NULL) {
      for (i = 0; i < num; i++) {
	res = midiOutGetDevCaps(i, &out_caps, sizeof(MIDIOUTCAPS));
	if ((res == MMSYSERR_NOERROR) && (out_caps.wTechnology == MOD_MIDIPORT)) {
	  err = midiOutOpen(&this->hmidiout, i, 0, 0, CALLBACK_NULL);
	  if (err == MMSYSERR_NOERROR) {
	    post("Warning: winmidiport: instead, opened MIDI out device: %s (id=%d)\n", out_caps.szPname, i);
	    fts_log("[winmidiport]: Instead, opened MIDI out device: %s (id=%d)\n", out_caps.szPname, i);
	    break;
	  } else {
	    this->hmidiout = NULL;
	  }
	}
      }
    }
    
    /* if the hardware midi devices failed, try opening a software synth */
    if (this->hmidiout == NULL) {
      for (i = 0; i < num; i++) {
	res = midiOutGetDevCaps(i, &out_caps, sizeof(MIDIOUTCAPS));
	if ((res == MMSYSERR_NOERROR) && (out_caps.wTechnology != MOD_MIDIPORT)) {
	  err = midiOutOpen(&this->hmidiout, i, 0, 0, CALLBACK_NULL);
	  if (err == MMSYSERR_NOERROR) {
	    post("Warning: winmidiport: instead, opened MIDI out device: %s (id=%d)\n", out_caps.szPname, i);
	    fts_log("[winmidiport]: Instead, opened MIDI out device: %s (id=%d)\n", out_caps.szPname, i);
	    break;
	  } else {
	    this->hmidiout = NULL;
	  } 
	}
      }
    }

    if (this->hmidiout == NULL) {
      post("Warning: winmidiport: failed to opened a midi out device; midi out not available\n");
      fts_log("[winmidiport]: Failed to opened a midi out device; midi out not available\n");

    } else {

      /* setup the buffers for outgoing sysex messages */
      for (i = 0; i < 2; i++) {
	this->outhdr[i].lpData = fts_malloc(SYSEX_BUFFER_SIZE);
	this->outhdr[i].dwBufferLength = SYSEX_BUFFER_SIZE;
	this->outhdr[i].dwBytesRecorded = 0;
	this->outhdr[i].dwUser = i;
	this->outhdr[i].dwFlags = (i == 0)? 0 : MHDR_DONE; 
      }
    }
  }

  /* open midi input device */

  /* check if there any midi devices installed */
  num = midiInGetNumDevs(); 

  if (num == 0) {
    post("Warning: winmidiport: no MIDI in devices found\n");
    fts_log("[winmidiport]: No MIDI in devices found\n");

  } else {

    for (i = 0; i < num; i++) {
      res = midiInGetDevCaps(i, &in_caps, sizeof(MIDIINCAPS));
      if (res == MMSYSERR_NOERROR) {
	fts_log("[winmidiport]: midi in port %d: %s\n", i, in_caps.szPname);
      }
    }

    /* try opening device 0 */
    res = midiInGetDevCaps(0, &in_caps, sizeof(MIDIINCAPS));
    if (res == MMSYSERR_NOERROR) {
      fts_log("[winmidiport]: trying to open midi in port \"%s\"\n", in_caps.szPname);
    }
    err = midiInOpen(&this->hmidiin, 0, (DWORD) winmidiport_callback_in, (DWORD) this, CALLBACK_FUNCTION);
    if (err != MMSYSERR_NOERROR) {
      post("Warning: winmidiport: couldn't open default MIDI in device: %s (error %d)\n", winmidiport_input_error(err), err);
      fts_log("[winmidiport]: Couldn't open default MIDI in device: %s (error %d)\n", winmidiport_input_error(err), err);
      this->hmidiin = NULL;
    } 
    
    /* if the default midi device failed, try opening any port */
    if (this->hmidiin == NULL) {
      for (i = 0; i < num; i++) {
	res = midiInGetDevCaps(i, &in_caps, sizeof(LPMIDIINCAPS));
	if (res == MMSYSERR_NOERROR) {
	  err = midiInOpen(&this->hmidiin, i, (DWORD) winmidiport_callback_in, (DWORD) this, CALLBACK_FUNCTION);
	  if (err == MMSYSERR_NOERROR) {
	    post("Warning: winmidiport: instead, opened MIDI in device: %s (id=%d)\n", in_caps.szPname, i);
	    fts_log("[winmidiport]: Instead, opened MIDI in device: %s (id=%d)\n", in_caps.szPname, i);
	    break;
	  } else {
	    this->hmidiin = NULL;
	  }
	}
      }
    }
    
    if (this->hmidiin == NULL) {
      post("Warning: winmidiport: failed to opened a midi in device; midi input not available\n");
      fts_log("[winmidiport]: Failed to opened a midi in device; midi input not available\n");

    } else {

      /* setup the buffers for incoming sysex messages */
      for (i = 0; i < 2; i++) {
	this->inhdr[i].lpData = fts_malloc(SYSEX_BUFFER_SIZE);
	this->inhdr[i].dwBufferLength = SYSEX_BUFFER_SIZE;
	this->inhdr[i].dwBytesRecorded = 0;
	this->inhdr[i].dwUser = i;
	this->inhdr[i].dwFlags = 0;
      
	/* prepare and add the buffer for incoming sysex messages */
	err = midiInPrepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));

	if (err == MMSYSERR_NOERROR ) {
	
	  err = midiInAddBuffer(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
	  if (err != MMSYSERR_NOERROR ) {
	    midiInGetErrorText(err, &msg[0], 256);
	    post("Warning: winmidiport: Couldn't add sysex buffer: %s\n", msg);
	    fts_log("Warning: winmidiport: Couldn't add sysex buffer: %s\n", msg);
	  }      
	
	} else {
	  midiInGetErrorText(err, &msg[0], 256);
	  post("Error: winmidiport: Couldn't prepare sysex buffer: %s\n", msg);
	  fts_log("Error: winmidiport: Couldn't prepare sysex buffer: %s\n", msg);
	}
      }

      /* start the midi input */
      err = midiInStart(this->hmidiin);
      if (err != MMSYSERR_NOERROR) {
	midiInGetErrorText(err, &msg[0], 256);
	post("Warning: winmidiport: failed to start the input device; midi input not available (%s)\n", msg);
	fts_log("[winmidiport]: Failed to start the input device; midi input not available (%s)\n", msg);
      } 
    }
  }

  fts_sched_add(o, FTS_SCHED_ALWAYS);
}

static void 
winmidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  winmidiport_t *this = (winmidiport_t *)o;
  int i, cur_send;

  fts_log("[winmidiport]: Closing MIDI ports\n");

  if (this->hmidiout != NULL) {

    /* make sure all sysex messages are sent */
    i = 0;
    cur_send = 1 - this->cur_outhdr;
    while (this->outhdr[cur_send].dwFlags & MHDR_DONE == 0) {
      Sleep(100);
      if (i++ == 10) {
	break;
      }
    }
    midiOutUnprepareHeader(this->hmidiout, &this->outhdr[cur_send], sizeof(MIDIHDR));

    for (i = 0; i < 2; i++) {
      if (this->outhdr[i].lpData != NULL) {
	fts_free(this->outhdr[i].lpData);
	this->outhdr[i].lpData = NULL;
      }
    }

    midiOutClose(this->hmidiout);
  }

  if (this->hmidiin != NULL) {
    for (i = 0; i < 2; i++) {
      midiInUnprepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
      if (this->inhdr[i].lpData != NULL) {
	fts_free(this->inhdr[i].lpData);
	this->inhdr[i].lpData = NULL;
      }
    }
    midiInClose(this->hmidiin);
  }

  fts_log("[winmidiport]: Done\n");

  fts_sched_remove(o);
}

static void
winmidiport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(winmidiport_t), winmidiport_init, winmidiport_delete);

  fts_midiport_class_init(cl);
  
  fts_class_message_varargs(cl, fts_s_sched_ready, winmidiport_dispatch);
}

void
winmidiport_config(void)
{
  fts_symbol_t winmidiport_symbol;
  winmidiport_symbol = fts_new_symbol("winmidiport");

  fts_class_install( winmidiport_symbol, winmidiport_instantiate);
  fts_midiport_set_default_class(winmidiport_symbol);
}
