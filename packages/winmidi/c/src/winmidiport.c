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


/*************************************************
 *
 *  Win midi port
 *
 */

#define BUFFER_SIZE 1024

typedef struct _winmidiport_t
{
  fts_midiport_t port;
  HMIDIOUT hmidiout;
  HMIDIIN hmidiin;
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
  err = midiOutOpen(&this->hmidiout, MIDI_MAPPER, (DWORD) NULL, (DWORD) this, CALLBACK_NULL);
  if (err != MMSYSERR_NOERROR) {
    post("Warning: winmidiport: couldn't open default MIDI out device: %s (error %d)\n", winmidiport_output_error(err), err);
    this->hmidiout = NULL;
  } 

  /* if the default midi mapper failed, try opening a hardware port */
  if (this->hmidiout == NULL) {
    for (i = 0; i < num; i++) {
      res = midiOutGetDevCaps(i, &out_caps, sizeof(MIDIOUTCAPS));
      if ((res == MMSYSERR_NOERROR) && (out_caps.wTechnology == MOD_MIDIPORT)) {
	err = midiOutOpen(&this->hmidiout, i, (DWORD) NULL, (DWORD) this, CALLBACK_NULL);
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
	err = midiOutOpen(&this->hmidiout, i, (DWORD) NULL, (DWORD) this, CALLBACK_NULL);
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
    post("Warning: winmidiport: failed to opened a midi devices; midi input not available\n");
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
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(NOTEON, channel, number, value));  
  }
}

static void
winmidiport_send_poly_pressure(fts_object_t *o, int channel, int number, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(KEYPRESSURE, channel, number, value));  
  }
}

static void
winmidiport_send_control_change(fts_object_t *o, int channel, int number, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(CONTROLCHANGE, channel, number, value));  
  }
}

static void
winmidiport_send_program_change(fts_object_t *o, int channel, int value, double time)
{	
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(PROGRAMCHANGE, channel, value, 0));  
  }
}

static void
winmidiport_send_channel_pressure(fts_object_t *o, int channel, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(CHANNELPRESSURE, channel, value, 0));  
  }
}

static void
winmidiport_send_pitch_bend(fts_object_t *o, int channel, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, msg_pack(PITCHBEND, channel, (value & 0x7f), ((value >> 7) & 0x7f)));  
  }
}

static void
winmidiport_send_system_exclusive_byte(fts_object_t *o, int value)
{
  winmidiport_t *this = (winmidiport_t *)o;
}

static void
winmidiport_send_system_exclusive_flush(fts_object_t *o, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;

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
  winmidiport_t *this = (winmidiport_t *)o;

  fts_midiport_init(&this->port);
  fts_midiport_set_input(&this->port);
  fts_midiport_set_output(&this->port, &winmidiport_output_functions);

  this->head = 0;
  this->tail = 0;

  winmidiport_open(this);

  fts_sched_add(o, FTS_SCHED_ALWAYS);
}

static void 
winmidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  winmidiport_t *this = (winmidiport_t *)o;

  winmidiport_close(this);

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
