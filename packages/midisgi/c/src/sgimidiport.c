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
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <unistd.h>
#include <dmedia/midi.h>
#include "fts.h"

#define MAX_MIDI_EVENTS 512

typedef struct _sgimidiport_
{
  fts_midiport_t head;
  MDport in;
  MDport out;
  fts_symbol_t name;
} sgimidiport_t;

int
sgimidiport_open(sgimidiport_t *port, fts_symbol_t name)
{
  if (mdInit() > 0)
    {
      port->in = mdOpenInPort(fts_symbol_name(name));
      port->out = mdOpenOutPort(fts_symbol_name(name));
      
      if(port->in && port->out)
	{
	  mdSetStampMode(port->in, MD_NOSTAMP);
	  mdSetStampMode(port->out, MD_NOSTAMP);      
	  
	  return 1;
	}
      else
	{
	  /*should be considered:*/
	  /*mdRegister(fts_symbol_name(name));*/

	  port->in = 0;
	  port->out = 0;

	  return 0;
	}
    }
  else
    return 0;  
}

void
sgimidiport_close(sgimidiport_t *port)
{
  mdPanic(port->out);

  mdClosePort(port->in);
  mdClosePort(port->out);
}

void
sgimidiport_dispatch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sgimidiport_t *port = (sgimidiport_t *)o;
  MDevent midi_events[MAX_MIDI_EVENTS];
  int n_events;
  int i;
  
  n_events = mdReceive(port->in, midi_events, MAX_MIDI_EVENTS);

  for(i=0; i<n_events; i++)
  {
    char *msg = midi_events[i].msg;
    int status = mdGetStatus(msg);

    switch(status)
      {
	case MD_NOTEOFF:
	  fts_midiport_channel_message(&port->head, fts_midi_status_note, mdGetChannel(msg) + 1, mdGetByte1(msg), 0, 0.0);
	  break;
	case MD_NOTEON:
	  fts_midiport_channel_message(&port->head, fts_midi_status_note, mdGetChannel(msg) + 1, mdGetByte1(msg), mdGetByte2(msg), 0.0);
	  break;
	case MD_POLYKEYPRESSURE:
	  fts_midiport_channel_message(&port->head, fts_midi_status_poly_pressure, mdGetChannel(msg) + 1, mdGetByte1(msg), mdGetByte2(msg), 0.0);
	  break;
	case MD_CONTROLCHANGE:
	  fts_midiport_channel_message(&port->head, fts_midi_status_control_change, mdGetChannel(msg) + 1, mdGetByte1(msg), mdGetByte2(msg), 0.0);
	  break;
	case MD_PROGRAMCHANGE:
	  fts_midiport_channel_message(&port->head, fts_midi_status_program_change, mdGetChannel(msg) + 1, mdGetByte1(msg), 0, 0.0);
	  break;
	case MD_CHANNELPRESSURE:
	  fts_midiport_channel_message(&port->head, fts_midi_status_channel_pressure, mdGetChannel(msg) + 1, mdGetByte1(msg), mdGetByte2(msg), 0.0);
	  break;
	case MD_PITCHBENDCHANGE:
	  fts_midiport_channel_message(&port->head, fts_midi_status_pitch_bend, mdGetChannel(msg) + 1, mdGetByte1(msg) + (mdGetByte2(msg) << 7), 0, 0.0);
	  break;
	case MD_SYSEX:
	  fts_midiport_system_exclusive(&port->head, mdGetChannel(msg), midi_events[i].msglen - 2, midi_events[i].sysexmsg + 1, 0.0);

	  if(midi_events[i].msglen > 0)
	    mdFree(midi_events[i].sysexmsg);

	  break;
      }
  }
}

/************************************************************
 *
 *  object
 *
 */

static void
sgimidiport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sgimidiport_t *this = (sgimidiport_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  int open = 0;

  fts_midiport_init(this);
  this->in = 0;
  this->out = 0;
  this->name = 0;

  if(name)
    {
      open = sgimidiport_open(this, name);
      this->name = name;
    }

  if(open)
    fts_sched_add_fd(fts_sched_get_current(), mdGetFd(this->in), 1, sgimidiport_dispatch, o);
}

static void 
sgimidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sgimidiport_t *this = (sgimidiport_t *)o;

  if(this->in)
    {
      fts_sched_remove_fd(fts_sched_get_current(), mdGetFd(this->in));
      sgimidiport_close(this);
    }
}

/************************************************************
 *
 *  MIDI port interface methods
 *
 */

