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
#define SYSEX_BUFFER_FULL 0xffffffff

#define NOTEOFF 0x80
#define NOTEON 0x90
#define KEYPRESSURE 0xa0
#define CONTROLCHANGE 0xb0
#define PROGRAMCHANGE 0xc0
#define CHANNELPRESSURE 0xd0
#define PITCHBEND 0xe0
#define SYSEX 0xf0

static char winmidiport_error_buffer[256];

#define msg_pack(_s,_c,_p1,_p2) ((DWORD) ((_s | (_c & 0x0f)) | ((_p1 & 0x7f) << 8) | ((_p2 & 0x7f) << 16)))

#define msg_type(_m)  (_m & 0xf0)
#define msg_chan(_m)  (_m & 0x0f)
#define msg_p1(_m)    ((_m >> 8) & 0x7f)
#define msg_p2(_m)    ((_m >> 16) & 0x7f)


DWORD winmidi_in = 0;
DWORD winmidi_out = 0;

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

static char* 
winmidiport_output_error(int no);

static char* 
winmidiport_input_error(int no);

void CALLBACK 
winmidiport_callback_in(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, 
			DWORD dwParam1, DWORD dwParam2);

/*************************************************
 *
 *  Open/close Win midi port
 *
 */
static int
winmidiport_open(winmidiport_t *this)
{
  MMRESULT res;
  UINT i, err, num;
  MIDIOUTCAPS out_caps;
  MIDIINCAPS in_caps;

  this->hmidiout = NULL;
  this->hmidiin = NULL;

  /* open midi output device */

  /* check if there any midi devices installed */
  num = midiOutGetNumDevs(); 
  if (num == 0) {
    post("Warning: winmidiport: no MIDI out devices found\n");
    return -1;
  }

  /* try opening the default midi mapper */
  err = midiOutOpen(&this->hmidiout, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
  if (err != MMSYSERR_NOERROR) {
    post("Warning: winmidiport: couldn't open default MIDI out device: %s (error %d)\n", winmidiport_output_error(err), err);
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
	  break;
	} else {
	  this->hmidiout = NULL;
	} 
      }
    }
  }

  if (this->hmidiout == NULL) {
    post("Warning: winmidiport: failed to opened a midi out device; midi out not available\n");
    return -1;
  }

  /* open midi input device */

  /* check if there any midi devices installed */
  num = midiInGetNumDevs(); 
  if (num == 0) {
    post("Warning: winmidiport: no MIDI in devices found\n");
    return -1;
  }

  /* try opening device 0 */
  err = midiInOpen(&this->hmidiin, 0, (DWORD) winmidiport_callback_in, (DWORD) this, CALLBACK_FUNCTION);
  if (err != MMSYSERR_NOERROR) {
    post("Warning: winmidiport: couldn't open default MIDI in device: %s (error %d)\n", winmidiport_input_error(err), err);
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
	  break;
	} else {
	  this->hmidiin = NULL;
	}
      }
    }
  }

  if (this->hmidiin == NULL) {
    post("Warning: winmidiport: failed to opened a midi device; midi input not available\n");
    return -1;
  }

  if (midiInStart(this->hmidiin) != MMSYSERR_NOERROR) {
    post("Warning: winmidiport: failed to start the input device; midi input not available\n");
    return -1;
  }

  return 0;
}

