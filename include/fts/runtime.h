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

#ifndef _FTS_RUNTIME_H_
#define _FTS_RUNTIME_H_

/* Audio */
#include <fts/runtime/audio/audio.h>
#include <fts/runtime/audio/audioport.h>
/* Client */
#include <fts/runtime/client/protocol.h>
#include <fts/runtime/client/client.h>
#include <fts/runtime/client/incoming.h>
#include <fts/runtime/client/outgoing.h>
#include <fts/runtime/client/updates.h>
/* Devices */
#include <fts/runtime/devices/bytestream.h>
#include <fts/runtime/devices/devices.h>
#include <fts/runtime/devices/unixdev.h>
/* Files */
#include <fts/runtime/files/atomfiles.h>
#include <fts/runtime/files/directories.h>
#include <fts/runtime/files/files.h>
#include <fts/runtime/files/midifiles.h>
#include <fts/runtime/files/parser.h>
#include <fts/runtime/files/post.h>
#include <fts/runtime/files/soundfiles.h>
#include <fts/runtime/files/soundformats.h>
/* MIDI */
#include <fts/runtime/midi/midi.h>
#include <fts/runtime/midi/midiport.h>
#include <fts/runtime/midi/midiparser.h>
/* Timing & Scheduler */
#include <fts/runtime/time/time.h>
#include <fts/runtime/sched/sched.h>

#endif