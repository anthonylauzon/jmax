/*
 * jMaxout
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <CoreMIDI/MIDIServices.h>

#include <fts/fts.h>
#include "macosxmidi.h"

/* MIDI status bytes */
#define STATUS_BYTE_SYSEX 0xf0
#define STATUS_BYTE_SYSEX_END 0xf7

#define BUFFER_LENGTH 256

fts_metaclass_t *macosxmidi_input_type = NULL;
fts_metaclass_t *macosxmidi_output_type = NULL;

/****************************************************
 *
 *  MIDI port
 *
 */
static void
macosxmidiport_parse_input(const MIDIPacketList *pktlist, void *o, void *src)
{
  fts_midiparser_t *parser = (fts_midiparser_t *)o;
  const MIDIPacket *packet = &pktlist->packet[0];
  int i, j;

  for(i=0; i<pktlist->numPackets; i++)
    {
      for(j=0; j<packet->length; j++)
        fts_midiparser_byte(parser, packet->data[j]);
  
      packet = MIDIPacketNext(packet);
    }
}

static void
macosxmidiport_fifo_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* write to MIDI fifo */
}

static void
macosxmidiport_fifo_poll(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;

  /* read from MIDI fifo and call listeners */

  /*
   fts_atom_t a;
   fts_set_object(&a, event);

   if(time == 0.0)
    {
    fts_object_refer(event);
    fts_midiport_input((fts_object_t *)parser, 0, 0, 1, &a);
    fts_object_release(event);
    }
  else
    fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)parser, fts_midiport_input, &a, time - fts_get_time());
*/
}

static void
macosxmidiport_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  MIDIPacketList pktlist;
  MIDIPacket *pkt = MIDIPacketListInit(&pktlist);

  if(fts_midievent_is_channel_message(event))
    {
    unsigned char buffer[3];

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
    }
  else
    {
    switch(fts_midievent_get_type(event))
      {
      case midi_system_exclusive:
        {
          Byte buffer[BUFFER_LENGTH];
          int size = fts_midievent_system_exclusive_get_size(event);
          fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(event);
          int i, n;

          buffer[0] = STATUS_BYTE_SYSEX;

          for(i=0, n=1; i<size; i++)
            {
            buffer[n++] = fts_get_int(atoms + i) & 0x7f;

            if(n == BUFFER_LENGTH)
              {
              pkt = MIDIPacketListAdd(&pktlist, sizeof(MIDIPacketList), pkt, 0, BUFFER_LENGTH, buffer);
              n = 0;
              }
            }

          buffer[n++] = STATUS_BYTE_SYSEX_END;
          MIDIPacketListAdd(&pktlist, sizeof(MIDIPacketList), pkt, 0, n, buffer);
        }
        break;

      case midi_real_time:
        {
          Byte byte = fts_midievent_real_time_get_status_byte(event);
          MIDIPacketListAdd(&pktlist, sizeof(MIDIPacketList), pkt, 0, 1, &byte);
        }
        break;

      default:
        break;
      }
    }

  if(this->port != NULL)
    MIDISend(this->port, this->ref, &pktlist);
  else
    MIDIReceived(this->ref, &pktlist);
}

static void
macosxmidi_input_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  fts_midiparser_t *parser = &this->parser;
  macosxmidi_t *manager;
  fts_symbol_t name;

  ac--;
  at++;

  manager = (macosxmidi_t *)fts_get_object(at);
  name = fts_get_symbol(at + 1);

  this->manager = manager;
  this->name = NULL;
  this->port = NULL;
  this->ref = NULL;
  this->id = 0;

  if(ac > 2) {
    fts_atom_t k, a;

    this->name = name;
    this->id = fts_get_int(at + 2);
    this->ref = (MIDIEndpointRef)macosxmidi_get_by_unique_id(this->id, kMIDIObjectType_Source);

    if(this->ref != NULL) {
      
      /* create Mac OS X port */
      MIDIInputPortCreate(manager->client, CFSTR("jMax port"), macosxmidiport_parse_input, (void *)&this->parser, &this->port);

      if(this->port != NULL) {
        
        /* connect port to source */
        MIDIPortConnectSource(this->port, this->ref, NULL);

        /* insert into source hashtable */
        fts_set_symbol(&k, name);
        fts_set_object(&a, o);
        fts_hashtable_put(&manager->sources, &k, &a);
        
      } else {
        fts_object_set_error(o, "cannot create port");
        return;
      }
      
    } else {
      fts_object_set_error(o, "invalid id");
      return;
    }
    
  } else {
    
    /* set midiport name to "export" */
    this->name = fts_s_export;
    
    /* create destination */
    MIDIDestinationCreate(manager->client, CFStringCreateWithCString(NULL, name, CFStringGetSystemEncoding()), macosxmidiport_parse_input, (void *)&this->parser, &this->ref);

    if(this->ref == NULL) {
      fts_object_set_error(o, "cannot create MIDI destination");
      return;
    }
  }

  /* init parser and port */
  fts_midiparser_init(&this->parser);
  fts_midiport_init((fts_midiport_t *)this);
  fts_midiport_set_input((fts_midiport_t *)this);
}