static void
winmidiport_close(winmidiport_t *this)
{
  if (this->hmidiout != NULL) {
    midiOutClose(this->hmidiout);
  }
  if (this->hmidiin != NULL) {
    midiInClose(this->hmidiin);
  }
}

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
    winmidi_in = dwParam1;
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
  int i;

  while (winmidiport_available(this)) {
    
    DWORD msg = this->incoming[this->tail++];
    if (this->tail == BUFFER_SIZE) {
      this->tail = 0;
    }
    
    switch (msg_type(msg)) {
    case NOTEOFF:
      fts_midiport_input_note(&this->port, msg_chan(msg) + 1, msg_p1(msg), 0, 0.0);
      break;
    case NOTEON:
      fts_midiport_input_note(&this->port, msg_chan(msg) + 1, msg_p1(msg), msg_p2(msg), 0.0);
      break;
    case KEYPRESSURE:
      fts_midiport_input_poly_pressure(&this->port, msg_chan(msg) + 1, msg_p1(msg), msg_p2(msg), 0.0);
      break;
    case CONTROLCHANGE:
      fts_midiport_input_control_change(&this->port, msg_chan(msg) + 1, msg_p1(msg), msg_p2(msg), 0.0);
      break;
    case PROGRAMCHANGE:
      fts_midiport_input_program_change(&this->port, msg_chan(msg) + 1, msg_p1(msg), 0.0);
      break;
    case CHANNELPRESSURE:
      fts_midiport_input_channel_pressure(&this->port, msg_chan(msg) + 1, msg_p1(msg), 0.0);
      break;
    case PITCHBEND:
      fts_midiport_input_pitch_bend(&this->port, msg_chan(msg) + 1, msg_p1(msg) + (msg_p2(msg) << 7), 0.0);
      break;
    case SYSEX:
      break;
    }
  }

  /* check for sysex messages */
  for (i = 0; i < 2; i++) {

    /* check for incoming messages */
    if (this->inhdr[i].dwFlags == MHDR_DONE) {
      int size = this->inhdr[i].dwBytesRecorded;
      int j;

      /* first,prepare and add the second buffer to the driver */
      if (this->inhdr[1 - i].dwFlags == 0) {
	if (midiInPrepareHeader(this->hmidiin, &this->inhdr[1 - i], sizeof(MIDIHDR)) != MMSYSERR_NOERROR ) {
	  post("Warning: winmidiport: Couldn't prepare sysex buffer\n");
	} else if (midiInAddBuffer(this->hmidiin, &this->inhdr[1 - i], sizeof(MIDIHDR)) != MMSYSERR_NOERROR ) {
	  post("Warning: winmidiport: Couldn't add sysex buffer\n");
	}      
      }

      /* send the sysex message thru the midiport */
      for (j = 0; j < size; j++) {
	fts_midiport_input_system_exclusive_byte(&this->port, this->inhdr[i].lpData[j]);
      }
      fts_midiport_input_system_exclusive_call(&this->port, 0.0);

      /* unprepare the buffer and flag it as available */
      midiInUnprepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
      this->inhdr[i].dwFlags = 0;
    }

    /* check if the outgoing messages are finished */
    if (this->outhdr[i].dwFlags == MHDR_DONE) {
      midiOutUnprepareHeader(this->hmidiout, &this->outhdr[i], sizeof(MIDIHDR));
      this->outhdr[i].dwFlags = 0;
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
winmidiport_send_note(fts_object_t *o, int channel, int number, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  winmidi_out = msg_pack(NOTEON, channel, number, value);
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, winmidi_out);  
  }
}

static void
winmidiport_send_poly_pressure(fts_object_t *o, int channel, int number, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  winmidi_out = msg_pack(KEYPRESSURE, channel, number, value);
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(KEYPRESSURE, channel, number, value));  
  }
}

static void
winmidiport_send_control_change(fts_object_t *o, int channel, int number, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  winmidi_out = msg_pack(CONTROLCHANGE, channel, number, value);
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(CONTROLCHANGE, channel, number, value));  
  }
}

static void
winmidiport_send_program_change(fts_object_t *o, int channel, int value, double time)
{	
  winmidiport_t *this = (winmidiport_t *)o;
  winmidi_out = msg_pack(PROGRAMCHANGE, channel, value, 0);
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(PROGRAMCHANGE, channel, value, 0));  
  }
}

static void
winmidiport_send_channel_pressure(fts_object_t *o, int channel, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  winmidi_out = msg_pack(CHANNELPRESSURE, channel, value, 0);
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(CHANNELPRESSURE, channel, value, 0));  
  }
}

static void
winmidiport_send_pitch_bend(fts_object_t *o, int channel, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  winmidi_out = msg_pack(PITCHBEND, channel, (value & 0x7f), ((value >> 7) & 0x7f));
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(PITCHBEND, channel, (value & 0x7f), ((value >> 7) & 0x7f)));  
  }
}

static void
winmidiport_send_system_exclusive_byte(fts_object_t *o, int value)
{
  winmidiport_t *this = (winmidiport_t *)o;
  int n = this->cur_outhdr;
  int len = this->outhdr[n].dwBufferLength;
  int bytes = this->outhdr[n].dwBytesRecorded;

  if (this->outhdr[n].dwFlags != 0) {
    return;
  }

  if (len == bytes) {
    char* newbuf = fts_malloc(len + SYSEX_BUFFER_SIZE);
    memcpy(newbuf, this->outhdr[n].lpData, len);
    fts_free(this->outhdr[n].lpData);
    this->outhdr[n].lpData = newbuf;
    this->outhdr[n].dwBufferLength += SYSEX_BUFFER_SIZE;
  }

  this->outhdr[n].lpData[bytes] = (char)(value & 0xff);
  this->outhdr[n].dwBytesRecorded++;
}

