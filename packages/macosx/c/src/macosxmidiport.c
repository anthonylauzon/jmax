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

#include <fts/fts.h>
#include "macosxmidi.h"

/* MIDI status bytes */
#define STATUS_BYTE_SYSEX 0xf0
#define STATUS_BYTE_SYSEX_END 0xf7

#define BUFFER_LENGTH 256

fts_class_t *macosxmidi_input_type = NULL;
fts_class_t *macosxmidi_output_type = NULL;

/****************************************************
 *
 *  MIDI port
 *
 */
static void
macosxmidiport_parse_input(const MIDIPacketList *pktlist, void *o, void *src)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  macosxmidi_t *manager = this->manager;
  fts_midiparser_t *parser = &this->parser;
  fts_midififo_t *fifo = &manager->fifo;
  const MIDIPacket *packet = &pktlist->packet[0];
  fts_midievent_t *event;
  int i, j;

  /* get event of next entry and set parser */
  event = fts_midififo_get_event(fifo);
  fts_midiparser_set_event(parser, event);

  for(i=0; i<pktlist->numPackets; i++) {
    double time = 0.000001 * (double)AudioConvertHostTimeToNanos(packet->timeStamp);

    if(event != NULL) {
      for(j=0; j<packet->length; j++) {
        fts_midievent_t *parsed = fts_midiparser_byte(parser, packet->data[j]);

        if(parsed != NULL) {
          /* write fifo entry */
          fts_midififo_write(fifo, o, time);

          /* get event of next entry and set parser */
          event = fts_midififo_get_event(fifo);
          fts_midiparser_set_event(parser, event);

          if(event == NULL)
            break;
        }
      }
    } else
      fts_object_signal_runtime_error((fts_object_t *)o, "MIDI buffer overflow");
    
    packet = MIDIPacketNext(packet);
  }
  
  fts_midiparser_set_event(parser, NULL);
}