static void
macosxmidi_output_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  fts_midiparser_t *parser = &this->parser;
  macosxmidi_t *manager;
  fts_symbol_t name;  

  ac--;
  at++;

  manager = (macosxmidi_t *)fts_get_object(at);
  name = fts_get_symbol(at + 1);

  this->manager = manager;
  this->name = NULL;
  this->port = NULL;
  this->ref = NULL;
  this->id = 0;
  
  fts_midiport_init((fts_midiport_t *)this);
  
  if(ac > 2) {
    fts_atom_t k, a;

    this->name = name;
    this->id = fts_get_int(at + 2);
    this->ref = (MIDIEndpointRef)macosxmidi_get_by_unique_id(this->id, kMIDIObjectType_Destination);

    if(this->ref != NULL) {

      /* create Mac OS X port */
      MIDIOutputPortCreate(manager->client, CFSTR("jMax port"), &this->port);

      if(this->port != NULL) {

        /* set parser output function */
        fts_midiport_set_output((fts_midiport_t *)this, macosxmidiport_output);

        /* insert into destination hashtable */
        fts_set_symbol(&k, name);
        fts_set_object(&a, o);
        fts_hashtable_put(&manager->destinations, &k, &a);
        
      } else {
        fts_object_set_error(o, "cannot create port");
        return;
      }

    } else {
      fts_object_set_error(o, "invalid id");
      return;
    }

  } else {
    
    /* set midiport name to "export" */
    this->name = fts_s_export;
    
    /* create destination */
    MIDISourceCreate(manager->client, CFStringCreateWithCString(NULL, name, CFStringGetSystemEncoding()), &this->ref);

    if(this->ref != NULL) {
      /* set parser output function */
      fts_midiport_set_output((fts_midiport_t *)this, macosxmidiport_output);
    } else {
      fts_object_set_error(o, "cannot create MIDI destination");
      return;
    }    
  }
}

static void
macosxmidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  fts_atom_t k, a;

  if(this->port != NULL) {
    fts_hashtable_t *ht;

    /* (device) midiport */
    if(fts_midiport_is_input((fts_midiport_t *)this)) {
      /* disconnect from source */
      MIDIPortDisconnectSource(this->port, this->ref);

      ht = &this->manager->sources;
    } else {
      ht = &this->manager->destinations;
    }

    /* remove midiport from hashtable and put back id if still valid */
    fts_set_symbol(&k, this->name);

    if(macosxmidi_get_by_unique_id(this->id, MACOSXMIDI_OBJECT_TYPE_ANY) != NULL) {
      /* remove midiport from hashtable and put reference instead */
      fts_set_int(&a, this->id);
      fts_hashtable_put(ht, &k, &a);
    } else {
      fts_hashtable_remove(ht, &k);
    }

    /* destroy port */
    MIDIPortDispose(this->port);
  } else {
    /* destroy created source or destination */
    MIDIEndpointDispose(this->ref);
  }

  if(fts_midiport_is_input((fts_midiport_t *)this))
    fts_midiparser_reset(&this->parser);

  fts_midiport_reset((fts_midiport_t *)this);
}

static fts_status_t
macosxmidi_input_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof( macosxmidiport_t), 0, 0, 0);
  
  fts_midiport_class_init(cl);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, macosxmidi_input_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, macosxmidiport_delete);
  
  return fts_Success;
}

static fts_status_t
macosxmidi_output_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof( macosxmidiport_t), 0, 0, 0);
  
  fts_midiport_class_init(cl);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, macosxmidi_output_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, macosxmidiport_delete);
  
  return fts_Success;
}

void 
macosxmidiport_config( void)
{
  macosxmidi_input_type = fts_class_install( fts_new_symbol("macosxmidi_input"), macosxmidi_input_instantiate);
  macosxmidi_output_type = fts_class_install( fts_new_symbol("macosxmidi_output"), macosxmidi_output_instantiate);
}
