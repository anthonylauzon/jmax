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
#include <fts/fts.h>

#define MAX_MIDI_EVENTS 512
#define SYSEX_BLOCK_SIZE 512

#define SYSEX_REALTIME 0x7f

#define mdGetStatus(msg) (msg[0] & MD_STATUSMASK)
#define mdGetChannel(msg) (msg[0] & MD_CHANNELMASK)
#define mdGetByte1(msg) (msg[1])
#define mdGetByte2(msg)(msg[2])

typedef struct _sgimidiport_
{
  fts_midiport_t head;
  MDport in;
  MDport out;
  fts_symbol_t name;
  char sysex_buf[SYSEX_BLOCK_SIZE];
  int sysex_size;
} sgimidiport_t;

int
sgimidiport_open(sgimidiport_t *port, fts_symbol_t name)
{
  if (mdInit() > 0)
    {
      port->in = mdOpenInPort(name);
      port->out = mdOpenOutPort(name);
      
      if(port->in && port->out)
	{
	  mdSetStampMode(port->in, MD_NOSTAMP);
	  mdSetStampMode(port->out, MD_NOSTAMP);      
	  
	  return 1;
	}
      else
	{
	  /*should be considered:*/
	  /*mdRegister(name);*/

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
	  fts_midiport_input_note(&port->head, mdGetChannel(msg) + 1, mdGetByte1(msg), 0, 0.0);
	  break;
	case MD_NOTEON:
	  fts_midiport_input_note(&port->head, mdGetChannel(msg) + 1, mdGetByte1(msg), mdGetByte2(msg), 0.0);
	  break;
	case MD_POLYKEYPRESSURE:
	  fts_midiport_input_poly_pressure(&port->head, mdGetChannel(msg) + 1, mdGetByte1(msg), mdGetByte2(msg), 0.0);
	  break;
	case MD_CONTROLCHANGE:
	  fts_midiport_input_control_change(&port->head, mdGetChannel(msg) + 1, mdGetByte1(msg), mdGetByte2(msg), 0.0);
	  break;
	case MD_PROGRAMCHANGE:
	  fts_midiport_input_program_change(&port->head, mdGetChannel(msg) + 1, mdGetByte1(msg), 0.0);
	  break;
	case MD_CHANNELPRESSURE:
	  fts_midiport_input_channel_pressure(&port->head, mdGetChannel(msg) + 1, mdGetByte1(msg), 0.0);
	  break;
	case MD_PITCHBENDCHANGE:
	  fts_midiport_input_pitch_bend(&port->head, mdGetChannel(msg) + 1, mdGetByte1(msg) + (mdGetByte2(msg) << 7), 0.0);
	  break;
	case MD_SYSEX:
	  {
	    int size = midi_events[i].msglen;
	    int j;

	    if(midi_events[i].sysexmsg[1] == SYSEX_REALTIME)
	      {
		for(j=2; j<size-1; j++)
		  fts_midiport_input_system_exclusive_byte(&port->head, midi_events[i].sysexmsg[j]);
		
		fts_midiport_input_system_exclusive_call(&port->head, 0.0);
		
		if(midi_events[i].msglen > 0)
		  mdFree(midi_events[i].sysexmsg);
	      }
	  }

	  break;
      }
  }
}

/************************************************************
 *
 *  MIDI port interface methods
 *
 */

static void
sgimidiport_send_note(fts_object_t *o, int channel, int number, int value, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)o;
  MDevent event;
  
  event.msg[0] = MD_NOTEON | ((channel - 1) & 0x0F);
  event.msg[1] = (char)(number & 127); /* note number */
  event.msg[2] = (char)(value & 127); /* velocity */
  event.msglen = 3;
  
  mdSend(this->out, &event, 1);
}

static void
sgimidiport_send_poly_pressure(fts_object_t *o, int channel, int number, int value, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)o;
  MDevent event;
  
  event.msg[0] = MD_POLYKEYPRESSURE | ((channel - 1) & 0x0F);
  event.msg[1] = (char)(number & 127); /* note number */
  event.msg[2] = (char)(value & 127); /* pressure value */
  event.msglen = 3;
  
  mdSend(this->out, &event, 1);
}

