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
 * Authors: Riccardo Borghesi, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <CoreMIDI/MIDIServices.h>

#include <fts/fts.h>

/* MIDI status bytes */
#define STATUS_BYTE_SYSEX 0xf0
#define STATUS_BYTE_SYSEX_END 0xf7

#define BUFFER_LENGTH 512

typedef struct _macosxmidiport_
{
  fts_midiparser_t head; /* parser is a MIDI port */

  MIDIClientRef ref;
  MIDIPortRef port;
  MIDIEndpointRef dest;  

  unsigned char receive_buffer[BUFFER_LENGTH]; /* system exclusive output buffer */
  int sysex_head;
} macosxmidiport_t;

static void
macosxmidiport_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;

  if(fts_midievent_is_channel_message(event))
    {
      unsigned char buffer[3];
      MIDIPacketList pktlist;
      MIDIPacket *pkt = MIDIPacketListInit(&pktlist);
      
      if(fts_midievent_channel_message_has_second_byte(event))
	{
	  buffer[0] = (unsigned char)fts_midievent_channel_message_get_status_byte(event);
	  buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);
	  buffer[2] = (unsigned char)(fts_midievent_channel_message_get_second(event) & 0x7f);

	  MIDIPacketListAdd(&pktlist, sizeof(MIDIPacketList), pkt, 0, 3, buffer);
	}
      else
	{
	  buffer[0] = (unsigned char)fts_midievent_channel_message_get_status_byte(event);
	  buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);

	  MIDIPacketListAdd(&pktlist, sizeof(MIDIPacketList), pkt, 0, 2, buffer);
	}

      MIDISend(this->port, this->dest, &pktlist);
    }
  else 
    {
      switch(fts_midievent_get_type(event))
	{
	case midi_system_exclusive:
	  {
	    unsigned char buffer[BUFFER_LENGTH];
	    int size = fts_midievent_system_exclusive_get_size(event);
	    fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(event);
	    int i, n;
	    
	    buffer[0] = STATUS_BYTE_SYSEX;
	    
	    for(i=0, n=1; i<size; i++)
	      {
		buffer[n++] = fts_get_int(atoms + i) & 0x7f;
		
		if(n == BUFFER_LENGTH)
		  {
		    /*snd_rawmidi_write( this->handle_out, buffer, BUFFER_LENGTH);*/
		    n = 0;
		  }
	      }
	    
	    buffer[n++] = STATUS_BYTE_SYSEX_END;
	    /*snd_rawmidi_write(this->handle_out, buffer, n);*/
	  }
	  break;
	  
	case midi_real_time:
	  {
	    unsigned char byte = (unsigned char)fts_midievent_real_time_get_status_byte(event);
	    /*snd_rawmidi_write(this->handle_out, &byte, 1);*/
	  }
	  break;
	  
	default:
	  break;
	}
    }
}

static void 
macosxmidiport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  fts_midiparser_t *parser = (fts_midiparser_t *)o;
  int n;
  ac--;
  at++;

  this->ref = NULL;
  this->port = NULL;
  this->dest = NULL;

  fts_midiparser_init( parser);

  MIDIClientCreate(CFSTR("jMax"), NULL, NULL, &this->ref);
  MIDIOutputPortCreate(this->ref, CFSTR("Output port"), &this->port);

  n = MIDIGetNumberOfDestinations();
  if (n > 0)
    this->dest = MIDIGetDestination(0);

  /*fts_midiport_set_input((fts_midiport_t *)parser);*/
  if(this->dest)
    fts_midiport_set_output((fts_midiport_t *)parser, macosxmidiport_output);

  this->sysex_head = 0;
}

static void
macosxmidiport_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  fts_midiparser_t *parser = (fts_midiparser_t *)o;

  /*MIDIPortDispose(this->port);*/
  MIDIClientDispose(this->ref);

  fts_midiparser_reset(parser);
}

static void 
macosxmidiport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;

  if( this->dest != NULL)
    fts_set_object( value, o);
  else
    fts_set_void( value);
}

static fts_status_t 
macosxmidiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof( macosxmidiport_t), 0, 0, 0);

  fts_midiport_class_init(cl);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, macosxmidiport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, macosxmidiport_delete);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, macosxmidiport_get_state);

  return fts_Success;
}

void 
macosxmidiport_config( void)
{
  fts_class_install( fts_new_symbol("macosxmidiport"), macosxmidiport_instantiate);
}
