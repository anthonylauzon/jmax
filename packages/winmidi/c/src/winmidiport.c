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

#define WINMIDI_READ 1
#define WINMIDI_WRITE 2


static char winmidiport_error_buffer[256];

#define msg_pack(_s,_p1,_p2) ((DWORD) ((_s) | (((_p1) & 0x7f) << 8) | (((_p2) & 0x7f) << 16)))

#define msg_type(_m) ((_m) & 0xf0)
#define msg_type_enum(_m) ((((_m) & 0xf0) - 144) >> 4)
#define msg_chan(_m) ((_m) & 0x0f)
#define msg_p1(_m) (((_m) >> 8) & 0x7f)
#define msg_p2(_m) (((_m) >> 16) & 0x7f)


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
  CRITICAL_SECTION critical_section;
  int cur_outhdr;
  MIDIHDR inhdr[2];
  DWORD incoming[BUFFER_SIZE];
  int head;
  int tail;
  unsigned int flags;
} winmidiport_t;


#define winmidiport_buffer_full(_this)  ((_this->head == _this->tail - 1) || ((_this->head == BUFFER_SIZE - 1) && (_this->tail == 0)))
#define winmidiport_available(_this)  (_this->head != _this->tail)

static void winmidiport_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void winmidiport_output(fts_object_t *o, fts_midievent_t *event, double time);
static char* winmidiport_output_error(int no);
static char* winmidiport_input_error(int no);
void CALLBACK winmidiport_callback_in(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
static unsigned char* winmidiport_realloc_sysex_buffer(MIDIHDR* hdr, int size);


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

    EnterCriticalSection(&this->critical_section);

    if ((this->hmidiin != NULL) && !winmidiport_buffer_full(this)) {
      this->incoming[this->head++] = dwParam1;
      if (this->head == BUFFER_SIZE) {
	this->head = 0;
      }
    }

    LeaveCriticalSection(&this->critical_section);

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

  EnterCriticalSection(&this->critical_section);

  if (this->hmidiin != NULL) {

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
	event = fts_midievent_channel_message_new(midi_note, msg_chan(msg), msg_p1(msg), msg_p2(msg));
	break;

      case NOTEON:
      case KEYPRESSURE:
      case CONTROLCHANGE:
      case PITCHBEND:
	event = fts_midievent_channel_message_new(msg_type_enum(msg), msg_chan(msg), msg_p1(msg), msg_p2(msg));
	break;

      case PROGRAMCHANGE:
      case CHANNELPRESSURE:
	event = fts_midievent_channel_message_new(msg_type_enum(msg), msg_chan(msg), msg_p1(msg), MIDI_EMPTY_BYTE);
	break;

      case SYSEX:
	break;
      }

      if (event != NULL) {
	fts_midiport_input((fts_midiport_t *) this, event, 0.0);
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

	  fts_midiport_input((fts_midiport_t *) this, sysex, 0.0);
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
  }

  if (this->hmidiout != NULL) {
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

  LeaveCriticalSection(&this->critical_section);
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

  EnterCriticalSection(&this->critical_section);

  if (this->hmidiout != NULL) {

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
	  if (n == len) {
	    buffer = winmidiport_realloc_sysex_buffer(&this->outhdr[cur], len + SYSEX_BUFFER_SIZE);
	  }
	  
	  for (i = 0; i < size; i++) {
	    buffer[n++] = fts_get_int(atoms + i) & 0x7f;
	    if (n == len) {
	      buffer = winmidiport_realloc_sysex_buffer(&this->outhdr[cur], len + SYSEX_BUFFER_SIZE);
	    }
	  }
	  
	  buffer[n++] = SYSEX_END;
	  if (n == len) {
	    buffer = winmidiport_realloc_sysex_buffer(&this->outhdr[cur], len + SYSEX_BUFFER_SIZE);
	  }
	  
	  this->outhdr[cur].dwBytesRecorded = n;	
	}
	break;
	
      case midi_real_time:
	res = midiOutShortMsg(this->hmidiout, msg_pack(fts_midievent_real_time_get_status_byte(event), 0, 0));
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

  LeaveCriticalSection(&this->critical_section);
}

static unsigned char* 
winmidiport_realloc_sysex_buffer(MIDIHDR* hdr, int newlen)
{
  unsigned char* newbuf;

  newbuf = fts_malloc(newlen);
  memcpy(newbuf, hdr->lpData, newlen);
  fts_free(hdr->lpData);

  hdr->lpData = newbuf;
  hdr->dwBufferLength = newlen;

  return newbuf;
}

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
winmidiport_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  winmidiport_t *this = (winmidiport_t *)o;
  MMRESULT res;
  UINT i, err, num;
  MIDIOUTCAPS out_caps;
  MIDIINCAPS in_caps;
  fts_symbol_t devname;
  char msg[256];
  int in_num = -1;
  int out_num = MIDI_MAPPER;
  int input = 1;
  fts_symbol_t mode;

  fts_midiport_init(&this->port);
  fts_midiport_set_input(&this->port);
  fts_midiport_set_output(&this->port, winmidiport_output);

  InitializeCriticalSection(&this->critical_section);

  EnterCriticalSection(&this->critical_section);

  this->head = 0;
  this->tail = 0;
  this->hmidiout = NULL;
  this->hmidiin = NULL;
  this->cur_outhdr = 0;

  /* check for the device name */
  devname = fts_get_symbol_arg(ac, at, 0, fts_s_default);

  /* Check for read or write access. I am doing a pretty pedantic
     error checking. In my experience, it does not help to be easy on
     this settings since it might provoke a system deadlock later if
     two devices are opened with the wrong mode. */

  if (ac >= 2) {
    if (fts_is_symbol(at + 1)) {
      mode = fts_get_symbol( at + 1);
    } else {
      post("Warning: winmidiport: Invalid argument (mode)\n");
      fts_log("[winmidiport]: Invalid argument (mode)\n");
      fts_object_set_error(o, "Invalid argument (mode)");
      goto error_recovery;
    }
  } else {
    post("Warning: winmidiport: Not enough arguments (mode)\n");
    fts_log("[winmidiport]: Not enough arguments (mode)\n");
    fts_object_set_error(o, "Not enough arguments (mode)");
    goto error_recovery;
  }

  if ((mode != fts_s_read) && (mode != fts_s_write) && (mode != fts_s_read_write)) {
    post("Warning: winmidiport: Invalid argument (mode=\"%s\")\n", fts_symbol_name(mode));
    fts_log("[winmidiport]: Invalid argument (mode=\"%s\")\n", fts_symbol_name(mode));
    fts_object_set_error(o, "Invalid argument (mode=\"%s\")", fts_symbol_name(mode));    
    goto error_recovery;
  }

  this->flags = (mode == fts_s_read)? WINMIDI_READ : (mode == fts_s_write)? WINMIDI_WRITE : WINMIDI_READ | WINMIDI_WRITE;


  /* open midi output device */
  if (this->flags & WINMIDI_WRITE) {

    /* check if there any midi devices installed */
    num = midiOutGetNumDevs(); 

    if (num == 0) {
      post("Warning: winmidiport: no MIDI out devices found\n");
      fts_log("[winmidiport]: No MIDI out devices found\n");

    } else {

      /* search the id of the default midi out (print out the list to the log for debugging) */
      for (i = 0; i < num; i++) {
	res = midiOutGetDevCaps(i, &out_caps, sizeof(MIDIOUTCAPS));
	if (res == MMSYSERR_NOERROR) {
	  fts_log("[winmidiport]: midi out port %d: \"%s\"\n", i, out_caps.szPname); 

	  if (strcmp(out_caps.szPname, fts_symbol_name(devname)) == 0) {
	    out_num = i;
	    fts_log("[winmidiport]: Found default midi out port in device list\n");
	  }
	}
      }

      /* I am being tough here. But I think it's for the best. */
      if ((out_num == MIDI_MAPPER) && (devname != fts_s_default)) {
	post("Warning: winmidiport: Invalid device (%s)\n", fts_symbol_name(devname));
	fts_log("[winmidiport]: Invalid device (%s)\n", fts_symbol_name(devname));
	fts_object_set_error(o, "Invalid device (%s)", fts_symbol_name(devname));    
	goto error_recovery;
      }

      /* try opening the default port */
      res = midiOutGetDevCaps(out_num, &out_caps, sizeof(MIDIOUTCAPS));

      if (res == MMSYSERR_NOERROR) {
	fts_log("[winmidiport]: Trying to open midi out port \"%s\"\n", out_caps.szPname);

	/*

	  I continue to have problems with the Microsoft Soft
	  Synth. Opening the synth sometimes blocks the system
	  completely. I haven't been able to reproduce the error on
	  the development machines. Until some final solution is
	  found, warn the user of a possible crash and let him decide
	  to take the risk.

	 */

	if ((strstr(out_caps.szPname, "Microsoft") != NULL) && 
	    (strstr(out_caps.szPname, "Map") == NULL)) {  /* No message for "Mappeur MIDI Microsoft" */
	  if (MessageBox(NULL, "The Microsoft Soft Synth is not supported by FTS and \n"
			 "might cause FTS to crash. If you have any problems, please \n"
			 "select an other MIDI device in the Windows Configuration Panels \n"
			 "and restart FTS. Do you want to continue?", 
			 "FTS Initialization", 
			 MB_YESNO | MB_ICONSTOP | MB_APPLMODAL) == IDNO) {
	    
	    fts_object_set_error(o, "Failed to open the MIDI port");
	    goto error_recovery;
	  }
	}

	err = midiOutOpen(&this->hmidiout, out_num, 0, 0, CALLBACK_NULL);
	
	if (err != MMSYSERR_NOERROR) {
	  post("Warning: winmidiport: couldn't open default MIDI out device: %s (error %d)\n", winmidiport_output_error(err), err);
	  fts_log("[winmidiport]: Couldn't open default MIDI out device: %s (error %d)\n", winmidiport_output_error(err), err);
	  this->hmidiout = NULL;
	} else {
	  fts_log("[winmidiport]: Opened\n");
	}
      }
    
      /*
	
	I'm not sure whether it's a good idea to try and open an other
	port if the default port failed. I'll keep it for now until
	someone complains.

       */

      /* if the default midi mapper failed, try opening a hardware port */
/*        if (this->hmidiout == NULL) { */
/*  	for (i = 0; i < num; i++) { */
/*  	  res = midiOutGetDevCaps(i, &out_caps, sizeof(MIDIOUTCAPS)); */
/*  	  if ((res == MMSYSERR_NOERROR) && (out_caps.wTechnology == MOD_MIDIPORT)) { */
/*  	    err = midiOutOpen(&this->hmidiout, i, 0, 0, CALLBACK_NULL); */
/*  	    if (err == MMSYSERR_NOERROR) { */
/*  	      post("Warning: winmidiport: instead, opened MIDI out device: %s (id=%d)\n", out_caps.szPname, i); */
/*  	      fts_log("[winmidiport]: Instead, opened MIDI out device: %s (id=%d)\n", out_caps.szPname, i); */
/*  	      break; */
/*  	    } else { */
/*  	      this->hmidiout = NULL; */
/*  	    } */
/*  	  } */
/*  	} */
/*        } */
    
      /* if the hardware midi devices failed, try opening a software synth */
/*        if (this->hmidiout == NULL) { */
/*  	for (i = 0; i < num; i++) { */
/*  	  res = midiOutGetDevCaps(i, &out_caps, sizeof(MIDIOUTCAPS)); */
/*  	  if ((res == MMSYSERR_NOERROR) && (out_caps.wTechnology != MOD_MIDIPORT)) { */
/*  	    err = midiOutOpen(&this->hmidiout, i, 0, 0, CALLBACK_NULL); */
/*  	    if (err == MMSYSERR_NOERROR) { */
/*  	      post("Warning: winmidiport: instead, opened MIDI out device: %s (id=%d)\n", out_caps.szPname, i); */
/*  	      fts_log("[winmidiport]: Instead, opened MIDI out device: %s (id=%d)\n", out_caps.szPname, i); */
/*  	      break; */
/*  	    } else { */
/*  	      this->hmidiout = NULL; */
/*  	    }  */
/*  	  } */
/*  	} */
/*        } */

      if (this->hmidiout == NULL) {
	post("Warning: winmidiport: failed to opened a midi out device; midi out not available\n");
	fts_log("[winmidiport]: Failed to opened a midi out device; midi out not available\n");
	fts_object_set_error(o, "Failed to open the MIDI port");
	goto error_recovery;
      } 
	
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
  
  if (this->flags & WINMIDI_READ) {

    /* check if there any midi devices installed */
    num = midiInGetNumDevs(); 
    
    if (num == 0) {
      post("Warning: winmidiport: no MIDI in devices found\n");
      fts_log("[winmidiport]: No MIDI in devices found\n");
	
    } else {
      
      for (i = 0; i < num; i++) {
	res = midiInGetDevCaps(i, &in_caps, sizeof(MIDIINCAPS));
	if (res == MMSYSERR_NOERROR) {
	  fts_log("[winmidiport]: MIDI in port %d: \"%s\"\n", i, in_caps.szPname);
	  if (strcmp(in_caps.szPname, fts_symbol_name(devname)) == 0) {
	    in_num = i;
	    fts_log("[winmidiport]: Found default midi in port in device list\n");
	  }
	}
      }

      if (in_num == -1) {

	/* I am being tough here. But I think it's for the best. */
	if (devname != fts_s_default) {
	  post("Warning: winmidiport: Invalid device (%s)\n", fts_symbol_name(devname));
	  fts_log("[winmidiport]: Invalid device (%s)\n", fts_symbol_name(devname));
	  fts_object_set_error(o, "Invalid device (%s)", fts_symbol_name(devname));    
	  goto error_recovery;
	} else {
	  in_num = 0;
	}
      }

      /* try opening the default device  */
      res = midiInGetDevCaps(in_num, &in_caps, sizeof(MIDIINCAPS));
      if (res == MMSYSERR_NOERROR) {
	fts_log("[winmidiport]: Trying to open midi in port \"%s\"\n", in_caps.szPname);
      }

      err = midiInOpen(&this->hmidiin, in_num, (DWORD) winmidiport_callback_in, (DWORD) this, CALLBACK_FUNCTION);

      if (err != MMSYSERR_NOERROR) {
	post("Warning: winmidiport: couldn't open default MIDI in device: %s (error %d)\n", winmidiport_input_error(err), err);
	fts_log("[winmidiport]: Couldn't open default MIDI in device: %s (error %d)\n", winmidiport_input_error(err), err);
	this->hmidiin = NULL;
      } else {
	fts_log("[winmidiport]: Opened\n");
      }
    }
    
    /* if the default midi device failed, try opening any port */
/*      if (this->hmidiin == NULL) { */
/*        for (i = 0; i < num; i++) { */
/*  	res = midiInGetDevCaps(i, &in_caps, sizeof(LPMIDIINCAPS)); */
/*  	if (res == MMSYSERR_NOERROR) { */
/*  	  err = midiInOpen(&this->hmidiin, i, (DWORD) winmidiport_callback_in, (DWORD) this, CALLBACK_FUNCTION); */
/*  	  if (err == MMSYSERR_NOERROR) { */
/*  	    post("Warning: winmidiport: instead, opened MIDI in device: %s (id=%d)\n", in_caps.szPname, i); */
/*  	    fts_log("[winmidiport]: Instead, opened MIDI in device: %s (id=%d)\n", in_caps.szPname, i); */
/*  	    break; */
/*  	  } else { */
/*  	    this->hmidiin = NULL; */
/*  	  } */
/*  	} */
/*        } */
/*      } */
    
    if (this->hmidiin == NULL) {
      post("Warning: winmidiport: failed to opened a midi in device; midi input not available\n");
      fts_log("[winmidiport]: Failed to opened a midi in device; midi input not available\n");
      fts_object_set_error(o, "Failed to open the MIDI port");
      goto error_recovery;
    } 
	
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
  
  LeaveCriticalSection(&this->critical_section);

  fts_sched_add(o, FTS_SCHED_ALWAYS);

  return;

 error_recovery:

  winmidiport_close(o, 0, NULL, 0, NULL);
  LeaveCriticalSection(&this->critical_section);
}

static void 
winmidiport_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  winmidiport_t *this = (winmidiport_t *)o;
  int i, cur_send;
  HMIDIOUT hmidiout;
  HMIDIIN hmidiin;

  fts_log("[winmidiport]: Closing MIDI port\n");

  EnterCriticalSection(&this->critical_section);

  /* Set the handle to NULL to avoid that trailing callbacks access
     this midiport */
  hmidiout = this->hmidiout;
  hmidiin = this->hmidiin;
  this->hmidiout = NULL;
  this->hmidiin = NULL;

  if (hmidiout != NULL) {

    /* mark the output buffers as done */
    midiOutReset(hmidiout);

    /* make sure everything is flushed */
    i = 0;
    cur_send = 1 - this->cur_outhdr;
    while (this->outhdr[cur_send].dwFlags & MHDR_DONE == 0) {
      Sleep(100);
      if (i++ == 10) {
	break;
      }
    }

    for (i = 0; i < 2; i++) {
      if (this->outhdr[i].lpData != NULL) {
	fts_free(this->outhdr[i].lpData);
	this->outhdr[i].lpData = NULL;
      }
    }

    midiOutClose(hmidiout);
  }

  if (hmidiin != NULL) {

    midiInReset(hmidiin);
    
    for (i = 0; i < 2; i++) {
      if (this->inhdr[i].dwFlags & MHDR_DONE) {
	if (this->inhdr[i].lpData != NULL) {
	  fts_free(this->inhdr[i].lpData);
	  this->inhdr[i].lpData = NULL;
	}
      } else {
	fts_log("[winmidiport]: Input buffers not done\n");	
      }
    }

    midiInClose(hmidiin);
  }

  fts_log("[winmidiport]: Done\n");

  LeaveCriticalSection(&this->critical_section);
  DeleteCriticalSection(&this->critical_section);

  fts_sched_remove(o);
}

static void
winmidiport_reopen(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  ac--;
  at++;

  winmidiport_close(o, 0, NULL, 0, NULL);
  winmidiport_open(o, 0, NULL, ac, at);
}

static void
winmidiport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  ac--;
  at++;
  
  winmidiport_open(o, winlet, s, ac, at);
}

static void 
winmidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  winmidiport_close(o, winlet, s, ac, at);
}