static void
sgimidiport_send_channel_message(fts_midiport_t *port, int status, int channel, int x, int y, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)port;

  channel--; /* 1 to 16 -> 0 to 15 */

  switch(status)
    {
    case fts_midi_status_note:
      {
	MDevent event;

	event.msg[0] = MD_NOTEON | (channel & 0x0F);
	event.msg[1] = (char)(x & 127); /* note number */
	event.msg[2] = (char)(y & 127); /* velocity */
	event.msglen = 3;
	
	mdSend(this->out, &event, 1);
	
	break;
      }
    case fts_midi_status_poly_pressure:
      {
	MDevent event;

	event.msg[0] = MD_POLYKEYPRESSURE | (channel & 0x0F);
	event.msg[1] = (char)(x & 127); /* note number */
	event.msg[2] = (char)(y & 127); /* pressure value */
	event.msglen = 3;
	
	mdSend(this->out, &event, 1);
	
	break;
      }
    case fts_midi_status_control_change:
      {
	MDevent event;

	event.msg[0] = MD_CONTROLCHANGE | (channel & 0x0F);
	event.msg[1] = (char)(x & 127); /* controller number */
	event.msg[2] = (char)(y & 127); /* controller value */
	event.msglen = 3;
	
	mdSend(this->out, &event, 1);
		
	break;
      }
    case fts_midi_status_program_change:
      {
	MDevent event;

	event.msg[0] = MD_PROGRAMCHANGE | (channel & 0x0F);
	event.msg[1] = (char)(x & 127); /* programm number */
	event.msg[2] = 0;
	event.msglen = 2;
	
	mdSend(this->out, &event, 1);
	
	break;
      }
    case fts_midi_status_channel_pressure:
      {
	MDevent event;

	event.msg[0] = MD_CHANNELPRESSURE | (channel & 0x0F);
	event.msg[1] = (char)(x & 127); /* pressure value */
	event.msg[2] = 0;
	event.msglen = 2;
	
	mdSend(this->out, &event, 1);
	
	break;
      }
    case fts_midi_status_pitch_bend:
      {
	MDevent event;

	event.msg[0] = MD_PITCHBENDCHANGE | (channel & 0x0F);
	event.msg[1] = (char)(x & 127); /* LSB */
	event.msg[2] = (char)((x >> 7) & 127); /* MSB */
	event.msglen = 3;

	mdSend(this->out, &event, 1);

	break;
      }
    }
}

static void
sgimidiport_send_system_exclusive(fts_midiport_t *port, int ac, const fts_atom_t *at, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)port;
  MDevent event;
  char c[1024];
  int i;

  c[0] = 0xF0;

  if(ac > 1022)
    ac = 1022;

  for(i=0; i<ac; i++)
    c[i + 1] = (char)(fts_get_int(at + i) & 0x7F);
  
  c[i + 1] = 0xF7; /* end of sysex EOX */

  event.msg[0] = MD_SYSEX;
  event.sysexmsg = c;
  event.msglen = ac + 2;
  
  mdSend(this->out, &event, 1);
}


/************************************************************
 *
 *  get midiport variable
 *
 */
static void
sgimidiport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  sgimidiport_t *this = (sgimidiport_t *)o;

  if(this->in)
    fts_set_object(value, o);
  else
    fts_set_void(value);
}

/************************************************************
 *
 *  default port
 *
 */
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
extern fts_symbol_t fts_midi_hack_default_device_name;
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

sgimidiport_t *sgimidiport_default = 0;

fts_midiport_t *
sgimidiport_get_default(void)
{
  if(!sgimidiport_default && fts_midi_hack_default_device_name)
    {
      fts_atom_t a[2];
      
      post("create SGI default MIDI port: %s\n", fts_symbol_name(fts_midi_hack_default_device_name));

      fts_set_symbol(a + 0, fts_new_symbol("_midiport"));
      fts_set_symbol(a + 1, fts_midi_hack_default_device_name);
      fts_object_new(0, 2, a, (fts_object_t **)&sgimidiport_default);
    }

  return (fts_midiport_t *)sgimidiport_default;
}

/************************************************************
 *
 *  class
 *
 */
static fts_status_t
sgimidiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sgimidiport_t), 1, 0, 0);

  fts_midiport_class_init(cl, sgimidiport_send_channel_message, sgimidiport_send_system_exclusive);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sgimidiport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sgimidiport_delete);

  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, sgimidiport_get_state);

  return fts_Success;
}

void
sgimidiport_config(void)
{
  fts_midiport_set_default_function(sgimidiport_get_default);

  fts_class_install(fts_s__midiport, sgimidiport_instantiate);
  fts_class_alias(fts_new_symbol("sgimidiport"), fts_s__midiport);
}

extern void sgimidiport_config(void);

fts_module_t midisgi_module = {"midisgi", "SGI MIDI classes", sgimidiport_config, 0, 0};