static void
winmidiport_send_system_exclusive_flush(fts_object_t *o, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  int n = this->cur_outhdr;

  if (this->outhdr[n].dwFlags != 0) {
    return;
  }

  if ((midiOutPrepareHeader(this->hmidiout, &this->outhdr[n], sizeof(MIDIHDR)) == MMSYSERR_NOERROR)
      && (midiOutLongMsg(this->hmidiout, &this->outhdr[n], sizeof(MIDIHDR) == MMSYSERR_NOERROR))) {
    this->cur_outhdr = 1 - this->cur_outhdr;
  } else {
    this->outhdr[n].dwBytesRecorded = 0;
  }
}

static fts_midiport_output_functions_t winmidiport_output_functions =
{
  winmidiport_send_note,
  winmidiport_send_poly_pressure,
  winmidiport_send_control_change,
  winmidiport_send_program_change,
  winmidiport_send_channel_pressure,
  winmidiport_send_pitch_bend,
  winmidiport_send_system_exclusive_byte,
  winmidiport_send_system_exclusive_flush,
};

/************************************************************
 *
 *  get midiport variable
 *
 */
static void
winmidiport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  winmidiport_t *this = (winmidiport_t *)o;

  fts_set_object(value, o);
}

/************************************************************
 *
 *  class
 *
 */

static void
winmidiport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  int i;
  winmidiport_t *this = (winmidiport_t *)o;

  fts_midiport_init(&this->port);
  fts_midiport_set_input(&this->port);
  fts_midiport_set_output(&this->port, &winmidiport_output_functions);

  this->head = 0;
  this->tail = 0;

  winmidiport_open(this);

  this->cur_outhdr = 0;

  /* setup the buffers for incoming and outgoing sysex messages */
  for (i = 0; i < 2; i++) {
    this->outhdr[i].lpData = fts_malloc(SYSEX_BUFFER_SIZE);
    this->outhdr[i].dwBufferLength = SYSEX_BUFFER_SIZE;
    this->outhdr[i].dwBytesRecorded = 0;
    this->outhdr[i].dwUser = i;
    this->outhdr[i].dwFlags = 0;

    this->inhdr[i].lpData = fts_malloc(SYSEX_BUFFER_SIZE);
    this->inhdr[i].dwBufferLength = SYSEX_BUFFER_SIZE;
    this->inhdr[i].dwBytesRecorded = 0;
    this->inhdr[i].dwUser = i;
    this->inhdr[i].dwFlags = 0;
  }

  /* prepare and add the first buffer for incoming sysex messages */
  if (midiInPrepareHeader(this->hmidiin, &this->inhdr[0], sizeof(MIDIHDR)) != MMSYSERR_NOERROR ) {
    post("Warning: winmidiport: Couldn't prepare sysex buffer\n");
  } else if (midiInAddBuffer(this->hmidiin, &this->inhdr[0], sizeof(MIDIHDR)) != MMSYSERR_NOERROR ) {
    post("Warning: winmidiport: Couldn't add sysex buffer\n");
  }

  fts_sched_add(o, FTS_SCHED_ALWAYS);
}

static void 
winmidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  winmidiport_t *this = (winmidiport_t *)o;
  int i;

  winmidiport_close(this);

  for (i = 0; i < 2; i++) {
    midiInUnprepareHeader(this->hmidiin, &this->inhdr[0], sizeof(MIDIHDR));
    if (this->inhdr[0].lpData != NULL) {
      fts_free(this->inhdr[0].lpData);
    }
  }

  for (i = 0; i < 2; i++) {
    midiOutUnprepareHeader(this->hmidiout, &this->outhdr[0], sizeof(MIDIHDR));
    if (this->outhdr[0].lpData != NULL) {
      fts_free(this->outhdr[0].lpData);
    }
  }

  fts_sched_remove(o);
}

static int 
winmidiport_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{ 
  return 1;
}

static fts_status_t
winmidiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(winmidiport_t), 1, 0, 0);
      
  fts_midiport_class_init(cl);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, winmidiport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, winmidiport_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, winmidiport_dispatch);
  
  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, winmidiport_get_state);
  
  return fts_Success;
}

void
winmidiport_config(void)
{
  fts_metaclass_install( fts_new_symbol("winmidiport"), winmidiport_instantiate, winmidiport_equiv);
}

void
winmidiport_cleanup(void)
{
}