static void
macosxmidiport_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  MIDIPacketList pktlist;
  MIDIPacket *pkt = MIDIPacketListInit(&pktlist);
  UInt64 hosttime = AudioGetCurrentHostTime();
  UInt64 ftstime = AudioConvertNanosToHostTime((UInt64)(1000000.0 * fts_get_time()));
  static UInt64 lasthost = 0;
  static UInt64 lastfts = 0;
  
  if(this->manager->delta == 0)
    this->manager->delta = hosttime - ftstime;
  
  pkt->timeStamp = ftstime + this->manager->delta;

  lasthost = hosttime;
  lastfts = ftstime;
  
  if(pkt->timeStamp < hosttime) {
    this->manager->delta = hosttime - ftstime;
    pkt->timeStamp = hosttime;
  }
    
  if(fts_midievent_is_channel_message(event)) {
    Byte buffer[3];

    if(fts_midievent_channel_message_has_second_byte(event)) {
      buffer[0] = (unsigned char)fts_midievent_channel_message_get_status_byte(event);
      buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);
      buffer[2] = (unsigned char)(fts_midievent_channel_message_get_second(event) & 0x7f);

      MIDIPacketListAdd(&pktlist, sizeof(MIDIPacketList), pkt, 0, 3, buffer);
    } else {
      buffer[0] = (unsigned char)fts_midievent_channel_message_get_status_byte(event);
      buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);

      MIDIPacketListAdd(&pktlist, sizeof(MIDIPacketList), pkt, 0, 2, buffer);
    }
  } else {
    switch(fts_midievent_get_type(event)) {
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
  macosxmidi_t *manager;
  fts_symbol_t name;
  fts_atom_t k, a;
  
  manager = (macosxmidi_t *)fts_get_object(at);
  name = fts_get_symbol(at + 1);

  this->manager = manager;
  this->name = name;
  this->port = NULL;
  this->ref = NULL;
  this->id = 0;

  if(ac > 2) {
    this->id = fts_get_int(at + 2);
    this->ref = macosxmidi_get_reference_by_id(this->id, macosxmidi_input);

    if(this->ref != NULL) {
      /* create Mac OS X MIDI port */
      MIDIInputPortCreate(manager->client, CFSTR("jMax port"), macosxmidiport_parse_input, (void *)this, &this->port);

      if(this->port != NULL) {
        /* connect port to source */
        MIDIPortConnectSource(this->port, this->ref, NULL);

        /* insert into input hashtable */
        fts_set_symbol(&k, name);
        fts_set_object(&a, o);
        fts_hashtable_put(&manager->inputs, &k, &a);
      } else {
        fts_object_set_error(o, "cannot create port");
        return;
      }
    } else {
      fts_object_set_error(o, "invalid Mac OS MIDI object id");
      return;
    }
  } else {    
    /* create Mac OS virtual MIDI destination */
    MIDIDestinationCreate(manager->client, CFStringCreateWithCString(NULL, name, CFStringGetSystemEncoding()), macosxmidiport_parse_input, (void *)this, &this->ref);

    if(this->ref != NULL) {
      /* insert into destination hashtable */
      fts_set_symbol(&k, name);
      fts_set_object(&a, o);
      fts_hashtable_put(&manager->destinations, &k, &a);
    } else {
      fts_object_set_error(o, "cannot create Mac OS virtual MIDI destination");
      return;
    }
  }

  /* init MIDI parser  */
  fts_midiparser_init(&this->parser);

  /* init FTS MIDI port  */
  fts_midiport_init((fts_midiport_t *)this);
  fts_midiport_set_input((fts_midiport_t *)this);
}

static void
macosxmidi_output_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidiport_t *this = (macosxmidiport_t *)o;
  macosxmidi_t *manager;
  fts_symbol_t name;
  fts_atom_t k, a;

  manager = (macosxmidi_t *)fts_get_object(at);
  name = fts_get_symbol(at + 1);

  this->manager = manager;
  this->name = name;
  this->port = NULL;
  this->ref = NULL;
  this->id = 0;
  
  fts_midiport_init((fts_midiport_t *)this);
  
  if(ac > 2) {
    this->id = fts_get_int(at + 2);
    this->ref = macosxmidi_get_reference_by_id(this->id, macosxmidi_output);

    if(this->ref != NULL) {
      /* create Mac OS X port */
      MIDIOutputPortCreate(manager->client, CFSTR("jMax port"), &this->port);

      if(this->port != NULL) {

        /* set parser output function */
        fts_midiport_set_output((fts_midiport_t *)this, macosxmidiport_output);

        /* insert into output hashtable */
        fts_set_symbol(&k, name);
        fts_set_object(&a, o);
        fts_hashtable_put(&manager->outputs, &k, &a);
      } else {
        fts_object_set_error(o, "cannot create Mac OS MIDI port");
        return;
      }
    } else {
      fts_object_set_error(o, "invalid Mac OS MIDI object id");
      return;
    }
  } else {
    /* create source */
    MIDISourceCreate(manager->client, CFStringCreateWithCString(NULL, name, CFStringGetSystemEncoding()), &this->ref);

    if(this->ref != NULL) {
      /* set parser output function */
      fts_midiport_set_output((fts_midiport_t *)this, macosxmidiport_output);

      /* insert into sources hashtable */
      fts_set_symbol(&k, name);
      fts_set_object(&a, o);
      fts_hashtable_put(&manager->sources, &k, &a);
    } else {
      fts_object_set_error(o, "cannot create Mac OS virtual MIDI source");
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
    macosxmidi_type_t type;

    /* (device) midiport */
    if(fts_midiport_is_input((fts_midiport_t *)this)) {
      /* disconnect from source */
      MIDIPortDisconnectSource(this->port, this->ref);

      ht = &this->manager->inputs;
      type = macosxmidi_input;
    } else if(fts_midiport_is_output((fts_midiport_t *)this)) {
      ht = &this->manager->outputs;
      type = macosxmidi_output;
    } else
      return;

    /* remove midiport from hashtable and put back id if still valid */
    fts_set_symbol(&k, this->name);

    if(macosxmidi_get_reference_by_id(this->id, type) != NULL) {
      /* remove midiport from hashtable and put reference instead */
      fts_set_int(&a, this->id);
      fts_hashtable_put(ht, &k, &a);
    } else {
      fts_hashtable_remove(ht, &k);
    }

    /* destroy port */
    MIDIPortDispose(this->port);
  } else {
    fts_hashtable_t *ht;
    
    /* virtual MIDI sources or destination */
    if(fts_midiport_is_input((fts_midiport_t *)this))
      ht = &this->manager->destinations;
    else if(fts_midiport_is_output((fts_midiport_t *)this))
      ht = &this->manager->sources;
    else
      return;

    /* remove from hashtable */
    fts_set_symbol(&k, this->name);
    fts_hashtable_remove(ht, &k);

    /* destroy created source or destination */
    MIDIEndpointDispose(this->ref);
  }

  if(fts_midiport_is_input((fts_midiport_t *)this))
    fts_midiparser_reset(&this->parser);

  fts_midiport_reset((fts_midiport_t *)this);
}

static void
macosxmidi_input_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( macosxmidiport_t), macosxmidi_input_init, macosxmidiport_delete);

  fts_midiport_class_init(cl);  
  }

static void
macosxmidi_output_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( macosxmidiport_t), macosxmidi_output_init, macosxmidiport_delete);

  fts_midiport_class_init(cl);
    }

void 
macosxmidiport_config( void)
{
  macosxmidi_input_type = fts_class_install( fts_new_symbol("macosxmidi_input"), macosxmidi_input_instantiate);
  macosxmidi_output_type = fts_class_install( fts_new_symbol("macosxmidi_output"), macosxmidi_output_instantiate);
}
