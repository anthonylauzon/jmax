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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include "fts.h"
#include "seqsym.h"
#include "sequence.h"
#include "seqref.h"

void 
seqref_set_reference(fts_object_t *o, int ac, const fts_atom_t *at)
{ 
  seqref_t *this = (seqref_t *)o;
  
  if(ac && fts_is_object(at))
    {
      fts_object_t *sequence = fts_get_object(at);
      
      if(fts_object_get_class_name(sequence) == seqsym_sequence)
	{
	  this->sequence = (sequence_t *)sequence;
	  
	  if(ac > 1 && fts_is_number(at + 1))
	    this->index = fts_get_number_int(at + 1) - 1;
	}
    }
}

eventtrk_t *
seqref_get_reference(fts_object_t *o)
{
  seqref_t *this = (seqref_t *)o;
  eventtrk_t *track = 0;

  if(this->sequence && this->index >= 0 && this->index < sequence_get_size(this->sequence))
    track = (eventtrk_t *)sequence_get_track_by_index(this->sequence, this->index);

  return track;
}
      
void
seqref_init(fts_object_t *o, int ac, const fts_atom_t *at)
{ 
  seqref_t *this = (seqref_t *)o;

  this->sequence = 0;
  this->track = 0;
  this->index = 0;
  
  seqref_set_reference(o, ac - 1, at + 1);
}

void
seqref_highlight_event(fts_object_t *o, event_t *event)
{
  seqref_t *this = (seqref_t *)o;

  if(sequence_editor_is_open(this->sequence))
    {
      fts_atom_t a[1];
      
      fts_set_object(a, (fts_object_t *)event);
      fts_client_send_message((fts_object_t *)this->track, seqsym_highlightEvents, 1, a);
    }
}
  
void
seqref_highlight_array(fts_object_t *o, int ac, const fts_atom_t *at)
{
  seqref_t *this = (seqref_t *)o;

  if(sequence_editor_is_open(this->sequence))
    fts_client_send_message((fts_object_t *)this->track, seqsym_highlightEvents, ac, at);
}