static fts_status_t
winmidiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(winmidiport_t), 1, 0, 0);
      
  fts_midiport_class_init(cl);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, winmidiport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_reopen, winmidiport_reopen);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, winmidiport_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, winmidiport_dispatch);
  
  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, winmidiport_get_state);
  
  return fts_Success;
}




/*************************************************
 *
 *  Win midi 
 *
 */

typedef struct _winmidi_t
{
  fts_object_t object;
} winmidi_t;

static void 
winmidi_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void 
winmidi_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void 
winmidi_ins(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a[64];
  fts_symbol_t name;
  MIDIINCAPS caps;
  int i, num;

  num = midiInGetNumDevs(); 

  if (num == 0) {
    return;
  }

  for (i = 0; i < num && i < 64; i++) {
    midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
    name = fts_new_symbol_copy(caps.szPname);
    fts_set_symbol(&a[i], name);
  }

  fts_outlet_send(o, 0, fts_s_list, num, a);
}

static void 
winmidi_outs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a[64];
  fts_symbol_t name;
  MIDIOUTCAPS caps;
  int i, num;

  num = midiOutGetNumDevs(); 

  if (num == 0) {
    return;
  }

  for (i = 0; i < num && i < 64; i++) {
    midiOutGetDevCaps(i, &caps, sizeof(MIDIOUTCAPS));
    name = fts_new_symbol_copy(caps.szPname);
    fts_set_symbol(&a[i], name);
  }

  fts_outlet_send(o, 0, fts_s_list, num, a);
}

static fts_status_t
winmidi_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(winmidi_t), 1, 1, 0);
      
  fts_midiport_class_init(cl);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, winmidi_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, winmidi_delete);
  fts_method_define_varargs(cl, 0, fts_new_symbol("ins"), winmidi_ins);
  fts_method_define_varargs(cl, 0, fts_new_symbol("outs"), winmidi_outs);

  return fts_Success;
}



/*************************************************
 *
 *  Module config 
 *
 */

void
winmidiport_config(void)
{
  fts_metaclass_install( fts_new_symbol("winmidiport"), winmidiport_instantiate, fts_always_equiv);
  fts_metaclass_install( fts_new_symbol("winmidi"), winmidi_instantiate, fts_always_equiv);
}
