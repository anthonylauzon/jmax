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


void CALLBACK 
winmidiport_callback(HMIDIOUT hmo, UINT wMsg, DWORD dwInstance, 
		     DWORD dwParam1, DWORD dwParam2);

static char* 
winmidiport_error(int no);


/*************************************************
 *
 *  Win midi port
 *
 */

typedef struct _winmidiport_t
{
  fts_midiport_t head;
  HMIDIOUT hmidiout;
} winmidiport_t;


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
  MIDIOUTCAPS caps;

  this->hmidiout = NULL;

  /* check if there any midi devices installed */
  num = midiOutGetNumDevs(); 
  if (num == 0) {
    post("Warning: winmidiport: no MIDI out devices found\n");
    return -1;
  }

  /* try opening the default midi mapper */
  err = midiOutOpen(&this->hmidiout, MIDI_MAPPER, (DWORD) NULL, (DWORD) this, CALLBACK_NULL);
  if (err != MMSYSERR_NOERROR) {
    post("Warning: winmidiport: couldn't open default MIDI out device: %s (error %d)\n", winmidiport_error(err), err);
    this->hmidiout = NULL;
  } 

  /* if the default midi mapper failed, try opening a hardware port */
  if (this->hmidiout == NULL) {
    for (i = 0; i < num; i++) {
      res = midiOutGetDevCaps(i, &caps, sizeof(MIDIOUTCAPS));
      if ((res == MMSYSERR_NOERROR) && (caps.wTechnology == MOD_MIDIPORT)) {
	err = midiOutOpen(&this->hmidiout, i, (DWORD) NULL, (DWORD) this, CALLBACK_NULL);
	if (err == MMSYSERR_NOERROR) {
	  post("Warning: winmidiport: instead, opened MIDI out device: %s (id=%d)\n", caps.szPname, i);
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
      res = midiOutGetDevCaps(i, &caps, sizeof(MIDIOUTCAPS));
      if ((res == MMSYSERR_NOERROR) && (caps.wTechnology != MOD_MIDIPORT)) {
	err = midiOutOpen(&this->hmidiout, i, (DWORD) NULL, (DWORD) this, CALLBACK_NULL);
	if (err == MMSYSERR_NOERROR) {
	  post("Warning: winmidiport: instead, opened MIDI out device: %s (id=%d)\n", caps.szPname, i);
	  break;
	} else {
	  this->hmidiout = NULL;
	} 
      }
    }
  }

  if (this->hmidiout == NULL) {
    post("Warning: winmidiport: failed to opened a midi devices; midi not available\n");
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
}

void CALLBACK 
winmidiport_callback(HMIDIOUT hmo, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
  
#if WINMIDI_DEBUG
  {
    char* s;

    switch (wMsg) {
      case MOM_OPEN: 
	s = "MOM_OPEN";
	break;

      case MOM_CLOSE:
	s = "MOM_CLOSE";
	break;

      case MOM_DONE:
	s = "MOM_DONE";
	break;
    }
  }
#endif
  
}

static char* 
winmidiport_error(int no)
{
  switch (no) {
  case MMSYSERR_NODRIVER: return "the driver is not installed";
  case MIDIERR_NODEVICE: return "no MIDI port was found";
  case MMSYSERR_ALLOCATED: return "the specified resource is already allocated"; 
  case MMSYSERR_BADDEVICEID: return "the specified device identifier is out of range";
  case MMSYSERR_INVALPARAM: return "the specified pointer or structure is invalid";
  case MMSYSERR_NOMEM: "The system is unable to allocate or lock memory"; 
  case MMSYSERR_ERROR:
  default: return "unspecified error";
  }
}

/************************************************************
 *
 *  MIDI port interface methods
 *
 */

#define midi_pack(_s,_c,_p1,_p2) ((DWORD) ((_s | (_c & 0x0f)) | ((_p1 & 0x7f) << 8) | ((_p2 & 0x7f) << 16)))

static void
winmidiport_send_note(fts_object_t *o, int channel, int number, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, midi_pack(NOTEON, channel, number, value));  
  }
}

static void
winmidiport_send_poly_pressure(fts_object_t *o, int channel, int number, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, midi_pack(KEYPRESSURE, channel, number, value));  
  }
}

static void
winmidiport_send_control_change(fts_object_t *o, int channel, int number, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, midi_pack(CONTROLCHANGE, channel, number, value));  
  }
}

static void
winmidiport_send_program_change(fts_object_t *o, int channel, int value, double time)
{	
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, midi_pack(PROGRAMCHANGE, channel, value, 0));  
  }
}

static void
winmidiport_send_channel_pressure(fts_object_t *o, int channel, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, midi_pack(CHANNELPRESSURE, channel, value, 0));  
  }
}

static void
winmidiport_send_pitch_bend(fts_object_t *o, int channel, int value, double time)
{
  winmidiport_t *this = (winmidiport_t *)o;
  if (this->hmidiout) {
    MMRESULT res = midiOutShortMsg(this->hmidiout, midi_pack(PITCHBEND, channel, (value & 0x7f), ((value >> 7) & 0x7f)));  
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

  fts_midiport_init(&this->head);
  fts_midiport_set_input(&this->head);
  fts_midiport_set_output(&this->head, &winmidiport_output_functions);

  winmidiport_open(this);
}

static void 
winmidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  winmidiport_t *this = (winmidiport_t *)o;

  winmidiport_close(this);
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
