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
#include "fts.h"
#include "sequence.h"

extern fts_symbol_t seqnote_symbol;

typedef struct _seqnote_
{
  sequence_event_t head;
  int pitch;
  double duration;
} seqnote_t;

#define seqnote_set_pitch(n, p) ((n)->pitch = (p))
#define seqnote_get_pitch(n) ((n)->pitch)

#define seqnote_set_duration(n, d) ((n)->duration = (d))
#define seqnote_get_duration(n) ((n)->duration)
