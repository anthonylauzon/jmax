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
 */

#ifndef _ALSASEQMIDI_H_
#define _ALSASEQMIDI_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <alsa/asoundlib.h>


typedef struct _alsaseqmidi_
{
    fts_midimanager_t manager;

    /* ALSA Sequencer client */
    /* needed to make query about available alsa sequencer client */
    snd_seq_t* seq; /* alsa sequencer handler */
    fts_symbol_t client_name; /* alsa sequencer client name */

    /* Default port */
    fts_symbol_t default_input;
    fts_symbol_t default_output;

    
    fts_hashtable_t inputs; /* alsa sequencer jmax registered destinations */
    fts_hashtable_t outputs; /* alsa sequencer jmax registered sources */

    fts_hashtable_t sources; /* alsa sequencer available sources */
    fts_hashtable_t destinations; /* alsa sequencer availabe destination */

/*    fts_midififo_t fifo;*/
} alsaseqmidi_t;


typedef struct _alsaseqmidiport_
{
    fts_midiport_t head;
    fts_midiparser_t parser;
    alsaseqmidi_t* manager;

    /* ALSA Sequencer client */
    snd_seq_t* seq; /* alsa sequencer handle */
    fts_symbol_t name; /* client name */

    /* MIDI Message parsing */
    snd_midi_event_t* midi_event_parser; /* alsa sequencer midi parser */
    unsigned char* buffer;  /* bytes stream buffer */
    int buffer_size; /* size of bytes stream buffer */


    /* ALSA Port Info */
    snd_seq_port_info_t* port_info;

    /* TO REMOVE */
    int port_id; /* port id can be get from port_info */


    /* ALSA Sequencer Port subscription */
    /* We store port subscription when we need to delete 
       this port */
    snd_seq_port_subscribe_t* subs;

    int fd; /* poll fd */
} alsaseqmidiport_t;

extern fts_metaclass_t *alsaseqmidiport_input_type;
extern fts_metaclass_t *alsaseqmidiport_output_type;

#endif /* _ALSASEQMIDI_H_ */