static void
sgimidiport_send_control_change(fts_object_t *o, int channel, int number, int value, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)o;
  MDevent event;

  event.msg[0] = MD_CONTROLCHANGE | ((channel - 1) & 0x0F);
  event.msg[1] = (char)(number & 127); /* controller number */
  event.msg[2] = (char)(value & 127); /* controller value */
  event.msglen = 3;
	
  mdSend(this->out, &event, 1);
}

static void
sgimidiport_send_program_change(fts_object_t *o, int channel, int value, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)o;
  MDevent event;

  event.msg[0] = MD_PROGRAMCHANGE | ((channel - 1) & 0x0F);
  event.msg[1] = (char)(value & 127); /* programm number */
  event.msg[2] = 0;
  event.msglen = 2;
	
  mdSend(this->out, &event, 1);
}

static void
sgimidiport_send_channel_pressure(fts_object_t *o, int channel, int value, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)o;
  MDevent event;

  event.msg[0] = MD_CHANNELPRESSURE | ((channel - 1) & 0x0F);
  event.msg[1] = (char)(value & 127); /* pressure value */
  event.msg[2] = 0;
  event.msglen = 2;
	
  mdSend(this->out, &event, 1);
}

static void
sgimidiport_send_pitch_bend(fts_object_t *o, int channel, int value, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)o;
  MDevent event;

  event.msg[0] = MD_PITCHBENDCHANGE | ((channel - 1) & 0x0F);
  event.msg[1] = (char)(value & 127); /* LSB */
  event.msg[2] = (char)((value >> 7) & 127); /* MSB */
  event.msglen = 3;

  mdSend(this->out, &event, 1);
}

static void
sgimidiport_send_system_exclusive_byte(fts_object_t *o, int value)
{
  sgimidiport_t *this = (sgimidiport_t *)o;

  this->sysex_buf[this->sysex_size] = (char)(value & 127);
  this->sysex_size++;

  if(this->sysex_size >= SYSEX_BLOCK_SIZE - 1)
    {
      MDevent event;

      event.msg[0] = MD_SYSEX; /* just to make sure */
      event.sysexmsg = this->sysex_buf;
      event.msglen = this->sysex_size;
      
      mdSend(this->out, &event, 1);

      this->sysex_size = 0;
    }
}

static void
sgimidiport_send_system_exclusive_flush(fts_object_t *o, double time)
{
  sgimidiport_t *this = (sgimidiport_t *)o;
  MDevent event;
  int n = 0;

  this->sysex_buf[this->sysex_size] = MD_EOX;
  this->sysex_size++;

  event.msg[0] = MD_SYSEX;
  event.sysexmsg = this->sysex_buf;
  event.msglen = this->sysex_size;
  
  mdSend(this->out, &event, 1);
  
  /* reset sysex buffer */
  port->sysex_buf[0] = MD_SYSEX;
  this->sysex_size = 1;
}

static fts_midiport_output_functions_t sgimidiport_output_functions =
{
  sgimidiport_send_note,
  sgimidiport_send_poly_pressure,
  sgimidiport_send_control_change,
  sgimidiport_send_program_change,
  sgimidiport_send_channel_pressure,
  sgimidiport_send_pitch_bend,
  sgimidiport_send_system_exclusive_byte,
  sgimidiport_send_system_exclusive_flush,
};
  
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

  this->in = 0;
  this->out = 0;
  this->name = 0;

  if(name)
    {
      open = sgimidiport_open(this, name);
      this->name = name;
    }

  fts_midiport_init(&this->head);

  if(open)
    {
      fts_midiport_set_input(&this->head);
      fts_midiport_set_output(&this->head, &sgimidiport_output_functions);

      fts_sched_add( o, FTS_SCHED_READ, mdGetFd(this->in));
    }

  /* init sysex buffer */
  port->sysex_buf[0] = MD_SYSEX;
  this->sysex_size = 1;
}

static void 
sgimidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sgimidiport_t *this = (sgimidiport_t *)o;

  if(this->in)
    {
      fts_sched_remove( o);
      sgimidiport_close(this);
    }

  fts_midiport_reset(&this->head);
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
 *  class
 *
 */
static fts_status_t
sgimidiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sgimidiport_t), 1, 0, 0);

  fts_midiport_class_init(cl);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sgimidiport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sgimidiport_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, sgimidiport_dispatch);

  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, sgimidiport_get_state);

  return fts_Success;
}

void
sgimidiport_config(void)
{
  fts_class_install( fts_new_symbol("sgimidiport"), sgimidiport_instantiate);
}
