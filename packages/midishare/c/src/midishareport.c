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
#include <MidiShare.h>
#include "fts.h"

typedef struct _midishareport_
{
  fts_midiport_t head;
  int refnum;
  fts_symbol_t name;
} midishareport_t;

void
midishareport_dispatch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midishareport_t *port = (midishareport_t *)o;
  int n_events = MidiCountEvs(port->refnum);
  int i;

  for(i=0; i<n_events; i++) 
    {
      MidiEvPtr evt = MidiGetEv(port->refnum);
      int type = EvType(evt);
      
      switch(type)
	{
	case typeKeyOn:
	  fts_midiport_channel_message(&port->head, fts_midi_status_note, Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1), 0.0);
	  break;
	case typeKeyOff:
	  fts_midiport_channel_message(&port->head, fts_midi_status_note, Chan(evt) + 1, MidiGetField(evt, 0), 0, 0.0);
	  break;
	case typeKeyPress:
	  fts_midiport_channel_message(&port->head, fts_midi_status_poly_pressure, Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1), 0.0);
	  break;
	case typeCtrlChange:
	  fts_midiport_channel_message(&port->head, fts_midi_status_control_change, Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1), 0.0);
	  break;
	case typeProgChange:
	  fts_midiport_channel_message(&port->head, fts_midi_status_program_change, Chan(evt) + 1, MidiGetField(evt, 0), 0, 0.0);
	  break;
	case typeChanPress:
	  fts_midiport_channel_message(&port->head, fts_midi_status_channel_pressure, Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1), 0.0);
	  break;
	case typePitchWheel:
	  fts_midiport_channel_message(&port->head, fts_midi_status_pitch_bend, Chan(evt) + 1, MidiGetField(evt, 0) + (MidiGetField(evt, 1) << 7), 0, 0.0);
	  break;
	case typeSysEx:
	  {
	    int size = MidiCountFields(evt);
	    int j;
	    
	    for(j=0; j<size - 2; j++)
	      fts_midiport_system_exclusive_add_byte(&port->head, MidiGetField(evt, j + 1));
	    
	    fts_midiport_system_exclusive(&port->head, 0.0);
	  }
	  
	  break;
	}

      MidiFreeEv(evt);
    }
}

/************************************************************
 *
 *  MIDI port interface methods
 *
 */

static void
midishareport_send_channel_message(fts_midiport_t *port, int status, int channel, int x, int y, double time)
{
  midishareport_t *this = (midishareport_t *)port;

  channel--; /* 1 to 16 -> 0 to 15 */

  switch(status)
    {
    case fts_midi_status_note:
      {
	MidiEvPtr evt = MidiNewEv(typeKeyOn);

	Chan(evt) = channel & 0x0F;

	MidiSetField(evt, 0, x & 127);
	MidiSetField(evt, 1, y & 127);

	MidiSendIm(this->refnum, evt);
	
	break;
      }
    case fts_midi_status_poly_pressure:
      {
	MidiEvPtr evt = MidiNewEv(typeKeyPress);

	Chan(evt) = channel & 0x0F;

	MidiSetField(evt, 0, x & 127);
	MidiSetField(evt, 1, y & 127);

	MidiSendIm(this->refnum, evt);
	
	break;
      }
    case fts_midi_status_control_change:
      {
	MidiEvPtr evt = MidiNewEv(typeCtrlChange);

	Chan(evt) = channel & 0x0F;

	MidiSetField(evt, 0, x & 127);
	MidiSetField(evt, 1, y & 127);

	MidiSendIm(this->refnum, evt);
	
	break;
      }
    case fts_midi_status_program_change:
      {	
	MidiEvPtr evt = MidiNewEv(typeProgChange);

	Chan(evt) = channel & 0x0F;

	MidiSetField(evt, 0, x & 127);

	MidiSendIm(this->refnum, evt);
	
	break;
      }
    case fts_midi_status_channel_pressure:
      {
	MidiEvPtr evt = MidiNewEv(typeChanPress);

	Chan(evt) = channel & 0x0F;

	MidiSetField(evt, 0, x & 127);

	MidiSendIm(this->refnum, evt);
	
	break;
      }
    case fts_midi_status_pitch_bend:
      {
	MidiEvPtr evt = MidiNewEv(typePitchWheel);

	Chan(evt) = channel & 0x0F;
	
	MidiSetField(evt, 0, x & 127);
	MidiSetField(evt, 1, (x >> 7) & 127);
	
	MidiSendIm(this->refnum, evt);
	
	break;
      }
    }
}

static void
midishareport_send_system_exclusive(fts_midiport_t *port, int ac, const fts_atom_t *at, double time)
{
  midishareport_t *this = (midishareport_t *)port;
  MidiEvPtr evt = MidiNewEv(typeSysEx);
  int i;

  Chan(evt) = 0;

  for(i=0; i<ac; i++)
    MidiAddField(evt, fts_get_int(at + i));

  MidiSendIm(this->refnum, evt);
}

/************************************************************
 *
 *  object
 *
 */

static void
midishareport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midishareport_t *this = (midishareport_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);

  fts_midiport_init(&this->head);

  this->refnum = -1;
  this->name = 0;

  if(name)
    {
      if (MidiShare())
	this->refnum = MidiOpen((MidiName)fts_symbol_name(name));
      else
	{
	  post("midishareport: didn't find MidiShare");
	}

      this->name = name;
    }
  
  if(this->refnum >= 0)
    {
      fts_midiport_set_input(&this->head);
      fts_midiport_set_output(&this->head, midishareport_send_channel_message, midishareport_send_system_exclusive);

      fts_sched_add(fts_sched_get_current(), midishareport_dispatch, o);
    }
}

static void 
midishareport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midishareport_t *this = (midishareport_t *)o;

  MidiClose(this->refnum);
}

/************************************************************
 *
 *  get midiport variable
 *
 */
static void
midishareport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  midishareport_t *this = (midishareport_t *)o;

  fts_set_object(value, o);
}

/************************************************************
 *
 *  default port
 *
 */
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
extern fts_symbol_t fts_midi_hack_default_device_name;
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

midishareport_t *midishareport_default = 0;

fts_midiport_t *
midishareport_get_default(void)
{
  if(!midishareport_default && fts_midi_hack_default_device_name)
    {
      fts_atom_t a[2];
      
      post("create SGI default MIDI port: %s\n", fts_symbol_name(fts_midi_hack_default_device_name));

      fts_set_symbol(a + 0, fts_new_symbol("midishareport"));
      fts_set_symbol(a + 1, fts_midi_hack_default_device_name);
      fts_object_new(0, 2, a, (fts_object_t **)&midishareport_default);
    }

  return (fts_midiport_t *)midishareport_default;
}

/************************************************************
 *
 *  class
 *
 */
static fts_status_t
midishareport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midishareport_t), 1, 0, 0);

  fts_midiport_class_init(cl);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midishareport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midishareport_delete);

  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, midishareport_get_state);

  return fts_Success;
}

void
midishareport_config(void)
{
  fts_midiport_set_default_function(midishareport_get_default);

  fts_class_install(fts_new_symbol("midishareport"), midishareport_instantiate);
}

fts_module_t midishare_module = {"midishare", "midishare MIDI classes", midishareport_config, 0, 0};
